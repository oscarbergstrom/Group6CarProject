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

#include "opendavinci/odcore/base/Lock.h"
#include "opendavinci/odcore/base/Thread.h"
#include "opendavinci/odcore/base/module/Breakpoint.h"
#include "opendavinci/odcore/base/module/InterruptibleModule.h"

namespace odcore {
    namespace base {
        namespace module {

            using namespace std;
            using namespace odcore::base;

            InterruptibleModule::InterruptibleModule() :
                    m_breakpointMutex(),
                    m_breakpoint(NULL) {}

            InterruptibleModule::~InterruptibleModule() {}

            void InterruptibleModule::setBreakpoint(Breakpoint *bp) {
                Lock l(m_breakpointMutex);
                m_breakpoint = bp;
            }

            void InterruptibleModule::calledGetModuleState() {
                Breakpoint *bp = NULL;

                // Check if it's necessary to call the breakpoint.
                {
                    Lock l(m_breakpointMutex);
                    bp = m_breakpoint;
                }

                // Reach breakpoint.
                if (bp != NULL) {
                    bp->reached();

                    // Yielding other threads to deliver containers.
                    Thread::usleepFor(100);
                }
                else {
                    // Yielding other threads and adjust to a fixed frequency if no breakpoint is set.
                    wait();
                }
            }

        }
    }
} // odcore::base
