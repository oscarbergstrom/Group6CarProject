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

#ifndef OPENDAVINCI_CORE_IO_STRINGPIPELINE_H_
#define OPENDAVINCI_CORE_IO_STRINGPIPELINE_H_

#include <queue>
#include <string>

#include "opendavinci/odcore/base/Condition.h"
#include "opendavinci/odcore/base/Mutex.h"
#include "opendavinci/odcore/base/Service.h"
#include "opendavinci/odcore/io/StringListener.h"
#include "opendavinci/odcore/io/StringObserver.h"

namespace odcore {
    namespace io {

        using namespace std;

        /**
         * This class distributes strings using an asynchronous pipeline to decouple
         * the processing of the data when invoking a StringListener at higher levels.
         */
        class StringPipeline : public odcore::base::Service, public StringObserver, public StringListener {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 */
                StringPipeline(const StringPipeline &);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 */
                StringPipeline& operator=(const StringPipeline &);

            public:
                StringPipeline();

                virtual ~StringPipeline();

                virtual void setStringListener(StringListener *sl);

                virtual void nextString(const string &s);

            private:
                virtual void beforeStop();

                virtual void run();

                /**
                 * This method is processing the entries in the queue.
                 */
                void processQueue();

            private:
                odcore::base::Condition m_queueCondition;
                odcore::base::Mutex m_queueMutex;
                queue<string> m_queue;

                odcore::base::Mutex m_stringListenerMutex;
                StringListener *m_stringListener;
        };

    }
} // odcore::io

#endif /*OPENDAVINCI_CORE_IO_STRINGPIPELINE_H_*/
