How to send bytes to a UDP server
"""""""""""""""""""""""""""""""""

OpenDaVINCI has a built-in UDP handling engine realized on Linux-based
and BSD-based operating systems using POSIX sockets and on Windows using WinSock.
The sources for this example are available at
https://github.com/se-research/OpenDaVINCI/tree/master/tutorials/udpsendbytes

In order to send bytes to a UDP server, you will find a simple example
below.

UDPSendBytes.cpp:

.. code-block:: c++

    #include <stdint.h>
    #include <iostream>
    #include <string>
    #include <memory>
    #include <opendavinci/odcore/io/udp/UDPSender.h>
    #include <opendavinci/odcore/io/udp/UDPFactory.h>

    using namespace std;

    // We add some of OpenDaVINCI's namespaces for the sake of readability.
    using namespace odcore;
    using namespace odcore::io::udp;

    int32_t main(int32_t argc, char **argv) {
        const string RECEIVER = "127.0.0.1";
        const uint32_t PORT = 1234;

        // We are using OpenDaVINCI's std::shared_ptr to automatically
        // release any acquired resources.
        try {
            std::shared_ptr<UDPSender> udpsender(UDPFactory::createUDPSender(RECEIVER, PORT));

            udpsender->send("Hello World\r\n");
        }
        catch(string &exception) {
            cerr << "Data could not be sent: " << exception << endl;
        }
    }

To send bytes over UDP to a UDP socket, your application needs to include
``<opendavinci/odcore/io/udp/UDPSender.h>`` and ``<opendavinci/odcore/io/udp/UDPFactory.h>`` that encapsulate
the platform-specific implementations.

``UDPFactory`` provides a static method called ``createUDPSender`` that
allows you to send bytes to the specified UDP server. On success, this call will return
a pointer to a ``UDPServer`` instance that is used to handle the data transfer.
On failure, the method ``createUDPSender`` will throw an exception of type
``string`` with an error message.

If the connection could be successfully established, the method ``send`` can be
used to send data of type ``string`` to the specified UDP server.

To conveniently handle the resource management of releasing the acquired system
resources, a ``std::shared_ptr`` is used that automatically releases memory that
is no longer used.

You can compile and link the example::

   g++ -std=c++11 -I /usr/include -c UDPSendBytes.cpp -o UDPSendBytes.o
   g++ -o udpsendbytes UDPSendBytes.o -lopendavinci -lpthread

To test the program, we create a simple UDP server awaiting data by using
the tool ``nc``(netcat)::

    $ nc -l -u -p 1234

The resulting program can be run::

    $ ./udpsendbytes
    $

The tool ``nc`` will print ``Hello World`` and then terminate as the connection
is closed on exiting ``udpsendbytes``.

