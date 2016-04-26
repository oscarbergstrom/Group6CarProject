/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/base/Lock.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/dmcp/ModuleConfigurationProvider.h"
#include "opendavinci/odcore/dmcp/ModuleStateListener.h"
#include "opendavinci/odcore/dmcp/connection/ModuleConnection.h"
#include "opendavinci/generated/odcore/data/Configuration.h"
#include "opendavinci/generated/odcore/data/dmcp/ModuleExitCodeMessage.h"
#include "opendavinci/generated/odcore/data/dmcp/ModuleStateMessage.h"
#include "opendavinci/generated/odcore/data/dmcp/PulseAckContainersMessage.h"
#include "opendavinci/generated/odcore/data/dmcp/PulseMessage.h"
#include "opendavinci/generated/odcore/data/dmcp/RuntimeStatistic.h"

namespace odcore {
    namespace dmcp {
        namespace connection {

            using namespace std;
            using namespace odcore::base;
            using namespace odcore::data;
            using namespace odcore::data::dmcp;

            ModuleConnection::ModuleConnection(std::shared_ptr<odcore::io::Connection> connection,
                                               ModuleConfigurationProvider &configProvider) :
                m_connection(connection),
                m_configurationProvider(configProvider),
                m_descriptorCondition(),
                m_descriptor(),
                m_hasDescriptor(),
                m_pulseAckCondition(),
                m_hasReceivedPulseAck(false),
                m_pulseAckContainersCondition(),
                m_hasReceivedPulseAckContainers(false),
                m_connectionLostMutex(),
                m_connectionLost(false),
                m_stateListener(),
                m_stateListenerMutex(),
                m_containersToBeTransferredToSupercomponent() {
                m_connection->setContainerListener(this);
                m_connection->setErrorListener(this);
                m_connection->start();
            }

            ModuleConnection::~ModuleConnection() {
                m_connection->setContainerListener(NULL);
                m_connection->setErrorListener(NULL);
                m_connection->stop();
            }

            void ModuleConnection::setModuleStateListener(ModuleStateListener* listener) {
                Lock l(m_stateListenerMutex);
                m_stateListener = listener;
            }

            void ModuleConnection::waitForModuleDescription() {
                Lock l(m_descriptorCondition);
                if (!m_hasDescriptor) {
                    m_descriptorCondition.waitOnSignal();
                }
            }

            const ModuleDescriptor ModuleConnection::getModuleDescriptor() const {
                return m_descriptor;
            }

            void ModuleConnection::pulse(const odcore::data::dmcp::PulseMessage &pm) {
                Container c(pm);
                m_connection->send(c);
            }

            void ModuleConnection::pulse_ack(const odcore::data::dmcp::PulseMessage &pm, const uint32_t &timeout) {
                // Unfortunately, we cannot prevent code duplication here (cf. pulse_ack_containers)
                // as in this case, the dependent client module will NOT send its containers to using
                // this TCP link but via the regular UDP multicast conference.
                bool connectionLost = true;

                {
                    Lock l(m_connectionLostMutex);
                    connectionLost = m_connectionLost;
                }

                // Only wait for a confirmation from dependent modules when they are still connected.
                if (!connectionLost) {
                    {
                        Lock l(m_pulseAckCondition);
                        m_hasReceivedPulseAck = false;
                    }

                    Container c(pm);
                    m_connection->send(c);

                    // Wait for the ACK message from client.
                    {
                        Lock l(m_pulseAckCondition);
                        if (!m_hasReceivedPulseAck) {
                            m_pulseAckCondition.waitOnSignalWithTimeout(timeout);
                        }
                    }
                }
            }

