/**
 * odcanmapper - Tool for mapping GenericCANMessages to
 *             high-level C++ data structures and vice-versa
 * Copyright (C) 2015 Christian Berger
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

#ifndef CANMAPPERTESTSUITE_H_
#define CANMAPPERTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <memory>
#include <opendavinci/odcore/reflection/Message.h>
#include <opendavinci/odcore/reflection/MessageToVisitableVisitor.h>
#include <opendavinci/odcore/reflection/MessagePrettyPrinterVisitor.h>

#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

// Include local header files.
#include "../include/CanMapper.h"

using namespace std;
using namespace odcore;
using namespace odcore::data;
using namespace automotive::odcantools;
using namespace odcore::reflection;
using namespace odcore::data::reflection;

/**
 * This class derives from SensorBoard to allow access to protected methods.
 */
class CanMapperTestling : public CanMapper {
    private:
        CanMapperTestling();
    
    public:
        CanMapperTestling(const int32_t &argc, char **argv) :
            CanMapper(argc, argv) {}

        // Here, you need to add all methods which are protected in CanMapper and which are needed for the test cases.
};

/**
 * The actual testsuite starts here.
 */
class CanMapperTest : public CxxTest::TestSuite {
    private:
        CanMapperTestling *dt;

    public:
        /**
         * This method will be called before each testXYZ-method.
         */
        void setUp() {
            // Prepare the data that would be available from commandline.
            string argv0("canmapper");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            // Create an instance of sensorboard through SensorBoardTestling which will be deleted in tearDown().
            dt = new CanMapperTestling(argc, argv);
        }

