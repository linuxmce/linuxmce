/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubyDCECodeSupplier.h"

#include "DCE/Message.h"
#include "DCE/Logger.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/Command_Impl.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "IR/IRDevice.h"
#include "PlutoUtils/DBHelper.h"
#include "DCE/DCEConfig.h"
using namespace std;

namespace DCE {

void 
RubyDCECodeSupplier::addCode(Command_Impl *pcmdimpl, DeviceData_Impl* pdevicedata, bool io) {
	if(rcode_.length() == 0) {
		rcode_ = "$:.unshift('/usr/pluto/bin')\n";
		rcode_ += "require 'Ruby_Generic_Serial_Device'""\n";
	}

	unsigned long devtemplid = pdevicedata->m_dwPK_DeviceTemplate;
	unsigned long devid = pdevicedata->m_dwPK_Device;
	
//	devtemplid;

	privateassigned_[devid] = procchildcmdassigned_[devid] = false;
	
	/* 
		header:
			in header we define type names for wrapper classes in order to shorten the names */

	if(!commandassigned_) {
        	rcode_ += "class Command < Ruby_Generic_Serial_Device::RubyCommandWrapper\nend\n";
			commandassigned_ = true;
	}
	/* header end*/	
	
	char tmpbuff[20];
	sprintf(tmpbuff, "%lu", devid);
	rcode_ += "class Device_"; rcode_ += tmpbuff; 
	if(io) {
    	    rcode_ += " < Ruby_Generic_Serial_Device::RubySerialIOWrapper""\n";
	} else {
    	    rcode_ += " < Ruby_Generic_Serial_Device::RubySerialWrapper""\n";
	}
	


	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Fetching Ruby code from Infrared Plugin");
	
	IRDevice irDevice;
	// this will have all the Ruby code, where int is the PK_Command and string is the codeint 
	int iSize = 0; char* pData = NULL; // Place holders for the 'out' parameter
	DCE::CMD_Get_Infrared_Codes_DT CMD_Get_Infrared_Codes_DT(devid, DEVICETEMPLATE_Infrared_Plugin_CONST,
				BL_SameHouse, devid, &pData, &iSize);
	if( !pcmdimpl->SendCommand(CMD_Get_Infrared_Codes_DT) || !iSize || !pData )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "I/R Plugin has no codes for us");
		delete pData;
		return;
	}
	irDevice.SerializeRead(iSize, pData); // De-serialize the data
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Fetched %d commands...", irDevice.m_mapCodes.size());
	std::map<std::string,std::string> rcode_setoutputparam;

	for(map<int,string>::iterator it = irDevice.m_mapCodes.begin(); it != irDevice.m_mapCodes.end(); it++ ) {
		int cmdid = (*it).first;
		string scmdid = StringUtils::itos(cmdid);
		if(!isCmdImplemented(devid, cmdid)) {
			if(cmdid == COMMAND_Private_Method_Listing_CONST) {
				if(!privateassigned_[devid]) {
					rcode_ += "#### PRIVATE METHODS ####################################################################\n";
					rcode_ += (*it).second;
					rcode_ += "\n";
					privateassigned_[devid] = true;
				} 
			} else
			if(cmdid == COMMAND_Process_Receive_Command_For_Child_CONST) {
				if(!procchildcmdassigned_[devid]) {
					rcode_ += "def cmd_ReceiveCommandForChild(cmd)\n";
					rcode_ += (*it).second;
					rcode_ += "\n""end""\n"; // def
					procchildcmdassigned_[devid] = true;
				}
			} else {
				string scmdtext = TranslateCommandToRuby((*it).second);
				if(!scmdtext.empty()) {
					/*for each command*/
					rcode_ += "#### " + scmdid + " ####################################################################\n";
					rcode_ += "def cmd_"; rcode_ += scmdid; rcode_ += "(";
		
					/*add command parameters*/
					std::string sql = "select PK_CommandParameter, CommandParameter.Description, Command_CommandParameter.IsOut, CommandParameter.FK_ParameterType "
										"from CommandParameter "
											"inner join Command_CommandParameter on FK_CommandParameter=PK_CommandParameter "
											"inner join Command on FK_Command=PK_Command "
										"where PK_Command = "; sql += scmdid;
					sql += " order by PK_CommandParameter asc";
											
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Running query to get params for Command %s: \n%s", scmdid.c_str(), sql.c_str());
					
					PARAMLIST& paramlist = devicemap_[devid][cmdid];
					PlutoSqlResult params;
					DCEConfig dceconf;
					DBHelper mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, dceconf.m_sDBName,dceconf.m_iDBPort);
					
					if((params.r = mySqlHelper.db_wrapper_query_result(sql.c_str()))) {
						DB_ROW rowparam;
						while((rowparam = db_wrapper_fetch_row(params.r))) {
							string rbparam = StringUtils::ToLower(FileUtils::ValidCPPName(rowparam[1]).c_str());
							
							PARAMPAIR parampair(atoi(rowparam[0]), rbparam);
							paramlist.push_back(parampair);
							
							rcode_ += rbparam;
							rcode_ += ", ";
							if(atoi(rowparam[2]) == 1)
							{
								rcode_setoutputparam[rbparam] = "def "+rbparam+"_set(value)\n@returnParamArray["+rowparam[0]+"]=value\nend\n";
								param_type_map[atoi(rowparam[0])]=atoi(rowparam[3]);
							}
						}
					}
					
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added %d parameeters for Command %s: ", paramlist.size(), scmdid.c_str());
					
					rcode_ += "cmd=nil)""\n"; // SetLevel(
					rcode_ += "@returnParamArray.clear\n";
					//rcode_ += "conn_ = getConn()""\n";
					rcode_ += TranslateCommandToRuby(scmdtext);
					/*insert ruby code for the method*/
					rcode_ += "\nreturn @returnParamArray\n";
					rcode_ += "end\n"; // def
				}
			}
		}
	}
		
	rcode_ += "#### START SETTERS ####################################################################\n";
	rcode_ += "def initialize()\nsuper\n@returnParamArray=Array.new\nend\n";
	for(map<std::string,std::string>::iterator it = rcode_setoutputparam.begin(); it != rcode_setoutputparam.end(); it++ ) {	
		rcode_ += (*it).second;
	}
	rcode_ += "####  END  SETTERS ####################################################################\n";
	rcode_ += "end""\n"; // class
}

