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

#ifndef OPENDAVINCI_CORE_WRAPPER_SIMPLEDB_SIMPLEDB_H_
#define OPENDAVINCI_CORE_WRAPPER_SIMPLEDB_SIMPLEDB_H_

#include <map>
#include <memory>
#include <string>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/wrapper/KeyValueDatabase.h"
#include "opendavinci/odcore/wrapper/KeyValueDatabaseLibraryProducts.h"

namespace odcore { namespace wrapper { class Mutex; } }
namespace odcore { namespace wrapper { template <odcore::wrapper::KeyValueDatabaseLibraryProducts product> class KeyValueDatabaseFactoryWorker; } }

namespace odcore {
    namespace wrapper {
        namespace SimpleDB {

            using namespace std;

            class SimpleDB : public KeyValueDatabase {
                friend class KeyValueDatabaseFactoryWorker<KeyValueDatabaseSimpleDB>;
                private:
                    /**
                     * "Forbidden" copy constructor. Goal: The compiler should warn
                     * already at compile time for unwanted bugs caused by any misuse
                     * of the copy constructor.
                     */
                    SimpleDB(const SimpleDB &);

                    /**
                     * "Forbidden" assignment operator. Goal: The compiler should warn
                     * already at compile time for unwanted bugs caused by any misuse
                     * of the assignment operator.
                     */
                    SimpleDB& operator=(const SimpleDB &);

                protected:
                    SimpleDB();

                public:
                    virtual ~SimpleDB();

                    virtual void put(const int32_t &key, const string &value);

                    virtual const string get(const int32_t &key) const;

                protected:
                    unique_ptr<Mutex> m_mutex;
                    mutable map<int, string> m_entries;
            };

        }
    }
} // odcore::wrapper::SimpleDB

#endif /*OPENDAVINCI_CORE_WRAPPER_SIMPLEDB_SIMPLEDB_H_*/
