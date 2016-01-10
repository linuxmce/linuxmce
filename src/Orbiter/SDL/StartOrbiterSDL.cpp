/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
  StartOrbiterSDL

  Copyright (C) 2004 Pluto, Inc., a Florida Corporation

  www.plutohome.com

  Phone: +1 (877) 758-8648

  This program is distributed according to the terms of the Pluto Public License, available at:
  http://plutohome.com/index.php?section=public_license

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "StartOrbiterSDL.h"
#include "PlutoUtils/CommonIncludes.h"
#include "DCE/Logger.h"
#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h"
#include "../pluto_main/Define_Screen.h"
#include "../Simulator.h"
#include "../Main.h"
#include "../ScreenHistory.h"

bool Init_System()
{
#ifndef DIRECTFB
	//disable repeated keys while orbiter is active
	system("/usr/bin/X11/xset r off");

    // as soon as possible
    if ( XInitThreads() == 0 )
    {
        cerr << "Unable to initialize multithreaded X11 code (XInitThreads failed)";
        return false;
    }

#endif
    return true;
}

extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;

bool g_WatchDogFlag=false;
void* WatchDogRoutine(void* param)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Started watchdog routine\n");
	usleep(10000000);
	if (g_WatchDogFlag)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Terminating Orbiter: %d watchdog detected hard deadlock, seems soft reload failed\n",(int) getpid());
		fflush(stdout);
		kill(getpid(), SIGKILL);
	}
		
	return NULL;
}

void DeadlockHandler(PlutoLock *pPlutoLock)
{
	// force reload - otherwise orbiter locks can block us forever
	pthread_t watchdog_thread;
	g_WatchDogFlag = true;
	pthread_create(&watchdog_thread, NULL,WatchDogRoutine, NULL);

	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
    if( g_pCommand_Impl )
    {
        
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StartOrbiterSDL Deadlock problem.  %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
        g_pCommand_Impl->OnReload();
    }
}
void SocketCrashHandler(Socket *pSocket)
{
	// force reload - otherwise orbiter locks can block us forever
	pthread_t watchdog_thread;
	g_WatchDogFlag = true;
	pthread_create(&watchdog_thread, NULL,WatchDogRoutine, NULL);

	// This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
    if( g_pCommand_Impl )
    {
        
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StartOrbiterSDL Socket problem. %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
        g_pCommand_Impl->OnReload();
    }
}

