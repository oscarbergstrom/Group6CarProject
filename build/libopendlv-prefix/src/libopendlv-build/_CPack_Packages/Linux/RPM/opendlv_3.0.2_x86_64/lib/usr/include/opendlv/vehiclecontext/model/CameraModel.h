/**
 * libvehiclecontext - Models for simulating automotive systems.
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef VEHICLECONTEXT_MODEL_CAMERAMODEL_H_
#define VEHICLECONTEXT_MODEL_CAMERAMODEL_H_

#include <string>

#include <opencv/cv.h>

#include <memory>
#include "opendavinci/odcore/base/Condition.h"
#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h"
#include "opendlv/core/wrapper/Image.h"
#include "opendavinci/odcontext/base/SystemFeedbackComponent.h"

#include "opendlv/data/environment/EgoState.h"
#include "opendlv/io/camera/ImageGrabber.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

#include "OpenGLGrabber.h"


namespace opendlv { namespace vehiclecontext {
    namespace model {

        using namespace std;

        /**
         * This class realizes a virtual camera feed model.
         */
        class OPENDAVINCI_API CameraModel : public odcontext::base::SystemFeedbackComponent {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 */
                CameraModel(const CameraModel&);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 */
                CameraModel& operator=(const CameraModel&);

            public:
                /**
                 * Constructor to create a CameraModel.
                 *
                 * @param configuration Configuration data.
                 */
                CameraModel(const string &configuration);

                /**
                 * Constructor to create a CameraModel. This constructor overrides
                 * any specified frequency in the configuration.
                 *
                 * @param freq Desired runtime frequency.
                 * @param configuration Configuration data.
                 */
                CameraModel(const float &freq, const string &configuration);

                virtual ~CameraModel();

                virtual float getFrequency() const;

                virtual void setup();

                virtual void tearDown();

                virtual void step(const odcore::wrapper::Time &t, odcontext::base::SendContainerToSystemsUnderTest &sender);

            private:
                odcore::base::KeyValueConfiguration m_kvc;
                float m_freq;

            public:
                /**
                 * This method returns the singleton instance.
                 *
                 * @return The singleton pointer.
                 */
                static CameraModel& getInstance();

                /**
                 * This method is called by OpenGL to draw the scene.
                 */
                void display();

                /**
                 * This method is called by OpenGL for resizing
                 * the window.
                 *
                 * @param w new width.
                 * @param h new height.
                 */
                void resize(int32_t w, int32_t h);

            private:
                static CameraModel* m_singleton;
                int m_renderWindow;
                opendlv::data::environment::EgoState m_egoState;
                OpenGLGrabber *m_grabber;
                std::shared_ptr<core::wrapper::Image> m_image;
                odcore::data::image::SharedImage m_sharedImage;

                float m_translationX;
                float m_translationY;
                float m_scale;
                float m_phi;
                float m_theta;
                float m_height;

                int32_t m_mouseX;
                int32_t m_mouseY;
                int32_t m_mouseButton;

                /**
                 * This method initializes the GLUT subsystem.
                 */
                void initGlut();

                /**
                 * This method initializes the GL subsystem.
                 */
                void initGL();

                /**
                 * This method draws the three dimensional scene.
                 */
                void drawScene();
        };

    }
} } // opendlv::vehiclecontext::model

#endif /*VEHICLECONTEXT_MODEL_CAMERAMODEL_H_*/
