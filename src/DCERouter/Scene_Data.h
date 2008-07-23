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
	int m_nRoomID;
	std::map<int, Command_Data> m_mapCommands;

public:

	Scene_Data(int nSceneID = 0, int nRoomID = 0)
	{
		m_nSceneID = nSceneID;
		m_nRoomID = nRoomID;
	}

	void Description(string sValue)
	{
		m_sDescription = sValue;
	}

	string Description() const
	{
		return m_sDescription;
	}

	int SceneID() const 
	{
		return m_nSceneID;
	}

	int RoomID() const
	{
		return m_nRoomID;
	}

	void RoomID(int nRoomID)
	{
		m_nRoomID = nRoomID;
	}

	std::map<int, Command_Data>& Commands()
	{
		return m_mapCommands;
	}
};
//----------------------------------------------------------------------------------------------
#endif //__SCENE_DATA_H__