        /**
         * This method will be called after each testXYZ-method.
         */
        void tearDown() {
            delete dt;
            dt = NULL;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // Below this line the actual testcases are defined.
        ////////////////////////////////////////////////////////////////////////////////////

        void testCanMapperSuccessfullyCreated() {
            TS_ASSERT(dt != NULL);
        }

        void testWheelSpeed() {
            automotive::vehicle::WheelSpeed wheelSpeed;
            wheelSpeed.setFrontLeft(1.2);
            wheelSpeed.setFrontRight(2.2);
            wheelSpeed.setRearLeft(3.2);
            wheelSpeed.setRearRight(4.2);

            stringstream sstr;
            sstr << wheelSpeed;

            automotive::vehicle::WheelSpeed wheelSpeed2;
            sstr >> wheelSpeed2;

            TS_ASSERT_DELTA(wheelSpeed.getFrontLeft(), wheelSpeed2.getFrontLeft(), 1e-3);
            TS_ASSERT_DELTA(wheelSpeed.getFrontRight(), wheelSpeed2.getFrontRight(), 1e-3);
            TS_ASSERT_DELTA(wheelSpeed.getRearLeft(), wheelSpeed2.getRearLeft(), 1e-3);
            TS_ASSERT_DELTA(wheelSpeed.getRearRight(), wheelSpeed2.getRearRight(), 1e-3);

            TS_ASSERT_DELTA(wheelSpeed2.getFrontLeft(), 1.2, 1e-3);
            TS_ASSERT_DELTA(wheelSpeed2.getFrontRight(), 2.2, 1e-3);
            TS_ASSERT_DELTA(wheelSpeed2.getRearLeft(), 3.2, 1e-3);
            TS_ASSERT_DELTA(wheelSpeed2.getRearRight(), 4.2, 1e-3);
        }

        void testMessageToWheelSpeed() {
            Message message;

            // First, we transfer all fields as double into a generic message.
            {
                uint16_t _wheel1 = 11;

                const double SCALE = 0.01;
                double v = _wheel1 * SCALE;

                Field<double> *f = new Field<double>(v);
                f->setLongFieldIdentifier(1); // The identifiers specified here must match with the ones defined in the .odvd file!
                f->setShortFieldIdentifier(1); // The identifiers specified here must match with the ones defined in the .odvd file!
                f->setLongFieldName("WheelSpeed.frontLeft");
                f->setShortFieldName("frontLeft");
                f->setFieldDataType(odcore::data::reflection::AbstractField::DOUBLE_T);
                f->setSize(sizeof(v));

                message.addField(std::shared_ptr<AbstractField>(f));
            }

            {
                uint16_t _wheel2 = 12;

                const double SCALE = 0.01;
                double v = _wheel2 * SCALE;

                Field<double> *f = new Field<double>(v);
                f->setLongFieldIdentifier(2); // The identifiers specified here must match with the ones defined in the .odvd file!
                f->setShortFieldIdentifier(2); // The identifiers specified here must match with the ones defined in the .odvd file!
                f->setLongFieldName("WheelSpeed.frontRight");
                f->setShortFieldName("frontRight");
                f->setFieldDataType(odcore::data::reflection::AbstractField::DOUBLE_T);
                f->setSize(sizeof(v));

                message.addField(std::shared_ptr<AbstractField>(f));
            }

            {
                uint16_t _wheel3 = 13;

                const double SCALE = 0.01;
                double v = _wheel3 * SCALE;

                Field<double> *f = new Field<double>(v);
                f->setLongFieldIdentifier(3); // The identifiers specified here must match with the ones defined in the .odvd file!
                f->setShortFieldIdentifier(3); // The identifiers specified here must match with the ones defined in the .odvd file!
                f->setLongFieldName("WheelSpeed.rearLeft");
                f->setShortFieldName("rearLeft");
                f->setFieldDataType(odcore::data::reflection::AbstractField::DOUBLE_T);
                f->setSize(sizeof(v));

                message.addField(std::shared_ptr<AbstractField>(f));
            }

            {
                uint16_t _wheel4 = 15;

                const double SCALE = 0.01;
                double v = _wheel4 * SCALE;

                Field<double> *f = new Field<double>(v);
                f->setLongFieldIdentifier(4); // The identifiers specified here must match with the ones defined in the .odvd file!
                f->setShortFieldIdentifier(4); // The identifiers specified here must match with the ones defined in the .odvd file!
                f->setLongFieldName("WheelSpeed.rearRight");
                f->setShortFieldName("rearRight");
                f->setFieldDataType(odcore::data::reflection::AbstractField::DOUBLE_T);
                f->setSize(sizeof(v));

                message.addField(std::shared_ptr<AbstractField>(f));
            }

            MessageToVisitableVisitor mtvv(message);

            // However, the high-level message as floats as data type and not double;
            // thus, we let the MessageToVisitableVisitor do the implicit compiler cast for us.
            automotive::vehicle::WheelSpeed wheelSpeed;

            wheelSpeed.accept(mtvv);

            {
                // Optional: Showing how to use the MessagePrettyPrinterVisitor to print the content of the unnamed message.
                MessagePrettyPrinterVisitor mppv;
                message.accept(mppv);
                mppv.getOutput(cout);

                // Optional: Just print the content for convenience purposes.
                cout << wheelSpeed.toString() << endl;
            }

            // Do the validation.
            TS_ASSERT_DELTA(wheelSpeed.getFrontLeft(), 0.11, 1e-3);
            TS_ASSERT_DELTA(wheelSpeed.getFrontRight(), 0.12, 1e-3);
            TS_ASSERT_DELTA(wheelSpeed.getRearLeft(), 0.13, 1e-3);
            TS_ASSERT_DELTA(wheelSpeed.getRearRight(), 0.15, 1e-3);
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // Below this line the necessary constructor for initializing the pointer variables,
        // and the forbidden copy constructor and assignment operator are declared.
        //
        // These functions are normally not changed.
        ////////////////////////////////////////////////////////////////////////////////////

    public:
        /**
         * This constructor is only necessary to initialize the pointer variable.
         */
        CanMapperTest() : dt(NULL) {}

    private:
        /**
         * "Forbidden" copy constructor. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the copy constructor.
         *
         * @param obj Reference to an object of this class.
         */
        CanMapperTest(const CanMapperTest &/*obj*/);

        /**
         * "Forbidden" assignment operator. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the assignment operator.
         *
         * @param obj Reference to an object of this class.
         * @return Reference to this instance.
         */
        CanMapperTest& operator=(const CanMapperTest &/*obj*/);

};

#endif /*CANMAPPERTESTSUITE_H_*/

