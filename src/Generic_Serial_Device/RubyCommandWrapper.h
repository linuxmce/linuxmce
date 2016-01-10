/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyCommandWrapper.h
For ?
*/
#ifndef DCERUBYCOMMANDWRAPPER_H
#define DCERUBYCOMMANDWRAPPER_H

#include <map>
#include <string>

#include "DCE/Message.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class RubyCommandWrapper
For ???
*/
class RubyCommandWrapper {
public:
	RubyCommandWrapper();
    RubyCommandWrapper(long devidfrom, long devidto, long priority, long type, long id);
    ~RubyCommandWrapper();

	void setParam(long id, const char* val) {
		params_[id] = val;
	}

	const char* getParam(long id) {
		return params_[id].c_str();
	}

	const std::map<long, std::string>& getParams() {
		return params_;
	}
	void setParams(const std::map<long, std::string>& params) {
		params_ = params;
	}

	long getDevIdFrom() { return devidfrom_; }
	void setDevIdFrom(long devidfrom) { devidfrom_ = devidfrom; }

	long getDevIdTo() { return devidto_; }
	void setDevIdTo(long devidto) { devidto_ = devidto; }

	long getPriority() { return priority_; }
	void setPriority(long priority) { priority_ = priority; }

	long getType() { return type_; }
	void setType(long type) { type_ = type; }

	long getId() { return id_; }
	void setId(long id) { id_ = id; }

	long getCategory() { return category_; }
	void setCategory(long category) { category_ = category; devidto_ = DEVICEID_CATEGORY; }

	long getTemplate() { return template_; }
	void setTemplate(long the_template) { template_ = the_template; devidto_ = DEVICEID_MASTERDEVICE; }

public:
	long devidfrom_;
	long devidto_;
	long priority_;
	long type_;
	long id_;

	long category_;
	long template_;

	std::map<long, std::string> params_;
};

};

#endif
