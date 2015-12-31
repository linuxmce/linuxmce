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
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "CoreMenuStructure.h"

CoreMenuStructure::CoreMenuStructure(VFD_LCD_Base *pVFD_LCD_Base) 
	: MenuStructure(pVFD_LCD_Base)
{
	MenuNode *pMenuNode1,*pMenuNode2,*pMenuNode3;
	m_pMenuNode = new MenuNode(this,"Main Menu");

	m_pMenuNode->m_vectMenuNode.push_back( (pMenuNode1=new MenuNode(this,"Operating Conditions")) );
		pMenuNode1->m_vectMenuNode.push_back( (pMenuNode2=new CPUTemperature(this,"CPU Temperature")) );
		pMenuNode1->m_vectMenuNode.push_back( (pMenuNode2=new MenuNode(this,"CPU Load")) );
		pMenuNode1->m_vectMenuNode.push_back( (pMenuNode2=new ActiveProcesses(this,"Most active processes")) );

	m_pMenuNode->m_vectMenuNode.push_back( (pMenuNode1=new MenuNode(this,"Maintain Storage (HDD/RAID)")) );

	m_pMenuNode->m_vectMenuNode.push_back( (pMenuNode1=new MenuNode(this,"Status Messages")) );

	m_pMenuNode->m_vectMenuNode.push_back( (pMenuNode1=new MenuNode(this,"Administrative Tasks")) );
}

void ActiveProcesses::PrepareNodes()
{
	for(size_t s=0;s<m_vectMenuNode.size();++s)
		delete m_vectMenuNode[s];
	m_vectMenuNode.clear();

	srand((int) time(NULL)); // Shuffle uses a random generator
	for(int i=0;i<10;++i)
		m_vectMenuNode.push_back( new ActiveProcess(123,m_pMenuStructure,"Process " + StringUtils::itos(rand())) );
}

ActiveProcess::ActiveProcess(int PID,MenuStructure *pMenuStructure,string sText) 
	: MenuNode(pMenuStructure,sText) 
{ 
	m_PID=PID; 
	m_vectMenuNode.push_back( new MenuNode(m_pMenuStructure,"Kill it") );
}

void ActiveProcess::Enter()
{
	bool b=false;
}

