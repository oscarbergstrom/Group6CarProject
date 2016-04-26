/**
 * odcanproxymapper - Tool combining canproxy and canmapper
 * Copyright (C) 2015 Christian Berger
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

#include <vector>

#include "CANProxyMapper.h"
#include "opendavinci/odcore/base/Thread.h"
#include "opendavinci/odcore/data/Container.h"

namespace automotive { class GenericCANMessage; }

namespace automotive {
    namespace odcantools {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;

        CANProxyMapper::CANProxyMapper(const int32_t &argc, char **argv) :
            CANProxy(argc, argv),
            m_canMapping() {}

        CANProxyMapper::~CANProxyMapper() {}

        void CANProxyMapper::nextGenericCANMessage(const GenericCANMessage &gcm) {
            // Try to get complete message with this additional information.
            vector<Container> listOfContainers = m_canMapping.mapNext(gcm);
            if (listOfContainers.size() > 0) {
                vector<Container>::iterator it = listOfContainers.begin();
                while (it != listOfContainers.end()) {
                    Container container = (*it++);
                    getConference().send(container);
                    Thread::usleepFor(100);
                }
            }
        }

    } // odcantools
} // automotive
