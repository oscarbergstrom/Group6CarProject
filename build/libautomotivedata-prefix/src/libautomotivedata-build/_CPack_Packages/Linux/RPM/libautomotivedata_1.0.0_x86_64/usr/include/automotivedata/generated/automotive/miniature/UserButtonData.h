/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef AUTOMOTIVE_MINIATURE_USERBUTTONDATA_H
#define AUTOMOTIVE_MINIATURE_USERBUTTONDATA_H

#include "opendavinci/odcore/opendavinci.h"


#include "opendavinci/odcore/base/Visitable.h"
#include "opendavinci/odcore/data/SerializableData.h"


namespace automotive {
	namespace miniature {
		using namespace std;
		
		class UserButtonData : public odcore::data::SerializableData, public odcore::base::Visitable {
			public:
				enum ButtonStatus {
					PRESSED = 1,
					RELEASED = 0,
					UNDEFINED = -1,
				};
			public:
				UserButtonData();
		
				UserButtonData(
				const ButtonStatus &val0, 
				const double &val1
				);
		
				virtual ~UserButtonData();
		
				/**
				 * Copy constructor.
				 *
				 * @param obj Reference to an object of this class.
				 */
				UserButtonData(const UserButtonData &obj);
		
				/**
				 * Assignment operator.
				 *
				 * @param obj Reference to an object of this class.
				 * @return Reference to this instance.
				 */
				UserButtonData& operator=(const UserButtonData &obj);
		
			public:
			public:
				/**
				 * @return buttonStatus.
				 */
				ButtonStatus getButtonStatus() const;
				
				/**
				 * This method sets buttonStatus.
				 *
				 * @param val Value for buttonStatus.
				 */
				void setButtonStatus(const ButtonStatus &val);
			public:
				/**
				 * @return duration.
				 */
				double getDuration() const;
				
				/**
				 * This method sets duration.
				 *
				 * @param val Value for duration.
				 */
				void setDuration(const double &val);
		
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
			private:
				ButtonStatus m_buttonStatus;
			private:
				double m_duration;
		
		};
	} // miniature
} // automotive

#endif /*AUTOMOTIVE_MINIATURE_USERBUTTONDATA_H*/
