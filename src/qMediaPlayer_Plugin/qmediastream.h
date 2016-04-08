#ifndef QMEDIASTREAM_H
#define QMEDIASTREAM_H

#include "../Media_Plugin/MediaHandlerInfo.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaDevice.h"

#include <map>
#include <list>
#include <string>

namespace DCE
{
using namespace std;

class QMediaStream: public MediaStream

{
public:
    QMediaStream( class qMediaPlayer_Plugin *pQtPlugin, class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);
private:
    bool					m_bIsStreaming;

public:
    int 	m_iPK_DesignObj_Remote_After_Menu; 		/** Store the real value if we're switching to a menu */
    int 	m_iPK_DesignObj_RemoteOSD_After_Menu;
    int 	m_iPK_DesignObj_Remote_Popup_After_Menu;

    virtual ~QMediaStream();  
    virtual int GetType();

  virtual  bool ShouldUseStreaming();

   virtual bool CanPlayMore();
   virtual  bool ContainsVideo();
    };
};

#endif // QMEDIASTREAM_H
