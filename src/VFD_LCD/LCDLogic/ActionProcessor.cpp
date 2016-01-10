#include "ActionProcessor.h"
#include "MenuItemAction.h"
//--------------------------------------------------------------------------------------------------------
#include "DCE/Command_Impl.h"
#include "DCE/Logger.h"
#include "PlutoUtils/StringUtils.h"
using namespace DCE;
//--------------------------------------------------------------------------------------------------------
ActionProcessor::ActionProcessor(Command_Impl *pCommandImpl) : m_pCommand_Impl(pCommandImpl)
{
}
//--------------------------------------------------------------------------------------------------------
ActionProcessor::~ActionProcessor()
{
	m_pCommand_Impl = NULL;
}
//--------------------------------------------------------------------------------------------------------
void ActionProcessor::ProcessAction(MenuItemAction *pAction)
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to execute action %s, type %d with %d parameters",
		pAction->Description().c_str(), pAction->Type(), pAction->Parameters().size());

	switch(pAction->Type())
	{
		case atExecuteScript:
			ExecuteScript(pAction->Description(), pAction->Parameters());
			break;
		case atExecuteCommand:
			ExecuteCommand(pAction->Description(), pAction->Parameters());
			break;
		case atCallMethod:
			CallMethod(pAction->Description(), pAction->Parameters());
			break;
		default:
			break;
	}
}
//--------------------------------------------------------------------------------------------------------
void ActionProcessor::ExecuteScript(string sCommandLine, const map<int, string>& mapParameters)
{
	for(map<int, string>::const_iterator it = mapParameters.begin(); it != mapParameters.end(); ++it)
	{
		string sToken = StringUtils::ltos(it->first);
		if(it->first == ptValue)
			sToken = "VALUE";

		string sParamToReplace = "{" + sToken + "}";
		sCommandLine = StringUtils::Replace(sCommandLine, sParamToReplace, it->second);
	}

	DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "Executing script: '%s'", sCommandLine.c_str());
	//system(sCommandLine.c_str());
}
//--------------------------------------------------------------------------------------------------------
void ActionProcessor::ExecuteCommand(string sCommandLine, const map<int, string>& mapParameters)
{
	for(map<int, string>::const_iterator it = mapParameters.begin(); it != mapParameters.end(); ++it)
	{
		string sToken = StringUtils::ltos(it->first);
		if(it->first == ptValue)
			sToken = "VALUE";

		string sParamToReplace = "{" + sToken + "}";
		sCommandLine = StringUtils::Replace(sCommandLine, sParamToReplace, it->second);
	}

	DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "Command to send: '%s'", sCommandLine.c_str());

	list<string> listArgs;

	string::size_type pos = 0;
	string token;
	do
	{
		size_t oldpos = pos;
		token = StringUtils::Tokenize(sCommandLine, " ", pos);

        if(token.size() > 0 && token[0] == '\"')
		{
			oldpos++; 
			token = StringUtils::Tokenize(sCommandLine, "\"", oldpos);
			pos = oldpos + 1;
		}

		listArgs.push_back(token);
	}
	while(pos < sCommandLine.length());

	char **ppArguments = new char *[listArgs.size()];

	int nIndex = 0;
	for(list<string>::iterator it = listArgs.begin(); it != listArgs.end(); ++it, ++nIndex)
	{
		ppArguments[nIndex] = new char[it->length() + 1];
		strcpy(ppArguments[nIndex], it->c_str());
	}

	Message *pMessage = new Message(static_cast<int>(listArgs.size()), ppArguments);

	nIndex = 0;
	for(list<string>::iterator it = listArgs.begin(); it != listArgs.end(); ++it, ++nIndex)
		delete [] ppArguments[nIndex];
	delete [] ppArguments;

	if(NULL != m_pCommand_Impl)
		m_pCommand_Impl->SendMessage(pMessage);
	else
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "Command NOT sent: we are not connected to router!");
	}
}
//--------------------------------------------------------------------------------------------------------
void ActionProcessor::CallMethod(string sMethodName, const map<int, string>& mapParameters)
{
	map<int, string> mapParametersClone(mapParameters);

	if(sMethodName == "RefreshDevices")
		RefreshDevices(mapParametersClone[1]);
}
//--------------------------------------------------------------------------------------------------------
void ActionProcessor::RefreshDevices(string sSomething)
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "ActionProcessor::RefreshDevices which parameter %s",
		sSomething.c_str());
}
//--------------------------------------------------------------------------------------------------------
