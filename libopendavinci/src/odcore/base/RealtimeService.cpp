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
#include "opendavinci/odcore/base/RealtimeService.h"
#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/wrapper/ConcurrencyFactory.h"
#include "opendavinci/odcore/wrapper/Thread.h"

namespace odcore {
    namespace base {

        using namespace exceptions;

        RealtimeService::RealtimeService(const enum PERIOD &period) throw (ThreadException) :
            RealtimeRunnable(period),
            m_thread(),
            m_realtimeServiceStateMutex(),
            m_realtimeServiceState(INITIALIZED) {
            m_thread = unique_ptr<odcore::wrapper::Thread>(odcore::wrapper::ConcurrencyFactory::createThread(*this));
            if (m_thread.get() == NULL) {
                OPENDAVINCI_CORE_THROW_EXCEPTION(ThreadException, "[core::base::RealtimeService] Thread could not be created!");
            }
        }

        RealtimeService::~RealtimeService() {
            stop();
        }

        void RealtimeService::start() {
            Lock l(m_realtimeServiceStateMutex);
            if (m_realtimeServiceState == INITIALIZED) {
                m_realtimeServiceState = RUNNING;
                m_thread->start();
            }
        }

        void RealtimeService::stop() {
            bool doStop = false;
            {
                Lock l(m_realtimeServiceStateMutex);
                if (m_realtimeServiceState == RUNNING) {
                    m_realtimeServiceState = STOPPED;
                    doStop = true;
                }
            }
            if (doStop) {
                m_thread->stop();
            }
        }

        bool RealtimeService::isRunning() {
            Lock l(m_realtimeServiceStateMutex);
            return (m_realtimeServiceState == RUNNING);
        }

    }
} // odcore::base
