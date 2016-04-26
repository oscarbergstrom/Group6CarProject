/**
 * egocontroller - Manual control of the vehicle (part of simulation environment)
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

#ifndef EGOCONTROLLER_SIMPLECONTROLBEHAVIOUR_H_
#define EGOCONTROLLER_SIMPLECONTROLBEHAVIOUR_H_

#include "ControlBehaviour.h"
#include "opendavinci/odcore/data/TimeStamp.h"
#include "opendlv/data/environment/EgoState.h"
#include "opendlv/data/environment/Point3.h"

namespace egocontroller {

    using namespace std;

    class SimpleControlBehaviour : public ControlBehaviour
    {
        public:
            SimpleControlBehaviour(const opendlv::data::environment::Point3 &translation, const double &rotZ);
            virtual ~SimpleControlBehaviour();

            virtual void accelerate(const double& value);
            virtual void brake(const double& value);
            virtual void turnLeft(const double& value);
            virtual void turnRight(const double& value);
            virtual void stop();
            virtual opendlv::data::environment::EgoState computeEgoState();

        protected:
            odcore::data::TimeStamp m_previousTime;
            opendlv::data::environment::Point3 m_oldPosition;
            opendlv::data::environment::Point3 m_orientation;

            double m_angle;
            double m_speed;
            double m_speedInLastCycle;
    };
}

#endif // EGOCONTROLLER_SIMPLECONTROLBEHAVIOUR_H_
