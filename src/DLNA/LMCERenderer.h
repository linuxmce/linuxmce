/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef LMCERenderer_h
#define LMCERenderer_h

#include <HUpnpAv/HRendererConnection>
#include <HUpnpAv/HSeekInfo>
#include <HUpnpAv/HObject>
#include <QtCore/QUrl>
#include "DLNA.h"

using namespace Herqq::Upnp::Av;
using namespace DCE;

class LMCERenderer : public HRendererConnection {
private:
	unsigned int m_PK_EntArea;
	QUrl* m_Url;
	DLNA* m_pDLNA;
public:
	LMCERenderer(QObject* parent, DLNA* pDLNA);
	virtual ~LMCERenderer();

	qint32 doPlay(const QString& speed);
	qint32 doStop();
	qint32 doPause();
	qint32 doSeek(const HSeekInfo& seekInfo);
	qint32 doNext();
	qint32 doPrevious();
	qint32 doSetResource ( const QUrl &  resourceUri, HObject *  cdsMetadata = 0);
	qint32 doSetNextResource ( const QUrl &  resourceUri, HObject *  cdsMetadata = 0);
	qint32 doSelectPreset ( const QString &  presetName);

};
#endif
