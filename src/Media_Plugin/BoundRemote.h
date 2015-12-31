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
#ifndef BoundRemote_h
#define BoundRemote_h

namespace DCE
{
	class BoundRemote
    {
    public:
        class OH_Orbiter *m_pOH_Orbiter;
        class EntertainArea *m_pEntertainArea;
        class Media_Plugin *m_pMedia_Plugin;
        string m_sPK_DesignObj_GraphicImage,m_sOptions;
		int m_PK_Screen;
        int m_iPK_Text_Synopsis;

        BoundRemote(class Media_Plugin *pMedia_Plugin) { m_pMedia_Plugin=pMedia_Plugin;}
        void UpdateOrbiter(class MediaStream *pMediaStream,bool bRefreshScreen,Message **p_pMessage=NULL);
    };

    typedef map<int,BoundRemote *> MapBoundRemote;
}

#endif