#ifndef MAEMO_NOKIA770
void translateSDLEventToOrbiterEvent(SDL_Event &sdlEvent, Orbiter::Event *orbiterEvent, struct keyboardState *kbdState, bool bUsingUIVersion2)
{
    orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;

    switch ( sdlEvent.type )
    {
        case SDL_QUIT: orbiterEvent->type = Orbiter::Event::QUIT;	break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:

            orbiterEvent->type = SDL_KEYDOWN != sdlEvent.type ? Orbiter::Event::BUTTON_UP :
                Orbiter::Event::BUTTON_DOWN;

			orbiterEvent->data.button.m_bSimulated = false;

            switch (sdlEvent.key.keysym.sym)
            {
                case SDLK_LSHIFT:   case SDLK_RSHIFT:       kbdState->bShiftDown = SDL_KEYDOWN == sdlEvent.type;      break;
                case SDLK_LCTRL:    case SDLK_RCTRL:		kbdState->bControlDown = SDL_KEYDOWN == sdlEvent.type;				break;
                case SDLK_LALT:     case SDLK_RALT:			kbdState->bAltDown = SDL_KEYDOWN == sdlEvent.type;					break;
                default:
                    break; //nothing here
            }

            if(SDL_KEYDOWN == sdlEvent.type && SDLK_CAPSLOCK == sdlEvent.key.keysym.sym)
                kbdState->bCapsLock = !kbdState->bCapsLock;

#ifdef DEBUG
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "key %s %d shif: %d ctrl: %d alt: %d caps: %d",
                                  SDL_KEYDOWN == sdlEvent.type ? "down" : "up",
                                  (int) sdlEvent.key.keysym.sym,
                                  (int) kbdState->bShiftDown,
                                  (int) kbdState->bControlDown,
                                  (int) kbdState->bAltDown,
                                  (int) kbdState->bCapsLock);
#endif

#ifndef PHONEKEYS
            if ( SDLK_a <= sdlEvent.key.keysym.sym && sdlEvent.key.keysym.sym <= SDLK_z )
            {
                if( ( ! kbdState->bCapsLock && ! kbdState->bShiftDown ) ||
                    (   kbdState->bCapsLock &&   kbdState->bShiftDown ) )
                    orbiterEvent->data.button.m_iPK_Button = BUTTON_a_CONST + sdlEvent.key.keysym.sym - SDLK_a;
                else
                    orbiterEvent->data.button.m_iPK_Button = BUTTON_A_CONST + sdlEvent.key.keysym.sym - SDLK_a;
            }
            else
#endif
/*
  if( sdlEvent.key.keysym.sym==SDLK_LSHIFT || sdlEvent.key.keysym.sym==SDLK_RSHIFT )
  {
  orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
  kbdState->bShiftDown=false;
  }
  else if( sdlEvent.key.keysym.sym==SDLK_LCTRL || sdlEvent.key.keysym.sym==SDLK_RCTRL )
  {
  orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
  kbdState->bControlDown=false;
  }
  else if( sdlEvent.key.keysym.sym==SDLK_LALT || sdlEvent.key.keysym.sym==SDLK_RALT )
  {
  orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
  kbdState->bAltDown=false;
  }

  else*/

                if( ! kbdState->bShiftDown && ! kbdState->bControlDown && ! kbdState->bAltDown )
                {
                    // No Modifiers were down
                    switch (sdlEvent.key.keysym.sym )
                    {
                        case SDLK_0: case SDLK_KP0: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_0_CONST; break;
                        case SDLK_1: case SDLK_KP1: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_1_CONST; break;
                        case SDLK_2: case SDLK_KP2: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_2_CONST; break;
                        case SDLK_3: case SDLK_KP3: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_3_CONST; break;
                        case SDLK_4: case SDLK_KP4: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_4_CONST; break;
                        case SDLK_5: case SDLK_KP5: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_5_CONST; break;
                        case SDLK_6: case SDLK_KP6: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_6_CONST; break;
                        case SDLK_7: case SDLK_KP7: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_7_CONST; break;
                        case SDLK_8: case SDLK_KP8: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_8_CONST; break;
                        case SDLK_9: case SDLK_KP9: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_9_CONST; break;
#ifdef PHONEKEYS
                        case SDLK_c:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_C_CONST; break;
                        case SDLK_p:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Pencil_CONST; break;
                        case SDLK_t:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Talk_CONST; break;
                        case SDLK_e:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_End_CONST; break;
                        case SDLK_l:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Soft_left_CONST; break;
                        case SDLK_r:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Soft_right_CONST; break;
                        case SDLK_ASTERISK: orbiterEvent->data.button.m_iPK_Button = BUTTON_Asterisk_CONST; break;
                        case SDLK_HASH:     orbiterEvent->data.button.m_iPK_Button = BUTTON_Pound_CONST; break;
#endif
                        case SDLK_F1:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F1_CONST; break;
                        case SDLK_F2:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F2_CONST; break;
                        case SDLK_F3:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F3_CONST; break;
                        case SDLK_F4:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F4_CONST; break;
                        case SDLK_F5:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F5_CONST; break;
                        case SDLK_F6:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F6_CONST; break;
                        case SDLK_F7:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F7_CONST; break;
                        case SDLK_F8:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F8_CONST; break;
                        case SDLK_F9:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F9_CONST; break;
                        case SDLK_F10:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F10_CONST; break;

                        case SDLK_F12:
                        	{
                                // TODO: remove when NewUI will be ready
                                // wx tests for new dialogs
                                break;
                            }

                        case SDLK_UP:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; break;
                        case SDLK_DOWN:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; break;
                        case SDLK_LEFT:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Left_Arrow_CONST; break;

                        case SDLK_KP_ENTER: case SDLK_RETURN: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Enter_CONST;  break;
                        case SDLK_SPACE:		orbiterEvent->data.button.m_iPK_Button = BUTTON_space_CONST; break;

                        case SDLK_BACKSPACE:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Back_CONST;  break;

                        case SDLK_UNDERSCORE:   orbiterEvent->data.button.m_iPK_Button = BUTTON_underscore_CONST;  break;
                        case SDLK_MINUS:	case SDLK_KP_MINUS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_dash_CONST;  break;
                        case SDLK_PLUS:		case SDLK_KP_PLUS:		orbiterEvent->data.button.m_iPK_Button = BUTTON_plus_CONST;  break;
                        case SDLK_EQUALS:	case SDLK_KP_EQUALS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_equals_sign_CONST;  break;

                        case SDLK_PERIOD:	case SDLK_KP_PERIOD:	orbiterEvent->data.button.m_iPK_Button = BUTTON_dot_CONST;  break;
                        case SDLK_COMMA:	orbiterEvent->data.button.m_iPK_Button = BUTTON_comma_CONST; break;
                        case SDLK_COLON:	 orbiterEvent->data.button.m_iPK_Button = BUTTON_colon_CONST; break;
                        case SDLK_SEMICOLON:	orbiterEvent->data.button.m_iPK_Button = BUTTON_semicolumn_CONST; break;
                        case SDLK_QUOTE:     orbiterEvent->data.button.m_iPK_Button = BUTTON_single_quote_CONST; break;
                        case SDLK_QUOTEDBL:    orbiterEvent->data.button.m_iPK_Button = BUTTON_double_quote_CONST; break;
			default:
                            orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
                            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unknown key: %d", (int) sdlEvent.key.keysym.sym);
                    };
                } // else if( !bShiftDown && !bControlDown && !bAltDown && !bRepeat )
                else if ( kbdState->bShiftDown && ! kbdState->bControlDown && ! kbdState->bAltDown )
                {
                    // The Shift key was pressed
                    switch (sdlEvent.key.keysym.sym)
                    {
                        case SDLK_UP:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Up_Arrow_CONST; break;
                        case SDLK_DOWN:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Down_Arrow_CONST; break;
                        case SDLK_LEFT:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Left_Arrow_CONST; break;
                        case SDLK_RIGHT:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Right_Arrow_CONST; break;

                        case SDLK_MINUS: case SDLK_KP_MINUS:  orbiterEvent->data.button.m_iPK_Button = BUTTON_underscore_CONST;  break;
                        case SDLK_EQUALS:     case SDLK_KP_EQUALS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_plus_CONST;  break;

                        case SDLK_0: case SDLK_KP0:   orbiterEvent->data.button.m_iPK_Button = BUTTON_right_parenthesis_CONST; break;
                        case SDLK_1: case SDLK_KP1:   orbiterEvent->data.button.m_iPK_Button = BUTTON_exclamation_point_CONST; break;
                        case SDLK_2: case SDLK_KP2:   orbiterEvent->data.button.m_iPK_Button = BUTTON_at_sign_CONST; break;
                        case SDLK_3: case SDLK_KP3:   orbiterEvent->data.button.m_iPK_Button = BUTTON_Pound_CONST; break;
                        case SDLK_4: case SDLK_KP4:   orbiterEvent->data.button.m_iPK_Button = BUTTON_dollar_CONST; break;
                        case SDLK_5: case SDLK_KP5:   orbiterEvent->data.button.m_iPK_Button = BUTTON_percent_CONST; break;
                        case SDLK_6: case SDLK_KP6:   orbiterEvent->data.button.m_iPK_Button = BUTTON_caret_CONST; break;
                        case SDLK_7: case SDLK_KP7:   orbiterEvent->data.button.m_iPK_Button = BUTTON_ampersand_CONST; break;
                        case SDLK_8: case SDLK_KP8:   orbiterEvent->data.button.m_iPK_Button = BUTTON_Asterisk_CONST; break;
                        case SDLK_9: case SDLK_KP9:   orbiterEvent->data.button.m_iPK_Button = BUTTON_left_parenthesis_CONST; break;

                        case SDLK_SEMICOLON:    orbiterEvent->data.button.m_iPK_Button = BUTTON_colon_CONST; break;
                        case SDLK_QUOTE:     orbiterEvent->data.button.m_iPK_Button = BUTTON_double_quote_CONST; break;

                        default:
                            orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
                    };
                }
            break;

        case SDL_MOUSEBUTTONDOWN:
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL_MOUSEBUTTONDOWN(b=%d, x=%d, y=%d)", sdlEvent.button.button, sdlEvent.button.x, sdlEvent.button.y);
#ifdef WIN32
            RecordMouseAction(sdlEvent.button.x, sdlEvent.button.y);
#endif
        case SDL_MOUSEBUTTONUP:
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL_MOUSEBUTTONUP(b=%d, x=%d, y=%d)", sdlEvent.button.button, sdlEvent.button.x, sdlEvent.button.y);

#ifdef AUDIDEMO
            orbiterEvent->type = (sdlEvent.type == SDL_MOUSEBUTTONDOWN) ? Orbiter::Event::BUTTON_DOWN : Orbiter::Event::BUTTON_UP;
            switch ( sdlEvent.button.button )
            {
                case 1:	orbiterEvent->data.button.m_iPK_Button = BUTTON_4_CONST; 			break;
                case 2: orbiterEvent->data.button.m_iPK_Button = BUTTON_Enter_CONST; 		break;
                case 3:	orbiterEvent->data.button.m_iPK_Button = BUTTON_5_CONST; 			break;
                case 4:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; 	break;
                case 5:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; 	break;
                case 6:	orbiterEvent->data.button.m_iPK_Button = BUTTON_2_CONST; 			break;
                case 7:	orbiterEvent->data.button.m_iPK_Button = BUTTON_1_CONST; 			break;

                default:
                    orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
            }
#else
            orbiterEvent->type = (sdlEvent.type == SDL_MOUSEBUTTONDOWN) ? Orbiter::Event::REGION_DOWN : Orbiter::Event::REGION_UP;
            orbiterEvent->data.region.m_iButton = sdlEvent.button.button;
            orbiterEvent->data.region.m_iX = sdlEvent.button.x;
            orbiterEvent->data.region.m_iY = sdlEvent.button.y;
#endif
            break;

        case SDL_MOUSEMOTION: // not handled
            //LoggerWrapper::GetInstance()->Write(LV_WARNING, "SDL_MOUSEMOTION(b=%d, x=%d, y=%d)", sdlEvent.button.button, sdlEvent.button.x, sdlEvent.button.y);
            orbiterEvent->type = Orbiter::Event::MOUSE_MOVE;
            orbiterEvent->data.region.m_iX = sdlEvent.button.x;
            orbiterEvent->data.region.m_iY = sdlEvent.button.y;
			break;

        default:
            orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
            break;
    }
}
#endif

