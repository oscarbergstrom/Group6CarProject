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
#include "opendavinci/odcore/io/StringSender.h"
#include "opendavinci/odcore/io/protocol/AbstractProtocol.h"

namespace odcore {
    namespace io {
        namespace protocol {

            using namespace odcore::base;

            AbstractProtocol::AbstractProtocol() :
                m_stringSenderMutex(),
                m_stringSender(NULL) {}

            AbstractProtocol::~AbstractProtocol() {
                setStringSender(NULL);
            }

            void AbstractProtocol::setStringSender(StringSender* sender) {
                Lock l(m_stringSenderMutex);
                m_stringSender = sender;
            }

            void AbstractProtocol::sendByStringSender(const string &data) {
                if (data.length() > 0) {
                    Lock l(m_stringSenderMutex);
                    if (m_stringSender != NULL) {
                        m_stringSender->send(data);
                    }
                }
            }

        }
    }
}

