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

#ifndef HESPERIA_DATA_ENVIRONMENT_EGOSTATE_H_
#define HESPERIA_DATA_ENVIRONMENT_EGOSTATE_H_

#include "opendavinci/odcore/opendavinci.h"

#include "opendlv/data/environment/PointShapedObject.h"

namespace opendlv {
    namespace data {
        namespace environment {

            using namespace std;

            /**
             * This class contains all relevant data for the own car.
             */
            class OPENDAVINCI_API EgoState : public PointShapedObject {
                public:
                    EgoState();

                    /**
                     * Constructor.
                     *
                     * @param position Position.
                     * @param rotation Rotation.
                     * @param velocity Velocity.
                     * @param acceleration Acceleration.
                     */
                    EgoState(const Point3 &position, const Point3 &rotation,
                             const Point3 &velocity, const Point3 &acceleration);

                    /**
                     * Copy constructor.
                     *
                     * @param obj Reference to an object of this class.
                     */
                    EgoState(const EgoState &obj);

                    virtual ~EgoState();

                    /**
                     * Assignment operator.
                     *
                     * @param obj Reference to an object of this class.
                     * @return Reference to this instance.
                     */
                    EgoState& operator=(const EgoState &obj);

                    virtual ostream& operator<<(ostream &out) const;
                    virtual istream& operator>>(istream &in);

                    /**
                     * This method returns the message id.
                     *
                     * @return Message id.
                     */
                    static int32_t ID();
                    virtual int32_t getID() const;
                    virtual const string getShortName() const;
                    virtual const string getLongName() const;
                    virtual const string toString() const;
            };

        }
    }
} // opendlv::data::environment

#endif /*HESPERIA_DATA_ENVIRONMENT_EGOSTATE_H_*/
