/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */

#include <memory>

#include "opendavinci/odcore/base/Hash.h"
#include "opendavinci/odcore/base/Deserializer.h"
#include "opendavinci/odcore/base/SerializationFactory.h"
#include "opendavinci/odcore/base/Serializer.h"


#include "opendavinci/generated/odcore/data/dmcp/Constants.h"

namespace odcore {
	namespace data {
		namespace dmcp {
				using namespace std;
				using namespace odcore::base;
			
			
				Constants::Constants() :
				    SerializableData(), Visitable()
				{
				}
			
			
				Constants::Constants(const Constants &obj) :
				    SerializableData(), Visitable()
				{
					(void)obj; // Avoid unused parameter warning.
				}
				
				Constants::~Constants() {
				}
			
				Constants& Constants::operator=(const Constants &obj) {
					(void)obj; // Avoid unused parameter warning.
					return (*this);
				}
			
				int32_t Constants::ID() {
					return 110;
				}
			
				const string Constants::ShortName() {
					return "Constants";
				}
			
				const string Constants::LongName() {
					return "odcore.data.dmcp.Constants";
				}
			
				int32_t Constants::getID() const {
					return Constants::ID();
				}
			
				const string Constants::getShortName() const {
					return Constants::ShortName();
				}
			
				const string Constants::getLongName() const {
					return Constants::LongName();
				}
			
			
				void Constants::accept(odcore::base::Visitor &v) {
					v.beginVisit();
					v.endVisit();
				}
			
				const string Constants::toString() const {
					stringstream s;
			
			
			
					return s.str();
				}
			
				ostream& Constants::operator<<(ostream &out) const {
			
					SerializationFactory& sf = SerializationFactory::getInstance();
			
					std::shared_ptr<Serializer> s = sf.getSerializer(out);(void)s; // Avoid unused variable warning.
			
					return out;
				}
			
				istream& Constants::operator>>(istream &in) {
			
					SerializationFactory& sf = SerializationFactory::getInstance();
			
					std::shared_ptr<Deserializer> d = sf.getDeserializer(in);(void)d; // Avoid unused variable warning.
			
					return in;
				}
		} // dmcp
	} // data
} // odcore
