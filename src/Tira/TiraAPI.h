// $Header: /cygdrive/e/CVS/Software/APIBase/APIBase.h,v 1.16 2005/08/25 02:35:38 Administrator Exp $
//=============================================================================
//  Copyright (C) 2004, Home Electronics ( http://www.home-electro.com )
//  support@home-electro.com
//
// This software is a part of Tira API. Permission is granted to use and modify
// as long as this notice is preserved, and the software remains a part of Tira API
// 
// No warranty express or implied.
//=============================================================================

#ifndef APIBaseH
#define APIBaseH

#if defined(_BORLAND) 
	#define TIRA_API extern "C" __declspec(dllexport)
	#define TIRA_API_H extern "C"
#elif !defined(__linux__)
	#define TIRA_API extern "C" __declspec(dllexport)
	#define TIRA_API_H extern "C" __declspec(dllexport)
#else
	#define TIRA_API extern "C"
	#define TIRA_API_H extern "C"
	#define __stdcall
	#define __fastcall
#endif

#define  MaxIRData 20
#define  TiraUSBDevice 1024
// READ ONLY parameter, tells if TIRA is CURRENTLY set to detect frequency
#define     FREQUENCY



TIRA_API_H   int __stdcall tira_dll_ver(char* s, int size);

TIRA_API_H   int __stdcall tira_device_cap (int parameter);
TIRA_API_H   int __stdcall tira_set_device (int parameter, int value);

TIRA_API_H   int __stdcall tira_init ();
TIRA_API_H   int __stdcall tira_cleanup ();

#ifndef   NO_CALLBACK
typedef int    (__stdcall * tira_six_byte_cb) (const char * eventstring);
TIRA_API_H   int __stdcall tira_set_handler ( tira_six_byte_cb cb );
#endif

TIRA_API_H int __stdcall tira_start ( int PortID );
// Activates Tira

TIRA_API_H  int  __stdcall tira_stop ();
// Disactivates Tira



TIRA_API_H  int  __stdcall tira_start_capture ();
// Activates capture mode. After that Tira will expect some IR data

TIRA_API_H  int __stdcall tira_cancel_capture ();

TIRA_API_H  int __stdcall tira_get_captured_data (const unsigned char** data, int* size );
// Returns pointer to the captured IR data. Receiving application assumes
// ownership of the pointer and is responsible for freeing the memory

TIRA_API_H   int __stdcall tira_transmit (
   int repeat, int frequency, const unsigned char* data, const int dataSize );

TIRA_API_H int  __stdcall  tira_delete (const unsigned char* ptr);

// Alternative to the callback function
// returns
TIRA_API_H  int __stdcall tira_get_ir_data ( char IRDataString[], int* StringLen );

TIRA_API_H  int __stdcall tira_get_captured_data_vb (unsigned char Data[], int* size );
// This function was added to allow use of the API from VisualBasic
// Due to restrictions on the argument types that can be accessed from VB,
// this function copies data into out array DATA and the size (in/out)
// size must indicate maximum number of bytes that can be written into
// Data
// the function fails (returns non-zero value) if the size of the input data array is not enough
//

TIRA_API  int tira_access_feature(unsigned int FeatureID, bool Write,
                                          unsigned int* Value,
                                          unsigned int Mask);

// Sets or reads corresponding feature of the device
// FeatureID
// 0x10000000  IR Sensor enable/disable
//             Value 0: disable
//             Value 1: enable
//             Mask unused
// Return 0: success
//        1: failed
//        2: feature not implemented


TIRA_API const char* tira_get_version(int Component);
// Returns the version of the specified component:
// 0: Current DLL Version
// 1: Current Device Version


#endif

// $Log: APIBase.h,v $
// Revision 1.16  2005/08/25 02:35:38  Administrator
// Added tira_get_version
// Added tira_access_feature
//
// Revision 1.15  2004/07/01 00:32:39  szilber
// Copyright notice added
//
// Revision 1.14  2004/06/22 20:12:13  szilber
// initial
//
// Revision 1.13  2004/04/06 04:44:11  Administrator
// renamed
//
// Revision 1.1  2004/04/06 04:40:07  Administrator
// moved to APIBase
//
// Revision 1.12  2004/03/03 22:32:52  Administrator
// some bug fixes
//
// Revision 1.11  2003/12/22 04:36:44  Administrator
// tira-dll-ver, leg,
//
// Revision 1.10  2003/12/08 00:31:36  Administrator
// minor changes
// (get_thread added)
//
// Revision 1.9  2003/10/27 01:58:29  Administrator
// device_cap added.
// few fixes
//
// Revision 1.8  2003/10/24 20:02:08  Administrator
// no real changes
//
// Revision 1.7  2003/10/06 19:27:46  Administrator
// Support for opening USB device is added
//
// Revision 1.6  2003/09/20 22:25:47  Administrator
// minor fix
//
// Revision 1.5  2003/09/17 00:58:06  Administrator
// *** empty log message ***
//
// Revision 1.4  2003/08/19 04:30:48  Administrator
// added VB support initial version
//
// Revision 1.3  2003/07/24 09:13:02  Administrator
// Major update for Tira-2
// So far Tira2.dll seems to be working
//
// Revision 1.2  2003/06/06 00:38:11  Administrator
// no message
//
// Revision 1.1  2003/05/11 13:18:20  Administrator
// First new release
//
