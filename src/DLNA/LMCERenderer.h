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
#include "Message.h"

using namespace Herqq::Upnp::Av;
using namespace DCE;

namespace DCE {
class DLNA;

class LMCERenderer : public HRendererConnection {
Q_OBJECT

private:
	unsigned int m_PK_EntArea;
	QUrl* m_Url;
	int m_iMediaType;
	DLNA* m_pDLNA;
	int m_iStreamID;
	long m_dwPK_Device_Playing;
	bool m_bIsPlaying;
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

	int GetMediaType(HObject* metadata, QUrl* url);
	int GetPK_EntertainArea();
	int GetStreamID();
	void MediaCommandIntercepted(Message *pMessage, long PK_Device);

};

}
#endif
