/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/10/05 21:48 (local)
    --------------------------------------------------------------
    Purpose:    iTunes event
**************************************************************************/

#include "stdafx.h"
#include "ITunesEventsSink.h"
#include "Log.h"

using namespace std;
using namespace PlutoITunesMediaConnector::Broadcaster;

ITunesEventsSink::ITunesEventsSink(	ITunes& iTunes,
									HANDLE iTunesUserQuitEvent,
									HANDLE iTunesDetachEvent)
:	_iTunes(iTunes)
,	_iTunesQuitEvent(iTunesUserQuitEvent)
,	_iTunesDetachEvent(iTunesDetachEvent) {
	REGISTER_RUNTIME_POINT("ITunesEventsSink::ITunesEventsSink", __FILE__, __LINE__);
	_dwRefCount = 0;
	ITypeLib* pITypeLib = NULL ;
    HRESULT	 hr = ::LoadRegTypeLib(
		LIBID_iTunesLib, 1, 5, // Major/Minor version numbers
		0x00, &pITypeLib);
	// Get type information for the interface of the object.
	hr = pITypeLib->GetTypeInfoOfGuid(
		DIID__IiTunesEvents, &_pITypeInfo);
	pITypeLib->Release();
}

ITunesEventsSink::~ITunesEventsSink(void) {
	REGISTER_RUNTIME_POINT("ITunesEventsSink::~ITunesEventsSink", __FILE__, __LINE__);
}

ULONG STDMETHODCALLTYPE ITunesEventsSink::AddRef() {
	REGISTER_RUNTIME_POINT("ITunesEventsSink::AddRef", __FILE__, __LINE__);
    InterlockedIncrement(&_dwRefCount);
    return _dwRefCount;
}
    
ULONG STDMETHODCALLTYPE ITunesEventsSink::Release() {
	REGISTER_RUNTIME_POINT("ITunesEventsSink::Release", __FILE__, __LINE__);
	InterlockedDecrement(&_dwRefCount);
	if (_dwRefCount == 0) {
		REGISTER_RUNTIME_POINT("ITunesEventsSink::Release", __FILE__, __LINE__);
		delete this;
		return 0;
	}
	return _dwRefCount;
}

HRESULT STDMETHODCALLTYPE ITunesEventsSink::QueryInterface(REFIID iid, void **ppvObject) {
	REGISTER_RUNTIME_POINT("ITunesEventsSink::QueryInterface", __FILE__, __LINE__);
    if (iid == IID_IDispatch || iid == DIID__IiTunesEvents) {
		REGISTER_RUNTIME_POINT("ITunesEventsSink::QueryInterface", __FILE__, __LINE__);
        _dwRefCount++;
        *ppvObject = this; //(_IiTunesEvents *)this;
        return S_OK;
    }
    if (iid == IID_IUnknown) {
		REGISTER_RUNTIME_POINT("ITunesEventsSink::QueryInterface", __FILE__, __LINE__);
        _dwRefCount++;
        *ppvObject = this; //(IUnknown *)this;
        return S_OK;
    }
    return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE ITunesEventsSink::GetTypeInfoCount(UINT* pctinfo) {
	REGISTER_RUNTIME_POINT("ITunesEventsSink::GetTypeInfoCount", __FILE__, __LINE__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ITunesEventsSink::GetTypeInfo(	UINT itinfo,
														LCID lcid,
														ITypeInfo** pptinfo) {
	REGISTER_RUNTIME_POINT("ITunesEventsSink::GetTypeInfo", __FILE__, __LINE__);
	return E_NOTIMPL;
}
	
HRESULT __stdcall ITunesEventsSink::GetIDsOfNames(	const IID& iid,
													OLECHAR** arrayNames,
													UINT countNames,
													LCID,          // Localization is not supported.
													DISPID* arrayDispIDs) {
	REGISTER_RUNTIME_POINT("ITunesEventsSink::GetIDsOfNames", __FILE__, __LINE__);
	if (iid != IID_NULL) {
		REGISTER_RUNTIME_POINT("ITunesEventsSink::GetIDsOfNames", __FILE__, __LINE__);
		return DISP_E_UNKNOWNINTERFACE;
	}
	HRESULT hr = _pITypeInfo->GetIDsOfNames(
		arrayNames, countNames, arrayDispIDs);
	return hr;
}

HRESULT STDMETHODCALLTYPE ITunesEventsSink::Invoke(	DISPID dispidMember,
													REFIID riid,
													LCID lcid,
													WORD wFlags,
													DISPPARAMS* pdispparams,
													VARIANT* pvarResult,
													EXCEPINFO* pexcepinfo,
													UINT* puArgErr) {

	REGISTER_RUNTIME_POINT("ITunesEventsSink::Invoke", __FILE__, __LINE__);
	
	switch (dispidMember) {
		case ITEventDatabaseChanged: // (1) iTunes database has changed
			REGISTER_RUNTIME_POINT("ITunesEventsSink::Invoke", __FILE__, __LINE__);
			_iTunes.onITunesDatabaseChanged(
					// HACK: It seems that parameters has been intricated in SDK's manual
					pdispparams->rgvarg[1], pdispparams->rgvarg[0]);
		    break;
	//	case ITEventPlayerPlay: // (2) a track has started playing
	//		MessageBox(NULL, "a track has started playing", "?????????????????", MB_OK);
	//	    break;
	//	case ITEventPlayerStop: // (3) a track has stopped playing
	//		MessageBox(NULL, "a track has stopped playing", "?????????????????", MB_OK);
	//	    break;
	//	case ITEventPlayerPlayingTrackChanged: // (4) information about the currently playing track has changed  
	//		MessageBox(NULL, "information about the currently playing track has changed  ", "?????????????????", MB_OK);
	//	    break;
	//	case ITEventCOMCallsDisabled: // (6) calls to the iTunes COM interface will be deferred (typically because a modal dialog is displayed)  
	//		MessageBox(NULL, "calls to the iTunes COM interface will be deferred (typically because a modal dialog is displayed)  ", "?????????????????", MB_OK);
	//	    break;
	//	case ITEventCOMCallsEnabled: // (7) calls to the iTunes COM interface will no longer be deferred (typically because the last modal dialog has been dismissed)  
	//		MessageBox(NULL, "calls to the iTunes COM interface will no longer be deferred (typically because the last modal dialog has been dismissed)  ", "?????????????????", MB_OK);
	//	    break;
		case ITEventQuitting: // (8) iTunes is about to quit, existing iTunes COM objects will no longer be valid  
		case ITEventAboutToPromptUserToQuit: // (9) iTunes is about to prompt the user to quit, you should release iTunes COM objects if you want to avoid the dialog  
			REGISTER_RUNTIME_POINT("ITunesEventsSink::Invoke", __FILE__, __LINE__);
			SetEvent(_iTunesQuitEvent);
			WaitForSingleObject(_iTunesDetachEvent, INFINITE);
			break;
	//	case ITEventSoundVolumeChanged: // (10) the sound output volume has changed  
	//		MessageBox(NULL, "the sound output volume has changed", "?????????????????", MB_OK);
	//		break;
	}
	return S_OK;
}
