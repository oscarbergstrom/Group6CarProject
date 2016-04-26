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

#ifndef HESPERIA_SCENEGRAPH_PRIMITIVES_POLYGON_H_
#define HESPERIA_SCENEGRAPH_PRIMITIVES_POLYGON_H_

#include <vector>

#include "opendavinci/odcore/opendavinci.h"

#include "opendlv/data/environment/Point3.h"
#include "opendlv/scenegraph/SceneNode.h"
#include "opendlv/scenegraph/SceneNodeDescriptor.h"

namespace opendlv {
    namespace scenegraph {
        namespace primitives {

            using namespace std;

            /**
             * This class represents a polygon.
             */
            class OPENDAVINCI_API Polygon : public SceneNode {
                public:
                    /**
                     * Constructor.
                     *
                     * @param sceneNodeDesciptor Description for this node.
                     * @param listOfGroundVertices List of vertices describing the shape of this polygon.
                     * @param color Color of this polygon.
                     * @param height Polygon's height.
                     */
                    Polygon(const SceneNodeDescriptor &sceneNodeDescriptor, const vector<opendlv::data::environment::Point3> &listOfGroundVertices, const opendlv::data::environment::Point3 &color, const float &height);

                    /**
                     * Copy constructor.
                     *
                     * @param obj Reference to an object of this class.
                     */
                    Polygon(const Polygon &obj);

                    /**
                     * Assignment operator.
                     *
                     * @param obj Reference to an object of this class.
                     * @return Reference to this instance.
                     */
                    Polygon& operator=(const Polygon &obj);

                    virtual ~Polygon();

                    /**
                     * @return List of ground vertices.
                     */
                    const vector<opendlv::data::environment::Point3>& getListOfGroundVertices() const;

                    /**
                     * @return Polygon's color.
                     */
                    const opendlv::data::environment::Point3& getColor() const;

                    /**
                     * @return Polygon's height.
                     */
                    float getHeight() const;

                private:
                    vector<opendlv::data::environment::Point3> m_listOfGroundVertices;
                    opendlv::data::environment::Point3 m_color;
                    float m_height;
            };

        }
    }
} // opendlv::scenegraph::primitives

#endif /*HESPERIA_SCENEGRAPH_PRIMITIVES_POLYGON_H_*/
