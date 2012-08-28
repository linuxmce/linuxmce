#include "qmediastream.h"
#include "../Media_Plugin/EntertainArea.h"
#include "pluto_main/Define_MediaType.h"

QMediaStream::QMediaStream(
        qMediaPlayer_Plugin *pQtPlugin,
        MediaHandlerInfo *pMediaHandlerInfo,
        int iPK_MediaProvider,
        MediaDevice *pMediaDevice,
        int PK_Users,
        SourceType sourceType,
        int iStreamID
        )
: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)

{
  m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
}


