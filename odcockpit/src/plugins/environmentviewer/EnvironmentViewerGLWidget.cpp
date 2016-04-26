/**
 * cockpit - Visualization environment
 * Copyright (C) 2012 - 2015 Christian Berger
 * Copyright (C) 2008 - 2011 (as monitor component) Christian Berger, Bernhard Rumpe
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

#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/base/Lock.h"
#include "opendavinci/odcore/base/TreeNode.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/exceptions/Exceptions.h"
#include "opendavinci/odcore/io/URL.h"
#include "opendlv/data/environment/EgoState.h"
#include "opendlv/data/environment/Line.h"
#include "opendlv/data/environment/Obstacle.h"
#include "opendlv/data/environment/Point3.h"
#include "opendlv/data/environment/Polygon.h"
#include "opendlv/data/planning/Route.h"
#include "opendlv/data/sensor/ContouredObject.h"
#include "opendlv/data/sensor/ContouredObjects.h"
#include "opendlv/scenario/SCNXArchiveFactory.h"
#include "opendlv/threeD/Node.h"
#include "opendlv/threeD/NodeDescriptor.h"
#include "opendlv/threeD/NodeRenderingConfiguration.h"
#include "opendlv/threeD/TransformGroup.h"
#include "opendlv/threeD/decorator/DecoratorFactory.h"
#include "opendlv/threeD/loaders/OBJXArchive.h"
#include "opendlv/threeD/loaders/OBJXArchiveFactory.h"
#include "opendlv/threeD/models/Grid.h"
#include "opendlv/threeD/models/Line.h"
#include "opendlv/threeD/models/Point.h"
#include "opendlv/threeD/models/XYZAxes.h"
#include "plugins/PlugIn.h"
#include "plugins/environmentviewer/CameraAssignableNodesListener.h"
#include "plugins/environmentviewer/EnvironmentViewerGLWidget.h"
#include "plugins/environmentviewer/SelectableNodeDescriptor.h"
#include "plugins/environmentviewer/TreeNodeVisitor.h"

class QWidget;
namespace opendlv { namespace scenario { class SCNXArchive; } }

namespace cockpit {
    namespace plugins {
        namespace environmentviewer {

            using namespace odcore::base;
            using namespace odcore::data;
            using namespace odcore::exceptions;
            using namespace odcore::io;
            using namespace odcore::data;
            using namespace opendlv::data::environment;
            using namespace opendlv::data::environment;
            using namespace opendlv::data::planning;
            using namespace opendlv::data::scenario;
            using namespace opendlv::data::sensor;
            using namespace opendlv::scenario;
            using namespace opendlv::threeD;
            using namespace opendlv::threeD::decorator;
            using namespace opendlv::threeD::loaders;

            EnvironmentViewerGLWidget::EnvironmentViewerGLWidget(const PlugIn &plugIn, QWidget *prnt, CameraAssignableNodesListener &canl, SelectableNodeDescriptorTreeListener &sndtl) :
                    AbstractGLWidget(plugIn, prnt),
                    m_rootMutex(),
                    m_root(NULL),
                    m_stationaryElements(NULL),
                    m_dynamicElements(NULL),
                    m_measurements(NULL),
                    m_plannedRoute(NULL),
                    m_lines(NULL),
                    m_egoStateNodeDescriptor(),
                    m_numberOfReceivedEgoStates(0),
                    m_egoStateNode(NULL),
                    m_mapOfTraceablePositions(),
                    m_contouredObjectsNode(NULL),
                    m_renderingConfiguration(),
                    m_obstaclesRoot(NULL),
                    m_mapOfObstacles(),
                    m_cameraAssignableNodesListener(canl),
                    m_listOfCameraAssignableNodes(),
                    m_cameraAssignedNodeDescriptor(),
                    m_mapOfCurrentPositions(),
                    m_selectableNodeDescriptorTree(NULL),
                    m_selectableNodeDescriptorTreeListener(sndtl) {}

            EnvironmentViewerGLWidget::~EnvironmentViewerGLWidget() {
                OPENDAVINCI_CORE_DELETE_POINTER(m_root);
                OPENDAVINCI_CORE_DELETE_POINTER(m_selectableNodeDescriptorTree);
            }

            void EnvironmentViewerGLWidget::createSceneGraph() {
                m_root = new TransformGroup();
                m_stationaryElements = new TransformGroup();
                m_dynamicElements = new TransformGroup();
                m_measurements = new TransformGroup();

                m_root->addChild(m_stationaryElements);
                m_root->addChild(m_dynamicElements);
                m_root->addChild(m_measurements);

                /*******************************************************************/
                /* Stationary elements.                                            */
                /*******************************************************************/
                m_stationaryElements->addChild(new opendlv::threeD::models::XYZAxes(NodeDescriptor("XYZAxes"), 1, 10));
                m_stationaryElements->addChild(new opendlv::threeD::models::Grid(NodeDescriptor("Grid"), 10, 1));

                // Setup surroundings.
                const URL urlOfSCNXFile(getPlugIn().getKeyValueConfiguration().getValue<string>("global.scenario"));
                if (urlOfSCNXFile.isValid()) {
                    SCNXArchive &scnxArchive = SCNXArchiveFactory::getInstance().getSCNXArchive(urlOfSCNXFile);

                    // Read scnxArchive and decorate it for getting displayed in an OpenGL scene.
                    Node *surroundings = DecoratorFactory::getInstance().decorate(scnxArchive);
                    if (surroundings != NULL) {
                        surroundings->setNodeDescriptor(NodeDescriptor("Surroundings"));
                        m_stationaryElements->addChild(surroundings);
                    }
                }

                /*******************************************************************/
                /* Dynamic elements.                                               */
                /*******************************************************************/
                const URL urlOfCar(getPlugIn().getKeyValueConfiguration().getValue<string>("global.car"));
                if (urlOfCar.isValid()) {
                    string objxModel(urlOfCar.getResource());
                    cout << "Opening file stream to car model " << objxModel << endl;
                    fstream fin(objxModel.c_str(), ios::in | ios::binary);
                    if (fin.good()) {
                        cout << "Loading car model" << endl;
                        OBJXArchive *objxArchive = OBJXArchiveFactory::getInstance().getOBJXArchive(fin);

                        fin.close();
                        if (objxArchive != NULL) {
                            // Decorate objxArchive for getting displayed in an OpenGL scene.
                            m_egoStateNodeDescriptor = NodeDescriptor("EgoCar");
                            m_listOfCameraAssignableNodes.push_back(m_egoStateNodeDescriptor);
                            m_egoStateNode = objxArchive->createTransformGroup(m_egoStateNodeDescriptor);
                        }

                        if (m_egoStateNode == NULL) {
                            OPENDAVINCI_CORE_THROW_EXCEPTION(InvalidArgumentException, "Could not load car model");
                        }
                        else {
                            m_dynamicElements->addChild(m_egoStateNode);

                            // EgoCar is traceable.
                            NodeDescriptor traceableNodeDescriptor = NodeDescriptor("EgoCar (Trace)");
                            TransformGroup *traceableNode = new TransformGroup(traceableNodeDescriptor);
                            m_mapOfTraceablePositions[traceableNodeDescriptor] = traceableNode;
                            m_dynamicElements->addChild(traceableNode);
                        }
                    }
                }

                m_plannedRoute = new TransformGroup(NodeDescriptor("Planned Route"));
                m_dynamicElements->addChild(m_plannedRoute);

                m_lines = new TransformGroup(NodeDescriptor("Individual Lines"));
                m_dynamicElements->addChild(m_lines);

                /*******************************************************************/
                /* Measurements.                                                   */
                /*******************************************************************/
                // Create node for showing contoured objects.
                m_contouredObjectsNode = new TransformGroup(NodeDescriptor("Contoured Objects"));
                m_measurements->addChild(m_contouredObjectsNode);

                m_obstaclesRoot = new TransformGroup(NodeDescriptor("Obstacles"));
                m_measurements->addChild(m_obstaclesRoot);
            }

            void EnvironmentViewerGLWidget::initScene() {
                // Setup scene graph.
                createSceneGraph();

                // Setup selectable scene graph.
                m_selectableNodeDescriptorTree = new TreeNode<SelectableNodeDescriptor>();
                TreeNode<SelectableNodeDescriptor> *stationaryElements = new TreeNode<SelectableNodeDescriptor>();
                stationaryElements->setValue(SelectableNodeDescriptor(NodeDescriptor("Stationary Elements"), false));
                m_selectableNodeDescriptorTree->addChild(stationaryElements);

                TreeNode<SelectableNodeDescriptor> *dynamicElements = new TreeNode<SelectableNodeDescriptor>();
                dynamicElements->setValue(SelectableNodeDescriptor(NodeDescriptor("Dynamic Elements"), false));
                m_selectableNodeDescriptorTree->addChild(dynamicElements);

                TreeNode<SelectableNodeDescriptor> *measuredData = new TreeNode<SelectableNodeDescriptor>();
                measuredData->setValue(SelectableNodeDescriptor(NodeDescriptor("Measured Data"), false));
                m_selectableNodeDescriptorTree->addChild(measuredData);

                // Collect data from scene graph using a visitor.
                TreeNodeVisitor tnvStationaryElements(m_renderingConfiguration, stationaryElements);
                m_stationaryElements->accept(tnvStationaryElements);

                TreeNodeVisitor tnvDynamicElements(m_renderingConfiguration, dynamicElements);
                m_dynamicElements->accept(tnvDynamicElements);

                TreeNodeVisitor tnvMeasuredData(m_renderingConfiguration, measuredData);
                m_measurements->accept(tnvMeasuredData);

                // Inform listener about selectable NodeDescriptors.
                m_selectableNodeDescriptorTreeListener.update(m_selectableNodeDescriptorTree);

                // Inform listener about assignable nodes.
                m_cameraAssignableNodesListener.updateListOfCameraAssignableNodes(m_listOfCameraAssignableNodes);
            }

            void EnvironmentViewerGLWidget::assignCameraTo(const NodeDescriptor &nd) {
                Lock l(m_rootMutex);
                m_cameraAssignedNodeDescriptor = nd;
            }

            void EnvironmentViewerGLWidget::setupOpenGL() {
                glEnable(GL_LIGHTING);

                glEnable(GL_LIGHT0);
                float light0Position[4] = {0, 0, 20, 0};
                float light0Ambient[4] = {0.5f, 0.5f, 0.5f, 0};
                float light0Diffuse[4] = {0.8f, 0.8f, 0.8f, 0};
                float light0Specular[4] = {0, 0, 0, 0};
                glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
                glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, light0Specular);
            }

            void EnvironmentViewerGLWidget::drawScene() {
                if (m_root != NULL) {
                    Lock l(m_rootMutex);

                    if (m_cameraAssignedNodeDescriptor.getName().size() > 0) {
                        Position assignedNode = m_mapOfCurrentPositions[m_cameraAssignedNodeDescriptor];
                        Point3 positionCamera;
                        Point3 lookAtPointCamera;
                        Point3 dirCamera(-10, 0, 0);
                        dirCamera.rotateZ(assignedNode.getRotation().getAngleXY());
                        positionCamera.setX(assignedNode.getPosition().getX() + dirCamera.getX());
                        positionCamera.setY(assignedNode.getPosition().getY() + dirCamera.getY());
                        positionCamera.setZ(10);

                        lookAtPointCamera.setX(assignedNode.getPosition().getX());
                        lookAtPointCamera.setY(assignedNode.getPosition().getY());
                        lookAtPointCamera.setZ(0);

                        glPushMatrix();
                            glLoadIdentity();

                            // Setup camera.
                            gluLookAt(positionCamera.getX(), positionCamera.getY(), positionCamera.getZ(),
                                      lookAtPointCamera.getX(), lookAtPointCamera.getY(), lookAtPointCamera.getZ(),
                                      0, 0, 1);

                            // Draw scene.

                            m_root->render(m_renderingConfiguration);
                        glPopMatrix();
                    }
                    else {
                        m_root->render(m_renderingConfiguration);
                    }
    /*
                    {
                        // Visualize camera using quaternions.
                        Position assignedNode = m_mapOfCurrentPositions[m_egoStateNodeDescriptor];
                        Point3 positionCamera(-10, 0, 8);

                        const double rotX = -cartesian::Constants::PI/2.0; // -90°
                        const double rotZ = assignedNode.getRotation().getAngleXY();

                        Quaternion qX;
                        qX.transform(rotX, Point3(1, 0, 0));

                        Quaternion qZ;
                        qZ.transform(rotZ, Point3(0, 0, 1));

                        Quaternion q;
                        q = qZ * qX;

                        positionCamera = positionCamera * qZ.transformToMatrix3x3();
                        positionCamera += assignedNode.getPosition();

                        Point3 up(0, 1, 0);
                        up = up * qZ.transformToMatrix3x3();

                        Point3 lookAtPointCamera;
                        lookAtPointCamera.setX(assignedNode.getPosition().getX());
                        lookAtPointCamera.setY(assignedNode.getPosition().getY());
                        lookAtPointCamera.setZ(0);

                        // Draw view direction.
                        glColor3f(0, 1, 0);
                        glBegin(GL_LINES);
                            glVertex3d(positionCamera.getX(), positionCamera.getY(), positionCamera.getZ());
                            glVertex3d(lookAtPointCamera.getX(), lookAtPointCamera.getY(), lookAtPointCamera.getZ());
                        glEnd();

                        // Draw up direction.
                        glColor3f(0, 0, 1);
                        glBegin(GL_LINES);
                            glVertex3d(positionCamera.getX(), positionCamera.getY(), positionCamera.getZ());
                            glVertex3d(positionCamera.getX()+5*up.getX(), positionCamera.getY()+5*up.getY(), positionCamera.getZ()+5*up.getZ());
                        glEnd();
                    }
                }
    */
    /*
                    {
                        // Visualize parallel scanner.
                        Position assignedNode = m_mapOfCurrentPositions[m_egoStateNodeDescriptor];
                        Point3 positionParallelScanner(0, 0, 1.65);

                        const double rotZ = assignedNode.getRotation().getAngleXY();

                        Quaternion qZ;
                        qZ.transform(rotZ, Point3(0, 0, 1));

                        positionParallelScanner = positionParallelScanner * qZ.transformToMatrix3x3();
                        positionParallelScanner += assignedNode.getPosition();

                        Point3 up(0, 1, 0);
                        up = up * qZ.transformToMatrix3x3();

                        Point3 lookAtPointCamera(15, 0, 0);
                        lookAtPointCamera.rotateZ(rotZ);
                        lookAtPointCamera += assignedNode.getPosition();
                        lookAtPointCamera.setZ(0);

                        // Draw view direction.
                        glColor3f(0, 1, 0);
                        glBegin(GL_LINES);
                            glVertex3d(positionParallelScanner.getX(), positionParallelScanner.getY(), positionParallelScanner.getZ());
                            glVertex3d(lookAtPointCamera.getX(), lookAtPointCamera.getY(), lookAtPointCamera.getZ());
                        glEnd();

                        // Draw up direction.
                        glColor3f(0, 0, 1);
                        glBegin(GL_LINES);
                            glVertex3d(positionParallelScanner.getX(), positionParallelScanner.getY(), positionParallelScanner.getZ());
                            glVertex3d(positionParallelScanner.getX()+5*up.getX(), positionParallelScanner.getY()+5*up.getY(), positionParallelScanner.getZ()+5*up.getZ());
                        glEnd();
                    }
    */
                }
            }

            void EnvironmentViewerGLWidget::update(TreeNode<SelectableNodeDescriptor> *node) {
                Lock l(m_rootMutex);
                if (node != NULL) {
                    modifyRenderingConfiguration(node);
                }
            }

            void EnvironmentViewerGLWidget::modifyRenderingConfiguration(TreeNode<SelectableNodeDescriptor> *node) {
                if (node != NULL) {
                    NodeDescriptor nd = node->getValue().getNodeDescriptor();
                    NodeRenderingConfiguration nrc = m_renderingConfiguration.getNodeRenderingConfiguration(nd);
                    nrc.setParameter(NodeRenderingConfiguration::ENABLED, node->getValue().isSelected());
                    m_renderingConfiguration.setNodeRenderingConfiguration(nd, nrc);

                    vector<TreeNode<SelectableNodeDescriptor>* > childrenOfNode = node->getChildren();
                    vector<TreeNode<SelectableNodeDescriptor>* >::iterator it = childrenOfNode.begin();
                    while (it != childrenOfNode.end()) {
                        TreeNode<SelectableNodeDescriptor> *child = (*it++);
                        modifyRenderingConfiguration(child);
                    }
                }
            }

            void EnvironmentViewerGLWidget::nextContainer(Container &c) {
                if (c.getDataType() == opendlv::data::environment::EgoState::ID()) {
                    m_numberOfReceivedEgoStates++;

                    if (m_egoStateNode != NULL) {
                        Lock l(m_rootMutex);
                        EgoState egostate = c.getData<EgoState>();
                        Point3 dir(0, 0, egostate.getRotation().getAngleXY());
                        m_egoStateNode->setRotation(dir);
                        m_egoStateNode->setTranslation(egostate.getPosition());

                        Position egoPosition;
                        egoPosition.setPosition(egostate.getPosition());
                        egoPosition.setRotation(egostate.getRotation());
                        m_mapOfCurrentPositions[m_egoStateNodeDescriptor] = egoPosition;

                        if ( (m_numberOfReceivedEgoStates % 30) == 0 ) {
                            NodeDescriptor nd("EgoCar (Trace)");
                            TransformGroup *tg = m_mapOfTraceablePositions[nd];
                            if (tg != NULL) {
                                Point3 color(0, 0, 1);
                                opendlv::threeD::models::Point *p = new opendlv::threeD::models::Point(NodeDescriptor("Trace"), egostate.getPosition(), color, 5);
                                tg->addChild(p);
                            }
                        }
                    }
                }
                if (c.getDataType() == ContouredObjects::ID()) {
                    if (m_contouredObjectsNode != NULL) {
                        Lock l(m_rootMutex);
                        ContouredObjects cos = c.getData<ContouredObjects>();
                        vector<ContouredObject> listOfContouredObjects = cos.getContouredObjects();
                        vector<ContouredObject>::iterator it = listOfContouredObjects.begin();
                        m_contouredObjectsNode->deleteAllChildren();
                        while (it != listOfContouredObjects.end()) {
                            vector<Point3> contour = (*it).getContour();
                            vector<Point3>::iterator jt = contour.begin();
                            while (jt != contour.end()) {
                                m_contouredObjectsNode->addChild(new opendlv::threeD::models::Point(NodeDescriptor("Point"), (*jt), Point3(1, 0, 0), 2));
                                jt++;
                            }
                            it++;
                        }
                    }
                }
                if (c.getDataType() == opendlv::data::planning::Route::ID()) {
                    if (m_plannedRoute != NULL) {
                        Lock l(m_rootMutex);
                        Route r = c.getData<Route>();
                        vector<Point3> listOfVertices = r.getListOfPoints();
                        const uint32_t SIZE = listOfVertices.size();
                        if (SIZE > 0) {
                            m_plannedRoute->deleteAllChildren();
                            for (uint32_t i = 0; i < SIZE - 1; i++) {
                                Point3 posA = listOfVertices.at(i);
                                posA.setZ(0.05);

                                Point3 posB = listOfVertices.at(i+1);
                                posB.setZ(0.05);

                                m_plannedRoute->addChild(new opendlv::threeD::models::Line(NodeDescriptor(), posA, posB, Point3(0, 1, 0), 6));
                            }
                        }
                    }
                }
                if (c.getDataType() == opendlv::data::environment::Line::ID()) {
                    if (m_lines != NULL) {
                        Lock l(m_rootMutex);
                        opendlv::data::environment::Line line = c.getData<Line>();

                        Point3 posA = line.getA();
                        posA.setZ(0.05);

                        Point3 posB = line.getB();
                        posB.setZ(0.05);

                        m_lines->addChild(new opendlv::threeD::models::Line(NodeDescriptor(), posA, posB, Point3(1, 0, 0), 6));
                    }
                }
                if (c.getDataType() == opendlv::data::environment::Obstacle::ID()) {
                    if (m_obstaclesRoot != NULL) {
                        Lock l(m_rootMutex);
                        Obstacle obstacle = c.getData<Obstacle>();
                        switch (obstacle.getState()) {
                            case Obstacle::REMOVE:
                            {
                                // Remove obstacle.
                                map<uint32_t, Node*>::iterator result = m_mapOfObstacles.find(obstacle.getObstacleID());
                                if (result != m_mapOfObstacles.end()) {
                                    // Remove child from scene graph node.
                                    m_obstaclesRoot->removeChild(result->second);

                                    // Remove entry from map.
                                    m_mapOfObstacles.erase(result);
                                }
                            }
                            break;

                            case Obstacle::UPDATE:
                            {
                                map<uint32_t, Node*>::iterator result = m_mapOfObstacles.find(obstacle.getObstacleID());
                                if (result != m_mapOfObstacles.end()) {
                                    // Remove child from scene graph node.
                                    m_obstaclesRoot->removeChild(result->second);

                                    // Remove entry from map.
                                    m_mapOfObstacles.erase(result);
                                }
                                // Update obstacle.
                                TransformGroup *contourTG = new TransformGroup();
                                vector<Point3> contour = obstacle.getPolygon().getVertices();
                                // Close polygons.
                                Point3 p = contour.at(0);
                                contour.push_back(p);
                                for (uint32_t k = 0; k < contour.size() - 1; k++) {
                                    Point3 A = contour.at(k); A.setZ(0.5);
                                    Point3 B = contour.at(k+1); B.setZ(0.5);

                                    contourTG->addChild(new opendlv::threeD::models::Line(NodeDescriptor(), A, B, Point3(0, 1, 0), 2));
                                }
                                m_mapOfObstacles[obstacle.getObstacleID()] = contourTG;
                                m_obstaclesRoot->addChild(contourTG);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
} // plugins::environmentviewer
