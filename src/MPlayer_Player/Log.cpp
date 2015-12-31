#include <iostream>

#include "DCE/Logger.h"

#include "Log.h"

using namespace DCE;

void Log(string sMessage) {
	//std::cout << "[" << getpid() << "]\t" << sMessage << std::endl;
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::Log %s", sMessage.c_str());
}

