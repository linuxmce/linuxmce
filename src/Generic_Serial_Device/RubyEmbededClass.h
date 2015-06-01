/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyEmbededClass.h
For ???
*/
#ifndef EMBRUBYRUBYEMBEDEDCLASS_H
#define EMBRUBYRUBYEMBEDEDCLASS_H

#include "RubyExceptions.h"

#include <list>
#include <ruby.h>
#include <stdarg.h>

/** @namespace EMBRUBY
Embedded Ruby ???
*/
namespace EMBRUBY {

/** @class RubyEmbededClass
For ???
*/
class RubyEmbededClass {
public:
	RubyEmbededClass();
	~RubyEmbededClass();

public:
	const VALUE getValue() {
		return value_;
	}
	void setValue(const VALUE value) {
		value_ = value;
	}

public:
/*	void *operator new( unsigned int num_bytes )
    {
		return malloc(num_bytes);
	} */

protected:
	struct ARGUMENTS {
		union {
			const char* classname_;
			VALUE classvalue_;
		} class_;
		const char* member_;
		int argc_;
		VALUE *argv_;

		ARGUMENTS(const char* sclass, const char* member, int argc, VALUE* argv)
			: member_(member), argc_(argc), argv_(argv)	{
			class_.classname_ = sclass;
		}

		ARGUMENTS(VALUE vclass, const char* member, int argc, VALUE* argv)
			: member_(member), argc_(argc), argv_(argv)	{
			class_.classvalue_ = vclass;
		}
	};

	static VALUE _callclass(VALUE arg);
	static VALUE _callmethod(VALUE arg);
	static std::string _backtrace();

protected:
	static VALUE StrToValue(const char* str);
	static VALUE IntToValue(int n);

private:
	VALUE value_;
};


/** @class RubyEmbededClassImpl
For ???
*/
template <class T = RubyEmbededClass>
class RubyEmbededClassImpl : public RubyEmbededClass {
public:
    RubyEmbededClassImpl() throw(RubyException);
    RubyEmbededClassImpl(const char *classname) throw(RubyException);
    ~RubyEmbededClassImpl();

protected:
	void Init(const char *classname) throw(RubyException);

public:
	VALUE callmethod(const char* methodname, unsigned num = 0, ...);
	VALUE callmethod(const char* methodname, const std::list<VALUE>& params);
};

template <class T>
RubyEmbededClassImpl<T>:: RubyEmbededClassImpl() throw(RubyException) {
	Init(T::_ClassName());
}

template <class T>
RubyEmbededClassImpl<T>:: RubyEmbededClassImpl(const char *classname) throw(RubyException) {
	if(classname != NULL) {
		Init(classname);
	}
}

template <class T>
void RubyEmbededClassImpl<T>::Init(const char *classname) throw(RubyException) {
	int error = 0;
	ARGUMENTS args(classname, "new", 0, 0);
	VALUE value = rb_protect(T::_callclass, reinterpret_cast<VALUE>(&args), &error);
	if(!error) {
		setValue(value);
	} else {
		throw RubyException(std::string("Cannot instantiate class: ") + classname);
	}
}

template <class T>
RubyEmbededClassImpl<T>::~RubyEmbededClassImpl() {
}

template <class T>
VALUE RubyEmbededClassImpl<T>::callmethod(const char* methodname, unsigned num, ...) {
	va_list marker;

	std::list<VALUE> params;
	if(num > 0) {
		va_start( marker, methodname );     /* Initialize variable arguments. */
		while( num > 0 )
		{
			VALUE v = va_arg( marker, VALUE );
			params.push_back(v);
			num--;
		}
		va_end( marker );              /* Reset variable arguments.      */
	}

	return callmethod(methodname, params);
}

template <class T>
VALUE RubyEmbededClassImpl<T>::callmethod(const char* methodname, const std::list<VALUE>& params) {
	VALUE *pv = NULL;
	if(params.size() > 0) {
		pv = new VALUE[params.size()];
		int i = 0;
		for(std::list<VALUE>::const_iterator it = params.begin(); it != params.end(); it++, i++) {
			pv[i] = *it;
		}
	}

	int error = 0;
	ARGUMENTS args(getValue(), methodname, params.size(), pv);
	VALUE vret = rb_protect(T::_callmethod, reinterpret_cast<VALUE>(&args), &error);
	if(pv != NULL) {
		delete pv;
	}

	if(error) {
		VALUE exception_instance = rb_gv_get("$!");
		VALUE message = rb_obj_as_string(exception_instance);

		std::string errpoint;
		errpoint = "\nerror: ";
#ifdef RUBY2_0
		errpoint += RSTRING_PTR(message);
#else
		errpoint += RSTRING(message)->ptr;
#endif
		errpoint += ", line: ";

		char tmpbuff[12];
#ifdef RUBY2_0
		sprintf(tmpbuff, "%d", rb_sourceline());
#else
		sprintf(tmpbuff, "%d", ruby_sourceline);
#endif
		errpoint += tmpbuff;

		errpoint += "\n";
		throw RubyException(std::string("Cannot call class method: ") + methodname + errpoint + "backtrace: " + _backtrace());
	}

	return vret;
}

#define DEFINE_CLASS_NAME(name) \
	static const char* _ClassName() { \
		return name; \
	}
};

#endif
