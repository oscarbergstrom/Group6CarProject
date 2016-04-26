/**
 * drivenpath - Export driven path (part of simulation environment)
 * Copyright (C) 2012 - 2015 Christian Berger
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

#include <iostream>
#include <string>
#include <vector>

#include "DrivenPath.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/io/URL.h"
#include "opendlv/core/wrapper/graph/DirectedGraph.h"
#include "opendlv/data/environment/EgoState.h"
#include "opendlv/data/environment/NamedLine.h"
#include "opendlv/data/environment/Point3.h"
#include "opendlv/data/scenario/Scenario.h"
#include "opendlv/scenario/LaneVisitor.h"
#include "opendlv/scenario/SCNXArchive.h"
#include "opendlv/scenario/SCNXArchiveFactory.h"

namespace odcore { namespace base { class KeyValueDataStore; } }

namespace measurements {

    using namespace std;
    using namespace odcore::base;
    using namespace odcore::data;
    using namespace odcore::io;
    using namespace opendlv::data::scenario;
    using namespace opendlv::data::environment;
    using namespace opendlv::data::environment;
    using namespace opendlv::scenario;

    DrivenPath::DrivenPath(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "DrivenPath") {}

    DrivenPath::~DrivenPath() {}

    void DrivenPath::setUp() {}

    void DrivenPath::tearDown() {}

    odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode DrivenPath::body() {
        KeyValueDataStore &kvs = getKeyValueDataStore();

        // Get scenario.
        const URL urlOfSCNXFile(getKeyValueConfiguration().getValue<string>("global.scenario"));
        
        // Create graph.
        core::wrapper::graph::DirectedGraph m_graph;

        // Read scenario.
        vector<NamedLine> listOfLines;
        if (urlOfSCNXFile.isValid()) {
            SCNXArchive &scnxArchive = SCNXArchiveFactory::getInstance().getSCNXArchive(urlOfSCNXFile);

            opendlv::data::scenario::Scenario &scenario = scnxArchive.getScenario();

            // Construct road network.
            LaneVisitor lv(m_graph, scenario);
            scenario.accept(lv);

            listOfLines = lv.getListOfLines();
        }

        
        vector<NamedLine>::const_iterator it = listOfLines.begin();
        while (it != listOfLines.end()) {
            NamedLine l =(*it++);

            // Draw line.
            Container c = Container(l);
            getConference().send(c);
/*
            cout << "Lines;" << l.getName() << ";" << l.getA().getX()
                                            << ";" << l.getA().getY()
                                            << ";" << l.getA().getZ()
                                            << ";" << l.getB().getX()
                                            << ";" << l.getB().getY()
                                            << ";" << l.getB().getZ() << endl;
*/
            cout << "LinesA;" << l.getName() << ";" << l.getA().getX()
                                            << ";" << l.getA().getY()
                                            << ";" << l.getA().getZ() << endl;
            cout << "LinesB;" << l.getName() << ";" << l.getB().getX()
                                            << ";" << l.getB().getY()
                                            << ";" << l.getB().getZ() << endl;
        }


        unsigned int counter = 0;

        while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
            // Get current ego state.
            Container c = kvs.get(opendlv::data::environment::EgoState::ID());
            EgoState es = c.getData<EgoState>();
            cout << counter << ";EgoState: '" << es.toString() << "'" << endl;
            cout << counter << ";EgoState-short;" << es.getPosition().getX() << ";" << es.getPosition().getY() << ";" << es.getPosition().getZ() << endl;

            // Algorithm:
            // 1. Get position of vehicle.
            // 2. Calculate perpendicular point to all lines in the list.
            // 3. Determine the distance between the position and the perpendicular point.
            // 4. Iterate through all named lines and determine the smallest distance.
            // 5. Print the smallest distance/named line.

            string oldName = "";
            double oldDistance = numeric_limits<double>::max();
            string oldNameOverall = "";
            double oldDistanceOverall = numeric_limits<double>::max();
            it = listOfLines.begin();
            while (it != listOfLines.end()) {
                NamedLine l =(*it++);
                if (l.getName().compare(oldName) != 0) {
                    // Print results:
                    if (oldName.compare("") != 0) {
                        cout << counter << ";Distance;" << oldName << ";" << oldDistance << endl;
                    }

                    // Reset values because we are starting a new turn.
                    oldDistance = numeric_limits<double>::max();
                }

                Point3 vehiclePosition = es.getPosition();
                Point3 perpendicularPoint = l.getPerpendicularPoint(vehiclePosition);

                // Compute distance between vehicle's position and lane segment.
                double length = (perpendicularPoint - vehiclePosition).lengthXY();
                if (length < oldDistance) {
                    oldDistance = length;
                }

                if (length < oldDistanceOverall) {
                    oldDistanceOverall = length;
                    oldNameOverall = l.getName();
                }

                oldName = l.getName();
            }

            cout << counter << ";DistanceOverall;" << oldNameOverall << ";" << oldDistanceOverall << endl;

            counter++;
        }
        return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
    }

} // measurements
