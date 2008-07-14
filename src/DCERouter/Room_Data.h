#ifndef __ROOM_DATA_H__
#define __ROOM_DATA_H__
//----------------------------------------------------------------------------------------------
#include <map>
#include <string>
using namespace std;
//----------------------------------------------------------------------------------------------
class Room_Data
{
private:

	int m_nRoomID;
	string m_sDescription;
	string m_sRoomType;

public:

	Room_Data(int nRoomID = 0)
	{
		m_nRoomID = nRoomID;
	}

	void Description(string sValue)
	{
		m_sDescription = sValue;
	}

	string Description()
	{
		return m_sDescription;
	}

	void RoomType(string sValue)
	{
		m_sRoomType = sValue;
	}

	string RoomType()
	{
		return m_sRoomType;
	}
};
//----------------------------------------------------------------------------------------------
#endif //__ROOM_DATA_H__
