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

#include <sstream>
#include <string>

#include <memory>
#include "opendavinci/odcore/base/Deserializer.h"
#include "opendavinci/odcore/base/Hash.h"
#include "opendavinci/odcore/base/SerializationFactory.h"
#include "opendavinci/odcore/base/Serializer.h"
#include "automotivedata/generated/cartesian/Matrix3x3.h"
#include "opendlv/data/camera/ExtrinsicParameters.h"
#include "opendlv/data/environment/Point3.h"

namespace opendlv {
    namespace data {
        namespace camera {

            using namespace std;
            using namespace odcore::base;
            using namespace opendlv::data::environment;

            ExtrinsicParameters::ExtrinsicParameters() :
                    m_translation(),
                    m_rotation() {}

            ExtrinsicParameters::ExtrinsicParameters(const Point3 &translation, const cartesian::Matrix3x3 &rotation) :
                    m_translation(translation),
                    m_rotation(rotation) {}

            ExtrinsicParameters::ExtrinsicParameters(const ExtrinsicParameters &obj) :
                    m_translation(obj.getTranslation()),
                    m_rotation(obj.getRotation()) {}

            ExtrinsicParameters::~ExtrinsicParameters() {}

            ExtrinsicParameters& ExtrinsicParameters::operator=(const ExtrinsicParameters &obj) {
                m_translation = obj.getTranslation();
                m_rotation = obj.getRotation();

                return (*this);
            }

            const Point3 ExtrinsicParameters::getTranslation() const {
                return m_translation;
            }

            void ExtrinsicParameters::setTranslation(const Point3 &translation) {
                m_translation = translation;
            }

            const cartesian::Matrix3x3 ExtrinsicParameters::getRotation() const {
                return m_rotation;
            }

            void ExtrinsicParameters::setRotation(const cartesian::Matrix3x3 &rotation) {
                m_rotation = rotation;
            }

            ostream& ExtrinsicParameters::operator<<(ostream &out) const {
                SerializationFactory& sf=SerializationFactory::getInstance();

                std::shared_ptr<Serializer> s = sf.getSerializer(out);

                s->write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('t', 'r', 'a', 'n', 's') >::RESULT,
                        m_translation);

                s->write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL3('r', 'o', 't') >::RESULT,
                        m_rotation);

                return out;
            }

            istream& ExtrinsicParameters::operator>>(istream &in) {
                SerializationFactory& sf=SerializationFactory::getInstance();

                std::shared_ptr<Deserializer> d = sf.getDeserializer(in);

                d->read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('t', 'r', 'a', 'n', 's') >::RESULT,
                       m_translation);

                d->read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL3('r', 'o', 't') >::RESULT,
                       m_rotation);

                return in;
            }

            int32_t ExtrinsicParameters::getID() const {
                return 21;
            }

            const string ExtrinsicParameters::getShortName() const {
                return "ExtrinsicParameters";
            }

            const string ExtrinsicParameters::getLongName() const {
                return "hesperia.data.camera.ExtrinsicParameters";
            }

            const string ExtrinsicParameters::toString() const {
                stringstream sstr;
                sstr << "T=" << m_translation.toString() << "/R=" << m_rotation.toString();
                return sstr.str();
            }

        }
    }
} // core::data::camera
