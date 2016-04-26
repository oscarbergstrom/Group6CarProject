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

#ifndef OPENDAVINCI_CORE_WRAPPER_SHAREDMEMORYFACTORYWORKER_H_
#define OPENDAVINCI_CORE_WRAPPER_SHAREDMEMORYFACTORYWORKER_H_

#include "opendavinci/odcore/opendavinci.h"

#include <memory>
#include "opendavinci/odcore/wrapper/SharedMemory.h"
#include "opendavinci/odcore/wrapper/SystemLibraryProducts.h"

namespace odcore {
    namespace wrapper {

        using namespace std;

        /**
         * This template class provides factory methods to the
         * SharedMemoryFactory. The factory methods' implementations
         * for different products have to be defined in specializations
         * of the SharedMemoryFactoryWorker template class.
         *
         * @See SharedMemoryFactory, SharedMemoryFactoryWorker,
         *      SystemLibraryProducts, WIN32SharedMemoryFactoryWorker,
         *      POSIXSharedMemoryFactoryWorker
         */

        template <SystemLibraryProducts product>
        class OPENDAVINCI_API SharedMemoryFactoryWorker {
            public:
                /**
                 * This method returns the shared memory.
                 *
                 * @param name Name of the shared memory to create.
                 * @param size Required size for the new shared memory.
                 * @return Shared memory based on the type of instance this factory is.
                 */
                static std::shared_ptr<SharedMemory> createSharedMemory(const string &name, const uint32_t &size);

                /**
                 * This method returns the shared memory.
                 *
                 * @param name Name of the shared memory to attach.
                 * @return Shared memory based on the type of instance this factory is.
                 */
                static std::shared_ptr<SharedMemory> attachToSharedMemory(const string &name);
        };

    }
} // odcore::wrapper

#endif /*OPENDAVINCI_CORE_WRAPPER_SHAREDMEMORYFACTORYWORKER_H_*/
