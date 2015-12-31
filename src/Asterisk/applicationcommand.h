/*
     Written for LinuxMCE. (c) LinuxMCE Foundation.
     Licensed under the GNU GPL v3, see COPYING for more details.

     Author: Adam Shortland <los93sol@gmail.com>
*/

#ifndef ASTERISKAPPLICATIONSTATEMACHINE_H
#define ASTERISKAPPLICATIONSTATEMACHINE_H

#include "asteriskcommand.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class ApplicationCommand : public IAsteriskCommand
{
public:
	ApplicationCommand();
	~ApplicationCommand();

public:
	void setPhoneNum(std::string phonenum);
	void setApplication(std::string application);
	void setApplicationData(std::string application_data);
	
	void handleStartup();
	void handleTerminate();
	bool handleResponse(Token &token);

private:
	std::string phonenum;
	std::string application;
	std::string application_data;
	
private:

};

};

#endif
