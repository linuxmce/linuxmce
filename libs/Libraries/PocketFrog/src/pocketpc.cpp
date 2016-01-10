//////////////////////////////////////////////////////////////////////////////
//
// PocketFrog - The Game Library for Pocket PC Devices
// Copyright 2002  Thierry Tremblay
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Thierry Tremblay makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//
//////////////////////////////////////////////////////////////////////////////

#include "pocketpc.h"
#include "display.h"
#include "internal/GAPILibrary.h"
#include "internal/Win32Drivers.h"

#include <memory>

namespace Frog {

using Internal::DisplayDriver;
using Internal::InputDriver;
using Internal::GAPILibrary;
using Internal::Win32Display;
using Internal::Win32Input;

DisplayDriver* l_displayDriver; //fixme: leak at exit
InputDriver*   l_inputDriver;   //fixme: leak at exit

void PocketPC_Cleanup()
{
	if(l_displayDriver)
	{
		delete l_displayDriver;
		l_displayDriver = NULL;
	}

	if(l_inputDriver)
	{
		delete l_inputDriver;
		l_inputDriver = NULL;
	}
}

namespace
{

//////////////////////////////////////////////////////////////////////////////
//
// Drivers
//
//////////////////////////////////////////////////////////////////////////////

bool InitDrivers( HWND hwnd, unsigned int desktopZoom )
{
    // Check if the drivers are already initialized
    if (l_displayDriver && l_inputDriver)
        return true;
    
    
    // Try GAPI first
    std::auto_ptr<GAPILibrary> GAPI( new GAPILibrary );
    if (GAPI->Initialize( hwnd ))
    {
        l_displayDriver = GAPI.get();
        l_inputDriver   = GAPI.release();
        return true;
    }


    // Try Win32 display
    std::auto_ptr<DisplayDriver> display( new Win32Display(desktopZoom) );
    std::auto_ptr<InputDriver>   input( new Win32Input );
    
    if (display->Initialize( hwnd ) && input->Initialize( hwnd ))
    {
        l_displayDriver = display.release();
        l_inputDriver   = input.release();
        return true;
    }

    return false;
}



//////////////////////////////////////////////////////////////////////////////
//
// Known Pocket PC models
//
//////////////////////////////////////////////////////////////////////////////

struct ModelID
{
    PocketPC::Model model;
    const TCHAR*    oemstring;
};


ModelID s_models[] =
{
    { PocketPC::MODEL_CASIO_E115,       _T("Pocket PC J580") },
    { PocketPC::MODEL_CASIO_E125,       _T("Pocket PC J670") },
    { PocketPC::MODEL_CASIO_EM500,      _T("Pocket PC J760") },
    { PocketPC::MODEL_JORNADA_540,      _T("HP, Jornada 540") },
    { PocketPC::MODEL_JORNADA_545,      _T("HP, Jornada 545") },
    { PocketPC::MODEL_JORNADA_568,      _T("HP, Jornada 568") },
    { PocketPC::MODEL_IPAQ_3600,        _T("Compaq iPAQ H3600") },
    { PocketPC::MODEL_IPAQ_3800,        _T("Compaq iPAQ H3800") },
    { PocketPC::MODEL_IPAQ_3900,        _T("Compaq iPAQ H3900") },
    { PocketPC::MODEL_SYMBOL_PPT_2700,  _T("PPT2700 by Symbol") },
    { PocketPC::MODEL_JORNADA_680,      _T("HP, Jornada 680") },
    { PocketPC::MODEL_JORNADA_720,      _T("HP, Jornada 720") }
};



} // end of anonymous namespace



//////////////////////////////////////////////////////////////////////////////
//
// PocketPC
//
//////////////////////////////////////////////////////////////////////////////

DisplayDevice* PocketPC::CreateDisplay( HWND hwnd, Orientation orientation, unsigned int desktopZoom )
{
    // Initialize the drivers
    if (!InitDrivers(hwnd, desktopZoom))
        return 0;

    // Create the display device object
    DisplayDevice* display = new DisplayDevice( l_displayDriver, orientation, desktopZoom );

    return display;
}



InputDevice* PocketPC::CreateInput( HWND hwnd, Orientation orientation )
{
    // Initialize the drivers
    if (!InitDrivers(hwnd, 0))
        return 0;

    // Create the input device object
    InputDevice* input = new InputDevice( l_inputDriver, orientation );

    return input;
}



size_t PocketPC::GetFullPathName( LPCTSTR filename, TCHAR pathname[MAX_PATH] )
{
    if (filename[0] == '\\')
    {
        // Absolute path was specified
        _tcscpy( pathname, filename );
        return _tcslen( pathname );
    }

#if defined(_WIN32_WCE)

    ::GetModuleFileName( 0, pathname, MAX_PATH );
  
    TCHAR* p = _tcsrchr( pathname, '\\' );
    if (p == 0) return 0;

    _tcscpy( p + 1, filename );

	return _tcslen( pathname );

#else

    TCHAR* p;
    return ::GetFullPathName( filename, MAX_PATH, pathname, &p );

#endif
}



PocketPC::Model PocketPC::GetModel()
{
    static Model model = MODEL_NOT_DETECTED;

    if (model == MODEL_NOT_DETECTED)
    {
        model = MODEL_UNKNOWN;

#if defined(_WIN32_WCE)
        
        TCHAR oeminfo[MAX_PATH];
        if (::SystemParametersInfo( SPI_GETOEMINFO, MAX_PATH, oeminfo, 0 ))
        {
            for (const ModelID* id = s_models; id != s_models + sizeof(s_models)/sizeof(s_models[0]); ++id)
            {
                if (0==_tcsncmp( oeminfo, id->oemstring, _tcslen(id->oemstring) ))
                {
                    model = id->model;
                    break;
                }
            }
        }
#endif
    }

    return model;
}



void PocketPC::Suspend()
{
    if (l_displayDriver) l_displayDriver->Suspend();
    if (l_inputDriver) l_inputDriver->Suspend();
}



void PocketPC::Resume()
{
    if (l_displayDriver) l_displayDriver->Resume();
    if (l_inputDriver) l_inputDriver->Resume();
}



} // end of namespace Frog
