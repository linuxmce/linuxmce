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
#include "PlutoUtils/CommonIncludes.h"	
#include "OGText.h"
#include "OrbiterGen.h"
#include "DesignObj_Generator.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_Language.h"
#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_Text.h"
#include "pluto_main/Table_DesignObjVariation.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"

CGText::CGText(class DesignObj_Generator *Parent,class Row_DesignObjVariation_Text_Skin_Language *drOVTSL,class Row_Orbiter *drOrbiter) 
{
if( drOVTSL->FK_DesignObjVariation_Text_getrow()->FK_Text_get()==792 )
{
int k=2;
}
	m_bPreRender=true;
	m_pParent=Parent;
	Database_pluto_main *mds = drOVTSL->Table_DesignObjVariation_Text_Skin_Language_get()->Database_pluto_main_get();
	m_pRow_DesignObjVariation_Text_Skin_Language=drOVTSL;
	m_pdrOrbiter=drOrbiter;

	Row_DesignObjVariation_Text *pp = drOVTSL->FK_DesignObjVariation_Text_getrow();
	m_PK_Text=pp->FK_Text_get();
	if( (m_pdrText_LS=GetText_LS(drOVTSL->FK_DesignObjVariation_Text_getrow()->FK_Text_get(),Parent->m_pOrbiterGenerator))==NULL )
		return;
int i=m_pdrText_LS->FK_Text_get();

	m_sText = m_pdrText_LS->Description_get();
	m_rPosition.X = m_pRow_DesignObjVariation_Text_Skin_Language->X_get();
	m_rPosition.Y = m_pRow_DesignObjVariation_Text_Skin_Language->Y_get();
	if( m_pRow_DesignObjVariation_Text_Skin_Language->Width_get()==0 )
		m_rPosition.Width = m_pParent->m_rPosition.Width - m_rPosition.X;
	else
		m_rPosition.Width = m_pRow_DesignObjVariation_Text_Skin_Language->Width_get();

	if(m_pRow_DesignObjVariation_Text_Skin_Language->Height_get()==0)
		m_rPosition.Height = m_pParent->m_rPosition.Height - m_rPosition.Y;
	else
		m_rPosition.Height = m_pRow_DesignObjVariation_Text_Skin_Language->Height_get();

	bool bContainsRunTimeVariables;
	m_sText = m_pParent->SubstituteVariables(m_sText,&bContainsRunTimeVariables);
if( m_sText.find("Copy")!=string::npos )
{
int k=2;
}
	/*


todo c++
	vector<Row_Text_LS_AltVersions *> vectAVs;
    m_pdrText_LS->Text_LS_AltVersions_FK_Text_getrows(&vectAVs);
	for(size_t i=0;i<vectAVs.size();++i)
	{
		Row_Text_LS_AltVersions *drTLAV = vectAVs[i];
		if( drTLAV->FK_Language_get()==1 || drTLAV->FK_Language_get()==drOrbiter->FK_Language_get() )
		{
			bool b;
			// if this is the matching language, then force the use of this variation, otherwise only use it if we don't already have a matching language
			if( drTLAV->FK_Language_get()==drOrbiter->FK_Language_get() || 
				m_mapAltTextVersions.find(drTLAV->Version_get())==m_mapAltTextVersions.end() )
				m_mapAltTextVersions[drTLAV->Version_get()]=Parent->SubstituteVariables(drTLAV->Description_get(),&b);
		}
	}
*/
	m_bPreRender = (!bContainsRunTimeVariables || m_mapAltVersions.size()>0);
}

Row_Text_LS *CGText::GetText_LS(int PK_Text,OrbiterGenerator *pOrbiterGen)
{
	Database_pluto_main *mds = pOrbiterGen->m_spDatabase_pluto_main.get();
	Row_Text_LS *pRow_Text_LS = mds->Text_LS_get()->GetRow(PK_Text,pOrbiterGen->m_pRow_Language->PK_Language_get());
	if( pRow_Text_LS==NULL )
		pRow_Text_LS=mds->Text_LS_get()->GetRow(PK_Text,0);
	if( pRow_Text_LS==NULL )
		pRow_Text_LS=mds->Text_LS_get()->GetRow(PK_Text,1);
	if( pRow_Text_LS==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Warning!  Text Object: %d no text",PK_Text);
		pRow_Text_LS=mds->Text_LS_get()->GetRow(TEXT_USR_ENTRY_CONST,1);  // Generic text object
	}

	return pRow_Text_LS;
}
