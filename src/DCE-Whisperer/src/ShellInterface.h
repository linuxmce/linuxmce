#ifndef SHELLINTERFACE_H
#define SHELLINTERFACE_H

#include "DCE/Message.h"
#include <string>
using namespace std;
using namespace DCE;

class ShellInterface
{
private:
	ShellInterface();

	static ShellInterface *pInstance;
	//int fdStdOut;
	FILE *fStdOut;
	pthread_t m_Thread;
	bool m_bCanProcess;
	bool m_bQuit;

	pthread_mutex_t m_ReplyMutex;
	bool m_bWaitingReply;
	string m_sWaitedReply;
	bool m_bReplyIsCmd;

	bool ReadRow(string &Row);
	void WriteToShell(const char *Cstring);
public:
	static ShellInterface* Get();

	virtual ~ShellInterface();

	void SendMessage(Message *pMessage, Message *&pMessage_Reply, string &sReply);
	void StartProcessing();
	void Quit();
	void *MainThread(void *Parm);
};

#define TheShell ShellInterface::Get()

#endif /*SHELLINTERFACE_H*/
