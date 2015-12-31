/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyDCECodeSupplier.h
For ?
*/
#ifndef DCERUBYDCECODESUPPLIER_H
#define DCERUBYDCECODESUPPLIER_H

#include "RubyEmbeder.h"

#include <map>
#include <list>

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

class DeviceData_Impl;
class Command_Impl;

/** @class RubyDCECodeSupplier
For ???
*/
class RubyDCECodeSupplier : public EMBRUBY::RubyEmbederCodeSupplier {
public:
    RubyDCECodeSupplier();
    virtual ~RubyDCECodeSupplier();

public:
	void addCode(Command_Impl *pcmdimpl, DeviceData_Impl* pdevicedata, bool io);
	void clearCode();

public:
	const char* getRubyCode() {
		return rcode_.c_str();
	}

	bool isCmdImplemented(unsigned devid, int cmd);
	bool isPrivateAssigned(unsigned devid) {
		return privateassigned_[devid];
	}
	bool isProcChildCommandAssigned(unsigned devid) {
		return procchildcmdassigned_[devid];
	}
	int getParamsOrderForCmd(unsigned devid, /*in*/int cmd, /*out*/std::list<int>& params);
	int getParamsNamesForCmd(unsigned devid, /*in*/int cmd, /*out*/std::list<std::string>& params);
	int getParamType(int param);

private:
	std::string TranslateCommandToRuby(const std::string& cmdtxt);

private:
	/*command to param names mapping*/
	typedef
		std::pair<int, std::string> PARAMPAIR; /*list of pairs: param id and ruby param name*/
	typedef
		std::list< std::pair<int, std::string> > PARAMLIST; /*list of pairs: param id and ruby param name*/
	typedef
		std::map<int, PARAMLIST> COMMANDPARAMMAP; /*map between command id and param map*/
	typedef
		std::map<unsigned, COMMANDPARAMMAP> DEVICEMAP;

	DEVICEMAP devicemap_;
	std::map<unsigned, bool> privateassigned_;
	std::map<unsigned, bool> procchildcmdassigned_;
	bool commandassigned_;
    std::map<int,int> param_type_map;

private:
	std::string rcode_;
};

};

#endif
