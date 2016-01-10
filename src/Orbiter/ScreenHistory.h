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
#ifndef __SCREEN_HISTORY_H__
#define __SCREEN_HISTORY_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include <time.h>
#include "DesignObj_Orbiter.h"
using namespace DCE;

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Contains a snapshot of the screen that was viewed and the state so that it can be restored with GoBack
	*/
	//-----------------------------------------------------------------------------------------------------

	class Message;

	class ScreenHistory
	{
	private:

		Orbiter *m_pOrbiter;
		DesignObj_Orbiter *m_pObj; /** < The screen we're viewing */
		list<DesignObj_Orbiter *> m_listObjs; /** < The list of design objects for this screen */
		map<int, string> m_mapVariable; /** < Any variables we need to restore when returning to this screen */
		time_t m_tTime; /** < The moment of time when this screen was created */
		map<DesignObj_Orbiter *, bool> m_mapVisibilityContext; /** < We keep here the visibily status for each object */

		string m_sID; /** < The id for current design object */
		int m_nPK_Screen; /** < The PK_Screen of the screen */

		/** < If we get a go back, skip over this screen unless "Force" is set to true */
		bool m_bCantGoBack; 

	public:
		/* Logic flaw here...  When we do click a button with a goto design obj without changing screens this code:
			void Orbiter::CMD_Goto_DesignObj(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,Message *pMessage)
			m_pOrbiterRenderer->ObjectOffScreen(m_pScreenHistory_Current->GetObj());
		calls the object off screen method for the prior method, and then assigns the new DesignObj as current:
			pScreenHistory_New->SetObj(pObj_New);
		then sets the screen to change to:
			NeedToRender::NeedToChangeScreens( this, pScreenHistory_New);
		Next as the stack unwinds
			bool Orbiter::RegionDown( int x,  int y )
			NeedToRender render( this, "Region Down" );  // Redraw anything that was changed by this command
		falls out of scope 
			NeedToRender::~NeedToRender()
		calls
			m_pOrbiter->NeedToChangeScreens(pScreenHistory);
		for the new object.  Since there is some deep logic here and I don't want to risk breaking anything,
		as a temporary solution in Goto_DesignObj I'll set m_bPutObjectsOffScreen to true
		and only call ObjectOffScreen from ~NeedToRender if it's false. 
		The problem was in the media browser in UI1 you'd select media, it would set the cover art with CMD_Update_Object_image
		and change to DesignObj 5088.  But ObjectOffScreen would get called for 5088 before it appeared, causing
				pObj->m_pvectCurrentGraphic = NULL;
		to set it to NULL and you wouldn't see the cover art.

			TODO - HACK!!
		*/
		bool m_bPutObjectsOffScreen;

		map<long, string> m_mapParameters; /** < Any parameters passed in when the screen was created */

		/** < We'll know if we need to add an object to history or not (maybe we are doing a go back) */
		static bool m_bAddToHistory; 

		/** < Constructor and destructor */
		ScreenHistory(int nPK_Screen, class ScreenHistory *pScreenHistory_Prior,Message *pMessage,Orbiter *pOrbiter);
		~ScreenHistory();

		/** < Getter and setter for current design object for this screen */
		DesignObj_Orbiter *GetObj();
		void SetObj(DesignObj_Orbiter *pObj);

		/** < It will add the current design object to history for this screen */
		void AddToHistory();

		/** < Returns true if list with design objects for this screen is empty */
		bool HistoryEmpty();

		/** < Purges the list with design object within history list for this screen */
		void PurgeHistory();

		/** < Returns true if we can go back in this screen (from a design obj to another) */
		bool GoBack();

		/** < Returns a dump with current state - used for debugging */
		string ToString();

		/** < can go back or not? */
		bool CantGoBack();
		void CantGoBack(bool bCantGoBack);

		/** < Setter and getter for current design obj's ID */
		string ScreenID();
		void ScreenID(string sID);

		/** < Setter and getter for screen's PK_Screen */
		int PK_Screen();
		void PK_Screen(int nPK_Screen);

		/** < Returns the moment of time when this screen was created */
		time_t TimeCreated() const;

		/** < Saves/restores to map with variables and the visibily status for each object */
		void SaveContext(const map<int, string>& mapVariable, 
			const map<DesignObj_Orbiter *, bool>& mapVisibilityContext);
		void RestoreContext(map<int, string>& mapVariable,
			map<DesignObj_Orbiter *, bool>& mapVisibilityContext);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
