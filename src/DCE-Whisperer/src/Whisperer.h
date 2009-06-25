#ifndef WHISPERER_H
#define WHISPERER_H

#include "DCE/Command_Impl.h"
using namespace DCE;

class Whisperer : public Command_Impl
{
private:
	Whisperer(int DeviceNo, const string &RouterAddress);
	Whisperer();

	static Whisperer *pInstance;
public:
	static void Init(int DeviceNo, const string &RouterAddress);
	static Whisperer* Get();

	virtual ~Whisperer();
	virtual ReceivedMessageResult ReceivedMessage(Message *pMessage);
};

#define TheWhisperer Whisperer::Get()
#endif /*WHISPERER_H*/