OrbiterLinux *CreateOrbiter(int PK_Device,int PK_DeviceTemplate,string sRouter_IP,string sLocalDirectory,bool bLocalMode, int Width, int Height, bool bFullScreen,bool& bMustQuit,bool& bNeedReload)
{
	bNeedReload = false;
	bMustQuit = false;

	OrbiterLinux *pCLinux =
        new OrbiterLinux(
            PK_Device, PK_DeviceTemplate, sRouter_IP,
            sLocalDirectory, bLocalMode, Width, Height, bFullScreen);
            
	// Add a handler to take care of crashes
	g_pCommand_Impl = pCLinux;
	g_pDeadlockHandler=DeadlockHandler;
	g_pSocketCrashHandler=SocketCrashHandler;

	// Don't validate the device template, since the same binary is used for lots of devices
	if (bLocalMode || (pCLinux->GetConfig() && pCLinux->Connect(0)))
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");
		pCLinux->Initialize(gtSDLGraphic);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter logic initialized!");

		if(!pCLinux->m_bQuit_get())
		{
			if (!bLocalMode)
			{
				if(NULL != pCLinux->m_pScreenHistory_Current && pCLinux->m_pScreenHistory_Current->PK_Screen() != SCREEN_VideoWizard_CONST)
					pCLinux->CMD_Regen_Screen();
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "About to initialize after relatives...");
        
			OrbiterRenderer_Linux *pOrbiterRenderer_Linux = dynamic_cast<OrbiterRenderer_Linux *>(pCLinux->Renderer());
			if(pOrbiterRenderer_Linux != NULL)
			{
				pOrbiterRenderer_Linux->InitializeAfterRelatives();
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Creating the simulator");
			Simulator::GetInstance()->m_pOrbiter = pCLinux;

			if(Simulator::GetInstance()->m_bEnableGenerator)
				Simulator::GetInstance()->StartRandomEventGenerator();
			
			// If media is playing set now playing will set m_sApplicationName before this finishes
			if( pCLinux->UsesUIVersion2() && !pCLinux->m_bNewOrbiter && pCLinux->m_sApplicationName_get().empty() )
				pCLinux->StartScreenSaver(false);

			return pCLinux;
		}
		else
		{
			if(pCLinux->m_bReload)
				bNeedReload = true;
		}
	}
	else
	{
			if( pCLinux->m_pEvent && pCLinux->m_pEvent->m_pClientSocket && pCLinux->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_CannotConnect )
			{
				bMustQuit = true;
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No Router.  Will abort");
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
			}
	}

	delete pCLinux;
	return NULL;
}

