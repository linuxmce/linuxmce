/**
 * EmulatorFactory - given a media type, instantiate the needed controller
 * and model.
 * 
 * return the controller object.
 *
 */

#include "EmulatorFactory.h"
#include "pluto_main/Define_MediaType.h"
#include "MAMEEmulatorController.h"
#include "MAMEEmulatorModel.h"

using namespace std;

namespace DCE
{
  EmulatorFactory::EmulatorFactory(Game_Player *pGamePlayer)
  {
    m_pGame_Player = pGamePlayer;
  }

  EmulatorFactory::~EmulatorFactory()
  {
    for (map<int, EmulatorController *>::iterator it = m_mapEmulatorControllerForMediaType.begin();
	 it != m_mapEmulatorControllerForMediaType.end();
	 ++it)
      {
	delete (*it).second;
      }

    m_mapEmulatorControllerForMediaType.clear();

  }

  void EmulatorFactory::addEmulatorForMediaType(int iPK_MediaType, EmulatorController *pEmulatorController)
  {
    if (!pEmulatorController)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorFactory::addEmulatorForMediaType(%d) pEmulatorController was null. Not adding.",iPK_MediaType);
      }
    else
      {
	m_mapEmulatorControllerForMediaType[iPK_MediaType] = pEmulatorController;
      }
  }

  EmulatorController* EmulatorFactory::getEmulatorForMediaType(int iPK_MediaType)
  {
    map<int, EmulatorController*>::iterator it=m_mapEmulatorControllerForMediaType.find(iPK_MediaType);
    return it == m_mapEmulatorControllerForMediaType.end() ? NULL : (*it).second;
  }

  bool EmulatorFactory::init()
  {
    
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_CONST,new MAMEEmulatorController(m_pGame_Player, new MAMEEmulatorModel()));
    
    return true;
  }

}
