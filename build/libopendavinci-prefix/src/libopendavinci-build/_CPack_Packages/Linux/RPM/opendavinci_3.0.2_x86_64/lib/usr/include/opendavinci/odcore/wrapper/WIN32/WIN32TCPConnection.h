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

#ifndef OPENDAVINCI_CORE_WRAPPER_WIN32IMPL_WIN32TCPCONNECTION_H_
#define OPENDAVINCI_CORE_WRAPPER_WIN32IMPL_WIN32TCPCONNECTION_H_

#include <memory>
#include <string>

#include "opendavinci/odcore/opendavinci.h"

#include "opendavinci/odcore/io/tcp/TCPConnection.h"
#include "opendavinci/odcore/wrapper/Runnable.h"
#include "opendavinci/odcore/wrapper/Thread.h"
#include "opendavinci/odcore/wrapper/Mutex.h"

namespace odcore {
    namespace wrapper {
        namespace WIN32Impl {

            using namespace std;

            class WIN32TCPConnection : public odcore::io::tcp::TCPConnection, public Runnable {
                private:
                    /**
                     * "Forbidden" copy constructor. Goal: The compiler should warn
                     * already at compile time for unwanted bugs caused by any misuse
                     * of the copy constructor.
                     */
                    WIN32TCPConnection(const WIN32TCPConnection &);

                    /**
                     * "Forbidden" assignment operator. Goal: The compiler should warn
                     * already at compile time for unwanted bugs caused by any misuse
                     * of the assignment operator.
                     */
                    WIN32TCPConnection& operator=(const WIN32TCPConnection &);

                public:
                    WIN32TCPConnection(const int32_t &fileDescriptor);

                    WIN32TCPConnection(const std::string &ip, const uint32_t &port);

                    virtual ~WIN32TCPConnection();

                    virtual void sendImplementation(const std::string& data);

                    virtual void start();
                    virtual void stop();

                    virtual bool isRunning();
                    virtual void run();

                protected:
                    void initialize();

                    unique_ptr<Thread> m_thread;

                    unique_ptr<Mutex> m_socketMutex;
                    int32_t m_fileDescriptor;

                    enum {BUFFER_SIZE = 65535};
                    char m_buffer[BUFFER_SIZE];
                    std::string m_ip;
                    uint32_t m_port;
            };
        }
    }
}
#endif /* OPENDAVINCI_CORE_WRAPPER_WIN32IMPL_WIN32TCPCONNECTION_H_ */
