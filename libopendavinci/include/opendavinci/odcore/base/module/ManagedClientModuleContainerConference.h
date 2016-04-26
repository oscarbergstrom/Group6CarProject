/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2014 - 2015 Christian Berger
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

#ifndef OPENDAVINCI_BASE_MANAGEDCLIENTMODULECONTAINERCONFERENCE_H_
#define OPENDAVINCI_BASE_MANAGEDCLIENTMODULECONTAINERCONFERENCE_H_

#include <vector>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/io/conference/ContainerConference.h"

namespace odcore {
    namespace base {
        namespace module {

            using namespace std;

            /**
             * This class is a software only ContainerConference as a substitute
             * to the UDP-driver ContainerConference.
             */
            class OPENDAVINCI_API ManagedClientModuleContainerConference : public odcore::io::conference::ContainerConference {
                private:
                    /**
                     * "Forbidden" copy constructor. Goal: The compiler should warn
                     * already at compile time for unwanted bugs caused by any misuse
                     * of the copy constructor.
                     */
                    ManagedClientModuleContainerConference(const ManagedClientModuleContainerConference&);

                    /**
                     * "Forbidden" assignment operator. Goal: The compiler should warn
                     * already at compile time for unwanted bugs caused by any misuse
                     * of the assignment operator.
                     */
                    ManagedClientModuleContainerConference& operator=(const ManagedClientModuleContainerConference&);

                public:
                    ManagedClientModuleContainerConference();

                    virtual ~ManagedClientModuleContainerConference();

                    virtual void send(odcore::data::Container &container) const;

                    void receiveFromLocal(odcore::data::Container &c);

                    /**
                     * This message clears the list of containers to be transferred from a connected
                     * module to supercomponent.
                     */
                    void clearListOfContainers();

                    /**
                     * This message returns the list of containers to be transferred from a connected
                     * module to supercomponent.
                     *
                     * @return List of containers to be transferred to supercomponent.
                     */
                    vector<odcore::data::Container> getListOfContainers() const;

                private:
                    vector<odcore::data::Container> m_listOfContainersToBeDelivered;
            };

        }
    }
} // odcore::base::module

#endif /*OPENDAVINCI_BASE_MANAGEDCLIENTMODULECONTAINERCONFERENCE_H_*/
