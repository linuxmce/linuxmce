#include "Whisperer.h"
#include "ShellInterface.h"
#include "Shortcuts.h"

#include "DCE/Message.h"
#include "DCE/Event_Impl.h"

#include "stdio.h"

/*static*/ Whisperer *Whisperer::pInstance = NULL;

/*private*/ Whisperer::Whisperer(int DeviceNo, const string &RouterAddress)
	: Command_Impl(DeviceNo, RouterAddress)
{
	m_pEvent = new Event_Impl(DeviceNo, 0, RouterAddress);
	m_pData = new DeviceData_Impl();
	m_pcRequestSocket = new Event_Impl(m_dwPK_Device, 0, RouterAddress);

	if( m_iInstanceID )
	{
		m_pEvent->m_pClientSocket->SendString("INSTANCE " + StringUtils::itos(m_iInstanceID));
		m_pcRequestSocket->m_pClientSocket->SendString("INSTANCE " + StringUtils::itos(m_iInstanceID));
	}
}

/*private*/ Whisperer::Whisperer()
	: Command_Impl(0, "")
{
	throw string("Default constructor called");
}

/*static*/ void Whisperer::Init(int DeviceNo, const string &RouterAddress)
{
	if (Whisperer::pInstance == NULL)
		Whisperer::pInstance = new Whisperer(DeviceNo, RouterAddress);
	else
		throw string("Whisperer already initialized");

	int dwSize;
	TheWhisperer->m_pEvent->GetConfig(dwSize);
	TheWhisperer->Connect(0);
	TheShell->StartProcessing();
}

/*static*/ Whisperer* Whisperer::Get()
{
	if (Whisperer::pInstance == NULL)
		throw string("Whisperer has not been initialized");
	return Whisperer::pInstance;
}

/*virtual*/ Whisperer::~Whisperer()
{
}

/*virtual*/ ReceivedMessageResult Whisperer::ReceivedMessage(Message *pMessage)
{
	LogWrite(LV_STATUS, "I've got a message from %ld for %ld.", pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To);
	LogWrite(LV_STATUS, "Parameters:");
	for (map<long, string>::iterator it = pMessage->m_mapParameters.begin(); it != pMessage->m_mapParameters.end(); ++it)
		LogWrite(LV_STATUS, "%ld: %s", it->first, it->second.c_str());
	LogWrite(LV_STATUS, "ToString: %s", pMessage->ToString().c_str());

	if (Command_Impl::ReceivedMessage(pMessage) == rmr_Processed)
	{
		LogWrite(LV_STATUS, "Message processed by framework");
		return rmr_Processed;
	}
	LogWrite(LV_STATUS, "The message will be processed by us: %ld", pMessage->m_dwID);

	Message *pMessage_Reply = NULL;
	string sReply;
	TheShell->SendMessage(pMessage, pMessage_Reply, sReply);
	
	if (pMessage_Reply != NULL)
	{
		LogWrite(LV_STATUS, "Reply message: %s", pMessage_Reply->ToString().c_str());
		SendMessage(pMessage_Reply);
	}
	else
	{
		LogWrite(LV_STATUS, "Reply string: %s", sReply.c_str());
		SendString(sReply);
	}

	return rmr_Processed;
}
