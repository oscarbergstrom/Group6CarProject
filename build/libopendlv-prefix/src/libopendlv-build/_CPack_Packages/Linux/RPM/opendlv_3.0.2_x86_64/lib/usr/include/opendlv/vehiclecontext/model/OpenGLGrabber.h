/**
 * camgen - Camera stream generator (part of simulation environment)
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

#ifndef VEHICLECONTEXT_MODEL_OPENGLGRABBER_H_
#define VEHICLECONTEXT_MODEL_OPENGLGRABBER_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <memory>
#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendlv/core/wrapper/Image.h"
#include "opendavinci/odcore/wrapper/SharedMemory.h"

#include "opendlv/data/environment/EgoState.h"
#include "opendlv/io/camera/ImageGrabber.h"
#include "opendlv/threeD/TransformGroup.h"

namespace opendlv { namespace vehiclecontext {
    namespace model {

        using namespace std;

        /**
         * This class implements a grabber providing images from
         * a given OpenGL scene.
         */
        class OpenGLGrabber : public opendlv::io::camera::ImageGrabber {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 *
                 * @param obj Reference to an object of this class.
                 */
                OpenGLGrabber(const OpenGLGrabber &);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 *
                 * @param obj Reference to an object of this class.
                 * @return Reference to this instance.
                 */
                OpenGLGrabber& operator=(const OpenGLGrabber &);

            public:
                /**
                 * Constructor.
                 *
                 * @param kvc KeyValueConfiguration.
                 * @param imageGrabberID Identifier for this image grabber.
                 * @param imageGrabberCalibration Calibration information for this grabber.
                 * @param egoState Current ego state.
                 */
                OpenGLGrabber(const odcore::base::KeyValueConfiguration &kvc,
                              const opendlv::data::camera::ImageGrabberID &imageGrabberID,
                              const opendlv::data::camera::ImageGrabberCalibration &imageGrabberCalibration);

                virtual ~OpenGLGrabber();

                virtual void delay();

                virtual std::shared_ptr<core::wrapper::Image> getNextImage();

            private:
                odcore::base::KeyValueConfiguration m_kvc;
                std::shared_ptr<core::wrapper::Image> m_image;
                std::shared_ptr<odcore::wrapper::SharedMemory> m_sharedMemory;
                std::shared_ptr<opendlv::threeD::TransformGroup> m_root;
        };

    }
} } // opendlv::vehiclecontext::model

#endif /*VEHICLECONTEXT_MODEL_OPENGLGRABBER_H_*/
