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


#include "test15/generated/Test15Simple.h"

	using namespace std;
	using namespace odcore::base;


	Test15Simple::Test15Simple() :
	    SerializableData(), Visitable()
		, m_buttonState(UNDEFINED) // TODO: Validation if the default value is of the desired type.
	{
	}

	Test15Simple::Test15Simple(
		const Test15Simple::ButtonState &val0
	) :
	    SerializableData(), Visitable()
		, m_buttonState(val0)
	{
	}

	Test15Simple::Test15Simple(const Test15Simple &obj) :
	    SerializableData(), Visitable()
		, m_buttonState(obj.m_buttonState)
	{
	}
	
	Test15Simple::~Test15Simple() {
	}

	Test15Simple& Test15Simple::operator=(const Test15Simple &obj) {
		m_buttonState = obj.m_buttonState;
		return (*this);
	}

	int32_t Test15Simple::ID() {
		return 15;
	}

	const string Test15Simple::ShortName() {
		return "Test15Simple";
	}

	const string Test15Simple::LongName() {
		return "Test15Simple";
	}

	int32_t Test15Simple::getID() const {
		return Test15Simple::ID();
	}

	const string Test15Simple::getShortName() const {
		return Test15Simple::ShortName();
	}

	const string Test15Simple::getLongName() const {
		return Test15Simple::LongName();
	}

	Test15Simple::ButtonState Test15Simple::getButtonState() const {
		return m_buttonState;
	}
	
	void Test15Simple::setButtonState(const Test15Simple::ButtonState &val) {
		m_buttonState = val;
	}

	void Test15Simple::accept(odcore::base::Visitor &v) {
		v.beginVisit();
		int32_t int32t_buttonState = m_buttonState;
		v.visit(CRC32 < CharList<'b', CharList<'u', CharList<'t', CharList<'t', CharList<'o', CharList<'n', CharList<'S', CharList<'t', CharList<'a', CharList<'t', CharList<'e', NullType> > > > > > > > > > >  >::RESULT, 0, "Test15Simple.buttonState", "buttonState", int32t_buttonState);
		v.endVisit();
	}

	const string Test15Simple::toString() const {
		stringstream s;


		switch(getButtonState()) {
			case PRESSED :
			s << "ButtonState: Test15Simple::PRESSED (1) ";
			break;
			case UNDEFINED :
			s << "ButtonState: Test15Simple::UNDEFINED (-1) ";
			break;
			case NOT_PRESSED :
			s << "ButtonState: Test15Simple::NOT_PRESSED (0) ";
			break;
		}

		return s.str();
	}

	ostream& Test15Simple::operator<<(ostream &out) const {

		SerializationFactory& sf = SerializationFactory::getInstance();

		std::shared_ptr<Serializer> s = sf.getSerializer(out);

		int32_t int32t_buttonState = m_buttonState;
		s->write(CRC32 < CharList<'b', CharList<'u', CharList<'t', CharList<'t', CharList<'o', CharList<'n', CharList<'S', CharList<'t', CharList<'a', CharList<'t', CharList<'e', NullType> > > > > > > > > > >  >::RESULT,
				int32t_buttonState);
		return out;
	}

	istream& Test15Simple::operator>>(istream &in) {

		SerializationFactory& sf = SerializationFactory::getInstance();

		std::shared_ptr<Deserializer> d = sf.getDeserializer(in);

		int32_t int32t_buttonState = 0;
		d->read(CRC32 < CharList<'b', CharList<'u', CharList<'t', CharList<'t', CharList<'o', CharList<'n', CharList<'S', CharList<'t', CharList<'a', CharList<'t', CharList<'e', NullType> > > > > > > > > > >  >::RESULT,
				int32t_buttonState);
		m_buttonState = static_cast<Test15Simple::ButtonState>(int32t_buttonState);
		return in;
	}
