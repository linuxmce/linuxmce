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
#include "A2600EmulatorModel.h"
#include "A2600EmulatorController.h"
#include "StellaEmulatorModel.h"
#include "StellaEmulatorController.h"
#include "A7800EmulatorController.h"
#include "IntellivisionEmulatorModel.h"
#include "IntellivisionEmulatorController.h"
#include "A5200EmulatorModel.h"
#include "A5200EmulatorController.h"
#include "ColecoVisionEmulatorModel.h"
#include "ColecoVisionEmulatorController.h"
#include "NESEmulatorModel.h"
#include "NESEmulatorController.h"
#include "FamicomEmulatorModel.h"
#include "FamicomEmulatorController.h"
#include "SG1000EmulatorModel.h"
#include "SG1000EmulatorController.h"
#include "SMSEmulatorModel.h"
#include "SMSEmulatorController.h"
#include "GenesisEmulatorModel.h"
#include "GenesisEmulatorController.h"
#include "MegadriveEmulatorModel.h"
#include "MegadriveEmulatorController.h"
#include "PCEngineEmulatorModel.h"
#include "PCEngineEmulatorController.h"
#include "TG16EmulatorModel.h"
#include "TG16EmulatorController.h"
#include "SGXEmulatorModel.h"
#include "SGXEmulatorController.h"
#include "SNESEmulatorModel.h"
#include "SNESEmulatorController.h"
#include "VectrexEmulatorModel.h"
#include "VectrexEmulatorController.h"
#include "VIC20EmulatorModel.h"
#include "VIC20EmulatorController.h"
#include "C64EmulatorModel.h"
#include "C64EmulatorController.h"
#include "Apple2EmulatorModel.h"
#include "Apple2EmulatorController.h"
#include "Atari800EmulatorModel.h"
#include "Atari800EmulatorController.h"
#include "TI994AEmulatorModel.h"
#include "TI994AEmulatorController.h"
#
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
	pEmulatorController->init(); // call any house cleaning functions needed.
      }
  }

  EmulatorController* EmulatorFactory::getEmulatorForMediaType(int iPK_MediaType)
  {
    map<int, EmulatorController*>::iterator it=m_mapEmulatorControllerForMediaType.find(iPK_MediaType);
    return it == m_mapEmulatorControllerForMediaType.end() ? NULL : (*it).second;
  }

  /** Register Game model/controllers here. Remember to add the headers up top! */
  bool EmulatorFactory::init()
  {    
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_CONST,new MAMEEmulatorController(m_pGame_Player, new MAMEEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_a2600_CONST,new StellaEmulatorController(m_pGame_Player, new StellaEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_a7800_CONST,new A7800EmulatorController(m_pGame_Player, new A7800EmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_intv_CONST,new IntellivisionEmulatorController(m_pGame_Player, new IntellivisionEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_a5200_CONST,new A5200EmulatorController(m_pGame_Player, new A5200EmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_coleco_CONST,new ColecoVisionEmulatorController(m_pGame_Player, new ColecoVisionEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_nes_CONST,new NESEmulatorController(m_pGame_Player, new NESEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_famicom_CONST,new FamicomEmulatorController(m_pGame_Player, new FamicomEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_sg1000_CONST,new SG1000EmulatorController(m_pGame_Player, new SG1000EmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_sms_CONST,new SMSEmulatorController(m_pGame_Player, new SMSEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_genesis_CONST,new GenesisEmulatorController(m_pGame_Player, new GenesisEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_megadriv_CONST,new MegadriveEmulatorController(m_pGame_Player, new MegadriveEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_pce_CONST,new PCEngineEmulatorController(m_pGame_Player, new PCEngineEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_tg16_CONST,new TG16EmulatorController(m_pGame_Player, new TG16EmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_sgx_CONST,new SGXEmulatorController(m_pGame_Player, new SGXEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_snes_CONST,new SNESEmulatorController(m_pGame_Player, new SNESEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_vectrex_CONST,new VectrexEmulatorController(m_pGame_Player, new VectrexEmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_vic20_CONST,new VIC20EmulatorController(m_pGame_Player, new VIC20EmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_c64_CONST,new C64EmulatorController(m_pGame_Player, new C64EmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_apple2_CONST,new Apple2EmulatorController(m_pGame_Player, new Apple2EmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_Atari800_CONST,new Atari800EmulatorController(m_pGame_Player, new Atari800EmulatorModel()));
    addEmulatorForMediaType(MEDIATYPE_lmce_Game_TI99_CONST,new TI994AEmulatorController(m_pGame_Player, new TI994AEmulatorModel()));
    return true;
  }

}
