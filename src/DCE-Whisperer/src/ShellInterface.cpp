#include "ShellInterface.h"
#include "Whisperer.h"
#include "Shortcuts.h"

#include "PlutoUtils/StringUtils.h"

#include <unistd.h>

/*static*/ ShellInterface *ShellInterface::pInstance = NULL;

/*static*/ ShellInterface* ShellInterface::Get()
{
	if (ShellInterface::pInstance == NULL)
		ShellInterface::pInstance = new ShellInterface();
	return ShellInterface::pInstance;
}

static void *StartShellThread(void *Parm)
{
	return TheShell->MainThread(Parm);
}

/*private*/ ShellInterface::ShellInterface()
	: m_bCanProcess(false), m_bQuit(false), m_bWaitingReply(false)
{
	// prevent the pollution of stdout and workaround an otherwise harmless thread and file descriptor issue
	//fdStdOut = dup2(1, 128);
	
	// fd 0 is always a bidirectional pathway: socket, ptmx; fortunately, socat doesn't select pipes; this one always remains unaltered
	//fdStdOut = 0; 
	fStdOut = fdopen(0, "r+");
	dup2(2, 1);
	pthread_mutex_init(&m_ReplyMutex, NULL);
}

/*private*/ bool ShellInterface::ReadRow(string &Row)
{
	char msg[16384];
	if (fgets(msg, 16384, stdin) == NULL)
		return false;
	msg[strlen(msg) - 1] = 0;
	Row = msg;
	return true;
}

/*private*/ void ShellInterface::WriteToShell(const char *Cstring)
{
	//dprintf(fdStdOut, "%s\n", sMessage.c_str());
	fprintf(fStdOut, "%s\n", Cstring);
	fflush(fStdOut);
}

/*virtual*/ ShellInterface::~ShellInterface()
{
	pthread_mutex_destroy(&m_ReplyMutex);
}

void *ShellInterface::MainThread(void *Parm)
{
	(void) Parm;
	while (!m_bQuit)
	{
		string MsgText;
		if (ReadRow(MsgText))
		{
			if (MsgText.size() > 0)
			{
				vector<string> vect_sTokens;
				StringUtils::Tokenize(MsgText, " ", vect_sTokens);
				size_t vect_sTokens_size = vect_sTokens.size();
				string CommandForDCE;

				if (vect_sTokens[0] == "DEVICE_REGISTERED")
				{
					if (vect_sTokens_size == 2)
					{
						int DeviceNumber = atoi(vect_sTokens[1].c_str());
						char cDeviceIsRegistered = TheWhisperer->DeviceIsRegistered(DeviceNumber);
						WriteToShell((vect_sTokens[0] + " " + cDeviceIsRegistered).c_str());
					}
					continue;
				}

				size_t idx_start = 0;
				bool b_isReply = false;
				if (vect_sTokens[0] == "reply")
				{
					/* This is a reply, not a command */
					idx_start = 2;
					b_isReply = true;
					if (vect_sTokens[1] == "msg")
						m_bReplyIsCmd = true;
					else /* txt */
						m_bReplyIsCmd = false;
				}
				for (size_t i = idx_start; i < vect_sTokens_size; ++i)
				{
					string sDecodedToken = StringUtils::URLDecode(vect_sTokens[i]);
					//if (strchr(sDecodedToken.c_str(), ' ') != NULL)
						sDecodedToken = "\"" + StringUtils::Replace(sDecodedToken, "\"", "\\\"") + "\"";
					CommandForDCE += (i > idx_start ? " " : "") + sDecodedToken;
				}

				if (b_isReply)
				{
					pthread_mutex_lock(&m_ReplyMutex);
					fprintf(stderr, "Processing a DCE reply\n");
					if (m_bWaitingReply)
					{
						fprintf(stderr, "This is an expected reply\n");
						m_sWaitedReply = CommandForDCE;
						m_bWaitingReply = false;
					}
					else
					{
						fprintf(stderr, "Spurious reply: %s\n", CommandForDCE.c_str());
					}
					fprintf(stderr, "Finished processing a DCE reply\n");
					pthread_mutex_unlock(&m_ReplyMutex);
				}
				else
				{
					fprintf(stderr, "Sending a DCE command: %s\n", CommandForDCE.c_str());
					TheWhisperer->QueueMessageToRouter(new Message(CommandForDCE));
				}
			}
		}
		else
			m_bQuit = true;
	}

	TheWhisperer->OnQuit();
	return NULL;
}

void ShellInterface::SendMessage(Message *pMessage, Message *&pMessage_Reply, string &sReply)
{
	string sMessage;
	if (pMessage->m_eExpectedResponse == ER_ReplyString || pMessage->m_eExpectedResponse == ER_DeliveryConfirmation)
	{
		sMessage += "-r ";
		m_bWaitingReply = true;
	}
	else if (pMessage->m_eExpectedResponse == ER_ReplyMessage)
	{
		sMessage += "-o ";
		m_bWaitingReply = true;
	}
	else
		sMessage += "-- ";

	sMessage += StringUtils::itos(pMessage->m_dwPK_Device_From) + " " +StringUtils::itos(pMessage->m_dwPK_Device_To) + " " +
		StringUtils::itos(pMessage->m_dwMessage_Type) + " " + StringUtils::itos(pMessage->m_dwID);
	map<long, string>::iterator ParmIt;
	for (ParmIt = pMessage->m_mapParameters.begin(); ParmIt != pMessage->m_mapParameters.end(); ++ParmIt)
	{
		if (ParmIt->second == "")
			continue; // skip empty parameters
		sMessage += " " + StringUtils::itos(ParmIt->first) + " " + StringUtils::URLEncode(ParmIt->second);
	}

	WriteToShell(sMessage.c_str());

	pthread_mutex_lock(&m_ReplyMutex);
	if (m_bWaitingReply)
	{
		while (m_bWaitingReply)
		{
			pthread_mutex_unlock(&m_ReplyMutex);
			Sleep(100);
			pthread_mutex_lock(&m_ReplyMutex);
		}
		if (m_bReplyIsCmd)
		{
			fprintf(stderr, "Reply is a message\n");
			pMessage_Reply = new Message(m_sWaitedReply);
		}
		else if (m_sWaitedReply != "")
		{
			fprintf(stderr, "Reply is a string\n");
			sReply = m_sWaitedReply.substr(1, m_sWaitedReply.size() - 2); // strip surrounding quotes
		}
		else
		{
			fprintf(stderr, "Reply is empty?\n");
		}
		m_sWaitedReply = "";
	}
	pthread_mutex_unlock(&m_ReplyMutex);
}

void ShellInterface::StartProcessing()
{
	if (m_bCanProcess)
		return;

	pthread_create(&m_Thread, NULL, StartShellThread, NULL);
	m_bCanProcess = true;
}

void ShellInterface::Quit()
{
	m_bQuit = true;
}
