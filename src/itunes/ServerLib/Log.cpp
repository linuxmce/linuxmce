
#include "stdafx.h"
#include "Log.h"

using namespace std;

#ifdef _DEBUG

using namespace PlutoITunesMediaConnector::Broadcaster;

Log Log::_instance;

Log::Log() {
	/*...*/
}

Log::~Log() {
	/*...*/
}

Log& Log::getInstance() {
	return _instance;
}

void Log::append(const char* const message, const char* const file, unsigned int line) {
	ofstream f("\\PlutoITunesMediaConnector.log", ios::out|ios::app);
	struct tm nowTm;
	__time64_t now;
	_time64(&now);
	nowTm = *_localtime64(&now);
	f	<< GetCurrentProcessId() << ":" << GetCurrentThreadId()
		<< " "
		<< setfill('0')
		<< (nowTm.tm_year + 1900) << "."
		<< setw(2) << (nowTm.tm_mon + 1) << "."
		<< setw(2) << (nowTm.tm_mday) << " "
		<< setw(2) << (nowTm.tm_hour) << ":"
		<< setw(2) << (nowTm.tm_min) << ":"
		<< setw(2) << (nowTm.tm_sec) << " - "
		<< message
		<< " (" << file << ":" << line << ")"
		<< endl;
}

#endif // _DEBUG