bool SDL_Event_Process(SDL_Event_Loop_Data &sdl_event_loop_data)
{
    if (sdl_event_loop_data.pOrbiter->m_bQuit_get())
        return false;

/*
    // crash in X11_CheckMouseMode () from /usr/lib/libSDL-1.2.so.0
    // our SDL lib is not thread-safe
    sdl_event_loop_data.pOrbiter->X_LockDisplay();
    bool bIsEventWaiting = SDL_PollEvent(&sdl_event_loop_data.event);
    sdl_event_loop_data.pOrbiter->X_UnlockDisplay();
    if (bIsEventWaiting)
    {
        if(sdl_event_loop_data.event.type == SDL_QUIT)
            return false;
    }
*/

	sdl_event_loop_data.pOrbiter->Renderer()->EventLoop();
    return sdl_event_loop_data.pOrbiter->m_bQuit_get()|| sdl_event_loop_data.pOrbiter->m_bReload;
}

bool SDL_Event_Loop_End(SDL_Event_Loop_Data &sdl_event_loop_data)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL_Event_Loop_End()");
    if (sdl_event_loop_data.pOrbiter == NULL)
        return false;
    bool bReload = sdl_event_loop_data.pOrbiter->m_bReload;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "ready to delete instance, End of SDL loop with reload: %s", (bReload ? "Y" : "N"));
    delete sdl_event_loop_data.pOrbiter;
    sdl_event_loop_data.pOrbiter = NULL;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "finished deleting pcLinux");
    return bReload;
}

