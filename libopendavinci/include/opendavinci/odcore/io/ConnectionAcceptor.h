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

#ifndef OPENDAVINCI_CORE_IO_CONNECTIONACCEPTOR_H_
#define OPENDAVINCI_CORE_IO_CONNECTIONACCEPTOR_H_

#include "opendavinci/odcore/opendavinci.h"
#include <memory>
#include "opendavinci/odcore/base/Mutex.h"
#include "opendavinci/odcore/io/tcp/TCPAcceptor.h"
#include "opendavinci/odcore/io/tcp/TCPAcceptorListener.h"

namespace odcore {
    namespace io {

class ConnectionAcceptorListener;
namespace tcp { class TCPConnection; }

        using namespace std;

        /**
         * This class is used to accept incoming connection. The
         * For every new Connection, the ConnectionAcceptorListener
         * is invoked with a new Connection object.
         *
         * To establish a connection, see #Connection
         */
        class OPENDAVINCI_API ConnectionAcceptor : public odcore::io::tcp::TCPAcceptorListener {
            private:
                /**
                 * Forbidden copy constructor
                 */
                ConnectionAcceptor(const ConnectionAcceptor&);

                /**
                 * Forbidden assignement operator
                 */
                ConnectionAcceptor& operator=(const ConnectionAcceptor&);

            public:
                ConnectionAcceptor(const uint32_t &port);
                virtual ~ConnectionAcceptor();

                void setConnectionAcceptorListener(ConnectionAcceptorListener* listener);

                void start();
                void stop();

            protected:
                base::Mutex m_listenerMutex;
                ConnectionAcceptorListener* m_listener;

                std::shared_ptr<odcore::io::tcp::TCPAcceptor> m_acceptor;

                void onNewConnection(std::shared_ptr<odcore::io::tcp::TCPConnection> connection);
        };
    }
}

#endif /* OPENDAVINCI_CORE_IO_CONNECTIONACCEPTOR_H_ */
