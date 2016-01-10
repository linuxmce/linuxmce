/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef FollowMe_Device_h
#define FollowMe_Device_h

namespace DCE
{
	/*	All Plug-ins that want to implement follow-me (Lighting, Media, Climate, Security and Telecom) are 
		derived from this base class and must implement ExecuteFollowMe which executes the actual 
		commands to move whatever is being moved.  This class implements HandleFollowMe which takes care of
		the common logic of determining when a user enters a room if he is the only one in the room, and the
		FollowMe_LeftRoom and FollowMe_EnteredRoom will happen immediately, or if someone else is in the room so 
		this user will be put in the queue and the FollowMe_LeftRoom will happen immediate, but the 
		FollowMe_EnteredRoom will take place after the user leaves.
	*/

	/*	When a user enters a room, but another orbiter is already linked to that room, we create an 
		instance of this so we can keep track of it.
	*/

	class PendingMove
	{
	public:
		char m_cType;
		class OH_Orbiter *pOH_Orbiter;
		class Room *pRoom_Prior,*pRoom_Current;
	};

	class FollowMe_Device
	{
	public:
		// Some things, like media, climate, etc. can only have 1 thing going.  Others like Telecom don't have that restriction.
		bool m_bOneAtATime;
		bool HandleFollowMe( class Message *pMessage );

		/*	When a user enters another room that's already occupied and his move is put in the queue, if he then cancels
			follow-me, we need to remove any such pending moves */
		void CancelPendingMovesByType(class OH_Orbiter *pOH_Orbiter);

		/*  When a user leaves or enters a room the appropriate action is fired, with "Left" happening first.  Sometimes a switch 
		is simultaneous and HandleFollowMe will get both a leave and entered at the same time.  In those cases, it still calls the LeftRoom and
		then the EnteredRoom, but it passes a value with the room Entered to the left room, and the room left to the Entered room.  In other
		words both functions will be called with the same parameters.  This is so that the implementing function can treat a clean switch
		differently from a leave and resetart.  For example media may choose to ignore a left room if both parms are specified since it knows
		it will be followed with an EnteredRoom with both specified, and when that happens we just want to move the media rather than 
		park it and restart it later. */

		virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)=0;
		virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)=0;

		FollowMe_Device() { m_bOneAtATime=false; }
		virtual ~FollowMe_Device() {}
	};
}

#endif
