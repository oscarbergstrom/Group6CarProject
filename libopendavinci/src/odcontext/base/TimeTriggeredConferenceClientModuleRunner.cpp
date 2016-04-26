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

#include <cerrno>
#include <iostream>
#include <string>

#include "opendavinci/odcontext/base/BlockableContainerListener.h"
#include "opendavinci/odcontext/base/BlockableContainerReceiver.h"
#include "opendavinci/odcontext/base/ControlledContainerConferenceForSystemUnderTest.h"
#include "opendavinci/odcontext/base/TimeConstants.h"
#include "opendavinci/odcontext/base/TimeTriggeredConferenceClientModuleRunner.h"
#include "opendavinci/odcore/base/Lock.h"
#include "opendavinci/odcore/base/Thread.h"
#include "opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h"
#include "opendavinci/odcore/exceptions/Exceptions.h"
#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/wrapper/Time.h"
#include "opendavinci/generated/odcore/data/dmcp/ModuleExitCodeMessage.h"
#include "opendavinci/generated/odcore/data/dmcp/ModuleStateMessage.h"

namespace odcontext {
    namespace base {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::base::module;
        using namespace odcore::exceptions;

        // If the dynamic cast fails, the user did not call RuntimeControl::setup(RuntimeControl::TAKE_CONTROL).
        TimeTriggeredConferenceClientModuleRunner::TimeTriggeredConferenceClientModuleRunner(TimeTriggeredConferenceClientModule &ttccm) :
            m_timeTriggeredConferenceClientModuleStarted(false),
            m_timeTriggeredConferenceClientModuleFinishedMutex(),
            m_timeTriggeredConferenceClientModuleFinished(false),
            m_timeTriggeredConferenceClientModule(ttccm),
            m_blockableContainerListener(dynamic_cast<ControlledContainerConferenceForSystemUnderTest&>(ttccm.getConference()).getBlockableContainerReceiver()),
            m_runModuleBreakpoint(dynamic_cast<ControlledContainerConferenceForSystemUnderTest&>(ttccm.getConference()).getBlockableContainerReceiver()) {
            ttccm.setBreakpoint(&m_runModuleBreakpoint);
        }

        TimeTriggeredConferenceClientModuleRunner::~TimeTriggeredConferenceClientModuleRunner() {
            // Unlock the app's sending.
            m_blockableContainerListener.setNextContainerAllowed(true);
        }

        float TimeTriggeredConferenceClientModuleRunner::getFrequency() const {
            return m_timeTriggeredConferenceClientModule.getFrequency();
        }

        void TimeTriggeredConferenceClientModuleRunner::step(const odcore::wrapper::Time &t) {
            if (needsExecution(t)) {
                clog << "[APP] at " << t.getSeconds() << "." << t.getPartialMicroseconds() << endl;

                // Start application as independent thread at first call.
                if (!m_timeTriggeredConferenceClientModuleStarted) {
                    start();
                    m_timeTriggeredConferenceClientModuleStarted = true;
                }
                else {
                    // OTHERWISE!!!! continue held execution.
                    m_runModuleBreakpoint.continueExecution();
                }

                // Waiting for breakpoint.
                uint32_t waitingForReachingBreakpoint = 0;
                while (!m_runModuleBreakpoint.hasReached()) {
                    Thread::usleepFor(TimeConstants::ONE_MILLISECOND_IN_MICROSECONDS);
                    waitingForReachingBreakpoint += TimeConstants::ONE_MILLISECOND_IN_MICROSECONDS;

                    if (waitingForReachingBreakpoint > TimeConstants::MAX_WAIT_FOR_REACHING_BREAKPOINT_PER_CYCLE) {
                        stringstream reason;
                        reason << m_timeTriggeredConferenceClientModule.getName() << " is not responding after " << (TimeConstants::MAX_WAIT_FOR_REACHING_BREAKPOINT_PER_CYCLE / TimeConstants::ONE_SECOND_IN_MICROSECONDS) << "s." << endl;

                        // Throw exception to kill ourselves.
                        errno = 0;
                        OPENDAVINCI_CORE_THROW_EXCEPTION(ModulesNotRespondingException, reason.str());
                    }
                }
            }
        }

        void TimeTriggeredConferenceClientModuleRunner::beforeStop() {
            // Stop module.
            m_timeTriggeredConferenceClientModule.setModuleState(odcore::data::dmcp::ModuleStateMessage::NOT_RUNNING);

            // Remove breakpoint.
            m_timeTriggeredConferenceClientModule.setBreakpoint(NULL);

            // Unlock the app's sending since we're leaving.
            m_blockableContainerListener.setNextContainerAllowed(true);

            // Release interrupted application for last time.
            m_runModuleBreakpoint.continueExecution();
        }

        bool TimeTriggeredConferenceClientModuleRunner::hasFinished() const {
            // This method returns true until runModule returned.
            bool retVal = true;
            {
                Lock l(m_timeTriggeredConferenceClientModuleFinishedMutex);
                retVal = m_timeTriggeredConferenceClientModuleFinished;
            }
            return retVal;
        }

        void TimeTriggeredConferenceClientModuleRunner::run() {
            // Allow sending containers for applications which already send containers BEFORE they call getModuleStateAndWaitForRemainingTimeInTimeslice() for the first time.
            m_blockableContainerListener.setNextContainerAllowed(true);

            serviceReady();

            if (m_timeTriggeredConferenceClientModule.runModule() == odcore::data::dmcp::ModuleExitCodeMessage::OKAY) {
                // After finishing the application, report that no more execution is necessary.
                Lock l(m_timeTriggeredConferenceClientModuleFinishedMutex);
                m_timeTriggeredConferenceClientModuleFinished = true;

                // Finally reaching breakpoint (since RuntimeControl is waiting for unlocking its waiting).
                m_runModuleBreakpoint.setFinallyReaching();
            }
            else {
                // Otherwise let RuntimeControl kill us.
            }
        }

    }
} // odcontext::base
