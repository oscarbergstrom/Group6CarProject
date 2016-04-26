/**
 * odplayer - Tool for playing back recorded data
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <cmath>
#include <iostream>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/base/Thread.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/io/URL.h"
#include "opendavinci/odtools/player/Player.h"
#include "opendavinci/generated/odcore/data/player/PlayerCommand.h"

#include "PlayerModule.h"

namespace odplayer {

    using namespace std;
    using namespace odcore::base;
    using namespace odcore::data;
    using namespace odcore::io;
    using namespace odtools::player;

    PlayerModule::PlayerModule(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "odplayer"),
        m_playerControl() {}

    PlayerModule::~PlayerModule() {}

    void PlayerModule::setUp() {}

    void PlayerModule::tearDown() {}

    void PlayerModule::wait() {
        AbstractModule::wait();
    }

    odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode PlayerModule::body() {
        // Check if the player is remotely controlled.
        bool remoteControl = (getKeyValueConfiguration().getValue<bool>("odplayer.remoteControl") != 0);

        // Read the URL of the file to replay.
        URL url(getKeyValueConfiguration().getValue<string>("odplayer.input"));

        // Read the scaling factor.
        double timeScale = getKeyValueConfiguration().getValue<double>("odplayer.timeScale");
        timeScale = (fabs(timeScale) > 1e-5 ? fabs(timeScale) : 1.0);

        // Do we have to rewind the stream on EOF?
        bool autoRewind = (getKeyValueConfiguration().getValue<int>("odplayer.autoRewind") != 0);

        // Size of the memory buffer.
        const uint32_t MEMORY_SEGMENT_SIZE = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.memorySegmentSize");

        // Number of memory segments.
        const uint32_t NUMBER_OF_SEGMENTS = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.numberOfMemorySegments");

        // Run player in asynchronous mode with data caching in background.
        const bool THREADING = true;

        // Add FIFOQueue for controlling the player.
        addDataStoreFor(odcore::data::player::PlayerCommand::ID(), m_playerControl);

        // Construct player.
        Player player(url, autoRewind, MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING);

        // The next container to be sent.
        Container nextContainerToBeSent;

        // Delay between two containers.
        uint32_t delay = 0;

        // If no remote control, simply play the stuff.
        bool playing = (!remoteControl);
        bool doStep = false;

        // The main loop.
        while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
            if (playing) {
                // Get container to be sent.
                nextContainerToBeSent = player.getNextContainerToBeSent();

                // Get delay to wait _after_ sending the container.
                delay = player.getDelay() * timeScale;

                // Here, the container is sent while discarding player commands.
                if ( (nextContainerToBeSent.getDataType() != Container::UNDEFINEDDATA) &&
                        (nextContainerToBeSent.getDataType() != odcore::data::player::PlayerCommand::ID()) ) {

                    // Process next token only if there's no new command.
                    if (!remoteControl || (m_playerControl.isEmpty())) {
                        CLOG1 << "[" << getName() << "(" << getIdentifier() << ")]: Sending " << nextContainerToBeSent.toString() << " (delay was " << delay << " us): " << nextContainerToBeSent.getReceivedTimeStamp().toString() << endl;

                        getConference().send(nextContainerToBeSent);
                    }

                    // Don't wait while stepping.
                    if (!doStep) {
                        Thread::usleepFor(delay);
                    }
                }
            }

            // If the last cycle was during a step, disable the stepwise playing and check for remote control.
            if (doStep) {
                playing = false;
                doStep = false;
            }

            // Check remote control.
            if (remoteControl) {
                if (!(m_playerControl.isEmpty())) {
                    Container container = m_playerControl.leave();

                    odcore::data::player::PlayerCommand pc;
                    pc = container.getData<odcore::data::player::PlayerCommand>();

                    CLOG1 << "[" << getName() << "(" << getIdentifier() << ")]: " << pc.toString() << endl;

                    switch (pc.getCommand()) {
                        case odcore::data::player::PlayerCommand::PLAY:
                            playing = true;
                            break;
                        case odcore::data::player::PlayerCommand::PAUSE:
                            playing = false;
                            break;
                        case odcore::data::player::PlayerCommand::STEP_FORWARD:
                            playing = true;
                            doStep = true;
                            break;
                        case odcore::data::player::PlayerCommand::REWIND:
                            player.rewind();
                            playing = false;
                            break;
                    }
                }
            }

            // If replaying is finished, exit.
            if (!remoteControl && !player.hasMoreData()) {
                break;
            }
        }

        return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
    }

} // odplayer
