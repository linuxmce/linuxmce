#ifndef __ACTION_PROCESSOR_H__
#define __ACTION_PROCESSOR_H__
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <map>
using namespace std;
//--------------------------------------------------------------------------------------------------------
namespace DCE
{
	class Command_Impl;
};
//--------------------------------------------------------------------------------------------------------
class MenuItemAction;
//--------------------------------------------------------------------------------------------------------
class ActionProcessor
{
private:

	DCE::Command_Impl *m_pCommand_Impl;

	//helpers
	void ExecuteScript(string sCommandLine, const map<int, string>& mapParameters);
	void ExecuteCommand(string sCommandLine, const map<int, string>& mapParameters);
	void CallMethod(string sMethodName, const map<int, string>& mapParameters);

	//methods to call
	void RefreshDevices(string sSomething);

public:

	ActionProcessor(DCE::Command_Impl *pCommandImpl);
	~ActionProcessor();

	void ProcessAction(MenuItemAction *pAction);
};
//--------------------------------------------------------------------------------------------------------
#endif //__ACTION_PROCESSOR_H__
