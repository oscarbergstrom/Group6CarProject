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

#ifndef CONTEXT_BASE_TIMETRIGGEREDCONFERENCECLIENTMODULERUNNER_H_
#define CONTEXT_BASE_TIMETRIGGEREDCONFERENCECLIENTMODULERUNNER_H_

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/base/Mutex.h"
#include "opendavinci/odcore/base/Service.h"
#include "opendavinci/odcontext/base/RunModuleBreakpoint.h"
#include "opendavinci/odcontext/base/Runner.h"

namespace odcore { namespace base { namespace module { class TimeTriggeredConferenceClientModule; } } }
namespace odcore { namespace wrapper { class Time; } }

namespace odcontext {
    namespace base {

class BlockableContainerListener;

        using namespace std;

        /**
         * This class encapsulates a given TimeTriggeredConferenceClientModule into an
         * independent thread which is controlled by the given breakpoint created by
         * this class.
         */
        class OPENDAVINCI_API TimeTriggeredConferenceClientModuleRunner : public Runner, public odcore::base::Service {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 */
                TimeTriggeredConferenceClientModuleRunner(const TimeTriggeredConferenceClientModuleRunner&);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 */
                TimeTriggeredConferenceClientModuleRunner& operator=(const TimeTriggeredConferenceClientModuleRunner&);

            public:
                /**
                 * Constructor.
                 *
                 * @param ttccm TimeTriggeredConferenceClientModule which is to be executed.
                 */
                TimeTriggeredConferenceClientModuleRunner(odcore::base::module::TimeTriggeredConferenceClientModule &ttccm);

                virtual ~TimeTriggeredConferenceClientModuleRunner();

                /**
                 * This method actually performs a step (i.e. executes
                 * exactly one cycle between to consecutive getModuleStateAndWaitForRemainingTimeInTimeslice()-
                 * calls).
                 *
                 * @param t Time.
                 */
                virtual void step(const odcore::wrapper::Time &t);

                virtual bool hasFinished() const;

            protected:
                virtual void beforeStop();

                virtual void run();

            private:
                virtual float getFrequency() const;

            private:
                bool m_timeTriggeredConferenceClientModuleStarted;

                mutable odcore::base::Mutex m_timeTriggeredConferenceClientModuleFinishedMutex;
                bool m_timeTriggeredConferenceClientModuleFinished;

                odcore::base::module::TimeTriggeredConferenceClientModule &m_timeTriggeredConferenceClientModule;
                BlockableContainerListener &m_blockableContainerListener;
                RunModuleBreakpoint m_runModuleBreakpoint;
        };

    }
} // odcontext::base

#endif /*CONTEXT_BASE_TIMETRIGGEREDCONFERENCECLIENTMODULERUNNER_H_*/