            vector<odcore::data::Container> ModuleConnection::pulse_ack_containers(const odcore::data::dmcp::PulseMessage &pm, const uint32_t &timeout) {
                // Unfortunately, we cannot prevent code duplication here (cf. pulse_ack)
                // as in this case, the dependent client module will send all its containers
                // via this TCP link and NOT via the regular UDP multicast conference.

                // Assume that we don't receive any further containers.
                m_containersToBeTransferredToSupercomponent.clear();

                bool connectionLost = true;
                {
                    Lock l(m_connectionLostMutex);
                    connectionLost = m_connectionLost;
                }

                // Only wait for a confirmation from dependent modules when they are still connected.
                if (!connectionLost) {
                    {
                        Lock l(m_pulseAckContainersCondition);
                        m_hasReceivedPulseAckContainers = false;
                    }

                    Container c(pm);
                    m_connection->send(c);

                    // Wait for the ACK message from client.
                    {
                        Lock l(m_pulseAckContainersCondition);
                        if (!m_hasReceivedPulseAckContainers) {
                            m_pulseAckContainersCondition.waitOnSignalWithTimeout(timeout);
                        }
                    }
                }

                return m_containersToBeTransferredToSupercomponent;
            }

            void ModuleConnection::nextContainer(Container &container) {
                if (container.getDataType() == Container::DMCP_CONFIGURATION_REQUEST) {
                    m_descriptor = container.getData<ModuleDescriptor>();
                    {
                        Lock l(m_descriptorCondition);
                        m_hasDescriptor = true;
                        m_descriptorCondition.wakeAll();
                    }

                    KeyValueConfiguration config = m_configurationProvider.getConfiguration(m_descriptor);

                    odcore::data::Configuration conf(config);
                    Container c(conf);
                    m_connection->send(c);
                    return;
                }
                if (container.getDataType() == ModuleStateMessage::ID()) {
                    ModuleStateMessage msg = container.getData<ModuleStateMessage>();

                    Lock l(m_stateListenerMutex);
                    if (m_stateListener) {
                        m_stateListener->handleChangeState(m_descriptor, msg.getModuleState());
                    }
                    return;
                }
                if (container.getDataType() == ModuleExitCodeMessage::ID()) {
                    ModuleExitCodeMessage msg = container.getData<ModuleExitCodeMessage>();

                    Lock l(m_stateListenerMutex);
                    if (m_stateListener) {
                        m_stateListener->handleExitCode(m_descriptor, msg.getModuleExitCode());
                    }
                    return;
                }
                if (container.getDataType() == RuntimeStatistic::ID()) {
                    RuntimeStatistic rs = container.getData<RuntimeStatistic>();

                    Lock l(m_stateListenerMutex);
                    if (m_stateListener) {
                        m_stateListener->handleRuntimeStatistics(m_descriptor, rs);
                    }
                    return;
                }
                if (container.getDataType() == PulseAckMessage::ID()) {
                    Lock l(m_pulseAckCondition);
                    m_hasReceivedPulseAck = true;
                    m_pulseAckCondition.wakeAll();
                    return;
                }
                if (container.getDataType() == PulseAckContainersMessage::ID()) {
                    Lock l(m_pulseAckContainersCondition);
                    m_hasReceivedPulseAckContainers = true;

                    // Get containers to be transferred to supercomponent.
                    PulseAckContainersMessage pac = container.getData<PulseAckContainersMessage>();
                    m_containersToBeTransferredToSupercomponent = pac.getListOfContainers();

                    m_pulseAckContainersCondition.wakeAll();
                    return;
                }

                {
                    Lock l(m_stateListenerMutex);
                    if (m_stateListener) {
                        m_stateListener->handleUnkownContainer(m_descriptor, container);
                    }
                }
            }

            void ModuleConnection::handleConnectionError() {
                // Set connection lost flag.
                {
                    Lock l(m_connectionLostMutex);
                    m_connectionLost = true;
                }

                // Awake any waiting conditions.
                {
                    Lock l(m_pulseAckCondition);
                    m_pulseAckCondition.wakeAll();
                }

                // Awake any waiting conditions.
                {
                    Lock l(m_pulseAckContainersCondition);
                    m_pulseAckContainersCondition.wakeAll();
                }

                // Change module's state.
                {
                    Lock l(m_stateListenerMutex);

                    if (m_stateListener) {
                        m_stateListener->handleConnectionLost(m_descriptor);
                    }
                }
            }

        }
    }
} // odcore::dmcp
