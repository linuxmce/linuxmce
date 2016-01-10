/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef DLNAEngine_h
#define DLNAEngine_h

#include <QtCore/QCoreApplication>
#include <HUpnpCore/HDeviceHost>

#include "DLNA.h"
#include "LMCERenderer.h"

using namespace Herqq::Upnp;

namespace DCE {
	class DLNA;
class DLNAEngine : public QObject {
private:
	HDeviceHost* m_pDeviceHost;
	bool m_bRunning;
	pthread_t m_AppThread;
	QCoreApplication* m_pApp;
	DLNA* m_pDLNA;

public:
	DLNAEngine(DLNA* pDlna);
	virtual ~DLNAEngine();

	int Init();

};
}
#endif
