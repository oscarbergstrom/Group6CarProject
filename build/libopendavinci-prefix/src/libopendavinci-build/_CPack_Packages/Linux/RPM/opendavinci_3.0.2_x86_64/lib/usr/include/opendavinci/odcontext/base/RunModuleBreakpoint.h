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

#ifndef CONTEXT_BASE_RUNMODULEBREAKPOINT_H_
#define CONTEXT_BASE_RUNMODULEBREAKPOINT_H_

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/base/Mutex.h"
#include "opendavinci/odcore/base/module/Breakpoint.h"

namespace odcontext {
    namespace base {

class BlockableContainerListener;

        /**
         * This class provides access to the SystemContext using elsewhere
         * prepared data.
         */
        class OPENDAVINCI_API RunModuleBreakpoint : public odcore::base::module::Breakpoint {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 */
                RunModuleBreakpoint(const RunModuleBreakpoint&);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 */
                RunModuleBreakpoint& operator=(const RunModuleBreakpoint&);

            public:
                /**
                 * Constructor.
                 *
                 * @param bcl BlockableContainerListener.
                 */
                RunModuleBreakpoint(BlockableContainerListener &bcl);

                virtual ~RunModuleBreakpoint();

                virtual void reached();

                /**
                 * This method returns true if the breakpoint was reached.
                 *
                 * @return true if the breakpoint was reached.
                 */
                bool hasReached() const;

                /**
                 * This method continues the application's execution.
                 */
                void continueExecution();

                /**
                 * This method finally unlocks this breakpoint. This
                 * method is intended to be used by TimeTriggeredConferenceClientModuleRunner
                 * to unlock the waiting RuntimeControl!
                 */
                void setFinallyReaching();

            private:
                /**
                 * This method returns true if the application can continue
                 * to execute.
                 *
                 * @return true if continue.
                 */
                bool hasContinue() const;

            private:
                BlockableContainerListener &m_blockableContainerListener;

                mutable odcore::base::Mutex m_reachedMutex;
                bool m_reached;

                mutable odcore::base::Mutex m_continueMutex;
                bool m_continue;
        };

    }
} // odcontext::base

#endif /*CONTEXT_BASE_RUNMODULEBREAKPOINT_H_*/
