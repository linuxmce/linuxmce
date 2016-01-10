/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
// BindStatusCallback.h: interface for the CBindStatusCallback class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDSTATUSCALLBACK_H__6B469ECE_B785_11D3_8D3B_D5CFB868D41C__INCLUDED_)
#define AFX_BINDSTATUSCALLBACK_H__6B469ECE_B785_11D3_8D3B_D5CFB868D41C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define DIALOG_TYPE_PLUTOVIPMAIN		1
#define DIALOG_TYPE_UPDATEDLG			2

#pragma warning(disable:4100)   // disable warnings about unreferenced params

class CCallback : public IBindStatusCallback  
{
public:
	void SetDialogType(UINT uiDialogType);
	void HaveProgress(BOOL bProgress);
	CCallback();
	~CCallback();

    // Pointer to the download progress dialog.
    CDialog* m_pDlg;

    // The time when the download should timeout.
    BOOL  m_bUseTimeout;
    CTime m_timeToStop;

    // IBindStatusCallback methods.  Note that the only method called by IE
    // is OnProgress(), so the others just return E_NOTIMPL.

    STDMETHOD(OnStartBinding)(
         DWORD dwReserved,
         IBinding __RPC_FAR *pib)
        { return E_NOTIMPL; }

    STDMETHOD(GetPriority)(
         LONG __RPC_FAR *pnPriority)
        { return E_NOTIMPL; }

    STDMETHOD(OnLowResource)(
         DWORD reserved)
        { return E_NOTIMPL; }

    STDMETHOD(OnProgress)(
         ULONG ulProgress,
         ULONG ulProgressMax,
         ULONG ulStatusCode,
         LPCWSTR wszStatusText);

    STDMETHOD(OnStopBinding)(
         HRESULT hresult,
         LPCWSTR szError)
        { return E_NOTIMPL; }

    STDMETHOD(GetBindInfo)(
         DWORD __RPC_FAR *grfBINDF,
         BINDINFO __RPC_FAR *pbindinfo)
        { return E_NOTIMPL; }

    STDMETHOD(OnDataAvailable)(
         DWORD grfBSCF,
         DWORD dwSize,
         FORMATETC __RPC_FAR *pformatetc,
         STGMEDIUM __RPC_FAR *pstgmed)
        { return E_NOTIMPL; }

    STDMETHOD(OnObjectAvailable)(
         REFIID riid,
        /* [iid_is][in] */ IUnknown __RPC_FAR *punk)
        { return E_NOTIMPL; }

    // IUnknown methods.  Note that IE never calls any of these methods, since
    // the caller owns the IBindStatusCallback interface, so the methods all
    // return zero/E_NOTIMPL.

    STDMETHOD_(ULONG,AddRef)()
        { return 0; }

    STDMETHOD_(ULONG,Release)()
        { return 0; }

    STDMETHOD(QueryInterface)(
     REFIID riid,
     void __RPC_FAR *__RPC_FAR *ppvObject)
        { return E_NOTIMPL; }
protected:
	UINT m_DialogType;
	BOOL m_HaveProgress;
};

#pragma warning(default:4100)

#endif // !defined(AFX_BINDSTATUSCALLBACK_H__6B469ECE_B785_11D3_8D3B_D5CFB868D41C__INCLUDED_)
