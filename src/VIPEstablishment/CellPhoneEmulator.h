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
#ifndef CellPhoneEmulator_H
#define CellPhoneEmulator_H


class CellPhoneEmulator
{
public:
	char *m_pcResponse;
	long m_dwResponseSize;
	class PlutoConfig *m_pPlutoConfig;
	class Logger *g_pPlutoLogger;
	void ShowMenu(class VR_ShowMenu *MenuAction);
	CellPhoneEmulator(class PlutoConfig *pPlutoConfig, class Logger *pLogger) 
	{ 
		m_pPlutoConfig=pPlutoConfig;
		g_pPlutoLogger=pLogger;
	}
	const char *ReceiveRequest(long iRequestSize,const char *pRequest,long *iResponseSize);
};

#endif
