/*
 * dummy_player.c: Player that does nothing (saves CPU time)
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: dummy_player.c,v 1.3 2007/10/15 00:15:07 phintuka Exp $
 *
 */

#include <vdr/status.h>

#include "dummy_player.h"
#include "tools/timer.h"

#define STILLPICTURE_INTERVAL (5*1000)   // 5 sec

//
// cDummyPlayerControl
//

extern const unsigned char v_mpg_vdrlogo[]; // vdrlogo_720x576.mpg.c
extern const int v_mpg_vdrlogo_length;      // vdrlogo_720x576.mpg.c
//extern const unsigned char v_mpg_nosignal[];// nosignal_720x576.mpg.c
//extern const int v_mpg_nosignal_length;     // nosignal_720x576.mpg.c
//extern const unsigned char v_mpg_black[];   // black_720x576.mpg.c
//extern const int v_mpg_black_length;        // black_720x576.mpg.c

class cDummyPlayer : public cPlayer {
  protected:
    virtual void Activate(bool On)
    {
      if(On) {
	TimerHandler();
        CreateTimerEvent(this, &cDummyPlayer::TimerHandler, STILLPICTURE_INTERVAL);
      } else {
        CancelTimerEvents(this);
      }
    }
    bool TimerHandler(void)
    {
      DeviceStillPicture(v_mpg_vdrlogo, v_mpg_vdrlogo_length);
      //DeviceStillPicture(v_mpg_nosignal, v_mpg_nosignal_length);
      //DeviceStillPicture(v_mpg_black, v_mpg_black_length);
      return true;
    }

  public:
    cDummyPlayer(void) {};
    virtual ~cDummyPlayer()
    {
      Activate(false);
      Detach();
    }
};

//
// cDummyPlayerControl
//

cDummyPlayer *cDummyPlayerControl::m_Player = NULL;
cMutex cDummyPlayerControl::m_Lock;

cDummyPlayerControl::cDummyPlayerControl(void) :
  cControl(OpenPlayer())
{
  cStatus::MsgReplaying(this, "none", NULL, true);
}

cDummyPlayerControl::~cDummyPlayerControl()
{
  cStatus::MsgReplaying(this, NULL, NULL, false);
  Close();
}

cDummyPlayer *cDummyPlayerControl::OpenPlayer(void)
{
  m_Lock.Lock();
  if(!m_Player)
    m_Player = new cDummyPlayer;
  m_Lock.Unlock();
  return m_Player;
}

void cDummyPlayerControl::Close(void)
{
  m_Lock.Lock();
  if(m_Player)
    delete m_Player;
  m_Player = NULL;
  m_Lock.Unlock();
}

eOSState cDummyPlayerControl::ProcessKey(eKeys Key)
{
  if(!ISMODELESSKEY(Key) || Key == kBack || Key == kStop) {
    Close();
    return osEnd;
  }
  return osContinue;
}

