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

#ifndef CONTEXT_BASE_CONTROLLEDCONTAINERCONFERENCEFORSYSTEMUNDERTEST_H_
#define CONTEXT_BASE_CONTROLLEDCONTAINERCONFERENCEFORSYSTEMUNDERTEST_H_

#include <string>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "opendavinci/odcore/io/conference/ContainerListener.h"

namespace odcore { namespace data { class Container; } }
namespace odcore { namespace io { namespace conference { class ContainerObserver; } } }

namespace odcontext {
    namespace base {

class BlockableContainerReceiver;

        using namespace std;


        /**
         * This class provides a controlled container conference.
         */
        class OPENDAVINCI_API ControlledContainerConferenceForSystemUnderTest : public odcore::io::conference::ContainerConference, public odcore::io::conference::ContainerListener {
            private:
                friend class ControlledContainerConferenceFactory;

            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 */
                ControlledContainerConferenceForSystemUnderTest(const ControlledContainerConferenceForSystemUnderTest &);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 */
                ControlledContainerConferenceForSystemUnderTest& operator=(const ControlledContainerConferenceForSystemUnderTest &);

            protected:
                /**
                 * Constructor.
                 *
                 * @param address Create controlled container conference for this address.
                 * @param port Create controlled container conference for this port.
                 * @param bcl BlockableContainerListener to which we send our Containers.
                 * @param receiveFromObserver ContainerObserver which delivers incoming Containers to us.
                 */
                ControlledContainerConferenceForSystemUnderTest(const string &address, const uint32_t &port, BlockableContainerReceiver &bcl, odcore::io::conference::ContainerObserver &receiveFromObserver);

            public:
                virtual ~ControlledContainerConferenceForSystemUnderTest();

                virtual void send(odcore::data::Container &container) const;

                virtual void nextContainer(odcore::data::Container &c);

                /**
                 * This method returns the BlockableContainerReceiver.
                 *
                 * @return BlockableContainerReceiver to which we send our data.
                 */
                BlockableContainerReceiver& getBlockableContainerReceiver();

            private:
                BlockableContainerReceiver &m_sendToListener;
                odcore::io::conference::ContainerObserver &m_receiveFromObserver;
        };

    }
} // odcontext::base

#endif /*CONTEXT_BASE_CONTROLLEDCONTAINERCONFERENCEFORSYSTEMUNDERTEST_H_*/
