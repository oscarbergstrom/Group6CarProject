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

#ifndef CORE_SHAREDMEMORYTESTSUITE_H_
#define CORE_SHAREDMEMORYTESTSUITE_H_

#include <iosfwd>                       // for stringstream, istream, etc
#include <string>                       // for operator==, basic_string

#include "cxxtest/TestSuite.h"          // for TS_ASSERT, TestSuite

#include "opendavinci/odcore/opendavinci.h"
#include <memory>
#include "opendavinci/odcore/base/Lock.h"             // for Lock
#include "opendavinci/odcore/base/Serializable.h"     // for operator<<, operator>>
#include "opendavinci/odcore/wrapper/SharedMemory.h"  // for SharedMemory
#include "opendavinci/odcore/wrapper/SharedMemoryFactory.h"  // for SharedMemoryFactory
#include "opendavinci/generated/odcore/data/SharedData.h"  // for SharedData

using namespace std;

class SharedMemoryTest : public CxxTest::TestSuite {
    public:
        void testSharedData1() {
            odcore::data::SharedData sd;

            stringstream sstr;
            sstr << sd;

            odcore::data::SharedData sd2;
            sstr >> sd2;
            TS_ASSERT(sd2.getName() == "");
            TS_ASSERT(sd2.getSize() == 0);
        }

        void testSharedData2() {
            odcore::data::SharedData sd("abc", 10);

            stringstream sstr;
            sstr << sd;

            odcore::data::SharedData sd2;
            sstr >> sd2;
            TS_ASSERT(sd2.getName() == "abc");
            TS_ASSERT(sd2.getSize() == 10);
        }

        void testSharedData3() {
            odcore::data::SharedData sd;
            sd.setName("def");
            sd.setSize(22);

            stringstream sstr;
            sstr << sd;

            odcore::data::SharedData sd2;
            sstr >> sd2;
            TS_ASSERT(sd2.getName() == "def");
            TS_ASSERT(sd2.getSize() == 22);
        }

        void testSharedMemory() {
            std::shared_ptr<odcore::wrapper::SharedMemory> memClient;

            std::shared_ptr<odcore::wrapper::SharedMemory> memServer = odcore::wrapper::SharedMemoryFactory::createSharedMemory("SharedMemoryTest", 10);
            TS_ASSERT(memServer->isValid());
            TS_ASSERT(memServer->getSize() == 10);
            memServer->lock();
            for (uint32_t i = 0; i < memServer->getSize(); i++) {
                *(((char*)(memServer->getSharedMemory())) + i) = ('A' + i);
            }
            memServer->unlock();

            TS_ASSERT(!memClient.get());

            memClient = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory("SharedMemoryTest");
            TS_ASSERT(memClient->isValid());
            TS_ASSERT(memClient->getSize() == 10);
            memClient->lock();
            for (uint32_t i = 0; i < memClient->getSize(); i++) {
                char c = *(((char*)(memClient->getSharedMemory())) + i);
                TS_ASSERT(c == (char)('A' + i));
            }
            memClient->unlock();
        }

        void testSharedMemoryWithScopedLock() {
            std::shared_ptr<odcore::wrapper::SharedMemory> memClient;

            std::shared_ptr<odcore::wrapper::SharedMemory> memServer = odcore::wrapper::SharedMemoryFactory::createSharedMemory("SharedMemoryTest", 10);
            TS_ASSERT(memServer->isValid());
            TS_ASSERT(memServer->getSize() == 10);
            {
                odcore::base::Lock l(memServer);

                for (uint32_t i = 0; i < memServer->getSize(); i++) {
                    *(((char*)(memServer->getSharedMemory())) + i) = ('A' + i);
                }
            }

            TS_ASSERT(!memClient.get());

            memClient = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory("SharedMemoryTest");
            TS_ASSERT(memClient->isValid());
            TS_ASSERT(memClient->getSize() == 10);
            {
                odcore::base::Lock l(memClient);
                for (uint32_t i = 0; i < memClient->getSize(); i++) {
                    char c = *(((char*)(memClient->getSharedMemory())) + i);
                    TS_ASSERT(c == (char)('A' + i));
                }
            }
        }

};

#endif /*CORE_SHAREDMEMORYTESTSUITE_H_*/
