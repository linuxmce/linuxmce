/*
 BD_CP_ShowList
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#endif
#endif

#ifndef SYMBIAN
#include "BD/BDCommandProcessor.h"
#else
#include "PlutoMOAppUi.h"
#include "Logger.h" 
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_ShowList.h"

#ifdef SMARTPHONE
#include "Orbiter/CENet_Smartphone/OrbiterApp.h"
#endif

#if !defined(SYMBIAN) && !defined(SMARTPHONE)
BD_CP_ShowList::BD_CP_ShowList(
		unsigned long x, 
		unsigned long y, 
		unsigned long Width, 
		unsigned long Height,
		unsigned long SelectedIndex,
		list<string>  &DataGridList,
		bool		  bSendSelectedOnMove,
		bool		  bTurnOn
) 
{
	m_x		 = x;
	m_y		 = y;
	m_Width  = Width;
	m_Height = Height;
	m_SelectedIndex = SelectedIndex;
	m_bSendSelectedOnMove = bSendSelectedOnMove;
	m_bTurnOn = bTurnOn;

	for(list<string>::iterator it = DataGridList.begin(); it != DataGridList.end(); ++it)
		m_DataGridList.push_back(*it);
}
#endif //SYMBIAN

BD_CP_ShowList::~BD_CP_ShowList()
{
#ifdef SYMBIAN
	m_DataGridList.Close();
#endif //SYMBIAN
}

void BD_CP_ShowList::ConvertCommandToBinary()
{
#if !defined(SYMBIAN) && !defined(SMARTPHONE)
	BDCommand::ConvertCommandToBinary();

	Write_long(m_x);
	Write_long(m_y);
	Write_long(m_Width);
	Write_long(m_Height);
	Write_long(m_bSendSelectedOnMove);
	Write_long(m_bTurnOn);
	Write_long(m_SelectedIndex);

	Write_long(long(m_DataGridList.size()));
	for(list<string>::iterator it = m_DataGridList.begin(); it != m_DataGridList.end(); ++it)
        Write_string(*it);

	m_DataGridList.clear();
#endif 
}

void BD_CP_ShowList::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size, data);

	m_x = Read_long();
	m_y = Read_long();
	m_Width = Read_long();
	m_Height = Read_long();
	m_bSendSelectedOnMove = !!Read_long();
	m_bTurnOn = !!Read_long();
	m_SelectedIndex = Read_long();
	unsigned long ListSize = Read_long();

#ifdef VIPPHONE

#ifdef SYMBIAN
	for(unsigned long i = 0; i < ListSize; i++)
	{
		string *s = new string;
		Read_string(*s);

		m_DataGridList.Append(s);
	}
#endif //SYMBIAN

#ifdef SMARTPHONE
	m_vectDataGrid.clear();
	string s;
	for(int i = 0; i < ListSize; i++)
	{
		Read_string(s);
		m_vectDataGrid.push_back(s);
	}
#endif //SMARTPHONE

#endif //VIPPHONE
}

bool BD_CP_ShowList::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE

#ifdef SYMBIAN
	LOG("#	Received 'ShowList' command  #\n"); 
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->ShowList(m_x, m_y, m_Width, m_Height, m_DataGridList,
		m_bSendSelectedOnMove, m_bTurnOn, m_SelectedIndex);
#endif //SYMBIAN

#ifdef SMARTPHONE
	OrbiterApp::GetInstance()->ShowList(m_x, m_y, m_Width, m_Height, m_vectDataGrid,
		m_bSendSelectedOnMove, m_bTurnOn, m_SelectedIndex);
#endif //SMARTPHONE

#endif //VIPPHONE


	return true;
}
