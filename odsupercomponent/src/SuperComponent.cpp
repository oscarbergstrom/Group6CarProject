/**
 * odsupercomponent - Configuration and monitoring component for
 *                    distributed software systems
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_LINUX_RT
    #include <sched.h>
    #include <sys/time.h>
    #include <time.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "opendavinci/odcore/opendavinci.h"

#include "opendavinci/odcore/base/CommandLineArgument.h"
#include "opendavinci/odcore/base/CommandLineParser.h"
#include "opendavinci/odcore/base/Lock.h"
#include "opendavinci/odcore/base/Thread.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"
#include "opendavinci/odcore/dmcp/connection/ModuleConnection.h"
#include "opendavinci/odcore/dmcp/connection/Server.h"
#include "opendavinci/odcore/dmcp/discoverer/Client.h"
#include "opendavinci/odcore/dmcp/discoverer/Server.h"
#include "opendavinci/odcore/exceptions/Exceptions.h"
#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "opendavinci/odcore/io/conference/ContainerConferenceFactory.h"
#include "opendavinci/odcore/strings/StringToolbox.h"
#include "opendavinci/generated/odcore/data/dmcp/ModuleDescriptor.h"
#include "opendavinci/generated/odcore/data/dmcp/ModuleStatistic.h"

#include "ConnectedModule.h"
#include "SuperComponent.h"

namespace odsupercomponent {

    using namespace odcore::base;
    using namespace odcore::data;
    using namespace odcore::exceptions;
    using namespace odcore::dmcp;
    using namespace odcore::dmcp::connection;
    using namespace odcore::io;
    using namespace odcore::io::conference;
    using namespace odcore::data::dmcp;

    SuperComponent::SuperComponent(const int &argc, char **argv) :
        MasterModule(argc, argv),
        m_configuration(),
        m_configurationProvider(),
        m_discovererServer(),
        m_connectionServer(NULL),
        m_conference(NULL),
        m_modules(),
        m_shutdownModules(),
        m_moduleStatisticsMapMutex(),
        m_moduleStatisticsMap(),
        m_managedLevel(odcore::data::dmcp::ServerInformation::ML_NONE),
        m_startOfCurrentCycle(),
        m_startOfLastCycle(),
        m_lastCycle(),
        m_lastWaitTime(0),
        m_shiftMicroseconds(0),
        m_timeoutACKMilliseconds(0),
        m_yieldMicroseconds(0),
        m_modulesToIgnore(),
        m_logLevel(odcore::data::LogMessage::NONE),
        m_logFile(NULL) {

        // Check for any running supercomponents.
        checkForSuperComponent();

        // We need to read the configuration file first as the command line parser queries some settings.
        const string configurationFile = checkForCommandLinePassedConfigurationFileOrDefaultConfigurationFile(argc, argv);
        CLOG1 << "[odsupercomponent]: Parsing configuration file '" << configurationFile << "'..." << endl;
        ifstream configStream(configurationFile.c_str(), ios::in);
        if (configStream.good()) {
            m_configuration.readFrom(configStream);
        }
        else {
            OPENDAVINCI_CORE_THROW_EXCEPTION(ConfigurationFileNotFoundException, "Configuration stream invalid.");
        }

        // Parse command line arguments.
        parseAdditionalCommandLineParameters(argc, argv);

        const uint32_t SERVER_PORT = odcore::data::dmcp::Constants::CONNECTIONSERVER_PORT_BASE + getCID();
        // Listen on all interfaces.
        ServerInformation serverInformation("0.0.0.0", SERVER_PORT, m_managedLevel);

        CLOG1 << "[odsupercomponent]: Server information: " << serverInformation.toString() << endl;
        CLOG1 << "[odsupercomponent]: Creating discoverer server..." << endl;
        m_discovererServer = new discoverer::Server(serverInformation,
                                                    getMultiCastGroup(),
                                                    odcore::data::dmcp::Constants::BROADCAST_PORT_SERVER,
                                                    odcore::data::dmcp::Constants::BROADCAST_PORT_CLIENT,
                                                    m_modulesToIgnore);
        m_discovererServer->startResponding();

        CLOG1 << "[odsupercomponent]: Creating connection server..." << endl;
        m_configurationProvider = GlobalConfigurationProvider(m_configuration);
        m_connectionServer = new connection::Server(serverInformation, m_configurationProvider);
        m_connectionServer->setConnectionHandler(this);

        m_conference = std::shared_ptr<ContainerConference>(ContainerConferenceFactory::getInstance().getContainerConference(getMultiCastGroup()));
        m_conference->setContainerListener(this);

        CLOG1 << "[odsupercomponent" << (isRealtime() ? " - real time mode" : "") << "]: Ready - managed level " << m_managedLevel << endl;
    }

    SuperComponent::~SuperComponent() {
        if (m_logFile != NULL) {
            m_logFile->flush();
            m_logFile->close();
        }
        OPENDAVINCI_CORE_DELETE_POINTER(m_logFile);
    }

    const TimeStamp SuperComponent::getStartOfCurrentCycle() const {
        return m_startOfCurrentCycle;
    }

    const TimeStamp SuperComponent::getStartOfLastCycle() const {
        return m_startOfLastCycle;
    }

    const string SuperComponent::checkForCommandLinePassedConfigurationFileOrDefaultConfigurationFile(const int &argc, char **argv) {
        string configurationFile = "configuration";
        CommandLineParser cmdParser;
        cmdParser.addCommandLineArgument("configuration");

        cmdParser.parse(argc, argv);

        CommandLineArgument cmdArgumentCONFIGURATION = cmdParser.getCommandLineArgument("configuration");

        // Check the centrally maintained managed level.
        if (cmdArgumentCONFIGURATION.isSet()) {
            configurationFile = cmdArgumentCONFIGURATION.getValue<string>();
            odcore::strings::StringToolbox::trim(configurationFile);
        }
        return configurationFile;
    }

    void SuperComponent::parseAdditionalCommandLineParameters(const int &argc, char **argv) {
        CommandLineParser cmdParser;
        cmdParser.addCommandLineArgument("managed");
        cmdParser.addCommandLineArgument("logfile");
        cmdParser.addCommandLineArgument("loglevel");

        cmdParser.parse(argc, argv);

        CommandLineArgument cmdArgumentLOGFILE = cmdParser.getCommandLineArgument("logfile");
        CommandLineArgument cmdArgumentLOGLEVEL = cmdParser.getCommandLineArgument("loglevel");
        CommandLineArgument cmdArgumentMANAGED = cmdParser.getCommandLineArgument("managed");

        // Check the log level.
        if (cmdArgumentLOGLEVEL.isSet()) {
            string logLevel = cmdArgumentLOGLEVEL.getValue<string>();

            if (odcore::strings::StringToolbox::equalsIgnoreCase(logLevel, "none")) {
                m_logLevel = odcore::data::LogMessage::NONE;
            }
            if (odcore::strings::StringToolbox::equalsIgnoreCase(logLevel, "info")) {
                m_logLevel = odcore::data::LogMessage::INFO;
            }
            if (odcore::strings::StringToolbox::equalsIgnoreCase(logLevel, "warn")) {
                m_logLevel = odcore::data::LogMessage::WARN;
            }
            if (odcore::strings::StringToolbox::equalsIgnoreCase(logLevel, "debug")) {
                m_logLevel = odcore::data::LogMessage::DEBUG;
            }
        }

        // Create a log file.
        if (cmdArgumentLOGFILE.isSet()) {
            string logFilename = cmdArgumentLOGFILE.getValue<string>();

            if (m_logLevel > 0) {
                OPENDAVINCI_CORE_DELETE_POINTER(m_logFile);

                m_logFile = new fstream();
                if (m_logFile != NULL) {
                    m_logFile->open(logFilename.c_str(), ios::out | ios::app);

                    if ((m_logFile != NULL) && !m_logFile->good()) {
                        OPENDAVINCI_CORE_DELETE_POINTER(m_logFile);
                    }
                }
            }
        }

        // Check the centrally maintained managed level.
        if (cmdArgumentMANAGED.isSet()) {
            string managedLevel = cmdArgumentMANAGED.getValue<string>();
            odcore::strings::StringToolbox::trim(managedLevel);

            if (odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "none")) {
                m_managedLevel = odcore::data::dmcp::ServerInformation::ML_NONE;
            }
            if (odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "pulse")) {
                m_managedLevel = odcore::data::dmcp::ServerInformation::ML_PULSE;
            }
            if (odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "pulse_shift")) {
                m_managedLevel = odcore::data::dmcp::ServerInformation::ML_PULSE_SHIFT;

                m_shiftMicroseconds = 10 * 1000;
                try {
                    m_shiftMicroseconds = m_configuration.getValue<uint32_t>("odsupercomponent.pulseshift.shift");
                }
                catch(...) {
                    CLOG1 << "[odsupercomponent]: Value for 'odsupercomponent.pulseshift.shift' not found in configuration, using " << m_shiftMicroseconds << " as default." << endl;
                }
            }
            if (odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "pulse_time")) {
                m_managedLevel = odcore::data::dmcp::ServerInformation::ML_PULSE_TIME;
            }
            if (odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "pulse_time_ack") || odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "simulation") || odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "simulation_rt")) {
                if (odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "pulse_time_ack")) {
                    m_managedLevel = odcore::data::dmcp::ServerInformation::ML_PULSE_TIME_ACK;
                }
                if (odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "simulation")) {
                    m_managedLevel = odcore::data::dmcp::ServerInformation::ML_SIMULATION;
                }
                if (odcore::strings::StringToolbox::equalsIgnoreCase(managedLevel, "simulation_rt")) {
                    m_managedLevel = odcore::data::dmcp::ServerInformation::ML_SIMULATION_RT;
                }

                m_timeoutACKMilliseconds = 1000;
                m_yieldMicroseconds = 5 * 1000;

                try {
                    m_timeoutACKMilliseconds = m_configuration.getValue<uint32_t>("odsupercomponent.pulsetimeack.timeout");
                }
                catch(...) {
                    CLOG1 << "[odsupercomponent]: Value for 'odsupercomponent.pulsetimeack.timeout' not found in configuration, using " << m_timeoutACKMilliseconds << " as default." << endl;
                }

                try {
                    m_yieldMicroseconds = m_configuration.getValue<uint32_t>("odsupercomponent.pulsetimeack.yield");
                }
                catch(...) {
                    CLOG1 << "[odsupercomponent]: Value for 'odsupercomponent.pulsetimeack.yield' not found in configuration, using " << m_yieldMicroseconds << " as default." << endl;
                }

                try {
                    string s = m_configuration.getValue<string>("odsupercomponent.pulsetimeack.exclude");
                    transform(s.begin(), s.end(), s.begin(), ::tolower);

                    m_modulesToIgnore = odcore::strings::StringToolbox::split(s, ',');
                }
                catch(...) {
                    // If "odsupercomponent.pulsetimeack.exclude" is not specified, just ignore exception.
                }

            }
        }
    }

    void SuperComponent::checkForSuperComponent() {
        string noName = "";
        discoverer::Client discovererClient(getMultiCastGroup(),
                                            odcore::data::dmcp::Constants::BROADCAST_PORT_SERVER,
                                            odcore::data::dmcp::Constants::BROADCAST_PORT_CLIENT,
                                            noName);

         if ( discovererClient.existsServer() ) {
             CLOG1 << "[odsupercomponent]: odsupercomponent already running for " << getMultiCastGroup() << endl;
             exit(-1);
         }
    }

    odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode SuperComponent::body() {
#ifdef HAVE_LINUX_RT
        struct timespec waitForSlice;
        if (isRealtime()) {
            // Setup realtime task using FIFO scheduling.
            struct sched_param param;
            param.sched_priority = getRealtimePriority();

            if (::sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
                OPENDAVINCI_CORE_THROW_EXCEPTION(InvalidArgumentException,
                                              "Failed to configure scheduler. Are you superuser?");
            }

            // Get actual time.
            ::clock_gettime(CLOCK_REALTIME , &waitForSlice);

            // Take the next time slice during initialization phase to ensure that this thread will start at after a completed second.
            waitForSlice.tv_sec += 1;
            waitForSlice.tv_nsec = 0;

            // Wait for next time slice.
            ::clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waitForSlice, NULL);
        }
#endif

        uint32_t cumulatedTimeSlice = 0;
        const long ONE_SECOND_IN_MICROSECONDS = 1000 * 1000 * 1;

        vector<Container> containersToBeDistributedToModules;

        m_lastCycle = TimeStamp();
        while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
            TimeStamp current;
            m_startOfCurrentCycle = current;
            m_startOfLastCycle = m_lastCycle;

            {
                // Update statistics.
                Lock l(m_moduleStatisticsMapMutex);
                ModuleStatistics ms;
                auto it = m_moduleStatisticsMap.cbegin();
                while (it != m_moduleStatisticsMap.cend()) {
                    ms.addTo_ListOfModuleStatistics(it->second);
                    it++;
                }
                Container c(ms);
                m_conference->send(c);
            }

            // Remove disconnected modules.
            m_shutdownModules.deleteAllModules();

            if (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_NONE) {
                // Just sleep 2 seconds in the case of managed level == NONE.
                Thread::usleepFor(2 * ONE_SECOND_IN_MICROSECONDS);
            }
            else if ( (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_PULSE) ||
                      (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_PULSE_SHIFT) ||
                      (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_PULSE_TIME) ||
                      (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_PULSE_TIME_ACK) ||
                      (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_SIMULATION) ||
                      (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_SIMULATION_RT) ) {

                const float FREQ = getFrequency();
                const long TIME_CONSUMPTION_OF_CURRENT_SLICE = (current.toMicroseconds() - m_lastCycle.toMicroseconds()) - m_lastWaitTime;
                m_lastCycle = current;
                const long NOMINAL_DURATION_OF_ONE_SLICE = static_cast<long>((1.0f/FREQ) * ONE_SECOND_IN_MICROSECONDS);
                long WAITING_TIME_OF_CURRENT_SLICE = NOMINAL_DURATION_OF_ONE_SLICE - TIME_CONSUMPTION_OF_CURRENT_SLICE;

                CLOG2 << "[odsupercomponent]: Waiting real slice: " << WAITING_TIME_OF_CURRENT_SLICE << ", nominal waiting slice: " << NOMINAL_DURATION_OF_ONE_SLICE << ", cumulated time slice: " << cumulatedTimeSlice << endl;

                const TimeStamp supercomponent_now;
                const vector<Container> EMPTY_LIST_OF_CONTAINERS;
                PulseMessage pm(supercomponent_now, NOMINAL_DURATION_OF_ONE_SLICE, cumulatedTimeSlice, EMPTY_LIST_OF_CONTAINERS);

                if (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_PULSE) {
                    m_modules.pulse(pm);
                }
                else if (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_PULSE_SHIFT) {
                    m_modules.pulseShift(pm, m_shiftMicroseconds);
                }
                else if (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_PULSE_TIME) {
                    // Managed level ML_PULSE_TIME is handled by the dependent modules and thus, the regular pulse method can be called.
                    m_modules.pulse(pm);
                }
                else if (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_PULSE_TIME_ACK) {
                    // Managed level ML_PULSE_TIME_ACK requires a confirmation from the dependent modules
                    // that the received PULSE has been processed.
                    //
                    // m_yieldMicroseconds specifies the amount of time that we are going to wait before
                    // we trigger the next module (send the pulse to it) to allow delivery of any packets
                    // on the OS level.
                    m_modules.pulse_ack(pm, m_timeoutACKMilliseconds, m_yieldMicroseconds, m_modulesToIgnore);
                }
                else if ( (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_SIMULATION) || (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_SIMULATION_RT) ) {
                    // Managed level ML_SIMULATION requires a confirmation from the dependent
                    // modules that the received PULSE has been processed and the connected modules need
                    // to return all Containers to be distributed to the connected modules in the next
                    // call cycle.
                    //
                    // m_yieldMicroseconds specifies the amount of time that we are going to wait before
                    // we trigger the next module (send the pulse to it) to allow delivery of any packets
                    // on the OS level.

                    // Set containers to be delivered to the connected modules.
                    pm.setListOfContainers(containersToBeDistributedToModules);

                    // Replicate containers to real UDP conference for modules that are excluded from the ML.
                    vector<Container>::iterator it = containersToBeDistributedToModules.begin();
                    while (it != containersToBeDistributedToModules.end()) {
                        m_conference->send(*it);
                        it++;
                        Thread::usleepFor(500);
                    }

                    // Clear containers from last cycle.
                    containersToBeDistributedToModules.clear();

                    // Save containers to be distributed in the next cycle.
                    containersToBeDistributedToModules = m_modules.pulse_ack_containers(pm, m_timeoutACKMilliseconds, m_yieldMicroseconds, m_modulesToIgnore);
                }

                // Increment the nomimal time slices.
                cumulatedTimeSlice = (cumulatedTimeSlice + NOMINAL_DURATION_OF_ONE_SLICE) % ONE_SECOND_IN_MICROSECONDS;

                // Ensure that we are not waiting in the case that we needed more than one second for this execution cycle.
                if (TIME_CONSUMPTION_OF_CURRENT_SLICE > ONE_SECOND_IN_MICROSECONDS) {
                    WAITING_TIME_OF_CURRENT_SLICE = 0;
                }
                m_lastWaitTime = WAITING_TIME_OF_CURRENT_SLICE;

                // Check if we really need to artificially consume this time slice in real time or if we can run as fast as possible.
                if (m_managedLevel == odcore::data::dmcp::ServerInformation::ML_SIMULATION) {
                    // We can run as fast as possible but we need to allow some scheduling for the connected modules.
                    Thread::usleepFor(1000);
                }
                else {
                    if (isRealtime()) {
#ifdef HAVE_LINUX_RT
                        // Constants in nanoseconds.
                        const long NANOSECOND = 1;
                        const long MICROSECOND = 1000 * NANOSECOND;  // 1000 nanoseconds are one microsecond.
                        const long MILLISECOND = 1000 * MICROSECOND; // 1000 microseconds are one millisecond.
                        const long SECOND = 1000 * MILLISECOND;      // 1000 milliseconds are one second.

                        waitForSlice.tv_nsec += (NOMINAL_DURATION_OF_ONE_SLICE * MICROSECOND);
                        while (waitForSlice.tv_nsec >= SECOND) {
                            waitForSlice.tv_nsec -= SECOND;
                            waitForSlice.tv_sec++;
                        }
                        // Wait for next time slice with wall clock.
                        ::clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waitForSlice, NULL);
#else
                        Thread::usleepFor(WAITING_TIME_OF_CURRENT_SLICE);
#endif
                    }
                    else {
                        Thread::usleepFor(WAITING_TIME_OF_CURRENT_SLICE);
                    }
                }
            }
        }

        m_conference->setContainerListener(NULL);

        // Clean up connections.
        CLOG1 << "[odsupercomponent]: Closing down... ";
        delete m_connectionServer;
        m_connectionServer = NULL;

        delete m_discovererServer;
        m_discovererServer = NULL;

        m_modules.deleteAllModules();

        cout << "done." << endl;

        return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
    }

    void SuperComponent::onNewModule(std::shared_ptr<odcore::dmcp::connection::ModuleConnection> mc) {
        if (mc.get()) {
            mc->waitForModuleDescription();
            CLOG1 << "[odsupercomponent]: New connected module " << mc->getModuleDescriptor().toString() << endl;

            ConnectedModule* module = new ConnectedModule(mc, odcore::data::dmcp::ModuleStateMessage::NOT_RUNNING);
            mc->setModuleStateListener(this);

            if (m_modules.hasModule(mc->getModuleDescriptor())) {
                clog << "[odsupercomponent]: WARNING!!! MODULE " << mc->getModuleDescriptor().toString() << " AREADY CONNECTED: Consider using --id to differentiate modules!" << endl;
            }
            else {
                m_modules.addModule(mc->getModuleDescriptor(), module);
            }
        }
    }

    void SuperComponent::handleChangeState(const ModuleDescriptor& md, const odcore::data::dmcp::ModuleStateMessage::ModuleState &ms) {
        string state;
        switch (ms) {
            case odcore::data::dmcp::ModuleStateMessage::NOT_RUNNING: state = "NOT_RUNNING"; break;
            case odcore::data::dmcp::ModuleStateMessage::RUNNING: state = "RUNNING"; break;
            case odcore::data::dmcp::ModuleStateMessage::UNDEFINED_STATE: state = "UNDEFINED_STATE"; break;
        }

        CLOG1 << "[odsupercomponent]: Module " << md.toString() << " changed state to " << state << endl;

        if (m_modules.hasModule(md)) {
            m_modules.getModule(md)->setState(ms);
        }
    }

    void SuperComponent::handleExitCode(const ModuleDescriptor& md, const odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode &me) {
        string exitcode;
        switch (me) {
            case odcore::data::dmcp::ModuleExitCodeMessage::OKAY: exitcode = "OKAY"; break;
            case odcore::data::dmcp::ModuleExitCodeMessage::EXCEPTION_CAUGHT: exitcode = "EXCEPTION_CAUGHT"; break;
            case odcore::data::dmcp::ModuleExitCodeMessage::SERIOUS_ERROR: exitcode = "SERIOUS_ERROR"; break;
            case odcore::data::dmcp::ModuleExitCodeMessage::CONNECTION_LOST: exitcode = "CONNECTION_LOST"; break;
            case odcore::data::dmcp::ModuleExitCodeMessage::NO_SUPERCOMPONENT: exitcode = "NO_SUPERCOMPONENT"; break;
            case odcore::data::dmcp::ModuleExitCodeMessage::UNDEFINED_EXITCODE: exitcode = "UNDEFINED_EXITCODE"; break;
        }

        CLOG1 << "[odsupercomponent]: Module " << md.toString() << " shutdown with exitcode " << exitcode << endl;

        if (m_modules.hasModule(md)) {
            ConnectedModule* module = m_modules.getModule(md);
            m_modules.removeModule(md);
            module->setExitCode();
            m_shutdownModules.addModule(md, module);
        }

        // Remove module from RuntimeStatistics after module shutdown.
        {
            Lock l(m_moduleStatisticsMapMutex);

            stringstream sstr;
            sstr << md.getName() << "-" << md.getIdentifier();

            auto it = m_moduleStatisticsMap.find(sstr.str());
            if (it != m_moduleStatisticsMap.end()) {
                m_moduleStatisticsMap.erase(it);
            }
        }
    }

    void SuperComponent::handleRuntimeStatistics(const ModuleDescriptor& md, const odcore::data::dmcp::RuntimeStatistic& rs) {
        Lock l(m_moduleStatisticsMapMutex);

        ModuleStatistic entry(md, rs);

        stringstream sstr;
        sstr << md.getName() << "-" << md.getIdentifier();

        m_moduleStatisticsMap[sstr.str()] = entry;
    }

    void SuperComponent::handleConnectionLost(const ModuleDescriptor& md) {
        // This methods is called when a module terminates not properly.
        if (m_modules.hasModule(md)) {
            CLOG1 << "[odsupercomponent]: Connection to module " << md.toString() << " lost" << endl;
            ConnectedModule* module = m_modules.getModule(md);
            m_modules.removeModule(md);
            m_shutdownModules.addModule(md, module);

            // Remove module from RuntimeStatistics.
            {
                Lock l(m_moduleStatisticsMapMutex);

                stringstream sstr;
                sstr << md.getName() << "-" << md.getIdentifier();

                auto it = m_moduleStatisticsMap.find(sstr.str());
                if (it != m_moduleStatisticsMap.end()) {
                    m_moduleStatisticsMap.erase(it);
                }
            }
        }
        else {
            if (m_shutdownModules.hasModule(md)) {
                if (!m_shutdownModules.getModule(md)->hasExitCode()) {
                    clog << "[odsupercomponent]: Something went wrong...lost connection to module " << md.toString();
                    clog << ", module is in shutdownModule list but has not set an ExitCode." << endl;
                }
            }
        }
    }

    void SuperComponent::handleUnkownContainer(const ModuleDescriptor& md, const Container& container) {
        CLOG1 << "[odsupercomponent]: Received unknown container " << container.toString() << "from " << md.toString() << endl;
    }

    void SuperComponent::nextContainer(Container &container) {
        if (container.getDataType() == odcore::data::LogMessage::ID()) {
            odcore::data::LogMessage logMessage = container.getData<odcore::data::LogMessage>();

            if (m_logLevel >= logMessage.getLogLevel()) {
                if (m_logFile != NULL) {
                    (*m_logFile) << TimeStamp().toMicroseconds() << ";"
                                 << logMessage.getComponentName() << ";"
                                 << logMessage.getLogLevel() << ";"
                                 << logMessage.getLogMessage() << ";"
                                 << endl;
                    m_logFile->flush();
                }
            }
        }
    }
    
} // odsupercomponent
