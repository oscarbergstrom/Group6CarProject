/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef ODCORE_DATA_SHAREDDATA_H
#define ODCORE_DATA_SHAREDDATA_H

#include "opendavinci/odcore/opendavinci.h"


#include "opendavinci/odcore/base/Visitable.h"
#include "opendavinci/odcore/data/SerializableData.h"


namespace odcore {
	namespace data {
		using namespace std;
		
		class SharedData : public odcore::data::SerializableData, public odcore::base::Visitable {
			public:
				SharedData();
		
				SharedData(
				const std::string &val0, 
				const uint32_t &val1
				);
		
				virtual ~SharedData();
		
				/**
				 * Copy constructor.
				 *
				 * @param obj Reference to an object of this class.
				 */
				SharedData(const SharedData &obj);
		
				/**
				 * Assignment operator.
				 *
				 * @param obj Reference to an object of this class.
				 * @return Reference to this instance.
				 */
				SharedData& operator=(const SharedData &obj);
		
			public:
				/**
				 * @return name.
				 */
				std::string getName() const;
				
				/**
				 * This method sets name.
				 *
				 * @param val Value for name.
				 */
				void setName(const std::string &val);
			public:
				/**
				 * @return size.
				 */
				uint32_t getSize() const;
				
				/**
				 * This method sets size.
				 *
				 * @param val Value for size.
				 */
				void setSize(const uint32_t &val);
		
			public:
				/**
				 * This method returns the message id.
				 *
				 * @return Message id.
				 */
				static int32_t ID();
		
				/**
				 * This method returns the short message name.
				 *
				 * @return Short message name.
				 */
				static const string ShortName();
		
				/**
				 * This method returns the long message name include package/sub structure.
				 *
				 * @return Long message name.
				 */
				static const string LongName();
		
				/**
				 * This method returns the message id.
				 *
				 * @return Message id.
				 */
				virtual int32_t getID() const;
		
				/**
				 * This method returns the short message name.
				 *
				 * @return Short message name.
				 */
				virtual const string getShortName() const;
		
				/**
				 * This method returns the long message name include package/sub structure.
				 *
				 * @return Long message name.
				 */
				virtual const string getLongName() const;
		
			public:
				virtual void accept(odcore::base::Visitor &v);
		
				virtual ostream& operator<<(ostream &out) const;
				virtual istream& operator>>(istream &in);
		
				virtual const string toString() const;
		
			private:
				std::string m_name;
			private:
				uint32_t m_size;
		
		};
	} // data
} // odcore

#endif /*ODCORE_DATA_SHAREDDATA_H*/
