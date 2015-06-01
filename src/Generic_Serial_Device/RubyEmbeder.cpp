/***************************************************************************
 *   Copyright (C) 2005 by Igor Spac,,,                                    *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "RubyEmbeder.h"
#include "RubyStdStreamRedirector.h"

#include <string>

#include <iostream>

#include "DCE/Logger.h"

using namespace std;
using namespace DCE;

namespace EMBRUBY {

RubyEmbeder* RubyEmbeder::s_instance_ = NULL;

RubyEmbeder::RubyEmbeder() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting Ruby...");
	
	ruby_init();
	ruby_init_loadpath();
	ruby_script("embed");
	
}


RubyEmbeder::~RubyEmbeder() {
	ruby_finalize();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ruby ended.");
}

RubyEmbeder* 
RubyEmbeder::getInstance() {
	if(!s_instance_) {
		static RubyEmbeder embeder;
		s_instance_ = &embeder;
	}
	return s_instance_;
}


void 
RubyEmbeder::loadCode(RubyEmbederCodeSupplier *psup) throw(RubyException) {
	string code = psup->getRubyCode();
	if(code.length() == 0) {
		throw RubyException("Empty code passed to Ruby Embeder");
	}

	string s = "RCODE:\n" + addLineNumbers(code);
	LoggerWrapper::GetInstance()->WriteBlock(s.c_str(),s.size());

/*	
	{	//destroy in before rb_load_file
		//RubyStdStreamRedirector in(HF_STDIN);
		//write(in, code.c_str(), code.length());
	}*/
	
//	rb_eval_string(code.c_str());
	int error = 0;
	rb_protect(_loadcode, reinterpret_cast<VALUE>(code.c_str()), &error);
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
		throw RubyException(std::string("Error loading code: ") + errpoint);
	}
}

VALUE 
RubyEmbeder::_loadcode(VALUE arg) {
	const char *pstr = reinterpret_cast<const char*>(arg);
	rb_eval_string(pstr);
	return 0;
}

std::string 
RubyEmbeder::addLineNumbers(std::string str)
{
	static std::string result;
	char buf[16];
	unsigned int i,j,k;
	
	result="";
	i=j=k=0;
	for(i=0;i<str.size();i++)
	{
		if(str[i]=='\n')
		{
			sprintf(buf,"%4d:",k++);
			result += buf + str.substr(j,i-j+1);
			j=i+1;
		}
	}
	sprintf(buf,"%3d:",++k);
	result += buf + str.substr(j,i-j);
	return result;
}
}