void 
RubyDCECodeSupplier::clearCode() {
	rcode_.clear();
	devicemap_.clear();
}


int 
RubyDCECodeSupplier::getParamsOrderForCmd(unsigned devid, /*in*/int cmd, /*out*/std::list<int>& params) {
	params.clear();
	
	PARAMLIST& paramlist = devicemap_[devid][cmd];
	PARAMLIST::iterator it = paramlist.begin();
	while(it != paramlist.end()) {
		params.push_back((*it).first);
		it++;
	}
	
	return params.size();
}

int 
RubyDCECodeSupplier::getParamsNamesForCmd(unsigned devid, /*in*/int cmd, /*out*/std::list<string>& params) {
	params.clear();
	
	PARAMLIST& paramlist = devicemap_[devid][cmd];
	PARAMLIST::iterator it = paramlist.begin();
	while(it != paramlist.end()) {
		params.push_back((*it).second);
		it++;
	}
	
	return params.size();
}

bool 
RubyDCECodeSupplier::isCmdImplemented(unsigned devid, int cmd) {
	return (devicemap_[devid].find(cmd) != devicemap_[devid].end());
}

RubyDCECodeSupplier::RubyDCECodeSupplier() 
	: commandassigned_(false)
{
}


RubyDCECodeSupplier::~RubyDCECodeSupplier() {
}

std::string 
RubyDCECodeSupplier::TranslateCommandToRuby(const std::string& cmdtxt) {
	if(cmdtxt.empty()) {
		return cmdtxt;
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Translating Command CODE (length %d): \n%s\n", cmdtxt.length(), cmdtxt.c_str());
	
	string ret;
	int first = -1, last = -1;
	while(last < (int)cmdtxt.length()) {
		first = cmdtxt.find("<$", last + 1);
		if(first < 0) {	
			ret += cmdtxt.substr(last + 1, cmdtxt.length() - last - 1);
			break;
		} else {
			ret += cmdtxt.substr(last + 1, first - last - 1);
			
			first += 2;
			last = cmdtxt.find("$>", first + 1);
			if(last < 0) {
				break;
			} else {
				ret += "conn_.Send(" + cmdtxt.substr(first, last - first) + ")\n";
				last += 2;
			}
		}
	}
	return ret;
}

int 
RubyDCECodeSupplier::getParamType(int param) {
	return param_type_map[param];
}

};

