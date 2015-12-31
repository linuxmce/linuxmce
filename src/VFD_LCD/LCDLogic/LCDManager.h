#ifndef __LCD_MANAGER_H__
#define __LCD_MANAGER_H__
//--------------------------------------------------------------------------------------------------------
#include "IInputProcessor.h"
#include "IRenderer.h"
//--------------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
class MenuHolder;
class MenuItem;
class Database_pluto_main;
//--------------------------------------------------------------------------------------------------------
class LCDManager : public IInputProcessor
{
private:

	//private variables
	DisplayState m_display_state;
	MenuHolder *m_pMenuHolder;
	pluto_pthread_mutex_t m_RenderMutex;
	list<IRenderer *> m_listRenderer;
	pthread_t m_RerendererThreadID;
	map<string, string> m_mapVariables;

	unsigned long m_ulRendererInterval;
	bool m_bQuit;

	Database_pluto_main *m_pDatabase_pluto_main;

	//private methods
	void Prepare();
	void CancelRerender();
	void ScheduleRerender(unsigned long ulMiliseconds);
	string ListItemDescription(MenuItem *MenuItem);
	bool Expand(MenuItem *pMenuItem);

	void GenerateOrbiterNodes(MenuItem *pMenuItem);
	void GenerateMDNodes(MenuItem *pMenuItem);
	void LoadNetworkInfo();

public:

	LCDManager(MenuHolder *pMenuHolder, Database_pluto_main *pDatabase_pluto_main);
	~LCDManager();

	void Render();
	bool Quit();
	unsigned long RerenderInterval();

	void SetVariable(string sVar, string sValue);
	void AddRenderer(IRenderer *pRenderer);
	bool ProcessInput(const Input &input);

	void ShowStatus(string sStatus, int nType);
	void RestoreState();

	void SetupCoreNodes();
};
//--------------------------------------------------------------------------------------------------------
#endif //__LCD_MANAGER_H__
