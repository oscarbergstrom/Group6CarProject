Using the logging interface in OpenDaVINCI
------------------------------------------

OpenDaVINCI has a built-in logging engine. Its fundamental concept is
realized by ``odsupercomponent`` as it is required to start a communication
session for distributed modules anyways.

In order to use the logging engine in a user-supplied module, you can
simply call the following method from within your module deriving from
``DataTriggeredConferenceClientModule`` or ``TimeTriggeredConferenceClientModule``::

   void toLogger(const odcore::data::LogMessage::LogLevel &logLevel, const string &msg);

Example::

   toLogger(odcore::data::LogMessage::INFO, "this is an info message");

By using this method, ``"this is an info message"`` is sent to the UDP
multicast conference using the logging level ``logLevel``. The ``logLevel`` can
be either ``odcore::data::LogMessage::NONE``, ``odcore::data::LogMessage::INFO``, ``odcore::data::LogMessage::WARN``, or ``odcore::data::LogMessage::DEBUG``. ``odsupercomponent``
is receiving the log data and storing it to a file depending on its
command line parameters.

On startup of ``odsupercomponent``, you need to specify the following
command line parameters to setup the logging:

#. ``--logLevel``
#. ``--logFile``

The first parameter specifies the logging level up to which messages from
distributed modules are stored in the log file. Thus,
if you specify ``--logLevel=INFO`` any messages that are tagged as
``odcore::data::LogMessage::NONE`` or ``odcore::data::LogMessage::INFO`` will be stored but
``odcore::data::LogMessage::WARN`` and ``odcore::data::LogMessage::DEBUG`` are discarded.

If you omit the parameter, ``odcore::data::LogMessage::NONE`` is assumed.

The second parameter specifies the log file where the log data is stored.
The data itself has the following format::

    <time stamp in microseconds> ; <name of the sending module> ; <numerical log level, 0 - 3> ; <message> ;

Example::

   $ odsupercomponent --cid=111 --loglevel=info --logfile=mylogfile.log

This feature was contributed by Ashfaq Hussain Farooqui (https://twitter.com/me_rafiki) and will be available from release 1.3.0.
