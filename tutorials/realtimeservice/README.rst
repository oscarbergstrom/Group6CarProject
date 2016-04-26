How to use realtime concurrency
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


OpenDaVINCI has a built-in realtime concurrency engine available only on Linux
platforms with rt-preempt kernels. The sources for this example are available
at https://github.com/se-research/OpenDaVINCI/tree/master/tutorials/realtimeservice

In order to use realtime concurrency in a user-supplied module, you will find
a simple example below.

MyRealtimeService.hpp:

.. code-block:: c++

    #include <opendavinci/odcore/base/RealtimeService.h>

    // Realtime concurrency is provided by the class odcore::base::RealtimeService.
    class MyRealtimeService : public odcore::base::RealtimeService {

        public:
            MyRealtimeService(const enum PERIOD &period);

            // Your class needs to implement the method void nextTimeSlice().
            virtual void nextTimeSlice();

    };

MyRealtimeService.cpp:

.. code-block:: c++

    #include <stdint.h>
    #include <iostream>
    #include <opendavinci/odcore/base/Thread.h>

    #include "MyRealtimeService.hpp"

    using namespace std;

    MyRealtimeService::MyRealtimeService(const enum PERIOD &period) :
        odcore::base::RealtimeService(period) {}

    void MyRealtimeService::nextTimeSlice() {
        cout << "This message is printed every 100 ms." << endl;
    }

    int32_t main(int32_t argc, char **argv) {
        MyRealtimeService rts(odcore::base::RealtimeService::ONEHUNDREDMILLISECONDS);

        rts.start();

        const uint32_t ONE_SECOND = 1000 * 1000;
        odcore::base::Thread::usleepFor(5 * ONE_SECOND);

        rts.stop();
    }

Your class needs to derive from ``odcore::base::RealtimeService``, which is provided in
``#include <opendavinci/odcore/base/RealtimeService.h>`` in the include directory ``opendavinci``.
This class provides one method that needs to be implemented in deriving classes:
``void nextTimeSlice()``. This method will be called with the specified periodicity.

Furthermore, the deriving class needs to pass the desired periodicity to the superclass ``RealtimeService``.

You can compile and link the example::

   g++ -std=c++11 -I /usr/include -c MyRealtimeService.cpp -o MyRealtimeService.o
   g++ -o realtimeservice MyRealtimeService.o -lopendavinci -lpthread -lrt

The resulting program can be run as superuser (as the scheduling properties will be
adjusted) and will print the following on the console::

    $ sudo ./realtimeservice
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
    This message is printed every 100 ms.
