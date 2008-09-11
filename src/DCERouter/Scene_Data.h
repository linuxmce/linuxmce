#ifndef __SCENE_DATA_H__
#define __SCENE_DATA_H__
//----------------------------------------------------------------------------------------------
#include <map>
#include "Command_Data.h"
//----------------------------------------------------------------------------------------------
class Scene_Data
{
private:

	int m_nSceneID;
	string m_sDescription;
	std::map<int, Command_Data> m_mapCommands;

public:

	Scene_Data(int nSceneID = 0)
	{
		m_nSceneID = nSceneID;
	}

	void Description(string sValue)
	{
		m_sDescription = sValue;
	}

	string Description()
	{
		return m_sDescription;
	}

	std::map<int, Command_Data>& Commands()
	{
		return m_mapCommands;
	}
};
//----------------------------------------------------------------------------------------------
#endif //__SCENE_DATA_H__
