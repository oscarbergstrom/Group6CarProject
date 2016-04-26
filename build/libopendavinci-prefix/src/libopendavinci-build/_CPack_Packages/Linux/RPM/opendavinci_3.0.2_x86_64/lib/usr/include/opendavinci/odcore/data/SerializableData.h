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

#ifndef OPENDAVINCI_CORE_DATA_SERIALIZABLEDATA_H_
#define OPENDAVINCI_CORE_DATA_SERIALIZABLEDATA_H_

#include <string>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/base/Serializable.h"

namespace odcore {
    namespace data {

        using namespace std;

        /**
         * This class is the superclass for all data.
         */
        class OPENDAVINCI_API SerializableData : public odcore::base::Serializable {
            public:
                SerializableData();

                virtual ~SerializableData();

                /**
                 * This method returns the message ID.
                 *
                 * @return Message identifier.
                 */
                virtual int32_t getID() const = 0;

                /**
                 * This method returns the message name.
                 *
                 * @return Message name.
                 */
                virtual const string getShortName() const = 0;

                /**
                 * This method returns the message name including namespace.
                 *
                 * @return Message name including namespace.
                 */
                virtual const string getLongName() const = 0;

                /**
                 * This method returns a human readable format
                 * of the contained data.
                 *
                 * @return Human readable representation.
                 */
                virtual const string toString() const = 0;
        };

    }
} // odcore::data

#endif /*OPENDAVINCI_CORE_DATA_SERIALIZABLEDATA_H_*/