SDL_App_Object::SDL_App_Object(int argc, char *argv[])
        : m_pSDL_Event_Loop_Data(NULL)
        , argc(argc)
        , argv(argv)
        , m_nExitCode(1)
{
}

SDL_App_Object::~SDL_App_Object()
{
    Destroy();
}

bool SDL_App_Object::Run()
{
    if (! LoadConfig())
    {
        SetExitCode(1);
        return false;
    }

	bool bAppError = false;
	bool bNeedReload = false;
    if (! Create(bAppError, bNeedReload))
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "No Orbiter instance was created, probably because we needed to reload.  Reloading");

		if(bNeedReload)
			SetExitCode(2);

		if(bAppError)
			SetExitCode(0);

        return false;
    }
    
	//while (EventProcess())
    //    ;
	EventProcess();

    Destroy();
    return true;
}

bool SDL_App_Object::LoadConfig()
{
	Simulator::GetInstance()->LoadConfigurationFile("/etc/Orbiter.conf");
    return  true;
};

bool SDL_App_Object::Create(bool &bAppError, bool &bNeedReload)
{

    m_pSDL_Event_Loop_Data = new SDL_Event_Loop_Data;
    CommandLineParams commandlineparams;
    bool bStartedOK = ParseCommandLineParams(argc, argv, commandlineparams);
    if (! bStartedOK)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "error returned by : ParseCommandLineParams()");
        SetExitCode(1);
        return false;
    }
	m_pSDL_Event_Loop_Data->pOrbiter = CreateOrbiter(commandlineparams.PK_Device, commandlineparams.PK_DeviceTemplate, commandlineparams.sRouter_IP, commandlineparams.sLocalDirectory, commandlineparams.bLocalMode, commandlineparams.Width, commandlineparams.Height, commandlineparams.bFullScreen,bAppError,bNeedReload);
    if (m_pSDL_Event_Loop_Data->pOrbiter == NULL)
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "No Orbiter was created. We've probably regen'd and needed to reload");
        return false;
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Created : ptr=%p", m_pSDL_Event_Loop_Data->pOrbiter);
    return true;
};

bool SDL_App_Object::EventProcess()
{
    if ( (m_pSDL_Event_Loop_Data == NULL) || (m_pSDL_Event_Loop_Data->pOrbiter == NULL) )
    {
        SetExitCode(2);
        return false;
    }
    SDL_Event_Process(*m_pSDL_Event_Loop_Data);
    SetExitCode(m_pSDL_Event_Loop_Data->pOrbiter->m_bReload ? 2 : 0);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL_App_Object::EventProcess() END, with return code : %d", GetExitCode());
    return true;
};

void SDL_App_Object::Destroy()
{
    if (m_pSDL_Event_Loop_Data == NULL)
        return;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDL_App_Object::Destroy()");
    SDL_Event_Loop_End(*m_pSDL_Event_Loop_Data);
    delete m_pSDL_Event_Loop_Data;
    m_pSDL_Event_Loop_Data = NULL;
    if (LoggerWrapper::GetInstance())
        
    
    return;
};

int SDL_App_Object::GetExitCode() const
{
    return m_nExitCode;
};

void SDL_App_Object::SetExitCode(int nExitCode)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "ExitCode: %d -> %d", m_nExitCode, nExitCode);
    m_nExitCode = nExitCode;
};

int main(int argc, char *argv[])
{
    int nExitCode = 0;
    if (! Init_System())
        return 1;
    SDL_App_Object *pSDL_App_Object = new SDL_App_Object(argc, argv);
    pSDL_App_Object->Run();
    nExitCode = pSDL_App_Object->GetExitCode();
    delete pSDL_App_Object;
    return nExitCode;
}
