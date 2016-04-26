/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef FROM_OPENDLV_PROXY_REVEREFH16_BRAKEREQUEST_H
#define FROM_OPENDLV_PROXY_REVEREFH16_BRAKEREQUEST_H

#include "opendavinci/odcore/opendavinci.h"


#include "opendavinci/odcore/base/Visitable.h"
#include "opendavinci/odcore/data/SerializableData.h"


namespace from {
	namespace opendlv {
		namespace proxy {
			namespace reverefh16 {
				using namespace std;
				
				class BrakeRequest : public odcore::data::SerializableData, public odcore::base::Visitable {
					public:
						BrakeRequest();
				
						BrakeRequest(
						const bool &val0, 
						const double &val1
						);
				
						virtual ~BrakeRequest();
				
						/**
						 * Copy constructor.
						 *
						 * @param obj Reference to an object of this class.
						 */
						BrakeRequest(const BrakeRequest &obj);
				
						/**
						 * Assignment operator.
						 *
						 * @param obj Reference to an object of this class.
						 * @return Reference to this instance.
						 */
						BrakeRequest& operator=(const BrakeRequest &obj);
				
					public:
						/**
						 * @return enable_brakerequest.
						 */
						bool getEnable_brakerequest() const;
						
						/**
						 * This method sets enable_brakerequest.
						 *
						 * @param val Value for enable_brakerequest.
						 */
						void setEnable_brakerequest(const bool &val);
					public:
						/**
						 * @return brakerequest.
						 */
						double getBrakerequest() const;
						
						/**
						 * This method sets brakerequest.
						 *
						 * @param val Value for brakerequest.
						 */
						void setBrakerequest(const double &val);
				
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
						bool m_enable_brakerequest;
					private:
						double m_brakerequest;
				
				};
			} // reverefh16
		} // proxy
	} // opendlv
} // from

#endif /*FROM_OPENDLV_PROXY_REVEREFH16_BRAKEREQUEST_H*/
