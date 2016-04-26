/**
 * OpenDLV - Simulation environment
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

#include <iostream>
#include <vector>

#include "opendlv/core/wrapper/parser/ASTNode.h"
#include "opendlv/data/environment/NamedLine.h"
#include "opendlv/data/scenario/Arc.h"
#include "opendlv/data/scenario/Connector.h"
#include "opendlv/data/scenario/Lane.h"
#include "opendlv/data/scenario/LaneModel.h"
#include "opendlv/data/scenario/PointModel.h"
#include "opendlv/data/scenario/Scenario.h"
#include "opendlv/data/scenario/ScenarioNode.h"
#include "opendlv/data/scenario/StraightLine.h"
#include "opendlv/scenario/FindNodeByPointIDVisitor.h"
#include "opendlv/scenario/LaneVisitor.h"
#include "opendlv/data/graph/WaypointsEdge.h"
#include "opendlv/data/graph/WaypointVertex.h"

namespace core { namespace wrapper { namespace graph { class DirectedGraph; } } }

namespace opendlv {
    namespace scenario {

        using namespace std;
        using namespace opendlv::data::environment;
        using namespace opendlv::data::graph;
        using namespace opendlv::data::scenario;

        LaneVisitor::LaneVisitor(core::wrapper::graph::DirectedGraph &g, opendlv::data::scenario::Scenario &scenario) :
            m_graph(g),
            m_scenario(scenario),
            m_listOfLines() {}

        LaneVisitor::~LaneVisitor() {}

        void LaneVisitor::visit(ScenarioNode &node) {
            try {
                Lane &l = dynamic_cast<Lane&>(node);

                if (l.getLaneModel() != NULL) {
                    const PointModel *pm = dynamic_cast<PointModel*>(l.getLaneModel());
                    bool wasPM = false;
                    if (!wasPM && pm != NULL) {
                        visit(pm);
                        wasPM = true;
                    }

                    bool wasArc = false;
                    const Arc *arc = dynamic_cast<Arc*>(l.getLaneModel());
                    if (!wasArc && !wasPM && arc != NULL) {
                        visit(arc);
                        wasArc = true;
                    }

                    const StraightLine *sl = dynamic_cast<StraightLine*>(l.getLaneModel());
                    bool wasSL = false;
                    if (!wasSL && !wasArc && !wasPM && (sl != NULL)) {
                        visit(sl);
                        wasSL = true;
                    }
                }
            }
            catch(...) {}
        }

        void LaneVisitor::visit(const opendlv::data::scenario::PointModel *pm) {
            if (pm != NULL) {
                stringstream name;
                name << pm->getLane()->getRoad()->getLayer()->getIdentifier() << "." << pm->getLane()->getRoad()->getIdentifier() << "." << pm->getLane()->getIdentifier();
                cerr << "Found PointModel Lane: " << name.str() << endl;

                const vector<IDVertex3> &listOfWaypoints = pm->getListOfIdentifiableVertices();
                const uint32_t SIZE = listOfWaypoints.size();
                for(uint32_t i = 0; i < (SIZE-1); i++) {
                    IDVertex3 vt1 = listOfWaypoints.at(i);
                    IDVertex3 vt2 = listOfWaypoints.at(i+1);

                    // Construct a line between to consecutive points.
                    opendlv::data::environment::NamedLine line(name.str(), vt1, vt2);
                    m_listOfLines.push_back(line);

                    WaypointVertex *v1 = new WaypointVertex();
                    v1->setLayerID(pm->getLane()->getRoad()->getLayer()->getIdentifier());
                    v1->setRoadID(pm->getLane()->getRoad()->getIdentifier());
                    v1->setLaneID(pm->getLane()->getIdentifier());
                    v1->setWaypointID(vt1.getIdentifier());
                    v1->setPosition(vt1);
                    cerr << "Constructed node: " << pm->getLane()->getRoad()->getLayer()->getIdentifier() << "." << pm->getLane()->getRoad()->getIdentifier() << "." << pm->getLane()->getIdentifier() << "." << vt1.getIdentifier() << endl;

                    WaypointVertex *v2 = new WaypointVertex();
                    v2->setLayerID(pm->getLane()->getRoad()->getLayer()->getIdentifier());
                    v2->setRoadID(pm->getLane()->getRoad()->getIdentifier());
                    v2->setLaneID(pm->getLane()->getIdentifier());
                    v2->setWaypointID(vt2.getIdentifier());
                    v2->setPosition(vt2);

                    cerr << "Constructed node: " << pm->getLane()->getRoad()->getLayer()->getIdentifier() << "." << pm->getLane()->getRoad()->getIdentifier() << "." << pm->getLane()->getIdentifier() << "." << vt2.getIdentifier() << endl;

                    WaypointsEdge *edge = new WaypointsEdge();
                    edge->setCosts(vt1.getXYDistanceTo(vt2));

                    // Add vertices and edge to graph.
                    m_graph.updateEdge(v1, v2, edge);
                }

                // Add edges for the connectors.
                const vector<Connector> &listOfConnectors = pm->getListOfConnectors();
                vector<Connector>::const_iterator mt = listOfConnectors.begin();
                while (mt != listOfConnectors.end()) {
                    Connector c = (*mt++);

                    cerr << "Found connector: " << c.toString() << " to add." << endl;
                    try {
                        // Find start vertex.
                        FindNodeByPointIDVisitor startVertexFinder(c.getSource());
                        m_scenario.accept(startVertexFinder);

                        // Find end vertex.
                        FindNodeByPointIDVisitor endVertexFinder(c.getTarget());
                        m_scenario.accept(endVertexFinder);

                        // Check if valid models.
                        if ( hasStartAndEndVertex(c) ) {
//                            const IDVertex3 startV = startPointModel->getIDVertex3(c.getSource().getPointID());
//                            const IDVertex3 endV = endPointModel->getIDVertex3(c.getTarget().getPointID());
                            const IDVertex3 startV = startVertexFinder.getIDVertex3();
                            const IDVertex3 endV = endVertexFinder.getIDVertex3();

                            WaypointVertex *v1 = new WaypointVertex();
                            v1->setLayerID(c.getSource().getLayerID());
                            v1->setRoadID(c.getSource().getRoadID());
                            v1->setLaneID(c.getSource().getLaneID());
                            v1->setWaypointID(c.getSource().getPointID());
                            v1->setPosition(startV);

                            WaypointVertex *v2 = new WaypointVertex();
                            v2->setLayerID(c.getTarget().getLayerID());
                            v2->setRoadID(c.getTarget().getRoadID());
                            v2->setLaneID(c.getTarget().getLaneID());
                            v2->setWaypointID(c.getTarget().getPointID());
                            v2->setPosition(endV);

                            WaypointsEdge *edge = new WaypointsEdge();
                            edge->setCosts(startV.getXYDistanceTo(endV));

                            m_graph.updateEdge(v1, v2, edge);
                            cerr << "Adding edge for connector between " << v1->toString() << " and " << v2->toString() << endl;
                        }
                    }
                    catch(...) {}
                }
            }
        }


        void LaneVisitor::visit(const opendlv::data::scenario::Arc *arc) {
            if (arc != NULL) {
                stringstream name;
                name << arc->getLane()->getRoad()->getLayer()->getIdentifier() << "." << arc->getLane()->getRoad()->getIdentifier() << "." << arc->getLane()->getIdentifier();
                cerr << "Found Arc Lane: " << name.str() << endl;

                IDVertex3 vt1 = arc->getStart();
                IDVertex3 vt2 = arc->getEnd();

                // Approximate arc by a set of lines.
                {
                    // ScUI graphical modeling tool starts Arc at 6pm but we use 3pm. Thus, we have to correct the Z-rotation.
                    double correctRotation = cartesian::Constants::PI / 2.0;

                    // Transposition of the arc.
                    opendlv::data::environment::Point3 t(0, 0, 0);
                    t.setX(vt1.getX() - arc->getRadius() * cos(arc->getRotationZ() - correctRotation));
                    t.setY(vt1.getY() - arc->getRadius() * sin(arc->getRotationZ() - correctRotation));      
                    t.setZ(0.05);

                    double beginInterval = arc->getBeginInterval();
                    double endInterval = arc->getEndInterval();
                    double stepSize = 5.0 * cartesian::Constants::PI / 180.0; // 5° to rad.
                    uint32_t steps = (unsigned int) round( (endInterval - beginInterval) / stepSize );

                    opendlv::data::environment::Point3 centerOld = vt1;

                    for(uint32_t i = 0; i < steps; i++) {
                        // Calculate the skeleton approximation.
                        opendlv::data::environment::Point3 center(0, 0, 0);
                        center.setX(arc->getRadius() * cos(arc->getRotationZ() - correctRotation + i * stepSize));
                        center.setY(arc->getRadius() * sin(arc->getRotationZ() - correctRotation + i * stepSize));

                        // Transpose the points.
                        center += t;

                        // Construct a line between to consecutive points.
                        opendlv::data::environment::NamedLine line(name.str(), centerOld, center);
                        m_listOfLines.push_back(line);

                        // Keep old point.
                        centerOld = center;
                    }
                }

                WaypointVertex *v1 = new WaypointVertex();
                v1->setLayerID(arc->getLane()->getRoad()->getLayer()->getIdentifier());
                v1->setRoadID(arc->getLane()->getRoad()->getIdentifier());
                v1->setLaneID(arc->getLane()->getIdentifier());
                v1->setWaypointID(vt1.getIdentifier());
                v1->setPosition(vt1);
                cerr << "Constructed node: " << arc->getLane()->getRoad()->getLayer()->getIdentifier() << "." << arc->getLane()->getRoad()->getIdentifier() << "." << arc->getLane()->getIdentifier() << "." << vt1.getIdentifier() << endl;

                WaypointVertex *v2 = new WaypointVertex();
                v2->setLayerID(arc->getLane()->getRoad()->getLayer()->getIdentifier());
                v2->setRoadID(arc->getLane()->getRoad()->getIdentifier());
                v2->setLaneID(arc->getLane()->getIdentifier());
                v2->setWaypointID(vt2.getIdentifier());
                v2->setPosition(vt2);

                cerr << "Constructed node: " << arc->getLane()->getRoad()->getLayer()->getIdentifier() << "." << arc->getLane()->getRoad()->getIdentifier() << "." << arc->getLane()->getIdentifier() << "." << vt2.getIdentifier() << endl;

                WaypointsEdge *edge = new WaypointsEdge();
                edge->setCosts(vt1.getXYDistanceTo(vt2));

                // Add vertices and edge to graph.
                m_graph.updateEdge(v1, v2, edge);

                // Add edges for the connectors.
                const vector<Connector> &listOfConnectors = arc->getListOfConnectors();
                vector<Connector>::const_iterator mt = listOfConnectors.begin();
                while (mt != listOfConnectors.end()) {
                    Connector c = (*mt++);

                    cerr << "Found connector: " << c.toString() << " to add." << endl;
                    try {
                        // Find start vertex.
                        FindNodeByPointIDVisitor startVertexFinder(c.getSource());
                        m_scenario.accept(startVertexFinder);

                        // Find end vertex.
                        FindNodeByPointIDVisitor endVertexFinder(c.getTarget());
                        m_scenario.accept(endVertexFinder);

                        // Check if valid models.
                        if ( hasStartAndEndVertex(c) ) {
                            const IDVertex3 startV = startVertexFinder.getIDVertex3();
                            const IDVertex3 endV = endVertexFinder.getIDVertex3();

                            v1 = new WaypointVertex();
                            v1->setLayerID(c.getSource().getLayerID());
                            v1->setRoadID(c.getSource().getRoadID());
                            v1->setLaneID(c.getSource().getLaneID());
                            v1->setWaypointID(c.getSource().getPointID());
                            v1->setPosition(startV);

                            v2 = new WaypointVertex();
                            v2->setLayerID(c.getTarget().getLayerID());
                            v2->setRoadID(c.getTarget().getRoadID());
                            v2->setLaneID(c.getTarget().getLaneID());
                            v2->setWaypointID(c.getTarget().getPointID());
                            v2->setPosition(endV);

                            edge = new WaypointsEdge();
                            edge->setCosts(startV.getXYDistanceTo(endV));

                            m_graph.updateEdge(v1, v2, edge);
                            cerr << "Adding edge for connector between " << v1->toString() << " and " << v2->toString() << endl;
                        }
                    }
                    catch(...) {}
                }
            }
        }

        void LaneVisitor::visit(const opendlv::data::scenario::StraightLine *sl) {
            if (sl != NULL) {
                stringstream name;
                name << sl->getLane()->getRoad()->getLayer()->getIdentifier() << "." << sl->getLane()->getRoad()->getIdentifier() << "." << sl->getLane()->getIdentifier();
                cerr << "Found StraightLine Lane: " << name.str() << endl;

                IDVertex3 vt1 = sl->getStart();
                IDVertex3 vt2 = sl->getEnd();

                // Construct a line between to consecutive points.
                opendlv::data::environment::NamedLine line(name.str(), vt1, vt2);
                m_listOfLines.push_back(line);

                WaypointVertex *v1 = new WaypointVertex();
                v1->setLayerID(sl->getLane()->getRoad()->getLayer()->getIdentifier());
                v1->setRoadID(sl->getLane()->getRoad()->getIdentifier());
                v1->setLaneID(sl->getLane()->getIdentifier());
                v1->setWaypointID(vt1.getIdentifier());
                v1->setPosition(vt1);
                cerr << "Constructed node: " << sl->getLane()->getRoad()->getLayer()->getIdentifier() << "." << sl->getLane()->getRoad()->getIdentifier() << "." << sl->getLane()->getIdentifier() << "." << vt1.getIdentifier() << endl;

                WaypointVertex *v2 = new WaypointVertex();
                v2->setLayerID(sl->getLane()->getRoad()->getLayer()->getIdentifier());
                v2->setRoadID(sl->getLane()->getRoad()->getIdentifier());
                v2->setLaneID(sl->getLane()->getIdentifier());
                v2->setWaypointID(vt2.getIdentifier());
                v2->setPosition(vt2);

                cerr << "Constructed node: " << sl->getLane()->getRoad()->getLayer()->getIdentifier() << "." << sl->getLane()->getRoad()->getIdentifier() << "." << sl->getLane()->getIdentifier() << "." << vt2.getIdentifier() << endl;

                WaypointsEdge *edge = new WaypointsEdge();
                edge->setCosts(vt1.getXYDistanceTo(vt2));

                // Add vertices and edge to graph.
                m_graph.updateEdge(v1, v2, edge);

                // Add edges for the connectors.
                const vector<Connector> &listOfConnectors = sl->getListOfConnectors();
                vector<Connector>::const_iterator mt = listOfConnectors.begin();
                while (mt != listOfConnectors.end()) {
                    Connector c = (*mt++);

                    cerr << "Found connector: " << c.toString() << " to add." << endl;
                    try {
                        // Find start vertex.
                        FindNodeByPointIDVisitor startVertexFinder(c.getSource());
                        m_scenario.accept(startVertexFinder);

                        // Find end vertex.
                        FindNodeByPointIDVisitor endVertexFinder(c.getTarget());
                        m_scenario.accept(endVertexFinder);

                        // Check if valid models.
                        if ( hasStartAndEndVertex(c) ) {
                            const IDVertex3 startV = startVertexFinder.getIDVertex3();
                            const IDVertex3 endV = endVertexFinder.getIDVertex3();

                            v1 = new WaypointVertex();
                            v1->setLayerID(c.getSource().getLayerID());
                            v1->setRoadID(c.getSource().getRoadID());
                            v1->setLaneID(c.getSource().getLaneID());
                            v1->setWaypointID(c.getSource().getPointID());
                            v1->setPosition(startV);

                            v2 = new WaypointVertex();
                            v2->setLayerID(c.getTarget().getLayerID());
                            v2->setRoadID(c.getTarget().getRoadID());
                            v2->setLaneID(c.getTarget().getLaneID());
                            v2->setWaypointID(c.getTarget().getPointID());
                            v2->setPosition(endV);

                            edge = new WaypointsEdge();
                            edge->setCosts(startV.getXYDistanceTo(endV));

                            m_graph.updateEdge(v1, v2, edge);
                            cerr << "Adding edge for connector between " << v1->toString() << " and " << v2->toString() << endl;
                        }
                    }
                    catch(...) {}
                }
            }
        }

        bool LaneVisitor::hasStartAndEndVertex(const opendlv::data::scenario::Connector &c) {
            bool startVertexFound = false;
            bool endVertexFound = false;

            // Find start vertex.
            {
                FindNodeByPointIDVisitor startVertexFinder(c.getSource());
                m_scenario.accept(startVertexFinder);

                const Arc *startArc = dynamic_cast<const Arc*>(startVertexFinder.getLaneModel());
//                const Clothoid *startClothoid = dynamic_cast<const Clothoid*>(startVertexFinder.getLaneModel());
                const PointModel *startPointModel = dynamic_cast<const PointModel*>(startVertexFinder.getLaneModel());
                const StraightLine *startStraightLine = dynamic_cast<const StraightLine*>(startVertexFinder.getLaneModel());

                startVertexFound |= (startArc != NULL) ||
//                                    (startClothoid != NULL) ||
                                    (startPointModel != NULL) ||
                                    (startStraightLine != NULL);
            }

            // Find end vertex.
            {
                FindNodeByPointIDVisitor endVertexFinder(c.getTarget());
                m_scenario.accept(endVertexFinder);

                const Arc *endArc = dynamic_cast<const Arc*>(endVertexFinder.getLaneModel());
//                const Clothoid *endClothoid = dynamic_cast<const Clothoid*>(endVertexFinder.getLaneModel());
                const PointModel *endPointModel = dynamic_cast<const PointModel*>(endVertexFinder.getLaneModel());
                const StraightLine *endStraightLine = dynamic_cast<const StraightLine*>(endVertexFinder.getLaneModel());

                endVertexFound |= (endArc != NULL) ||
//                                  (endClothoid != NULL) ||
                                  (endPointModel != NULL) ||
                                  (endStraightLine != NULL);
            }

            return (startVertexFound && endVertexFound);
        }

        vector<opendlv::data::environment::NamedLine> LaneVisitor::getListOfLines() const {
            return m_listOfLines;
        }

    }
} // opendlv::scenario
