

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Thu Jan 18 20:55:43 2007
 */
/* Compiler settings for .\netfw.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_INetFwRemoteAdminSettings,0xD4BECDDF,0x6F73,0x4A83,0xB8,0x32,0x9C,0x66,0x87,0x4C,0xD2,0x0E);


MIDL_DEFINE_GUID(IID, IID_INetFwIcmpSettings,0xA6207B2E,0x7CDD,0x426A,0x95,0x1E,0x5E,0x1C,0xBC,0x5A,0xFE,0xAD);


MIDL_DEFINE_GUID(IID, IID_INetFwOpenPort,0xE0483BA0,0x47FF,0x4D9C,0xA6,0xD6,0x77,0x41,0xD0,0xB1,0x95,0xF7);


MIDL_DEFINE_GUID(IID, IID_INetFwOpenPorts,0xC0E9D7FA,0xE07E,0x430A,0xB1,0x9A,0x09,0x0C,0xE8,0x2D,0x92,0xE2);


MIDL_DEFINE_GUID(IID, IID_INetFwService,0x79FD57C8,0x908E,0x4A36,0x98,0x88,0xD5,0xB3,0xF0,0xA4,0x44,0xCF);


MIDL_DEFINE_GUID(IID, IID_INetFwServices,0x79649BB4,0x903E,0x421B,0x94,0xC9,0x79,0x84,0x8E,0x79,0xF6,0xEE);


MIDL_DEFINE_GUID(IID, IID_INetFwAuthorizedApplication,0xB5E64FFA,0xC2C5,0x444E,0xA3,0x01,0xFB,0x5E,0x00,0x01,0x80,0x50);


MIDL_DEFINE_GUID(IID, IID_INetFwAuthorizedApplications,0x644EFD52,0xCCF9,0x486C,0x97,0xA2,0x39,0xF3,0x52,0x57,0x0B,0x30);


MIDL_DEFINE_GUID(IID, IID_INetFwProfile,0x174A0DDA,0xE9F9,0x449D,0x99,0x3B,0x21,0xAB,0x66,0x7C,0xA4,0x56);


MIDL_DEFINE_GUID(IID, IID_INetFwPolicy,0xD46D2478,0x9AC9,0x4008,0x9D,0xC7,0x55,0x63,0xCE,0x55,0x36,0xCC);


MIDL_DEFINE_GUID(IID, IID_INetFwMgr,0xF7898AF5,0xCAC4,0x4632,0xA2,0xEC,0xDA,0x06,0xE5,0x11,0x1A,0xF2);


MIDL_DEFINE_GUID(IID, LIBID_NetFwPublicTypeLib,0xDB4F3345,0x3EF8,0x45ED,0xB9,0x76,0x25,0xA6,0xD3,0xB8,0x1B,0x71);


MIDL_DEFINE_GUID(CLSID, CLSID_NetFwOpenPort,0x0CA545C6,0x37AD,0x4A6C,0xBF,0x92,0x9F,0x76,0x10,0x06,0x7E,0xF5);


MIDL_DEFINE_GUID(CLSID, CLSID_NetFwAuthorizedApplication,0xEC9846B3,0x2762,0x4A6B,0xA2,0x14,0x6A,0xCB,0x60,0x34,0x62,0xD2);


MIDL_DEFINE_GUID(CLSID, CLSID_NetFwMgr,0x304CE942,0x6E39,0x40D8,0x94,0x3A,0xB9,0x13,0xC4,0x0C,0x9C,0xD4);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

