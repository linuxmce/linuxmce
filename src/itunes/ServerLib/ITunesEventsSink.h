/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/10/05 21:48 (local)
    --------------------------------------------------------------
    Purpose:    iTunes event
**************************************************************************/

#include "ITunes.h"

#ifndef __HEADER_INCLUDED_BROADCASTER_ITUNESEVENSTSINK__
#define __HEADER_INCLUDED_BROADCASTER_ITUNESEVENSTSINK__

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {

/// iTunes event
class ITunesEventsSink : public _IiTunesEvents {

private:

	long _dwRefCount;
	ITypeInfo* _pITypeInfo;

	/// sets it if user quits from iTunes
	HANDLE _iTunesQuitEvent;
	/// waits while iTunes-object detaches from iTunes
	HANDLE _iTunesDetachEvent;

	/// iTunes instance
	ITunes &_iTunes;

public:

	/**
	 * @param iTunes iTunes instance
	 * @param iTunesUserQuitEvent sets it if user quits from iTunes
	 * @param iTunesDetachEvent waits while iTunes-object detaches from iTunes
	 */
	ITunesEventsSink(	ITunes& iTunes,
						HANDLE iTunesUserQuitEvent,
						HANDLE iTunesDetachEvent);

	~ITunesEventsSink(void);

	ULONG STDMETHODCALLTYPE AddRef();
    
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void **);

	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT*);

	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**);
	   
 	HRESULT __stdcall GetIDsOfNames(const IID&, OLECHAR**, UINT, LCID, DISPID*);

    HRESULT STDMETHODCALLTYPE Invoke(
		DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
  
};


	} // namespace Broadcaster 
} // namespace PlutoITunesMediaConnector

#endif // __HEADER_INCLUDED_BROADCASTER_ITUNESEVENTSSINK__
