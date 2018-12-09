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
#include <iostream>
#include <string>
#include "OGArray.h"
#include "OGText.h"
#include "OGCommand.h"
#include "DesignObj_Generator.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "OrbiterGen.h"
#include "SerializeClass/ShapesColors.h"
#include "Renderer.h"
#include "SerializeClass/SerializeClass.h"
#include "Orbiter/RendererMNG.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_Floorplan.h"
#include "pluto_main/Table_FloorplanType.h"
#include "pluto_main/Table_FloorplanObjectType.h"
#include "pluto_main/Table_CachedScreens.h"
#include "pluto_main/Table_Image.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Installation_Users.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_D_Command.h"
#include "pluto_main/Table_DesignObj.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_DesignObjVariation.h"
#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_Text.h"
#include "pluto_main/Table_DesignObjVariation_Zone.h"
#include "pluto_main/Table_DesignObjVariation_DesignObj.h"
#include "pluto_main/Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_DesignObjParameter.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Size.h"
#include "pluto_main/Table_Language.h"
#include "pluto_main/Table_UI.h"
#include "pluto_main/Table_Skin.h"
#include "pluto_main/Table_Style.h"
#include "pluto_main/Table_StyleVariation.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_D.h"
#include "pluto_main/Table_Icon.h"
#include "pluto_main/Table_Array.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_DesignObjType.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_DesignObj.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_VertAlignment.h"
#include "pluto_main/Define_Text.h"
#include "pluto_media/Table_MediaType_AttributeType.h"
#include "pluto_media/Table_AttributeType.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_FileFormat.h"
#include "pluto_media/Table_MediaSubType.h"
#include "pluto_media/Table_MediaSource.h"
#include "pluto_media/Table_Bookmark.h"

extern bool g_bBootSplash;

#define TOTAL_ESTIMATED_SCREENS 330

DesignObj_Generator::DesignObj_Generator(OrbiterGenerator *pGenerator,class Row_DesignObj * drDesignObj,class PlutoRectangle rPosition,class DesignObj_Generator *ocoParent,bool bAddToGenerated,bool bDontShare,bool bProcess)
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"const %p %d",this,drDesignObj->PK_DesignObj_get());
    m_pRow_DesignObjVariation=NULL;
    m_pRow_DesignObjVariation_Standard=NULL;
    m_bContainsArrays=false;
    m_bDontShare=false;
    m_bCanBeHidden=false;
    m_bChildrenBeforeText=false;
    m_bChildrenBehind=false;
    m_bDontMergeBackground=false;
    m_bHideByDefault=false;
    m_bValuesScaled=false;
    m_iVersion=0;
    m_iFloorplanPage=0;
    m_iFloorplanDevice=0; // Only used for floorplan objects
    m_iPK_CommandGroup_Touch_Extra=0;  // An extra action group to execute, used when this object is a button in an action group array
    m_pRow_Device_Goto=NULL;
    m_DesignObj_GeneratorGoto=NULL;
    m_pOrbiterGenerator=pGenerator;
    m_bProcessed=m_bRendered=false;
	m_bUseOCG=pGenerator->m_bUseOCG;
	m_bIsPopup=false;
	m_PK_DesignObj_Goto=0;
	m_iNumFloorplanItems=0;

	m_bPreserveAspectRatio = m_pOrbiterGenerator->m_pRow_Size->PreserveAspectRatio_get() == 1;

	if( ocoParent )
		m_iScale=m_iScaleFromParent=ocoParent->m_iScale;
	else
		m_iScaleFromParent=m_iScale=100;

	m_bContainsFloorplans=false;

    m_mds=drDesignObj->Table_DesignObj_get()->Database_pluto_main_get();
    m_VariableMap = m_pOrbiterGenerator->m_mapVariable;

	// See if there is another screen we should use instead
	vector<Row_DesignObj *> vectRow_DesignObj_Alt;
	m_mds->DesignObj_get()->GetRows("FK_DesignObj_SubstForSkin=" + StringUtils::itos(drDesignObj->PK_DesignObj_get()) + " AND FK_Skin_SubstForSkin=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get()), &vectRow_DesignObj_Alt);
	if( vectRow_DesignObj_Alt.size() )
		m_pRow_DesignObj=vectRow_DesignObj_Alt[0];
	else
		m_pRow_DesignObj=drDesignObj;
    m_ocoParent=ocoParent;
    m_rPosition=rPosition;
	if( m_rPosition.Width==-1 )
		m_rPosition.Right( m_ocoParent->m_rPosition.Right() );
	if( m_rPosition.Height==-1 )
		m_rPosition.Bottom( m_ocoParent->m_rPosition.Bottom() );
	m_pDesignObj_TopMost=this;
	while( m_pDesignObj_TopMost->m_ocoParent )
		m_pDesignObj_TopMost = m_pDesignObj_TopMost->m_ocoParent;

    m_bDontShare=bDontShare;
    m_bUsingCache=false;

	if( m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
		m_iFloorplanPage = m_pOrbiterGenerator->m_iFloorplanPage;

/*
if( m_pRow_DesignObj->PK_DesignObj_get()==4871 ||  m_pRow_DesignObj->PK_DesignObj_get()==5088 )// ||  m_pRow_DesignObj->PK_DesignObj_get()==5112 ) 
{
    int k=2; 
}
*/
    // Pick the standard variation, the specialized variation we will use for parameters, and all matching variations for including child objects
    PickVariation(m_pOrbiterGenerator,m_pRow_DesignObj,&m_pRow_DesignObjVariation,&m_pRow_DesignObjVariation_Standard,&m_alDesignObjVariations);

    if( !m_pRow_DesignObjVariation_Standard )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Aborting building of: %d for no var",m_pRow_DesignObj->PK_DesignObj_get());
		m_pOrbiterGenerator->m_iPK_CommandGroup=0;
        return;
    }

	m_bDontScale = GetParm(DESIGNOBJPARAMETER_Dont_Scale_CONST, 0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get())=="1";

	m_bLocationSpecific=false;
	if( m_pOrbiterGenerator->m_mapDesignObj_WithArrays.find(m_pRow_DesignObjVariation_Standard->FK_DesignObj_get())!=
		m_pOrbiterGenerator->m_mapDesignObj_WithArrays.end() ||
		m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu==m_pRow_DesignObj || m_pOrbiterGenerator->m_pRow_DesignObj_Sleeping==m_pRow_DesignObj )
	{
		bAddToGenerated = true;
		m_bDontShare = true;
        m_iVersion = m_pOrbiterGenerator->m_iLocation;
		m_bLocationSpecific=true;
		if( m_pOrbiterGenerator->m_dwMediaType )
			m_iVersion = m_pOrbiterGenerator->m_dwMediaType;
    }

	if( bAddToGenerated )
    {
//if( m_pRow_DesignObj->PK_DesignObj_get()!=1255 )
//return;
		int Percent = ++m_pOrbiterGenerator->m_iScreensTotal * 100 / TOTAL_ESTIMATED_SCREENS;
		if( !m_pOrbiterGenerator->m_iLastReportedPercentage || Percent - m_pOrbiterGenerator->m_iLastReportedPercentage > 3 )
		{
			m_pOrbiterGenerator->m_iLastReportedPercentage = max(1,Percent);
			if( g_bBootSplash )
				system( ("/usr/pluto/bin/BootMessage.sh \"Stage 1 of 2 - Generating screen " + StringUtils::itos(m_pOrbiterGenerator->m_iScreensTotal) + "\"").c_str());
			else
			{
				m_pOrbiterGenerator->m_pRow_Orbiter->Reload();
				m_pOrbiterGenerator->m_pRow_Orbiter->RegenStatus_set("Stage 1 of 2 - Generating screen " + StringUtils::itos(m_pOrbiterGenerator->m_iScreensTotal));
				m_pOrbiterGenerator->m_pRow_Orbiter->RegenPercent_set(m_pOrbiterGenerator->m_iLastReportedPercentage);
				m_pOrbiterGenerator->m_pRow_Orbiter->Table_Orbiter_get()->Commit();
			}
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Generating screen: %d ver: %d/%s room: %d ea: %d, orb: %d",
			drDesignObj->PK_DesignObj_get(),m_iVersion,(m_bLocationSpecific ? "Y" : "N"),
			(m_pOrbiterGenerator->m_pRow_Room ? m_pOrbiterGenerator->m_pRow_Room->PK_Room_get() : 0),
			(m_pOrbiterGenerator->m_pRow_EntertainArea ? m_pOrbiterGenerator->m_pRow_EntertainArea->PK_EntertainArea_get() : 0),
			pGenerator->m_pRow_Device->PK_Device_get());
		if( m_pOrbiterGenerator->m_pRow_Size->PreserveAspectRatio_get()==2 )  // Means only preserve the backgrounds
			m_bPreserveAspectRatio=true;

		listDesignObj_Generator *al = m_pOrbiterGenerator->m_htGeneratedScreens[StringUtils::itos(drDesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion)];
        if( al==NULL )
        {
            al=new listDesignObj_Generator();
            m_pOrbiterGenerator->m_htGeneratedScreens[StringUtils::itos(drDesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion)]=al;
if( StringUtils::itos(drDesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion)=="1270.0" )
int k=2;
		}
        al->push_back(this);

		bool bSoleScreenCache = m_pOrbiterGenerator->m_map_PK_DesignObj_SoleScreenToGen.size()!=0 && m_pOrbiterGenerator->m_map_PK_DesignObj_SoleScreenToGen[m_pRow_DesignObj->PK_DesignObj_get()]==false;
        if( m_pOrbiterGenerator->m_bOrbiterChanged==false || m_pOrbiterGenerator->m_map_PK_DesignObj_SoleScreenToGen.size() )
        {
            // Let's see if we can just use a cached version
            Row_CachedScreens *pdrCachedScreen = m_mds->CachedScreens_get()->GetRow(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get(),m_pRow_DesignObj->PK_DesignObj_get(),m_iVersion);
            if( pdrCachedScreen && pdrCachedScreen->Schema_get()==ORBITER_SCHEMA &&
				(m_pOrbiterGenerator->m_map_PK_DesignObj_SoleScreenToGen.size()==0 || m_pOrbiterGenerator->m_map_PK_DesignObj_SoleScreenToGen[m_pRow_DesignObj->PK_DesignObj_get()]==false) )
            {
				time_t lModDate1 = StringUtils::SQLDateTime(pdrCachedScreen->Modification_LastGen_get());
				time_t lModDate2 = StringUtils::SQLDateTime(m_pRow_DesignObj->psc_mod_get());
				if( lModDate1==lModDate2 || bSoleScreenCache )
				{
					string Filename = m_pOrbiterGenerator->m_sOutputPath + "screen " + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get()) + "." +
						StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion) + "." + StringUtils::itos((int) lModDate1) + ".cache";
					if( FileUtils::FileExists(Filename) )
					{
						if( !ReadRegenVersion(Filename+".regen") )
						{
							m_vectRegenMonitor.clear();
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"Regenerating: cache can't be read");
						}
						else if( !bSoleScreenCache && !CachedVersionOK() )
						{
							m_vectRegenMonitor.clear();
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"Regenerating: cache not ok (sole screen %d)",(int) bSoleScreenCache);
						}
						else if( SerializeRead(Filename) )
						{
							m_bUsingCache=true;
							ReadFloorplanInfo(Filename+".fp");
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"Not building screen %s found valid cache",
								(StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion)).c_str());
							m_pOrbiterGenerator->m_iPK_CommandGroup=0;
							return;
						}
						else
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"Regenerating: cannot serialize cache file");
					}
					else
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Regenerating: cache file %s doesn't exist",Filename.c_str());
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Regenerating: screen has changed from %d to %d %s/%s",long(lModDate1),long(lModDate2),pdrCachedScreen->Modification_LastGen_get().c_str(),m_pRow_DesignObj->psc_mod_get().c_str());
            }
			else
			{
				if( pdrCachedScreen==NULL )
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Regenerating:  cached screen not in DB");
				else
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Regenerating: schema changed from %d to %d",pdrCachedScreen->Schema_get(),(int) ORBITER_SCHEMA);
			}
        }
        string Filespec = m_pOrbiterGenerator->m_sOutputPath + "*" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get()) + "." +
			StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion) + ".*";
		string Filespec2 = m_pOrbiterGenerator->m_sOutputPath + StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion) + ".*";
#ifdef WIN32
        // Remove any old cached files
        string::size_type s;

        // Replace the \ with / for windows
        while( (s=Filespec.find("/"))!=string::npos )
            Filespec.replace(s,1,"\\");
        while( (s=Filespec2.find("/"))!=string::npos )
            Filespec2.replace(s,1,"\\");

        system( ("del \"" + Filespec + "\" 2> null").c_str() );
        system( ("del \"" + Filespec2 + "\" 2> null").c_str() );
#else
        // Remove any old cached files
        system( ("rm " + Filespec + " 2>/dev/null").c_str() );
        system( ("rm " + Filespec2 + " 2>/dev/null").c_str() );
#endif

		m_pOrbiterGenerator->m_iScreensToRender++;
    }

    if( !m_pRow_DesignObjVariation->FK_Criteria_Orbiter_isNull() )
        m_pOrbiterGenerator->m_mapUsedOrbiterCriteria[m_pRow_DesignObjVariation->FK_Criteria_Orbiter_get()]=true;

    // This is an object that is part of an array, and this button will execute the designated action group
    if( m_pOrbiterGenerator->m_iPK_CommandGroup!=0 )
    {
        m_iPK_CommandGroup_Touch_Extra = m_pOrbiterGenerator->m_iPK_CommandGroup;
        m_pOrbiterGenerator->m_iPK_CommandGroup=0;  // reset back to 0 so it doesn't affect the children
    }
	m_sAdjustments = GetParm(DESIGNOBJPARAMETER_Adjustments_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get());

	if( bProcess )
		Process();
}
	
void DesignObj_Generator::Process()
{
	if( m_bProcessed )  // Don't process anything twice
		return;
	m_bProcessed = true;

	HandleAdjustments();
	for(int GraphicType=1;GraphicType<=4;++GraphicType)
    {
        // Handle the background image
        string o;

        if( m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
        {
            if( GraphicType==1 )
            {
                o = "/../../floorplans/inst" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + "/" + StringUtils::itos(m_iFloorplanPage) + ".png";
/*
                string[] sFiles = Directory.GetFiles(m_pOrbiterGenerator->m_GraphicsBasePath + "/Floorplans/I" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + "/","Page" + StringUtils::itos(m_iFloorplanPage) + ".*");
                if( sFiles.Length>0 )
                {
                    o = sFiles[0].substr(sFiles[0].IndexOf("/Floorplans/"));
                }
*/
            }
        }
        else
        {
            if( GraphicType==1 )
			{
				if( m_pOrbiterGenerator->m_iLocation < int(m_pOrbiterGenerator->m_dequeLocation.size()) &&
						m_pRow_DesignObj==m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu &&
						FileUtils::FileExists(m_pOrbiterGenerator->m_GraphicsBasePath + "/" + m_pOrbiterGenerator->m_pRow_Skin->DataSubdirectory_get() + 
						"/../../orbiter_bg/" + StringUtils::itos(m_pOrbiterGenerator->m_iPK_Orbiter) + "_" + 
						StringUtils::itos(m_pOrbiterGenerator->m_dequeLocation[m_pOrbiterGenerator->m_iLocation]->PK_Room) + ".png") )
					o = "../../orbiter_bg/" + StringUtils::itos(m_pOrbiterGenerator->m_iPK_Orbiter) + "_" + StringUtils::itos(m_pOrbiterGenerator->m_dequeLocation[m_pOrbiterGenerator->m_iLocation]->PK_Room) + ".png";
				else if( m_pRow_DesignObj==m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu &&
						FileUtils::FileExists(m_pOrbiterGenerator->m_GraphicsBasePath + "/" + m_pOrbiterGenerator->m_pRow_Skin->DataSubdirectory_get() + "/../../orbiter_bg/" + StringUtils::itos(m_pOrbiterGenerator->m_iPK_Orbiter) + ".png") )
					o = "../../orbiter_bg/" + StringUtils::itos(m_pOrbiterGenerator->m_iPK_Orbiter) + ".png";
				else
					o=GetParm(DESIGNOBJPARAMETER_Graphic_Filename_CONST, 0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get());
			}
            else if( GraphicType==2 && !m_pOrbiterGenerator->m_bIgnoreSelected )
                o=GetParm(DESIGNOBJPARAMETER_Selected_Graphic_Fil_CONST, 0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get());
            else if( GraphicType==3 && !m_pOrbiterGenerator->m_bIgnoreHighlighted)
                o=GetParm(DESIGNOBJPARAMETER_Highlighted_Graphic_CONST, 0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get());
            else if( !m_pOrbiterGenerator->m_bIgnoreAlt )
                o=GetParm(DESIGNOBJPARAMETER_Alt_Graphic_File_CONST, 0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get());
        }

        string sGraphicFile = o;

        if( sGraphicFile.length()>0 )
        {
            if( m_pOrbiterGenerator->m_bNoEffects )
            {
                sGraphicFile = StringUtils::Replace(&sGraphicFile,".MNG",".PNG");
                sGraphicFile = StringUtils::Replace(&sGraphicFile,".mng",".png");
            }
            size_t scPos=0;
            for(;;)
            {
                // Handle the fact that if this is an alt file it may be a ; delimted list
                if( GraphicType==4 )
                {
                    size_t NewscPos = o.find(";",scPos);
                    if( NewscPos!=string::npos )
                    {
                        sGraphicFile = o.substr(scPos,NewscPos-scPos);
                        scPos = NewscPos+1;
                    }
                    else if( scPos>=o.length() )
                        break;
                    else
                    {
                        sGraphicFile = o.substr(scPos);
                        scPos=o.length(); // The last time around
                    }

                }

                Row_Image * drImage=NULL;
                // Be sure there are no windows paths in here.  Windows will work with forward slashes also, use them instead
                sGraphicFile = StringUtils::Replace(&sGraphicFile,"\\","/");

				string sAltFile;
				string::size_type pos_alt_file;
				if( (pos_alt_file=sGraphicFile.find('|'))!=string::npos )
				{
					sAltFile = sGraphicFile.substr(pos_alt_file+1);
					sGraphicFile = sGraphicFile.substr(0,pos_alt_file);
				}
retry_alt_file:
                string sOriginalFile = sGraphicFile;
	            if( sGraphicFile[0]=='/' )
                    sGraphicFile = m_pOrbiterGenerator->m_GraphicsBasePath + sGraphicFile;
                else
                    sGraphicFile = m_pOrbiterGenerator->m_GraphicsBasePath + "/" + m_pOrbiterGenerator->m_pRow_Skin->DataSubdirectory_get() + "/" + sGraphicFile;

                if( !FileUtils::FileExists(sGraphicFile) )
                {
                    sGraphicFile = m_pOrbiterGenerator->m_GraphicsBasePath + "/" + m_mds->Skin_get()->GetRow(1)->DataSubdirectory_get() + "/" + sOriginalFile;  // hack in skin 1
                    if( !FileUtils::FileExists(sGraphicFile) )
                    {
						if( sAltFile.size() )
						{
							sGraphicFile=sAltFile;
							sAltFile="";
							goto retry_alt_file;
						}

						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Error reading graphic: %s %d",sGraphicFile.c_str(),m_pRow_DesignObjVariation->FK_DesignObj_get());
                        sGraphicFile = "";
                    }
                }

                if( sGraphicFile.length()>0 )
                {
Table_Image *p = m_mds->Image_get();
                    drImage = m_mds->Image_get()->GetRow(sGraphicFile);
                    if( drImage && (drImage->Width_get()==0 || drImage->Height_get()==0) )
                    {
                        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Image: %s has width/height=0",sGraphicFile.c_str());
                        break;
                    }

//                  time_t t1 = StringUtils::SQLDateTime(drImage->Date_get());
//                  time_t t2 = FileUtils::FileDate(sGraphicFile);
                    if( !drImage || drImage->Date_isNull() || StringUtils::SQLDateTime(drImage->Date_get())!=FileUtils::FileDate(sGraphicFile) )
                    {
                        // Calculate the width and height.  Brute force so we don't have to link another library
                        unsigned char buf[200];

                        unsigned long Width=0;
                        unsigned long Height=0;
						if( StringUtils::ToUpper(FileUtils::FindExtension(sGraphicFile))=="MNG" )
                        {
							RendererMNG *pRendererMNG = Renderer::GetInstance().CreateMNGFromFile(sGraphicFile, PlutoSize(0,0));
//							RendererImage *pRendererImage = pRendererMNG->GetFrame(0);
//							Width=pRendererImage->m_pSDL_Surface->w;
//							Height=pRendererImage->m_pSDL_Surface->h;
							Height = pRendererMNG->GetHeader().frame_height;
							Width = pRendererMNG->GetHeader().frame_width;

							delete pRendererMNG;
						}
						else
						{
							FILE *file = fopen(sGraphicFile.c_str(),"rb");
							if( !file )
							{
								LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"File %s exists, but is not readable",sGraphicFile.c_str());
								return;
							}
							size_t s_read = fread(buf,1,200,file);
							size_t i;
							for(i=0;i<s_read-15;++i)
							{
								if( strncmp((const char *)&buf[i],"IHDR",4)==0 )
									break;
							}

							if( i>=s_read-15 )
							{
								LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot get width and height of  %s",sGraphicFile.c_str());
								return;
							}

							Width=buf[i+6]*256+buf[i+7];
							Height=buf[i+10]*256+buf[i+11];
							fclose(file);
						}
                        if( !drImage )
                        {
                            drImage = m_mds->Image_get()->AddRow();
                            drImage->PK_Image_set(sGraphicFile);
                            drImage->Date_set(StringUtils::SQLDateTime(FileUtils::FileDate(sGraphicFile)));
                            drImage->Width_set(Width);
                            drImage->Height_set(Height);
                            m_mds->Image_get()->Commit();
                        }
                        else
                        {
                            drImage->Width_set(Width);
                            drImage->Height_set(Height);
                            drImage->Date_set(StringUtils::SQLDateTime(FileUtils::FileDate(sGraphicFile)));
                            m_mds->Image_get()->Commit();
                        }
                    }
                    if( GraphicType==1 )
                    {
                        int X=m_rPosition.X,Y=m_rPosition.Y;
                        Row_DesignObjVariation_DesignObjParameter * drOVCP_C_X = GetParmRow(DESIGNOBJPARAMETER_X_Position_CONST,m_pRow_DesignObjVariation);
                        if( drOVCP_C_X==NULL && m_pRow_DesignObjVariation->PK_DesignObjVariation_get()!=m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get() )
                            drOVCP_C_X = GetParmRow(DESIGNOBJPARAMETER_X_Position_CONST,m_pRow_DesignObjVariation);

                        Row_DesignObjVariation_DesignObjParameter * drOVCP_C_Y = GetParmRow(DESIGNOBJPARAMETER_Y_Position_CONST,m_pRow_DesignObjVariation);
                        if( drOVCP_C_Y==NULL && m_pRow_DesignObjVariation->PK_DesignObjVariation_get()!=m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get() )
                            drOVCP_C_Y = GetParmRow(DESIGNOBJPARAMETER_Y_Position_CONST,m_pRow_DesignObjVariation);

						Row_DesignObjVariation_DesignObjParameter * drOVCP_C_CX = GetParmRow(DESIGNOBJPARAMETER_Crop_X_CONST,m_pRow_DesignObjVariation);
                        if( drOVCP_C_CX==NULL && m_pRow_DesignObjVariation->PK_DesignObjVariation_get()!=m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get() )
                            drOVCP_C_CX = GetParmRow(DESIGNOBJPARAMETER_Crop_X_CONST,m_pRow_DesignObjVariation);

                        Row_DesignObjVariation_DesignObjParameter * drOVCP_C_CY = GetParmRow(DESIGNOBJPARAMETER_Crop_Y_CONST,m_pRow_DesignObjVariation);
                        if( drOVCP_C_CY==NULL && m_pRow_DesignObjVariation->PK_DesignObjVariation_get()!=m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get() )
                            drOVCP_C_CY = GetParmRow(DESIGNOBJPARAMETER_Crop_Y_CONST,m_pRow_DesignObjVariation);

                        if( drOVCP_C_X && !drOVCP_C_X->Value_isNull() && drOVCP_C_X->Value_get()!="" )
                            m_rBitmapOffset.X = atoi(drOVCP_C_X->Value_get().c_str());
                        if( drOVCP_C_Y && !drOVCP_C_Y->Value_isNull() && drOVCP_C_Y->Value_get()!="" )
                            m_rBitmapOffset.Y = atoi(drOVCP_C_Y->Value_get().c_str());
                        if( drOVCP_C_CX && !drOVCP_C_CX->Value_isNull() && drOVCP_C_CX->Value_get()!="" )
                            m_rBitmapOffset.Width = atoi(drOVCP_C_CX->Value_get().c_str());
                        if( drOVCP_C_CY && !drOVCP_C_CY->Value_isNull() && drOVCP_C_CY->Value_get()!="" )
                            m_rBitmapOffset.Height = atoi(drOVCP_C_CY->Value_get().c_str());

						m_rBackgroundPosition.Location(PlutoPoint(X,Y));
                    }

                    // If we have an image, and this is either the standard, or a non-primary image but there was no primary (width & height=0) set it
                    if( drImage && (GraphicType==1 || (m_rBackgroundPosition.Width==0 && m_rBackgroundPosition.Height==0) ) )
                    {
                        m_sOriginalSize = PlutoSize(drImage->Width_get(),drImage->Height_get());
				        
						// 12/10/2004 Aaron - The code below (10/6) forces all floorplans to be the size of the graphic.  We can't guarantee users will upload floorplans of the right size
						if( m_pRow_DesignObj->FK_DesignObjType_get()!=DESIGNOBJTYPE_Floorplan_CONST )
						{
							/* 10/6/2004 Aaron - Designer adds wrong width/heights sometimes, particularly with objects with multiple variations.  For now always use the actual w/h */
							/* 5/14/2006 Aaron - This isn't right.  Put it back and see why it was removed in the first place.  Now icons that are small are causing the button to be stretched too big */
							if( m_rPosition.Width>0 )
								m_rBackgroundPosition.Width = m_rPosition.Width;
							else
							{
								m_rPosition.Width = drImage->Width_get();
								m_rBackgroundPosition.Width = drImage->Width_get();
							}
							/*
							m_rPosition.Width = drImage->Width_get();
							m_rBackgroundPosition.Width = drImage->Width_get();
							*/

			                string sWidth = GetParm(DESIGNOBJPARAMETER_Width_CONST,true);
							if( sWidth.size() )
							{
								m_bPreserveAspectRatio=false;
								m_rPosition.Width = m_rBackgroundPosition.Width = atoi(sWidth.c_str());
							}

							/* 10/6/2004 Aaron - Designer adds wrong width/heights sometimes, particularly with objects with multiple variations.  For now always use the actual w/h */
							/* 5/14/2006 Aaron - This isn't right.  Put it back and see why it was removed in the first place.  Now icons that are small are causing the button to be stretched too big */
							if( m_rPosition.Height>0 )
								m_rBackgroundPosition.Height = m_rPosition.Height;
							else
							{
								m_rPosition.Height = drImage->Height_get();
								m_rBackgroundPosition.Height = drImage->Height_get();
							}
							/*
							m_rPosition.Height = drImage->Height_get();
							m_rBackgroundPosition.Height = drImage->Height_get();
							*/

							string sHeight = GetParm(DESIGNOBJPARAMETER_Height_CONST,true);
							if( sHeight.size() )
							{
								m_bPreserveAspectRatio=false;
								m_rPosition.Height = m_rBackgroundPosition.Height = atoi(sHeight.c_str());
							}
						}
						else
							m_rBackgroundPosition = m_rPosition;
                    }

                    if( GraphicType==1 )
                        m_sOrigBackgroundFile = sGraphicFile;
                    else if( GraphicType==2 )
                        m_sOrigSelectedFile = sGraphicFile;
                    else if( GraphicType==3 )
                        m_sOrigHighlightGraphicFilename = sGraphicFile;
                    else
                    {
                        m_vectOrigAltGraphicFilename.push_back(sGraphicFile);
                    }
                }

                if( GraphicType!=4 )
                    break;

            }
        }
    }

	// See if there's a button
	if( !m_pRow_DesignObjVariation->FK_Button_isNull() )
		m_iPK_Button = m_pRow_DesignObjVariation->FK_Button_get();
	else if( !m_pRow_DesignObjVariation_Standard->FK_Button_isNull() )
		m_iPK_Button = m_pRow_DesignObjVariation_Standard->FK_Button_get();
	else
		m_iPK_Button = 0;

    // Add all text objects
    for(size_t sov=0;sov<m_alDesignObjVariations.size();++sov)
    {
        Row_DesignObjVariation * drOV=m_alDesignObjVariations[sov];
        vector<class Row_DesignObjVariation_Text *> vectrovt;
        drOV->DesignObjVariation_Text_FK_DesignObjVariation_getrows(&vectrovt);
        for(size_t s=0;s<vectrovt.size();++s)
        {
            Row_DesignObjVariation_Text *drOVT = vectrovt[s];
            vector<class Row_DesignObjVariation_Text_Skin_Language *> vectovtsr;
            drOVT->DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text_getrows(&vectovtsr);
            Row_DesignObjVariation_Text_Skin_Language * drOVTSL_match=NULL;
            bool bMatchLanguage=false;
            bool bMatchSkin=false;

            for(size_t s2=0;s2<vectovtsr.size();++s2)
            {
                Row_DesignObjVariation_Text_Skin_Language * drOVTSL = vectovtsr[s2];
                if( !drOVTSL->FK_Skin_isNull() && drOVTSL->FK_Skin_get()==m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get() &&
                    !drOVTSL->FK_Language_isNull() && drOVTSL->FK_Language_get()==m_pOrbiterGenerator->m_pRow_Language->PK_Language_get() )
                {
                    drOVTSL_match = drOVTSL;
                    break;
                }
                else if( !drOVTSL->FK_Skin_isNull() && drOVTSL->FK_Skin_get()==m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get() &&
                    drOVTSL->FK_Language_isNull() )
                {
                    drOVTSL_match = drOVTSL;
                    bMatchSkin=true;
                }
                else if( !drOVTSL->FK_Language_isNull() && drOVTSL->FK_Language_get()==m_pOrbiterGenerator->m_pRow_Language->PK_Language_get() &&
                    drOVTSL->FK_Skin_isNull() && !bMatchSkin )
                {
                    drOVTSL_match = drOVTSL;
                    bMatchLanguage=true;
                }
                else if( drOVTSL->FK_Language_isNull() && drOVTSL->FK_Skin_isNull() && !bMatchSkin && !bMatchLanguage)
                {
                    drOVTSL_match = drOVTSL;
                }
            }

            // We found text to put on the screen
            if( drOVTSL_match!=NULL )
            {
                Row_Style *drStyle=NULL;
                if( !drOVTSL_match->FK_Style_isNull() )
                    drStyle = drOVTSL_match->FK_Style_getrow();
                else
                    drStyle = m_pOrbiterGenerator->m_pRow_Skin->FK_Style_getrow();

                if( !drStyle )
                {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot open style %d",drOVTSL_match->FK_Style_get());
                }
                CGText *pCGText = new CGText(this,drOVTSL_match,m_pOrbiterGenerator->m_pRow_Orbiter);
if( pCGText->m_sText=="1" )
{
int k=2;
}
                m_vectDesignObjText.push_back(pCGText);
                PickStyleVariation(drStyle,m_pOrbiterGenerator,pCGText->m_mapTextStyle);

                if( !pCGText->m_mapTextStyle.size() )
                {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find a matching style variation %d",drStyle->PK_Style_get());
                }

                // Be sure we have put all used styles in the map
                MapTextStyle::iterator it;
                for(it=pCGText->m_mapTextStyle.begin();it!=pCGText->m_mapTextStyle.end();++it)
                {
                    m_pOrbiterGenerator->m_htUsedStyleVariation[(*it).first]=1;
                }

                pCGText->m_rPosition.X += m_rPosition.X;  // Offset by our position
                pCGText->m_rPosition.Y += m_rPosition.Y;
            }
        }
    }

    // Add all the zones and actions
    for(size_t s=0;s<m_alDesignObjVariations.size();++s)
    {
        Row_DesignObjVariation * drOV = m_alDesignObjVariations[s];

        vector<class Row_DesignObjVariation_Zone *> vectovz;
        drOV->DesignObjVariation_Zone_FK_DesignObjVariation_getrows(&vectovz);
        for(size_t s2=0;s2<vectovz.size();++s2)
        {
            Row_DesignObjVariation_Zone * drZone = vectovz[s2];
            CGZone *oczone = new CGZone(drZone->FK_CommandGroup_D_getrow(),this);
            oczone->m_Rect = PlutoRectangle(drZone->X_get(),drZone->Y_get(),drZone->Width_get(),drZone->Height_get());
            if( oczone->m_Rect.X<=m_pOrbiterGenerator->m_sizeScreen->Width && oczone->m_Rect.Y<=m_pOrbiterGenerator->m_sizeScreen->Height )
                m_ZoneList.push_back(oczone);
        }
        if( !drOV->FK_CommandGroup_D_OnActivate_isNull() )
        {
            CGZone *oczone = new CGZone(drOV->FK_CommandGroup_D_OnActivate_getrow(),this);
            m_ZoneList.push_back(oczone);
        }
        // Use CGZone to build the actions for on load and unload
        if( !drOV->FK_CommandGroup_D_OnLoad_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnLoad_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_LoadList.push_back(oa);
            }
        }
        if( !drOV->FK_CommandGroup_D_OnUnload_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnUnload_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_UnloadList.push_back(oa);
            }
        }
        // Use CGZone to build the actions for on load and unload
        if( !drOV->FK_CommandGroup_D_OnHighlight_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnHighlight_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_HighlightList.push_back(oa);
            }
        }
        if( !drOV->FK_CommandGroup_D_OnUnhighlight_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnUnhighlight_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_UnhighlightList.push_back(oa);
            }
        }
        if( !drOV->FK_CommandGroup_D_OnTimeout_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnTimeout_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_TimeoutList.push_back(oa);
            }
        }
        if( !drOV->FK_CommandGroup_D_OnStartup_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnStartup_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
				if( oa->m_PK_Command==COMMAND_Scale_this_object_CONST )
					m_iScale = atoi(oa->m_ParameterList[COMMANDPARAMETER_Value_CONST].c_str());
				else if( oa->m_PK_Command==COMMAND_Center_Parents_Text_If_Blank_CONST )
				{
					if( m_sOrigBackgroundFile.size()==0 )
					{
						for(size_t s=0;s<m_ocoParent->m_vectDesignObjText.size();++s)
						{
							DesignObjText *pDesignObjText = m_ocoParent->m_vectDesignObjText[s];
							pDesignObjText->m_iPK_VertAlignment=VERTALIGNMENT_Middle_CONST;
						}
					}
				}
				else
	                m_Action_StartupList.push_back(oa);
            }
        }
    }

	//assign here the effects for each effect event for an object, based on Generator's mapping between
	//effect type and effect for the right skin
	m_FK_Effect_Selected_WithChange = m_pOrbiterGenerator->m_mapEffects[m_pRow_DesignObjVariation->FK_EffectType_Selected_WithChange_get()]; 
	m_FK_Effect_Selected_NoChange = m_pOrbiterGenerator->m_mapEffects[m_pRow_DesignObjVariation->FK_EffectType_Selected_NoChange_get()]; 
	m_FK_Effect_Highlighted = m_pOrbiterGenerator->m_mapEffects[m_pRow_DesignObjVariation->FK_EffectType_Highlighted_get()]; 

    if( m_iPK_CommandGroup_Touch_Extra!=0 )
    {
        Row_CommandGroup *drAG = m_mds->CommandGroup_get()->GetRow(m_iPK_CommandGroup_Touch_Extra);
        CGZone *oczone = new CGZone(drAG,this);
        m_ZoneList.push_back(oczone);
    }

    if( m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST && m_sOriginalSize.Width>0 && m_sOriginalSize.Height>0 )
    {
		// This object and all it's parents contain floorplans
		DesignObj_Generator *pObjFP = this;
		while( pObjFP )
		{
			pObjFP->m_bContainsFloorplans=true;
			pObjFP = pObjFP->m_ocoParent;
		}

		// This is a floorplan object, first we're going to have to scale this ourselves
        // since the background is of an unknown size, and then
        // add all the children for this page and type
        PlutoSize cgs = m_rBackgroundPosition.Size();
        PlutoPoint cgp(cgs.Width,cgs.Height);
        PlutoSize szTargetSize(ScaleValue(&cgp,m_pOrbiterGenerator->m_sScale.Width * m_iScale/100,m_pOrbiterGenerator->m_sScale.Height * m_iScale/100));
        int ScaleWidth = szTargetSize.Width * 1000 / m_sOriginalSize.Width;
        int ScaleHeight = szTargetSize.Height * 1000 / m_sOriginalSize.Height;
        int ScaleFactor = min(ScaleWidth,ScaleHeight);

        // Adjust the width and the height so that we don't mess up the aspect ratio
        m_rBackgroundPosition.Width = m_sOriginalSize.Width * ScaleFactor / m_pOrbiterGenerator->m_sScale.Width;
        m_rBackgroundPosition.Height = m_sOriginalSize.Height * ScaleFactor / m_pOrbiterGenerator->m_sScale.Height;
        m_rPosition.Width = m_rBackgroundPosition.Width;
        m_rPosition.Height = m_rBackgroundPosition.Height;

        int FloorplanType = atoi(GetParm(DESIGNOBJPARAMETER_Type_CONST).c_str());
        Row_FloorplanType *drFloorplanType = m_mds->FloorplanType_get()->GetRow(FloorplanType);

        vector<Row_Device_DeviceData *> vectRow_Device_DeviceData;
        vector<Row_EntertainArea *> vectRow_EntertainArea;

		if( FloorplanType==FLOORPLANTYPE_Entertainment_Zone_CONST )
		{
			string SQL = "JOIN Room ON FK_Room=PK_Room WHERE FK_Installation=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get());
			m_mds->EntertainArea_get()->GetRows(SQL,&vectRow_EntertainArea);
		}
		else
		{
			string SQL = "JOIN " + string(DEVICE_TABLE) + " ON " + DEVICE_DEVICEDATA_FK_DEVICE_FIELD + "=" + DEVICE_PK_DEVICE_FIELD +
				" JOIN " + FLOORPLANOBJECTTYPE_TABLE + " ON " + DEVICE_DEVICEDATA_IK_DEVICEDATA_FIELD + "=" + FLOORPLANOBJECTTYPE_PK_FLOORPLANOBJECTTYPE_FIELD +
				" WHERE " + DEVICE_FK_INSTALLATION_FIELD + "=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) +
				" AND " + DEVICE_DEVICEDATA_FK_DEVICEDATA_FIELD + "=" + StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) +
				" AND " + FLOORPLANOBJECTTYPE_FK_FLOORPLANTYPE_FIELD + "=" + StringUtils::itos(FloorplanType);

			m_mds->Device_DeviceData_get()->GetRows(SQL,&vectRow_Device_DeviceData);
		}

		string sRegenMonitor = m_pOrbiterGenerator->m_pRegenMonitor->GetModInfo_Floorplan(FloorplanType);
		if( sRegenMonitor.size()>0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"obj: %d regen cache %d %s",m_pRow_DesignObj->PK_DesignObj_get(),(unsigned int)m_pDesignObj_TopMost->m_vectRegenMonitor.size(),sRegenMonitor.c_str());
			m_pDesignObj_TopMost->m_vectRegenMonitor.push_back( sRegenMonitor );
		}

		Row_Device_DeviceData *pRow_Device_DeviceData_FPInfo=NULL;  // We may need this down below
		for(size_t s=0;s< (FloorplanType==FLOORPLANTYPE_Entertainment_Zone_CONST ? vectRow_EntertainArea.size() : vectRow_Device_DeviceData.size());++s)
        {
			string FPInfo_Value;
			string Description;
			int PK_Device_EA=0;
            Row_FloorplanObjectType *pRow_FloorplanObjectType = NULL;
			if( FloorplanType==FLOORPLANTYPE_Entertainment_Zone_CONST )
			{
				Row_EntertainArea *pRow_EntertainArea = vectRow_EntertainArea[s];
				Description = pRow_EntertainArea->Description_get();
				PK_Device_EA = pRow_EntertainArea->PK_EntertainArea_get() * -1;
				pRow_FloorplanObjectType = pRow_EntertainArea->FK_FloorplanObjectType_getrow();
				if( pRow_FloorplanObjectType )
					FPInfo_Value = pRow_EntertainArea->FloorplanInfo_get();
			}
			else
			{
				Row_Device_DeviceData *pRow_Device_DeviceData = vectRow_Device_DeviceData[s];
		        pRow_Device_DeviceData_FPInfo = m_mds->Device_DeviceData_get()->GetRow(pRow_Device_DeviceData->FK_Device_get(),DEVICEDATA_Floorplan_Info_CONST);
			    Row_Device_DeviceData *pRow_Device_DeviceData_ObjType = m_mds->Device_DeviceData_get()->GetRow(pRow_Device_DeviceData->FK_Device_get(),DEVICEDATA_PK_FloorplanObjectType_CONST);
				if( !pRow_Device_DeviceData_FPInfo || !pRow_Device_DeviceData_ObjType )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"01\t**Error: Floorplan object %d %s missing info",
						pRow_Device_DeviceData->FK_Device_getrow()->FK_DeviceTemplate_get(),
						pRow_Device_DeviceData->FK_Device_getrow()->FK_DeviceTemplate_getrow()->Description_get().c_str());
				}
				else
				{
					int FloorplanObjectType = atoi(pRow_Device_DeviceData_ObjType->IK_DeviceData_get().c_str());
					pRow_FloorplanObjectType = m_mds->FloorplanObjectType_get()->GetRow(FloorplanObjectType);
					FPInfo_Value = pRow_Device_DeviceData_FPInfo->IK_DeviceData_get();
					Description = pRow_Device_DeviceData_FPInfo->FK_Device_getrow()->Description_get();
					PK_Device_EA = pRow_Device_DeviceData_FPInfo->FK_Device_get();
				}
			}
            if( FPInfo_Value.length() )
            {
                // Parse the parameters to see if any of these sensors are on this page
                string::size_type pos=0;
                while(true)
                {
                    string sFloorplan = StringUtils::Tokenize(FPInfo_Value,string(","),pos);
                    if( sFloorplan.length()==0 )
                        break;
                    int X = atoi(StringUtils::Tokenize(FPInfo_Value,",",pos).c_str());
                    int Y = atoi(StringUtils::Tokenize(FPInfo_Value,",",pos).c_str());
					if( X<0 || Y<0 )
						continue;
                    if( atoi(sFloorplan.c_str())==m_iFloorplanPage )
                    {
                        m_VariableMap[VARIABLE_Array_ID_CONST] = pRow_FloorplanObjectType->Description_get();
                        m_VariableMap[VARIABLE_Array_Desc_CONST] = Description;

						if( X>m_sOriginalSize.Width || Y>m_sOriginalSize.Height )
						{
							if(NULL != pRow_Device_DeviceData_FPInfo)
							{
								pRow_Device_DeviceData_FPInfo->IK_DeviceData_set("");
								pRow_Device_DeviceData_FPInfo->Table_Device_DeviceData_get()->Commit();
								LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Floorplan info for device %d out of bounds",
									pRow_Device_DeviceData_FPInfo->FK_Device_get());
							}
							continue;
						}
                        // We got ourselves an object to appear on this map
                        X = (X * ScaleFactor) / 1000;
                        Y = (Y * ScaleFactor) / 1000;
                        PlutoRectangle rectangle(m_rPosition.X+X,m_rPosition.Y+Y,0,0);
						if( !pRow_FloorplanObjectType->FK_DesignObj_Control_isNull() )
						{
							DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,pRow_FloorplanObjectType->FK_DesignObj_Control_getrow(),
								rectangle,this,false,false,false);  // See if it will fill in the wdith/height automatically
							if( pDesignObj_Generator->m_pRow_DesignObjVariation )
							{
								pDesignObj_Generator->Process(); // Process this now
								pDesignObj_Generator->m_bCanBeHidden = false;
								pDesignObj_Generator->m_bHideByDefault = false;
								pDesignObj_Generator->m_bChildrenBeforeText = false;
								pDesignObj_Generator->m_bChildrenBehind = false;
								pDesignObj_Generator->m_bDontMergeBackground = false;
								pDesignObj_Generator->m_bTabStop = true;
								pDesignObj_Generator->m_iFloorplanPage = m_iFloorplanPage;
								pDesignObj_Generator->m_iFloorplanDevice = PK_Device_EA;

								// Scale these here because we need to reset all the x/y positions since we may have used a different scale factor
								// they will be offset by the parent floorplan object
								// and they are to be centered on the point, rather than upper/left like the others.  That will be 
								// handled when ScaleAllValues is called the second time.
								if( pDesignObj_Generator->m_bDontScale )
									pDesignObj_Generator->ScaleAllValues(m_pOrbiterGenerator->m_sScale_Orig.Width,m_pOrbiterGenerator->m_sScale_Orig.Height,NULL);
								else
									pDesignObj_Generator->ScaleAllValues(m_pOrbiterGenerator->m_sScale.Width,m_pOrbiterGenerator->m_sScale.Height,NULL);
								m_alChildDesignObjs.push_back(pDesignObj_Generator);
								pDesignObj_Generator->m_rBackgroundPosition.X=X;
								pDesignObj_Generator->m_rBackgroundPosition.Y=Y;
								// Keep original X,Y values, we need them to calculate the childrens offset below
								int orgX = pDesignObj_Generator->m_rPosition.X;
								int orgY = pDesignObj_Generator->m_rPosition.Y;
								pDesignObj_Generator->m_rPosition.X=X;
								pDesignObj_Generator->m_rPosition.Y=Y;
								for(size_t s=0;s<pDesignObj_Generator->m_vectDesignObjText.size();++s)
								{
									CGText *ot = (CGText *) pDesignObj_Generator->m_vectDesignObjText[s];
									// Set to parents position(x,y) + the childs original offset (calculated from its current value - the parents current value)
									ot->m_rPosition.X=X+(ot->m_rPosition.X-orgX);
									ot->m_rPosition.Y=Y+(ot->m_rPosition.Y-orgY);
								}

								pDesignObj_Generator->m_pFloorplanFillPoint = new PlutoPoint(pRow_FloorplanObjectType->FillX_get() * ScaleFactor / 1000,pRow_FloorplanObjectType->FillY_get() * ScaleFactor / 1000);
							}
							else
								delete pDesignObj_Generator;
						}
                    }
                }
            }
        }
    }

    vector<Row_DesignObjVariation_DesignObj_Skin_Language *> alArrays;

    // Add all child objects, except for arrays, which we just store in alArrays
    for(size_t s=0;s<m_alDesignObjVariations.size();++s)
    {
        Row_DesignObjVariation * drOV=m_alDesignObjVariations[s];

		// Get the matching child rows
		string sSQL =
			"SELECT DesignObjVariation_DesignObj_Skin_Language.* FROM DesignObjVariation_DesignObj_Skin_Language "
			"JOIN DesignObjVariation_DesignObj ON PK_DesignObjVariation_DesignObj=FK_DesignObjVariation_DesignObj "
			"WHERE (FK_Skin IS NULL OR FK_Skin=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get()) + 
			") AND (FK_Language IS NULL OR FK_Language=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Language->PK_Language_get()) + ") AND FK_DesignObjVariation_Parent="
			+ StringUtils::itos(drOV->PK_DesignObjVariation_get()) + " " 
			"ORDER BY PK_DesignObjVariation_DesignObj,FK_Language DESC,FK_Skin DESC";

		// This will have the matching rows, but not in the display order.  Get the PK values and do another
		// select to get them in the right order
		string sPK;
		Row_DesignObjVariation_DesignObj_Skin_Language *pRow_DesignObjVariation_DesignObj_Skin_Language_Last=NULL;
		vector<Row_DesignObjVariation_DesignObj_Skin_Language *> vectRow_DesignObjVariation_DesignObj_Skin_Language;
		m_mds->DesignObjVariation_DesignObj_Skin_Language_get()->GetRows(sSQL,&vectRow_DesignObjVariation_DesignObj_Skin_Language);
		for(vector<Row_DesignObjVariation_DesignObj_Skin_Language *>::iterator it=vectRow_DesignObjVariation_DesignObj_Skin_Language.begin();
			it!=vectRow_DesignObjVariation_DesignObj_Skin_Language.end();++it)
		{
			Row_DesignObjVariation_DesignObj_Skin_Language *pRow_DesignObjVariation_DesignObj_Skin_Language = *it;

			if( pRow_DesignObjVariation_DesignObj_Skin_Language_Last && 
				pRow_DesignObjVariation_DesignObj_Skin_Language_Last->FK_DesignObjVariation_DesignObj_get()==
				pRow_DesignObjVariation_DesignObj_Skin_Language->FK_DesignObjVariation_DesignObj_get() )
					continue;  // Only pick the variation that's the closest match

			// We have a new child object to include
			pRow_DesignObjVariation_DesignObj_Skin_Language_Last = pRow_DesignObjVariation_DesignObj_Skin_Language;
			if( pRow_DesignObjVariation_DesignObj_Skin_Language_Last->Ignore_get()==1 )
				continue;  // This variation may ignore it

			if( sPK.empty()==false )
				sPK += ",";
			sPK += StringUtils::itos(pRow_DesignObjVariation_DesignObj_Skin_Language->PK_DesignObjVariation_DesignObj_Skin_Language_get());
		}
        
		vector<Row_DesignObjVariation_DesignObj_Skin_Language *> vectovo;
		if( sPK.empty()==false )
			m_mds->DesignObjVariation_DesignObj_Skin_Language_get()->GetRows(
				"WHERE PK_DesignObjVariation_DesignObj_Skin_Language IN (" + sPK + ") ORDER BY DisplayOrder",
				&vectovo);
        for(size_t s2=0;s2<vectovo.size();++s2)
        {
            Row_DesignObjVariation_DesignObj_Skin_Language * drOVO = vectovo[s2];
			Row_DesignObj *pRow_DesignObj_Child = drOVO->FK_DesignObjVariation_DesignObj_getrow()->FK_DesignObj_Child_getrow();

			if( (m_rPosition.X+drOVO->X_get())*m_iScale/100<m_pOrbiterGenerator->m_sizeScreen->Width && (m_rPosition.Y+drOVO->Y_get())*m_iScale/100<m_pOrbiterGenerator->m_sizeScreen->Height )
            {
                if( pRow_DesignObj_Child->FK_DesignObjType_get()==DESIGNOBJTYPE_Array_CONST )
                    alArrays.push_back(drOVO);
                else
                {
                    if( pRow_DesignObj_Child->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
                    {
                        // Add 1 child for each floorplan page
                        vector<Row_Floorplan *> vectRow_Floorplan;
                        m_mds->Floorplan_get()->GetRows(string(FLOORPLAN_FK_INSTALLATION_FIELD) + " Is Null OR " + DEVICE_FK_INSTALLATION_FIELD + "=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()),&vectRow_Floorplan);
						if( vectRow_Floorplan.size()==0 )
						{
							// When we have a floorplan object and no floorplans, look instead for an object to substitute
							Row_DesignObjVariation * pRow_DesignObjVariation=NULL,*pRow_DesignObjVariation_Standard=NULL;
							vector<class Row_DesignObjVariation *> alDesignObjVariations;
						    PickVariation(m_pOrbiterGenerator,pRow_DesignObj_Child,&pRow_DesignObjVariation,&pRow_DesignObjVariation_Standard,&alDesignObjVariations);
							if( pRow_DesignObjVariation )
							{
								Row_DesignObjVariation_DesignObjParameter *pRow_DesignObjVariation_DesignObjParameter = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(pRow_DesignObjVariation->PK_DesignObjVariation_get(),DESIGNOBJPARAMETER_PK_DesignObj_Cell_CONST);
								if( pRow_DesignObjVariation_DesignObjParameter )
								{
									Row_DesignObj *pRow_DesignObj_Substitute = m_mds->DesignObj_get()->GetRow( atoi(pRow_DesignObjVariation_DesignObjParameter->Value_get().c_str()) );
									if( pRow_DesignObj_Substitute )
									{
										PlutoRectangle rectangle(m_rPosition.X+drOVO->X_get(),m_rPosition.Y+drOVO->Y_get(),drOVO->Width_get(),drOVO->Height_get());
										DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,pRow_DesignObj_Substitute,rectangle,this,false,false,false);
										if( pDesignObj_Generator->m_pRow_DesignObjVariation )
										{
											pDesignObj_Generator->m_bTabStop = true;
											pDesignObj_Generator->m_bChildrenBeforeText = drOVO->DisplayChildrenBeforeText_get()==1;
											pDesignObj_Generator->m_bChildrenBehind = drOVO->DisplayChildrenBehindBackground_get()==1;
											pDesignObj_Generator->m_bDontMergeBackground = drOVO->DontMergeBackground_get()==1;
											m_alChildDesignObjs.push_back(pDesignObj_Generator);
										}
										else
											delete pDesignObj_Generator;
									}
								}
							}
						}
						else
						{
							for(size_t s=0;s<vectRow_Floorplan.size();++s)
							{
								Row_Floorplan *pRow_Floorplan = vectRow_Floorplan[s];
								m_pOrbiterGenerator->m_iFloorplanPage = pRow_Floorplan->Page_get();
								PlutoRectangle rectangle(m_rPosition.X+drOVO->X_get(),m_rPosition.Y+drOVO->Y_get(),drOVO->Width_get(),drOVO->Height_get());
								DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,pRow_DesignObj_Child,rectangle,this,false,false,false);
								if( pDesignObj_Generator->m_pRow_DesignObjVariation )
								{
									pDesignObj_Generator->m_bCanBeHidden = true;
									pDesignObj_Generator->m_bHideByDefault = true;
									pDesignObj_Generator->m_bChildrenBeforeText = drOVO->DisplayChildrenBeforeText_get()==1;
									pDesignObj_Generator->m_bChildrenBehind = drOVO->DisplayChildrenBehindBackground_get()==1;
									pDesignObj_Generator->m_bDontMergeBackground = drOVO->DontMergeBackground_get()==1;
									m_alChildDesignObjs.push_back(pDesignObj_Generator);
								}
								else
									delete pDesignObj_Generator;
							}
						}
                    }
                    else
                    {
						// Don't auto process because we want to process children with m_sAdjustments after we do those without since they may depend on others
                        DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,pRow_DesignObj_Child,PlutoRectangle(m_rPosition.X+(drOVO->X_get()*m_iScale/100),m_rPosition.Y+(drOVO->Y_get()*m_iScale/100),drOVO->Width_get(),drOVO->Height_get()),this,false,false,false);
                        if( !pDesignObj_Generator->m_pRow_DesignObjVariation )
                        {
                            LoggerWrapper::GetInstance()->Write(LV_STATUS,"Not adding object: %d to %d  because there are no qualifying variations.",pRow_DesignObj_Child->PK_DesignObj_get(),drOVO->FK_DesignObjVariation_DesignObj_getrow()->FK_DesignObjVariation_Parent_getrow()->FK_DesignObj_get());
                            delete pDesignObj_Generator; // Abort adding this object as a child, there were no variations
                        }
                        else
                        {
                            pDesignObj_Generator->m_bCanBeHidden = drOVO->CanBeHidden_get()==1;
                            pDesignObj_Generator->m_bHideByDefault = drOVO->HideByDefault_get()==1;
                            pDesignObj_Generator->m_bChildrenBeforeText = drOVO->DisplayChildrenBeforeText_get()==1;
                            pDesignObj_Generator->m_bChildrenBehind = drOVO->DisplayChildrenBehindBackground_get()==1;
                            pDesignObj_Generator->m_bDontMergeBackground = drOVO->DontMergeBackground_get()==1;
                            pDesignObj_Generator->m_bTabStop = drOVO->IsTabStop_get()==1;

                            pDesignObj_Generator->m_PK_DesignObj_Up=drOVO->FK_DesignObj_Up_get();
                            pDesignObj_Generator->m_PK_DesignObj_Down=drOVO->FK_DesignObj_Down_get();
                            pDesignObj_Generator->m_PK_DesignObj_Left=drOVO->FK_DesignObj_Left_get();
                            pDesignObj_Generator->m_PK_DesignObj_Right=drOVO->FK_DesignObj_Right_get();

                            pDesignObj_Generator->m_sPK_DesignObj_TiedTo=drOVO->sFK_DesignObj_TiedTo_get();
                            pDesignObj_Generator->m_sVisibleState=StringUtils::ToUpper(drOVO->VisibleStates_get());
                            m_alChildDesignObjs.push_back(pDesignObj_Generator);
                        }
                    }
                }
            }
        }
    }

	// If this is a datagrid, add datagrid children
    if( m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Datagrid_CONST )
	{
		AddDataGridObjects();
	}

    // Now fill in all the arrays
    for(size_t s=0;s<alArrays.size();++s)
    {
        Row_DesignObjVariation_DesignObj_Skin_Language * drOVO = alArrays[s];

		int PK_Array;
		vector<class ArrayValue *> *alArrayValues = GetArrayValues(drOVO,PK_Array);  // PK_Array will be filled in
        PlutoRectangle rtArray(m_rPosition.X + drOVO->X_get(),m_rPosition.Y + drOVO->Y_get(),drOVO->Width_isNull() ? 0 : drOVO->Width_get(),drOVO->Height_isNull() ?  0 : drOVO->Height_get());
        if( rtArray.Right()>m_pOrbiterGenerator->m_sizeScreen->Width || rtArray.Width==0 )
            rtArray.Width = m_pOrbiterGenerator->m_sizeScreen->Width - rtArray.Left();
        if( rtArray.Bottom()>m_pOrbiterGenerator->m_sizeScreen->Height || rtArray.Height==0 )
            rtArray.Height = m_pOrbiterGenerator->m_sizeScreen->Height - rtArray.Top();

        CGArray *pCGArray = new CGArray(this,drOVO,alArrayValues,rtArray,0,0,PK_Array);
	    if( m_pRow_DesignObj->FK_DesignObjType_get()!=DESIGNOBJTYPE_Floorplan_CONST  )
			m_rPosition = PlutoRectangle::Union(m_rPosition,pCGArray->m_rBounds);
		
		if( pCGArray->m_bContainsMore)
        {
            if( m_alMPArray.size()>0 )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"There is more than one multi-page array for object variation: %d",drOVO->FK_DesignObjVariation_DesignObj_get());

            m_alMPArray.push_back(pCGArray);
            int Page=1;

            while( pCGArray->m_bContainsMore )
            {
                pCGArray = new CGArray(this,drOVO,alArrayValues,rtArray,pCGArray->m_iLastVisibleArrayEntry,Page++,PK_Array);
                m_alMPArray.push_back(pCGArray);
            }
        }
        else
            m_alNonMPArrays.push_back(pCGArray);
    }

	// Make 2 passes and process first the objects with no adjustments
	for(int Pass=0;Pass<2;++Pass)
	{
		for(vector<class DesignObj_Generator *>::iterator it=m_alChildDesignObjs.begin();it!=m_alChildDesignObjs.end();++it)
		{
			DesignObj_Generator *pObj = *it;
			if( (pObj->m_sAdjustments.empty()==true && Pass==0) || (pObj->m_sAdjustments.empty()==false && Pass==1) )
				pObj->Process();
		}
	}

	// Don't stretch out the floorplan objects
	if( m_bUsingCache )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"not stretching out cached screen %s", m_ObjectID.c_str());
	}
	else if( m_pRow_DesignObj->FK_DesignObjType_get()!=DESIGNOBJTYPE_Floorplan_CONST )
    {
        // First go through all of the arrays and set the appropriate bounds so we can calculate the size of this object
        for(size_t s=0;s<m_alChildDesignObjs.size();++s)
        {
            DesignObj_Generator *oco = m_alChildDesignObjs[s];
			if( oco->m_iScale!=100 )
				continue; // If this is a specially scaled object don't factor it in
            if( oco->m_rPosition.Width>0 )
            {
                if( m_rPosition.Width>0 )
                    m_rPosition = PlutoRectangle::Union(m_rPosition,oco->m_rPosition);
                else
				{
					m_rPosition.Right(oco->m_rPosition.Right());
					m_rPosition.Bottom(oco->m_rPosition.Bottom());
				}
            }
        }
    }

	int PK_DesignObj_Goto = 0;
    int tmpDesignObj_Goto;

    DesignObjCommandList::iterator itActions;
    for(itActions=m_Action_StartupList.begin();itActions!=m_Action_StartupList.end();++itActions)
    {
        CGCommand *oca = (CGCommand *) *itActions;
        if( oca->m_PK_Command == COMMAND_Set_Floorplan_CONST )
        {
            map<int, string>::iterator itParm=oca->m_ParameterList.find(COMMANDPARAMETER_PK_DesignObj_CONST);
			if( itParm!=oca->m_ParameterList.end() )
			{
				int PK_DesignObj_Goto = atoi(itParm->second.c_str());
				if( oca->m_ParameterList.find(COMMANDPARAMETER_TrueFalse_CONST)!=oca->m_ParameterList.end() &&
						oca->m_ParameterList[COMMANDPARAMETER_TrueFalse_CONST]=="1" )
					m_pOrbiterGenerator->m_mapPopups_set(PK_DesignObj_Goto,true);
		
				bool bDontShare=m_bDontShare;
				m_bDontShare=false;  // Floorplans are shared across all rooms
				HandleGoto(PK_DesignObj_Goto);
				m_bDontShare=bDontShare;
            }
        }

    }

	DesignObjZoneList::iterator itZone;
    for(itZone=m_ZoneList.begin();itZone!=m_ZoneList.end();++itZone)
    {
        CGZone *oz = (CGZone *) *itZone;
        tmpDesignObj_Goto = LookForGoto(&oz->m_Commands);
        if( tmpDesignObj_Goto==-1 )
        {
            PK_DesignObj_Goto=-1;
            break;
        }
        if( tmpDesignObj_Goto!=0 )
        {
            if( PK_DesignObj_Goto!=0 )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DesignObj %d %s has more than 1 goto",
					m_pRow_DesignObj->PK_DesignObj_get(),m_pRow_DesignObj->Description_get().c_str());
            else
                PK_DesignObj_Goto = tmpDesignObj_Goto;
        }
    }

    if( PK_DesignObj_Goto!=-1 )
    {
        tmpDesignObj_Goto = LookForGoto(&m_Action_LoadList);
        if( tmpDesignObj_Goto==0 )
	        tmpDesignObj_Goto = LookForGoto(&m_Action_HighlightList);
	    if( tmpDesignObj_Goto==-1 )
		    PK_DesignObj_Goto=-1;
        else if( tmpDesignObj_Goto!=0 )
        {
            if( PK_DesignObj_Goto!=0 )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DesignObj %d %s has more than 1 goto",m_pRow_DesignObj->PK_DesignObj_get(),m_pRow_DesignObj->Description_get().c_str());
            else
                PK_DesignObj_Goto = tmpDesignObj_Goto;
        }
    }

    if( PK_DesignObj_Goto>0 )
    {
        m_sSelectedFile = "";  // Don't bother with the selected graphic
        HandleGoto(PK_DesignObj_Goto);
    }

    /* why did I have more than 1 goto???
    // Build a list of gotos
    Hashtable htGotos = new Hashtable();
    if( PK_DesignObj_Goto!=0 )
        htGotos.Add(PK_DesignObj_Goto,1);
    if( m_pRow_Device_Goto!=NULL )
    {
        m_pOrbiterGenerator->m_htUsedDevices.Add(m_pRow_Device_Goto,1);
        foreach(DataRow drobj in m_pRow_Device_Goto->FK_DeviceTemplate_getrow().dr.GetChildRows(Database_pluto_main.MyRelations.DeviceTemplate_DesignObj_FK_DeviceTemplate))
        {
            Row_DeviceTemplate_DesignObj * drMDL = new Row_DeviceTemplate_DesignObj *(drobj);
            htGotos.Add(drMDL->FK_DesignObj_get(),1);
        }
        foreach(DataRow drobj in m_pRow_Device_Goto->FK_DeviceTemplate_getrow()->FK_Device_get()Category_DataRow.dr.GetChildRows(Database_pluto_main.MyRelations.DeviceCategory_DesignObj_FK_DeviceCategory))
        {
            Row_DeviceCategory_DesignObj * drDCO = new Row_DeviceCategory_DesignObj *(drobj);
            htGotos.Add(drDCO->FK_DesignObj_get(),1);
        }
    }

    IDictionaryEnumerator enGotos = htGotos.GetEnumerator();
    while( enGotos.MoveNext() )
    {
        PK_DesignObj_Goto = Convert.ToInt32(enGotos.Key);
        HandleGoto(PK_DesignObj_Goto);
        GC.Collect(); // The background file probably took a lot of memory
        Thread.Sleep(250);  // see if this does something to let the gc work, becuase it doesn't seem to be working!!!!!
    }
    */

    m_bPreserveTransparencies = 0 != m_pRow_DesignObjVariation->PreserveTransparencies_get();
}

DesignObj_Generator::~DesignObj_Generator()
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"dest %p",this);
}

int DesignObj_Generator::LookForGoto(DesignObjCommandList *alCommands)
{
    int PK_DesignObj_Goto=0;

    DesignObjCommandList::iterator itActions;
    for(itActions=alCommands->begin();itActions!=alCommands->end();++itActions)
    {
        CGCommand *oca = (CGCommand *) *itActions;
        if( oca->m_PK_Command == COMMAND_Goto_DesignObj_CONST || oca->m_PK_Command == COMMAND_Goto_Screen_CONST || oca->m_PK_Command == COMMAND_Show_Popup_CONST )
        {
            map<int, string>::iterator itParm;
			if( oca->m_PK_Command == COMMAND_Goto_Screen_CONST )
				itParm=oca->m_ParameterList.find(COMMANDPARAMETER_PK_Screen_CONST);
			else
				itParm=oca->m_ParameterList.find(COMMANDPARAMETER_PK_DesignObj_CONST);

			if( itParm!=oca->m_ParameterList.end() )
			{
                if(PK_DesignObj_Goto!=0)
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DesignObj(2) %d %s has more than 1 goto",m_pRow_DesignObj->PK_DesignObj_get(),m_pRow_DesignObj->Description_get().c_str());

                bool b;
                size_t pos;

                string objgoto;
				if( oca->m_PK_Command == COMMAND_Goto_Screen_CONST )
				{
					Row_DesignObj *pRow_DesignObj = m_pOrbiterGenerator->GetDesignObjFromScreen(atoi(SubstituteVariables((*itParm).second,&b).c_str()));
					if( pRow_DesignObj )
						objgoto = StringUtils::itos(pRow_DesignObj->PK_DesignObj_get());
				}
				else
					objgoto = SubstituteVariables((*itParm).second,&b);

				if( (pos = objgoto.find('.'))!=string::npos )
                    objgoto = objgoto.substr(0,pos);

                // Be sure there's no substitution
                if( objgoto.find("<%=")==string::npos )
                {
                    try
                    {
                        PK_DesignObj_Goto = atoi(objgoto.c_str());
						if( oca->m_PK_Command == COMMAND_Show_Popup_CONST )
							m_pOrbiterGenerator->m_mapPopups_set(PK_DesignObj_Goto,true);
                    }
                    catch(...)
                    {
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot go to screen: %s from %d",(*itParm).second.c_str(),m_pRow_DesignObj->PK_DesignObj_get());
                    }
                }
            }
        }

    }

    return PK_DesignObj_Goto;
}

void DesignObj_Generator::HandleGoto(int PK_DesignObj_Goto)
{
	m_PK_DesignObj_Goto=PK_DesignObj_Goto;
if( PK_DesignObj_Goto==1324 )
int k=2;
	string sVersion = ".0";
bool b=false;
	if( m_pOrbiterGenerator->m_mapDesignObj_WithArrays.find(PK_DesignObj_Goto)!=m_pOrbiterGenerator->m_mapDesignObj_WithArrays.end() )
{
b=true;
		sVersion = "." + StringUtils::itos(m_pOrbiterGenerator->m_iLocation);
}

	listDesignObj_Generator *pListScreens = m_pOrbiterGenerator->m_htGeneratedScreens[StringUtils::itos(PK_DesignObj_Goto) + sVersion];
if( StringUtils::itos(PK_DesignObj_Goto) + sVersion=="1270.0" )
int k=2;
    if( PK_DesignObj_Goto==m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu->PK_DesignObj_get() )
    {
        return; // All the main menu's are automatically included.  The system will figure out which one at runtime
    }

    if( !pListScreens )
    {
        pListScreens = new listDesignObj_Generator();
        m_pOrbiterGenerator->m_htGeneratedScreens[StringUtils::itos(PK_DesignObj_Goto) + sVersion] = pListScreens;
    }
    else if( pListScreens->size()==0 )
    {
        // This means the screen is currently being created, and there's a goto that loops back to itself
        // because the object is added to the list only after it's created.  Do nothing in this case.
        return;
    }

    // Does this exist already?
    if( pListScreens->size()==1 ) // && !(*pListScreens)[0]->m_bDontShare ) since the key is now screen X.Y, not just X, we don't worry about sharing anymore??
{
        m_DesignObj_GeneratorGoto = (*pListScreens)[0];
if( m_PK_DesignObj_Goto==1270 )
{
int k=2;
}
}
    else
    {
        // We need to generate the screen
        map<int,string> htNewVariables;
        map<int,string>::iterator it;
        for(it=m_pOrbiterGenerator->m_mapVariable.begin(); it!=m_pOrbiterGenerator->m_mapVariable.end(); ++it)
            htNewVariables[(*it).first]=(*it).second;

        // See if we have any variable assigns in our actions
        DesignObjZoneList::iterator itZone;
        for(itZone=m_ZoneList.begin();itZone!=m_ZoneList.end();++itZone)
        {
            CGZone *oz = (CGZone *) *itZone;

            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                if( oa->m_PK_Command == COMMAND_Set_Variable_CONST )
                {
                    int PK_Variable=0;
                    string oValue="";
                    map<int, string>::iterator itParm;
                    for(itParm=oa->m_ParameterList.begin();itParm!=oa->m_ParameterList.end();++itParm)
                    {
                        if( (*itParm).first == COMMANDPARAMETER_PK_Variable_CONST )
                            PK_Variable = atoi((*itParm).second.c_str());
                        else if( (*itParm).first == COMMANDPARAMETER_Value_To_Assign_CONST )
                            oValue = (*itParm).second;
                    }

                    if( PK_Variable>0 )
                        htNewVariables[PK_Variable]=oValue;
                }
            }
        }

        Row_DesignObj * drDesignObj_new = m_mds->DesignObj_get()->GetRow(PK_DesignObj_Goto);
        if( !drDesignObj_new )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Illegal attempt to goto screen: %d from %d screen %d",PK_DesignObj_Goto,m_pRow_DesignObj->PK_DesignObj_get(),m_pOrbiterGenerator->m_iPK_DesignObj_Screen);
			return;
		}

        map<int,string> *htPriorVariables = &m_pOrbiterGenerator->m_mapVariable;
        m_pOrbiterGenerator->m_mapVariable = htNewVariables;

if( this->m_pRow_DesignObj->PK_DesignObj_get()==3407 )
int k=2;
        m_pOrbiterGenerator->m_iPK_DesignObj_Screen = PK_DesignObj_Goto;
        m_DesignObj_GeneratorGoto = new DesignObj_Generator(m_pOrbiterGenerator,drDesignObj_new,PlutoRectangle(0,0,0,0),NULL,pListScreens->size()==0,false);
//  I think this is not needed since the if( bAddToGenerated at the top of the constructor does this since pListScreens->size()==0???       pListScreens->push_back(m_DesignObj_GeneratorGoto);
//      m_pOrbiterGenerator->m_mapVariable = htPriorVariables;
//        if( m_bDontShare )
  //          m_DesignObj_GeneratorGoto->m_iVersion=(int) pListScreens->size();
    }
}

void DesignObj_Generator::PickVariation(OrbiterGenerator *pGenerator,class Row_DesignObj *drDesignObj,class Row_DesignObjVariation **drDesignObjVariation,class Row_DesignObjVariation **drStandardVariation, vector<class Row_DesignObjVariation *> *alDesignObjVariations)
{
    vector<class Row_DesignObjVariation *> vectrov;
    drDesignObj->DesignObjVariation_FK_DesignObj_getrows(&vectrov);
    if( vectrov.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No variation for object: %d",drDesignObj->PK_DesignObj_get());
		return;
	}

	Row_DesignObjVariation *drVariation_Alt=NULL;

    for(size_t s=0;s<vectrov.size();++s)
    {
        Row_DesignObjVariation *drOV = vectrov[s];
        // see if a specific criteria was specified for this skin
        Row_Device *pDevice = pGenerator->m_spDatabase_pluto_main->Device_get()->GetRow(pGenerator->m_pRow_Orbiter->PK_Orbiter_get());

		if( drOV->FK_UI_isNull() )
        {
            *drStandardVariation = drOV;
			if( pGenerator->m_pRow_UI->IncludeStandardUI_get()==2 )
				alDesignObjVariations->push_back(drOV);   // Always include this
        }
        else if( drOV->FK_UI_get() == pGenerator->m_pRow_UI->PK_UI_get() )
        {
            *drDesignObjVariation = drOV;
            alDesignObjVariations->push_back(drOV);
        }
		else if( drOV->FK_UI_get() == pGenerator->m_pRow_UI->FK_UI_Alt_get() )
			drVariation_Alt = drOV;
    }

    if( *drDesignObjVariation==NULL && drVariation_Alt!=NULL )
	{
		*drDesignObjVariation = drVariation_Alt;
		alDesignObjVariations->push_back(drVariation_Alt);
	}

    // It's possible there's just no standard variation.  In that case the chosen variation will be standard.
    if( *drStandardVariation==NULL && *drDesignObjVariation!=NULL )
    {
        *drStandardVariation = *drDesignObjVariation;
    }
    else if( *drDesignObjVariation==NULL )
    {
        if( *drStandardVariation==NULL || pGenerator->m_pRow_UI->IncludeStandardUI_get()==0 )  // Confirm they're not both null
        {
			*drStandardVariation=NULL;
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WARNING: Cannot find any variation for object: %d",
				drDesignObj->PK_DesignObj_get());
        }
        else
		{
            *drDesignObjVariation = *drStandardVariation;
			if( pGenerator->m_pRow_UI->IncludeStandardUI_get()==1 )
				alDesignObjVariations->push_back(*drDesignObjVariation);   // Include this since we didn't find another
		}
    }
}

void DesignObj_Generator::PickStyleVariation(class Row_Style * drStyle,OrbiterGenerator *pGenerator,MapTextStyle &mapTextStyle)
{
    vector<Row_StyleVariation *> vectrsv;
    drStyle->StyleVariation_FK_Style_getrows(&vectrsv);
    if( vectrsv.size()==0 )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WARNING! No variation for style: %d reverting",
			drStyle->PK_Style_get());
        drStyle = drStyle->Table_Style_get()->GetRow(1);
        drStyle->StyleVariation_FK_Style_getrows(&vectrsv);
        if( vectrsv.size()==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No variation for style: %d",drStyle->PK_Style_get());
			return;
		}
    }

    // Find the best style for each variation
    TextStyle *pTextStyle = PickStyleVariation(vectrsv,pGenerator,0);
    if( pTextStyle )
    {
		#pragma message("NOTE: if style specifies alternates for selected, highlighted, alt pick them and add them to the map && get rid of version below! (DesignObj_Generator::PickStyleVariation)")
		pTextStyle->m_iVersion=0;
        mapTextStyle[0]=pTextStyle;
    }
}

TextStyle *DesignObj_Generator::PickStyleVariation(vector<Row_StyleVariation *> &vectrsv,OrbiterGenerator *pGenerator,int Version)
{
    class Row_StyleVariation *drCorrectMatch=NULL;

    Row_StyleVariation * drSV_NoSkin = NULL, *drSV_NoCriteria = NULL, *drSV_Neither = NULL;
    for(size_t s=0;s<vectrsv.size();++s)
    {
        Row_StyleVariation * drStyleVariation = vectrsv[s];
        if( !drStyleVariation->FK_Skin_isNull() && drStyleVariation->FK_Skin_get()!=pGenerator->m_pRow_Skin->PK_Skin_get() )
            continue;

        if( !drStyleVariation->FK_UI_isNull() && drStyleVariation->FK_UI_get()!=pGenerator->m_pRow_UI->PK_UI_get() )
			continue;

        // We don't have a confirmed non-match
        if( drStyleVariation->FK_UI_isNull() && drStyleVariation->FK_Skin_isNull() )
            drSV_Neither = drStyleVariation;
        else if( drStyleVariation->FK_UI_isNull() )
            drSV_NoCriteria = drStyleVariation;
        else if( drStyleVariation->FK_Skin_isNull() )
            drSV_NoSkin = drStyleVariation;
        else
        {
            drCorrectMatch= drStyleVariation;
            break;
        }
    }

    if( !drCorrectMatch )
    {
        if( drSV_NoCriteria!=NULL )
            drCorrectMatch= drSV_NoCriteria;
        else if( drSV_NoSkin!=NULL )
            drCorrectMatch= drSV_NoSkin;
        else
            drCorrectMatch= drSV_Neither;
    }

    if( !drCorrectMatch )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"***ERROR*** No matching style variation with %d choices",(int) vectrsv.size());
		return NULL;
	}

    TextStyle *pTextStyle = pGenerator->m_mapTextStyle_Find(drCorrectMatch->FK_Style_get());
    if( !pTextStyle )
    {
        pTextStyle = new TextStyle(drCorrectMatch);
        pTextStyle->m_iPixelHeight = drCorrectMatch->PixelHeight_get() * pGenerator->m_sScale.Height / 1000;
        pGenerator->m_mapTextStyle[drCorrectMatch->FK_Style_get()]=pTextStyle;
    }
    return pTextStyle;
}

vector<class ArrayValue *> *DesignObj_Generator::GetArrayValues(Row_DesignObjVariation_DesignObj_Skin_Language *pRow_DesignObjVariation_DesignObj_Skin_Language,int &PK_Array)
{
    vector<class ArrayValue *> *alArray = new vector<class ArrayValue *>;

    Row_DesignObjVariation *drDesignObjVariation=NULL,*drStandardVariation=NULL;
    vector<class Row_DesignObjVariation *> alDesignObjVariations;
    PickVariation(m_pOrbiterGenerator,pRow_DesignObjVariation_DesignObj_Skin_Language->FK_DesignObjVariation_DesignObj_getrow()->FK_DesignObj_Child_getrow(),&drDesignObjVariation,&drStandardVariation,&alDesignObjVariations);

    string oArray = GetParm(DESIGNOBJPARAMETER_PK_Array_CONST,drDesignObjVariation);
    if( oArray.length()==0 )
        return alArray;

	string sExtraInfo = GetParm(DESIGNOBJPARAMETER_Extra_Info_CONST,drDesignObjVariation);
	bool bNoSubstitutions = sExtraInfo.find('X')!=string::npos;
	bool bNoIcons = sExtraInfo.find('I')!=string::npos;

    PK_Array = atoi(oArray.c_str());
    int PriorSort=-1;
    switch(PK_Array)
    {
        case ARRAY_Lighting_Scenarios_CONST:
        case ARRAY_Climate_Scenarios_CONST:
        case ARRAY_Security_Scenarios_CONST:
        case ARRAY_Communication_Scenarios_CONST:
        case ARRAY_Misc_Scenarios_CONST:
        case ARRAY_Sleeping_Scenarios_CONST:
            if( m_pOrbiterGenerator->m_pRow_Room )
            {
                int PriorSort=-1;
                vector<class Row_CommandGroup_Room *> vectAGs;
                string sql = string(COMMANDGROUP_ROOM_FK_ROOM_FIELD) + "=" +
                    StringUtils::itos(m_pOrbiterGenerator->m_pRow_Room->PK_Room_get()) + " ORDER BY " + COMMANDGROUP_ROOM_SORT_FIELD;
                m_mds->CommandGroup_Room_get()->GetRows(sql,&vectAGs);

                for(size_t s=0;s<vectAGs.size();++s)
                {
                    Row_CommandGroup_Room *drAG_R = vectAGs[s];
					Row_CommandGroup *pRow_CommandGroup=drAG_R->FK_CommandGroup_getrow();
                    if( pRow_CommandGroup && pRow_CommandGroup->FK_Array_get()==PK_Array && !pRow_CommandGroup->Disabled_get() )
                    {
                        alArray->push_back(new ArrayValue(drAG_R->FK_CommandGroup_getrow()->AltID_isNull() ? StringUtils::itos(drAG_R->FK_CommandGroup_get()) : StringUtils::itos(drAG_R->FK_CommandGroup_getrow()->AltID_get()),
                            (!drAG_R->FK_CommandGroup_getrow()->FK_Icon_isNull() && m_pOrbiterGenerator->m_iUiVersion==1 ? "~cb~" : "") +  
(!drAG_R->FK_CommandGroup_getrow()->FK_Text_isNull() ? GetText(drAG_R->FK_CommandGroup_getrow()->FK_Text_get()) : drAG_R->FK_CommandGroup_getrow()->Description_get()),
                            drAG_R->FK_CommandGroup_getrow()->FK_Icon_isNull() ? NULL : drAG_R->FK_CommandGroup_getrow()->FK_Icon_getrow(),
                            drAG_R->FK_CommandGroup_getrow()->FK_Criteria_Orbiter_isNull() ? 0 : drAG_R->FK_CommandGroup_getrow()->FK_Criteria_Orbiter_get(),
                            bNoSubstitutions || drAG_R->FK_CommandGroup_getrow()->FK_DesignObj_isNull() ? 0 : drAG_R->FK_CommandGroup_getrow()->FK_DesignObj_get(),
                            drAG_R->FK_CommandGroup_get(),VARIABLE_PK_CommandGroup_CONST,drAG_R->FK_CommandGroup_getrow()->CanBeHidden_get()==1,false,PriorSort==drAG_R->Sort_get()));
                        PriorSort=drAG_R->Sort_get();
                    }
                }
				if( m_pOrbiterGenerator->m_pRow_UI->PK_UI_get()==UI_V2_Normal_Horizontal_16_9_CONST )
				{
					// Add the floorplan icon
					int PK_DesignObj=0;
					switch(PK_Array)
					{
						case ARRAY_Lighting_Scenarios_CONST:
							PK_DesignObj=DESIGNOBJ_butLightsOverview_CONST;
							break;
						case ARRAY_Climate_Scenarios_CONST:
							PK_DesignObj=DESIGNOBJ_butClimateOverview_CONST;
							break;
						case ARRAY_Security_Scenarios_CONST:
							PK_DesignObj=DESIGNOBJ_butSecurityOverview_CONST;
							break;
						case ARRAY_Communication_Scenarios_CONST:
							PK_DesignObj=DESIGNOBJ_butTeleOverview_CONST;
							break;
					}
					if( PK_DesignObj )
					{
						alArray->push_back(new ArrayValue("","",NULL,
							0,PK_DesignObj,
							0, 0, false,false,false));
					}
				}
            }
            break;
        case ARRAY_Media_Scenarios_CONST:
            if( m_pOrbiterGenerator->m_pRow_EntertainArea==NULL )
                break;

            {
				if( m_pOrbiterGenerator->m_iUiVersion!=2 )
				{
					int PK_DesignObj_NowPlaying = DESIGNOBJ_butCurrentlyPlaying_CONST;
					string::size_type p=sExtraInfo.find("NP:");
					if( p!=string::npos )
						PK_DesignObj_NowPlaying = atoi(sExtraInfo.substr(p+3).c_str());

					if( PK_DesignObj_NowPlaying )
						alArray->push_back(new ArrayValue("","",NULL,0,PK_DesignObj_NowPlaying,0,0,false,false,false));
				}
				PriorSort=-1;

				vector<class Row_CommandGroup_EntertainArea *> vectEGs;
                string sql = string(COMMANDGROUP_ENTERTAINAREA_FK_ENTERTAINAREA_FIELD) + "=" + \
                    StringUtils::itos(m_pOrbiterGenerator->m_pRow_EntertainArea->PK_EntertainArea_get()) + " ORDER BY " + COMMANDGROUP_ENTERTAINAREA_SORT_FIELD;
                m_mds->CommandGroup_EntertainArea_get()->GetRows(sql,&vectEGs);

                for(size_t s=0;s<vectEGs.size();++s)
                {
                    Row_CommandGroup_EntertainArea *drAG_E = vectEGs[s];
					Row_CommandGroup *pRow_CommandGroup=drAG_E->FK_CommandGroup_getrow();
                    if( pRow_CommandGroup && pRow_CommandGroup->FK_Array_get()==PK_Array && !pRow_CommandGroup->Disabled_get() )
                    {
                        alArray->push_back(new ArrayValue(drAG_E->FK_CommandGroup_getrow()->AltID_isNull() ? StringUtils::itos(drAG_E->FK_CommandGroup_get()) : StringUtils::itos(drAG_E->FK_CommandGroup_getrow()->AltID_get()),
                            (!drAG_E->FK_CommandGroup_getrow()->FK_Icon_isNull() && m_pOrbiterGenerator->m_iUiVersion==1 ? "~cb~" : "") +  
								// Try to get descriprion from Text table if we have PK_Text
								(!drAG_E->FK_CommandGroup_getrow()->FK_Text_isNull() ? GetText(drAG_E->FK_CommandGroup_getrow()->FK_Text_get()) : drAG_E->FK_CommandGroup_getrow()->Description_get()),
                            drAG_E->FK_CommandGroup_getrow()->FK_Icon_isNull() ? NULL : drAG_E->FK_CommandGroup_getrow()->FK_Icon_getrow(),
                            drAG_E->FK_CommandGroup_getrow()->FK_Criteria_Orbiter_isNull() ? 0 : drAG_E->FK_CommandGroup_getrow()->FK_Criteria_Orbiter_get(),
                            !bNoSubstitutions && drAG_E->FK_CommandGroup_getrow()->FK_DesignObj_isNull() ? 0 : drAG_E->FK_CommandGroup_getrow()->FK_DesignObj_get(),
                            drAG_E->FK_CommandGroup_get(),VARIABLE_PK_CommandGroup_CONST,drAG_E->FK_CommandGroup_getrow()->CanBeHidden_get()==1,false,PriorSort==drAG_E->Sort_get()));
                        PriorSort=drAG_E->Sort_get();
                    }
                }
            }
			if( m_pOrbiterGenerator->m_pRow_UI->PK_UI_get()==UI_V2_Normal_Horizontal_16_9_CONST )
				alArray->push_back(new ArrayValue("","",NULL,
					0,DESIGNOBJ_butMediaOverview_CONST,
					0, 0, false,false,false));
            break;

		case ARRAY_PK_MediaType_CONST:
                alArray->push_back(new ArrayValue(TOSTRING(MEDIATYPE_pluto_StoredVideo_CONST),	GetText(TEXT_MediaType_Video_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));
                alArray->push_back(new ArrayValue(TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST),	GetText(TEXT_MediaType_Audio_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));
                alArray->push_back(new ArrayValue(TOSTRING(MEDIATYPE_pluto_Pictures_CONST),	GetText(TEXT_MediaType_Pictures_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));
				alArray->push_back(new ArrayValue(TOSTRING(MEDIATYPE_np_Game_CONST), GetText(TEXT_MediaType_Games_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));
                alArray->push_back(new ArrayValue(TOSTRING(MEDIATYPE_misc_DocViewer_CONST),	GetText(TEXT_MediaType_Data_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));
            break;

		case ARRAY_Media_Sort_Options_CONST:
			{
				string sSQL = "EK_MediaType=" + StringUtils::itos(m_pOrbiterGenerator->m_dwMediaType) + 
					" and MediaSortOption is not null order by MediaSortOption";

                alArray->push_back(new ArrayValue("-1",	GetText(TEXT_Recently_Used_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));

				alArray->push_back(new ArrayValue("0",	GetText(TEXT_Filename_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));
				
				// TODO: Add another filter to show recently added media files
				//alArray->push_back(new ArrayValue("-2",	GetText(TEXT_Recently_Added_CONST),
                //    NULL, 0, 0, 0, 0,false,false,false));

				vector<Row_MediaType_AttributeType *> vectRow_MediaType_AttributeType;
				m_pOrbiterGenerator->m_spDatabase_pluto_media->MediaType_AttributeType_get()->GetRows(sSQL,&vectRow_MediaType_AttributeType);
				for( vector<Row_MediaType_AttributeType *>::iterator it=vectRow_MediaType_AttributeType.begin();it!=vectRow_MediaType_AttributeType.end();++it )
				{
					Row_MediaType_AttributeType *pRow_MediaType_AttributeType = *it;
					string sValue = StringUtils::itos(pRow_MediaType_AttributeType->FK_AttributeType_get());
					alArray->push_back(new ArrayValue(sValue,
						pRow_MediaType_AttributeType->FK_AttributeType_getrow()->Description_get(),
	                    NULL, 0, 0, 0, 0,false,false,false));
				}
			}
            break;

		case ARRAY_Media_Filter_Genres_CONST:
			{
                alArray->push_back(new ArrayValue("0", GetText(TEXT_All_CONST),
	                NULL, 0, 0, 0, 0,false,false,false));

				string sSQL = "SELECT DISTINCT Attribute.* from Attribute "
					"JOIN File_Attribute ON FK_Attribute=PK_Attribute "
					"JOIN File ON FK_File=PK_File "
					"WHERE EK_MediaType=" + StringUtils::itos(m_pOrbiterGenerator->m_dwMediaType) +
					" AND FK_AttributeType="  TOSTRING(ATTRIBUTETYPE_Genre_CONST) " AND Missing=0";

				vector<Row_Attribute *> vectRow_Attribute;
				m_pOrbiterGenerator->m_spDatabase_pluto_media->Attribute_get()->GetRows(sSQL,&vectRow_Attribute);
				for( vector<Row_Attribute *>::iterator it=vectRow_Attribute.begin();it!=vectRow_Attribute.end();++it )
				{
					Row_Attribute *pRow_Attribute = *it;
					alArray->push_back(new ArrayValue(StringUtils::itos(pRow_Attribute->PK_Attribute_get()),
						pRow_Attribute->Name_get(),
	                    NULL, 0, 0, 0, 0,false,false,false));
				}
			}
            break;

		case ARRAY_Media_Filter_Subtype_CONST:
			{
                alArray->push_back(new ArrayValue("0",	GetText(TEXT_All_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));

				string sSQL = "JOIN MediaType_MediaSubType on FK_MediaSubType = PK_MediaSubType WHERE EK_MediaType=" + StringUtils::itos(m_pOrbiterGenerator->m_dwMediaType);
				vector<Row_MediaSubType *> vectRow_MediaSubType;
				m_pOrbiterGenerator->m_spDatabase_pluto_media->MediaSubType_get()->GetRows(sSQL,&vectRow_MediaSubType);
				for( vector<Row_MediaSubType *>::iterator it=vectRow_MediaSubType.begin();it!=vectRow_MediaSubType.end();++it )
				{
					Row_MediaSubType *pRow_MediaSubType = *it;
					alArray->push_back(new ArrayValue(StringUtils::itos(pRow_MediaSubType->PK_MediaSubType_get()),
						pRow_MediaSubType->Description_get(),
	                    NULL, 0, 0, 0, 0,false,false,false));
				}
			}
            break;

		case ARRAY_Media_Filter_Source_CONST:
			{
				string sSQL = "JOIN MediaType_MediaSource on FK_MediaSource = PK_MediaSource WHERE EK_MediaType=" + StringUtils::itos(m_pOrbiterGenerator->m_dwMediaType);
				vector<Row_MediaSource *> vectRow_MediaSource;
				m_pOrbiterGenerator->m_spDatabase_pluto_media->MediaSource_get()->GetRows(sSQL,&vectRow_MediaSource);
				for( vector<Row_MediaSource *>::iterator it=vectRow_MediaSource.begin();it!=vectRow_MediaSource.end();++it )
				{
					Row_MediaSource *pRow_MediaSource = *it;
					alArray->push_back(new ArrayValue(StringUtils::itos(pRow_MediaSource->PK_MediaSource_get()),
						pRow_MediaSource->Description_get(),
	                    NULL, 0, 0, 0, 0,false,false,false));
				}

				sSQL = "SELECT IK_DeviceData FROM Device LEFT JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate "
					"LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceTemplate=PK_DeviceTemplate "
					"WHERE FK_DeviceData=" TOSTRING(DEVICEDATA_Media_Catalog_CONST);
				PlutoSqlResult result_set;
				DB_ROW row;
				if( (result_set.r=m_pOrbiterGenerator->m_spDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
				{
					while ((row = db_wrapper_fetch_row(result_set.r)))
					{
						if( row[0] && row[0][0] )
						{
							string::size_type pos = 0;
							string sData = row[0];
							while(true)
							{
								string sLine = StringUtils::Tokenize(sData,"\r\n",pos,true);
								if(sLine.empty())
									break;
								string::size_type pos2 = 0;
								int PK_MediaType = atoi( StringUtils::Tokenize(sLine,",",pos2).c_str() );
								string sToken = StringUtils::Tokenize(sLine,",",pos2);
								string sDescription = StringUtils::Tokenize(sLine,",",pos2);
								if( PK_MediaType!=m_pOrbiterGenerator->m_dwMediaType )
									continue; // It's not for this type of media
								alArray->push_back(new ArrayValue(sToken,
									sDescription,
									NULL, 0, 0, 0, 0,false,false,false));
							}
						}
					}
				}
			}
            break;

		case ARRAY_Media_Filter_File_Format_CONST:
			{
                alArray->push_back(new ArrayValue("0",	GetText(TEXT_All_CONST),
                    NULL, 0, 0, 0, 0,false,false,false));

				string sSQL = "JOIN MediaType_FileFormat on FK_FileFormat = PK_FileFormat WHERE EK_MediaType=" + StringUtils::itos(m_pOrbiterGenerator->m_dwMediaType);
				vector<Row_FileFormat *> vectRow_FileFormat;
				m_pOrbiterGenerator->m_spDatabase_pluto_media->FileFormat_get()->GetRows(sSQL,&vectRow_FileFormat);
				for( vector<Row_FileFormat *>::iterator it=vectRow_FileFormat.begin();it!=vectRow_FileFormat.end();++it )
				{
					Row_FileFormat *pRow_FileFormat = *it;
					alArray->push_back(new ArrayValue(StringUtils::itos(pRow_FileFormat->PK_FileFormat_get()),
						pRow_FileFormat->Description_get(),
	                    NULL, 0, 0, 0, 0,false,false,false));
				}
			}
            break;

		// TODO -- Huh?  this one makes non sense.  PK_DeviceTemplate = ARRAY_Entertainment_Areas_CONST;????
        case ARRAY_Entertainment_Areas_CONST:
        case ARRAY_Viewable_Cameras_CONST:
        {
            int PK_DeviceTemplate=-1;
            switch(PK_Array)
            {
                case ARRAY_Entertainment_Areas_CONST:
                    PK_DeviceTemplate = ARRAY_Entertainment_Areas_CONST;
                    break;

            }

            {
                vector<class Row_Device *> vectD;
                string sql = string(DEVICE_FK_DEVICETEMPLATE_FIELD) + "=" +
                    StringUtils::itos(PK_DeviceTemplate) + " AND " + DEVICE_FK_INSTALLATION_FIELD + "=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + " ORDER BY " + DEVICE_DESCRIPTION_FIELD;
                m_mds->Device_get()->GetRows(sql,&vectD);

                for(size_t s=0;s<vectD.size();++s)
                {
                    Row_Device *drDevice = vectD[s];
                    alArray->push_back(new ArrayValue(StringUtils::itos(drDevice->PK_Device_get()),drDevice->Description_get(),NULL,0,0,0,VARIABLE_PK_Device_CONST,pRow_DesignObjVariation_DesignObj_Skin_Language->CanBeHidden_get()==1,pRow_DesignObjVariation_DesignObj_Skin_Language->HideByDefault_get()==1,false));
                }
            }
            break;
        }

		case ARRAY_All_Phones_CONST:
		case ARRAY_Hard_Phones_CONST:
		case ARRAY_Soft_Phones_CONST:
		case ARRAY_Soft_Video_Phones_CONST:
		case ARRAY_Media_Directors_CONST:
        {
            int PK_DeviceCategory=-1;
            switch(PK_Array)
            {
                case ARRAY_All_Phones_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Phones_CONST;
                    break;
                case ARRAY_Hard_Phones_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Hard_Phones_CONST;
                    break;
                case ARRAY_Soft_Phones_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Soft_Phones_CONST;
                    break;
                case ARRAY_Soft_Video_Phones_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Video_Soft_Phones_CONST;
                    break;
                case ARRAY_Media_Directors_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Media_Director_CONST;
                    break;
            }

            {
                vector<class Row_Device *> vectD;
                string sql = string("SELECT Device.* FROM Device ") + 
					"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate " +
					"JOIN DeviceCategory ON FK_DeviceCategory=DeviceCategory.PK_DeviceCategory " +
					"LEFT JOIN DeviceCategory As ParentCategory ON DeviceCategory.FK_DeviceCategory_Parent=ParentCategory.PK_DeviceCategory " +
					"WHERE FK_Installation=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + " AND (DeviceCategory.PK_DeviceCategory="+StringUtils::itos(PK_DeviceCategory)+
					" OR ParentCategory.PK_DeviceCategory="+StringUtils::itos(PK_DeviceCategory)+" OR ParentCategory.FK_DeviceCategory_Parent="+StringUtils::itos(PK_DeviceCategory)+ ") " +
                    " ORDER BY Device.Description";
                m_mds->Device_get()->GetRows(sql,&vectD);

                for(size_t s=0;s<vectD.size();++s)
                {
                    Row_Device *drDevice = vectD[s];
					Row_Room *pRow_Room = drDevice->FK_Room_getrow();
					string sRoom;
					if( pRow_Room )
						sRoom = pRow_Room->Description_get();
                    alArray->push_back(new ArrayValue(StringUtils::itos(drDevice->PK_Device_get()),drDevice->Description_get() + " (" + sRoom + ")",NULL,0,0,0,VARIABLE_PK_Device_CONST,pRow_DesignObjVariation_DesignObj_Skin_Language->CanBeHidden_get()==1,pRow_DesignObjVariation_DesignObj_Skin_Language->HideByDefault_get()==1,false));
                }
            }
            break;
        }

        case ARRAY_Phone_Users_CONST:
        case ARRAY_All_Users_CONST:
            {
                vector<Row_Installation_Users *> vectIUs;
				Row_Installation *pRow_Installation = m_pOrbiterGenerator->m_pRow_Device->FK_Installation_getrow();
				if( pRow_Installation )
				{
                    pRow_Installation->Installation_Users_FK_Installation_getrows(&vectIUs);
					for(size_t s=0;s<vectIUs.size();++s)
					{
						Row_Installation_Users *drIU = vectIUs[s];
						Row_Users *pRow_Users = drIU->FK_Users_getrow();
						if( !pRow_Users )
							continue;

						if( pRow_Users->HideFromOrbiter_get()==1 )
							continue;
						if( PK_Array==ARRAY_Phone_Users_CONST && drIU->FK_Users_getrow()->Extension_isNull() )
							continue;
						//User selection names.
						//Use Nickname when available. 
						string sName = "";						
						if( drIU->FK_Users_getrow()->Nickname_get().size() && !(drIU->FK_Users_getrow()->Nickname_isNull())) {
							//There is a Nickname, use it!
							sName = pRow_Users->Nickname_get();
						} else {
							//There is no Nickname, so use the UserName		
							sName = pRow_Users->UserName_get();
						}

						alArray->push_back(new ArrayValue(StringUtils::itos(pRow_Users->PK_Users_get()),sName,NULL,0,0,
							0,0,pRow_DesignObjVariation_DesignObj_Skin_Language->CanBeHidden_get()==1,pRow_DesignObjVariation_DesignObj_Skin_Language->HideByDefault_get()==1,false));
					}
				}
            }
            break;
        case ARRAY_Locations_CONST:
            {
                DequeLocationInfo::iterator it;
                for(it=m_pOrbiterGenerator->m_dequeLocation.begin();it!=m_pOrbiterGenerator->m_dequeLocation.end();++it)
                {
                    LocationInfo *li = (*it);
                    alArray->push_back(new ArrayValue(StringUtils::itos(li->iLocation) + "," + StringUtils::itos(li->PK_Room) + "," + StringUtils::itos(li->PK_EntertainArea),li->Description,li->drIcon,0,0,0,VARIABLE_PK_Device_CONST,pRow_DesignObjVariation_DesignObj_Skin_Language->CanBeHidden_get()==1,pRow_DesignObjVariation_DesignObj_Skin_Language->HideByDefault_get()==1,false));
                }
            }
            break;
        case ARRAY_Rooms_CONST:
            {
				vector<Row_Room *> vectRow_Room;
				m_mds->Room_get()->GetRows("FK_Installation=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()),&vectRow_Room);
				for(size_t s=0;s<vectRow_Room.size();++s)
				{
					Row_Room *pRow_Room = vectRow_Room[s];
                    alArray->push_back(new ArrayValue(StringUtils::itos(pRow_Room->PK_Room_get()),pRow_Room->Description_get(),NULL,0,0,0,0,pRow_DesignObjVariation_DesignObj_Skin_Language->CanBeHidden_get()==1,pRow_DesignObjVariation_DesignObj_Skin_Language->HideByDefault_get()==1,false));
                }
            }
            break;
        case ARRAY_Floorplans_CONST:
            {
                vector<class Row_Floorplan *> vectFs;
                string sql = string(FLOORPLAN_FK_INSTALLATION_FIELD) + "=" + \
                    StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + " ORDER BY " + FLOORPLAN_PAGE_FIELD;
                m_mds->Floorplan_get()->GetRows(sql,&vectFs);

                for(size_t s=0;s<vectFs.size();++s)
                {
                    Row_Floorplan *drF = vectFs[s];

                    alArray->push_back(new ArrayValue(StringUtils::itos(drF->Page_get()),drF->Description_get(),
                        drF->FK_Icon_isNull() ? NULL : drF->FK_Icon_getrow(),
                        0,0,0,0,pRow_DesignObjVariation_DesignObj_Skin_Language->CanBeHidden_get()==1,pRow_DesignObjVariation_DesignObj_Skin_Language->HideByDefault_get()==1,false));
                }
            }
            break;
    case ARRAY_Favourite_TV_Channels_CONST:
      {
	DBHelper *pDBHelper_Myth = new DBHelper("localhost", "root", "","mythconverg");
	vector <class Row_Bookmark *> vectBookMarks;
	string sWhereQuery = "WHERE EK_MediaType = 1";  // Grab all TV Related bookmarks.
	m_pOrbiterGenerator->m_spDatabase_pluto_media->Bookmark_get()->GetRows(sWhereQuery,&vectBookMarks);
	for ( vector<Row_Bookmark *>::iterator it=vectBookMarks.begin();it!=vectBookMarks.end();++it )
	  {
	    // Get channel from bookmark position, grab related data in mythconverg.
	    Row_Bookmark *pRow_Bookmark = *it;
	    vector<string> vectTmp;
	    string sPosition = pRow_Bookmark->Position_get();
	    string sChannel = sPosition.substr(sPosition.find(":")+1);
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"FAVE: %s",sChannel.c_str());;
	    string sSQLForChannel = "select chanid, callsign, icon FROM channel WHERE chanid = '"+sChannel+"'";
	    PlutoSqlResult result_set_channel;
	    if ((result_set_channel.r=pDBHelper_Myth->db_wrapper_query_result(sSQLForChannel))!=NULL &&
		result_set_channel.r->row_count>0 )
	      {
		DB_ROW row=db_wrapper_fetch_row(result_set_channel.r);
		string sCallsign=row[1];
		string sIcon=row[2];
		alArray->push_back(new ArrayValue(sPosition,sCallsign,NULL,0,0,0,0,false,false,false));
	      }
	  }
      }
      break;

    }

	string sRegenMonitor = m_pOrbiterGenerator->m_pRegenMonitor->GetModInfo_Array(PK_Array);
	if( sRegenMonitor.size()>0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"obj: %d regen cache %d %s",m_pRow_DesignObj->PK_DesignObj_get(),(unsigned int)m_pDesignObj_TopMost->m_vectRegenMonitor.size(),sRegenMonitor.c_str());
		m_pDesignObj_TopMost->m_vectRegenMonitor.push_back( sRegenMonitor );
	}
    return alArray;

}

void DesignObj_Generator::ScaleAllValues(int FactorX_Input,int FactorY_Input,class DesignObj_Generator *pTopmostObject)
{
	int FactorX = FactorX_Input * m_iScale/100;
	int FactorY = FactorY_Input * m_iScale/100;
	int FactorX_Origin = FactorX_Input;
	int FactorY_Origin = FactorY_Input;

	if( !m_bValuesScaled )
    {
        PlutoPoint p3(m_rBackgroundPosition.Location());
        PlutoPoint p2(ScaleValue(&p3,FactorX_Origin,FactorY_Origin));
        m_rBackgroundPosition.Location(p2);
        PlutoSize plutoSize = m_rBackgroundPosition.Size();
        PlutoPoint p(plutoSize.Width,plutoSize.Height);
        PlutoSize plutoSize2(ScaleValue(&p,FactorX,FactorY));
        m_rBackgroundPosition.Size(plutoSize2);
	PlutoPoint p6(m_rPosition.Location());
		p=ScaleValue(&p6,FactorX_Origin,FactorY_Origin);
        m_rPosition.Location(p);
		PlutoSize plutoSize3=m_rPosition.Size();
        p=PlutoPoint(plutoSize3.Width,plutoSize3.Height);
        p=PlutoPoint(ScaleValue(&p,FactorX,FactorY));
        m_rPosition.Size(p);

		for(size_t s=0;s<m_vectDesignObjText.size();++s)
        {
            CGText *ot = (CGText *) m_vectDesignObjText[s];
            PlutoPoint p5 = ot->m_rPosition.Location();
            PlutoSize plutoSize4(*(ScaleValue(&p5,FactorX_Origin,FactorY_Origin)));
            ot->m_rPosition.Location(PlutoPoint(plutoSize4.Width,plutoSize4.Height));
            PlutoSize plutoSize5(ot->m_rPosition.Size());  // Linux won't let me pass these in
            //PlutoPoint p4(ot->m_rPosition.Size()); // Works on vs.net, but not gcc
            PlutoPoint p4(plutoSize5.Width,plutoSize5.Height);
            PlutoPoint p(ScaleValue(&p4,FactorX,FactorY));
            ot->m_rPosition.Size(p);
        }

        m_bValuesScaled=true;
    }
	else if( m_ocoParent && m_ocoParent->m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
	{
		// Floorplans were scaled already, but they need to be offset by the parent floorplan object
		// and they are to be centered on the point, rather than upper/left like the others
		m_rBackgroundPosition.X += m_ocoParent->m_rBackgroundPosition.X - m_rBackgroundPosition.Width/2;
		m_rBackgroundPosition.Y += m_ocoParent->m_rBackgroundPosition.Y - m_rBackgroundPosition.Height/2;
		m_rPosition.X += m_ocoParent->m_rPosition.X - m_rPosition.Width/2;
		m_rPosition.Y += m_ocoParent->m_rPosition.Y - m_rPosition.Height/2;

		for(size_t s=0;s<m_vectDesignObjText.size();++s)
		{
			CGText *ot = (CGText *) m_vectDesignObjText[s];
			ot->m_rPosition.X += m_ocoParent->m_rPosition.X - m_rPosition.Width/2;
			ot->m_rPosition.Y += m_ocoParent->m_rPosition.Y - m_rPosition.Height/2;
		}
	}

    for(size_t s=0;s<m_alChildDesignObjs.size();++s)
    {
        DesignObj_Generator *oc = m_alChildDesignObjs[s];
		if( oc->m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST && pTopmostObject )
            pTopmostObject->m_bContainsArrays=true; // We don't want to cache floorplans either

        oc->ScaleAllValues(FactorX_Input,FactorY_Input,pTopmostObject);
    }

    for(size_t s=0;s<m_alNonMPArrays.size();++s)
    {
        CGArray *oa = m_alNonMPArrays[s];
        if( pTopmostObject )
            pTopmostObject->m_bContainsArrays=true;

        for(size_t s2=0;s2<oa->m_alChildDesignObjs_OfArray.size();++s2)
        {
            DesignObj_Generator *oc = oa->m_alChildDesignObjs_OfArray[s2];
            oc->ScaleAllValues(FactorX_Input,FactorY_Input,pTopmostObject);
        }
    }
    for(size_t s=0;s<m_alMPArray.size();++s)
    {
        CGArray *oa = m_alMPArray[s];
        if( pTopmostObject )
            pTopmostObject->m_bContainsArrays=true;

		for(size_t s2=0;s2<oa->m_alChildDesignObjs_OfArray.size();++s2)
        {
            DesignObj_Generator *oc = oa->m_alChildDesignObjs_OfArray[s2];
            oc->ScaleAllValues(FactorX_Input,FactorY_Input,pTopmostObject);
        }
    }
}

PlutoPoint *DesignObj_Generator::ScaleValue(PlutoPoint *pt,int FactorX,int FactorY)
{
    return new PlutoPoint(pt->X * FactorX / 1000,pt->Y * FactorY / 1000);
}

string DesignObj_Generator::GetParm(int PK_DesignObjParameter)
{
    return GetParm(PK_DesignObjParameter,m_pRow_DesignObjVariation,false);
}
string DesignObj_Generator::GetParm(int PK_DesignObjParameter,bool bReplaceNulls)
{
    return GetParm(PK_DesignObjParameter,m_pRow_DesignObjVariation,bReplaceNulls);
}

string DesignObj_Generator::GetParm(int PK_DesignObjParameter,Row_DesignObjVariation * drDesignObjVariation)
{
    return GetParm(PK_DesignObjParameter,drDesignObjVariation,false);
}
string DesignObj_Generator::GetParm(int PK_DesignObjParameter,Row_DesignObjVariation * drDesignObjVariation,bool bReplaceNulls)
{
	map< int,string >::iterator it=m_mapParameterOverides.find(PK_DesignObjParameter);
	if( it!=m_mapParameterOverides.end() )
		return it->second;

    string oValue="";
    Row_DesignObjVariation_DesignObjParameter * drODP = GetParmRow(PK_DesignObjParameter,drDesignObjVariation);

    if( drDesignObjVariation->PK_DesignObjVariation_get()!=m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get() &&
        (!drODP || (drODP->Value_isNull() && bReplaceNulls)) )
    {
        drODP = GetParmRow(PK_DesignObjParameter,m_pRow_DesignObjVariation_Standard);
    }

    if( !drODP )
        return string();

    bool btemp;
    return SubstituteVariables(drODP->Value_get(),&btemp);
}

Row_DesignObjVariation_DesignObjParameter *DesignObj_Generator::GetParmRow(int PK_DesignObjParameter,Row_DesignObjVariation * drDesignObjVariation)
{
    vector<class Row_DesignObjVariation_DesignObjParameter *> vectRow_DesignObjVariation_DesignObjParameter;
	string sSQL = "FK_DesignObjParameter=" + StringUtils::itos(PK_DesignObjParameter) + " AND FK_DesignObjVariation=" + 
		StringUtils::itos(drDesignObjVariation->PK_DesignObjVariation_get()) + " AND "
		+ "(FK_Language IS NULL OR FK_Language=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Language->PK_Language_get()) + ") AND "
		+ "(FK_Skin IS NULL OR FK_Skin=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get()) + ")";
	m_mds->DesignObjVariation_DesignObjParameter_get()->GetRows(sSQL,&vectRow_DesignObjVariation_DesignObjParameter);
    Row_DesignObjVariation_DesignObjParameter * pRow_DesignObjVariation_DesignObjParameter=NULL;
    bool bMatchLanguage=false;
    bool bMatchSkin=false;

	for(vector<class Row_DesignObjVariation_DesignObjParameter *>::iterator it=vectRow_DesignObjVariation_DesignObjParameter.begin();it!=vectRow_DesignObjVariation_DesignObjParameter.end();++it)
    {
        Row_DesignObjVariation_DesignObjParameter *pRow_DesignObjVariation_DesignObjParameter_Test = *it;
		if( pRow_DesignObjVariation_DesignObjParameter_Test->Ignore_get()==1 )
			continue;

        if( !pRow_DesignObjVariation_DesignObjParameter_Test->FK_Skin_isNull() && pRow_DesignObjVariation_DesignObjParameter_Test->FK_Skin_get()==m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get() &&
            !pRow_DesignObjVariation_DesignObjParameter_Test->FK_Language_isNull() && pRow_DesignObjVariation_DesignObjParameter_Test->FK_Language_get()==m_pOrbiterGenerator->m_pRow_Language->PK_Language_get() )
        {
            return pRow_DesignObjVariation_DesignObjParameter_Test;
        }
        else if( !pRow_DesignObjVariation_DesignObjParameter_Test->FK_Skin_isNull() && pRow_DesignObjVariation_DesignObjParameter_Test->FK_Skin_get()==m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get() &&
            pRow_DesignObjVariation_DesignObjParameter_Test->FK_Language_isNull() )
        {
            pRow_DesignObjVariation_DesignObjParameter = pRow_DesignObjVariation_DesignObjParameter_Test;
            bMatchSkin=true;
        }
        else if( !pRow_DesignObjVariation_DesignObjParameter_Test->FK_Language_isNull() && pRow_DesignObjVariation_DesignObjParameter_Test->FK_Language_get()==m_pOrbiterGenerator->m_pRow_Language->PK_Language_get() &&
            pRow_DesignObjVariation_DesignObjParameter_Test->FK_Skin_isNull() && !bMatchSkin )
        {
            pRow_DesignObjVariation_DesignObjParameter = pRow_DesignObjVariation_DesignObjParameter_Test;
            bMatchLanguage=true;
        }
        else if( pRow_DesignObjVariation_DesignObjParameter_Test->FK_Language_isNull() && pRow_DesignObjVariation_DesignObjParameter_Test->FK_Skin_isNull() && !bMatchSkin && !bMatchLanguage)
        {
            pRow_DesignObjVariation_DesignObjParameter = pRow_DesignObjVariation_DesignObjParameter_Test;
        }
    }

	return pRow_DesignObjVariation_DesignObjParameter;
}

string DesignObj_Generator::SubstituteVariables(string Text,bool *bContainsRunTimeVariables)
{
if( Text.find("MTB")!=string::npos )
{
int k=2;
}
    *bContainsRunTimeVariables=false;
    size_t Pos = Text.find("<%=");
    while( Pos!=string::npos )
    {
        size_t EndPos = Text.find("%>",Pos);
        string sVariable = Text.substr(Pos+3,EndPos-Pos-3);
        bool bRunTime=true;  // If there's a :0, it means it is not run-time
        size_t Option=sVariable.find(':');
		if( sVariable.size()>1 && sVariable[0]=='K' )
			bRunTime=true;
        else if( Option!=string::npos && sVariable.substr(Option+1,1)=="0" )
            bRunTime = false;

        string sValue="**NO~VALUE**";  // Use a special value to differentiate between a real "empty" vs. a no-match

        if( sVariable[0]=='!' && sVariable.length()==1 )
            sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get());
		else if( sVariable=="L" )
			sValue = StringUtils::itos(m_pOrbiterGenerator->m_iLocation);
		else if( sVariable=="RIL" ) // Room in location
		{
			string::size_type pos = m_pOrbiterGenerator->m_iID_Array.find(',');
			string::size_type pos2 = m_pOrbiterGenerator->m_iID_Array.find(',',pos+1);
			if( pos!=string::npos && pos2!=string::npos )
				sValue = m_pOrbiterGenerator->m_iID_Array.substr(pos+1,pos2-pos-1);
		}
		else if( sVariable.substr(0,2)=="MM" )
            sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu->PK_DesignObj_get());
        else if( sVariable.substr(0,2)=="MS" )
        {
            if( m_pOrbiterGenerator->m_pRow_DesignObj_Sleeping )
                sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_DesignObj_Sleeping->PK_DesignObj_get());
            else
                sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu->PK_DesignObj_get());
        }
        else if( sVariable[0]=='U' && sVariable.length()>1 && sVariable[1]!='N' )  // <%=U592%> means user 592.  <%=U%> means current user, and is evaluated at runtime
        {
            int PK_User = atoi(sVariable.substr(2).c_str());
            Row_Users * drUser = m_mds->Users_get()->GetRow(PK_User);

            if( !drUser )
            {
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"User %d in text %s is invalid",PK_User,Text.c_str());
				sValue = "";
            } else {
		   //not sure where this is used exactly?
	           if(drUser->Nickname_get().size() && !(drUser->Nickname_isNull())) {
			//There is a Nickname, use it!
			sValue = drUser->Nickname_get();
		   } else {
			//There is no Nickname, so use the UserName
			sValue = drUser->UserName_get();
		   }
	    }
        }
        else if( sVariable[0]=='T' && sVariable.length()>1 )  // <%=T592%> means text object 592
        {
            int PK_Text = atoi(sVariable.substr(1).c_str());
			Row_Text_LS *pRow_Text_LS = CGText::GetText_LS(PK_Text,m_pOrbiterGenerator);

            if( !pRow_Text_LS  )
            {
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Text %d is invalid",PK_Text);
            }

            sValue = pRow_Text_LS->Description_get();
        }
        /* todo
        else if( sVariable.substr(0,2)=="SP" )
        {
            Row_Device * drDeviceOrbiter = m_mds->Device_get()->GetRow(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get());
            vector<Row_Device *> vectd;
            drDeviceOrbiter->Device_FK_Device_ControlledVia_getrows(&vectd);
            for(size_t s=0;s<vectd.size();++s)
            {
                Row_Device * drDeviceChild = vectd[s];
                if( drDeviceChild->FK_DeviceTemplate_get()==DeviceTemplate_CE_SIP_CONST )
                {
                    sValue = StringUtils::itos(drDeviceChild->PK_Device_get());
                    break;
                }
            }
        }
*/
        else if( sVariable.substr(0,2)=="IM" )
        {
            if( m_pOrbiterGenerator->m_pRow_Icon!=NULL )
                sValue = m_pOrbiterGenerator->m_pRow_Icon->MainFileName_get();
            else
                sValue = "";
        }
        else if( sVariable.substr(0,2)=="IS" )
        {
            if( m_pOrbiterGenerator->m_pRow_Icon!=NULL && !m_pOrbiterGenerator->m_pRow_Icon->SelectedFileName_isNull() )
                sValue = m_pOrbiterGenerator->m_pRow_Icon->SelectedFileName_get();
            else
                sValue = "";
        }
        else if( sVariable.substr(0,2)=="IA" )
        {
            if( m_pOrbiterGenerator->m_pRow_Icon!=NULL && !m_pOrbiterGenerator->m_pRow_Icon->AltFileNames_isNull() )
                sValue = m_pOrbiterGenerator->m_pRow_Icon->AltFileNames_get();
            else
                sValue = "";
        }
        else if( sVariable[0]=='I' && sVariable.length()==1 )
        {
            sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get());
        }
        else
        {
            try
            {
                int PK_Variable = atoi(Option>0 ? sVariable.substr(0,Option).c_str() : sVariable.c_str());

                if( bRunTime )
                {
                    *bContainsRunTimeVariables=true;
                }
                else
                {
                    if( PK_Variable==VARIABLE_Array_Desc_CONST )
                        sValue = m_pOrbiterGenerator->m_sDesc_Array;
                    else if( PK_Variable==VARIABLE_Array_ID_CONST )
                        sValue = m_pOrbiterGenerator->m_iID_Array;
                    else
                    {
                        string o = m_pOrbiterGenerator->m_mapVariable[PK_Variable];
                        if( o.length()>0 )
                            sValue = o;
                    }
                }
            }
            catch(...)
            {
                *bContainsRunTimeVariables=true;
                sValue=""; // just to be sure
            }  // Maybe it's a <%=S%> or some other locally used substitution, just let it go through
        }

        if( sValue!="**NO~VALUE**" )
        {
            Text = Text.substr(0,Pos) + sValue + Text.substr(EndPos+2);
            Pos = Text.find("<%=",Pos);  // Maybe the string substituted also has a substitute
        }
        else
            Pos = Text.find("<%=",Pos+1);
    }

    return Text;
}

void DesignObj_Generator::HandleRotation(int iRotate)
{
	if( !m_pOrbiterGenerator )
		return; // We were serialized from disk anyway
	m_rBackgroundPosition.Rotate(iRotate,m_pOrbiterGenerator->m_sScaledSize);
	m_rPosition.Rotate(iRotate,m_pOrbiterGenerator->m_sScaledSize);
	m_rBitmapOffset.Rotate(iRotate,m_pOrbiterGenerator->m_sScaledSize);
	m_sOriginalSize.Rotate(iRotate);

	for(size_t s=0;s<m_vectDesignObjText.size();++s)
	{
		CGText *pDesignObjText = (CGText *) m_vectDesignObjText[s];
		pDesignObjText->m_rPosition.Rotate(iRotate,m_pOrbiterGenerator->m_sScaledSize);
	}

	for(DesignObjZoneList::iterator it=m_ZoneList.begin();it!=m_ZoneList.end();++it)
		(*it)->m_Rect.Rotate(iRotate,m_pOrbiterGenerator->m_sScaledSize);

	for(DesignObj_DataList::iterator it=m_ChildObjects.begin();it!=m_ChildObjects.end();++it)
		((DesignObj_Generator *)(*it))->HandleRotation(iRotate);
}

bool DesignObj_Generator::CachedVersionOK()
{
	m_pOrbiterGenerator->m_pRegenMonitor->SetRoomEA(m_pOrbiterGenerator->m_pRow_Room,m_pOrbiterGenerator->m_pRow_EntertainArea);

	for(size_t s=0;s<m_vectRegenMonitor.size();++s)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"obj: %d %d/%d %s",m_pRow_DesignObj->PK_DesignObj_get(),(unsigned int)s,(unsigned int)m_vectRegenMonitor.size() ,m_vectRegenMonitor[s].c_str());
		if( !m_pOrbiterGenerator->m_pRegenMonitor->CachedVersionOK(m_vectRegenMonitor[s]) )
			return false;
	}
	return true;
}

void DesignObj_Generator::WriteRegenVersion(string sFilename)
{
	SerializeClass sc;
	sc.StartWriting();
	sc.Write_long(long(m_vectRegenMonitor.size()));
	for(size_t s=0;s<m_vectRegenMonitor.size();++s)
		sc.Write_string(m_vectRegenMonitor[s]);

	FILE *file = fopen(sFilename.c_str(),"wb");
	fwrite(sc.m_pcDataBlock,sc.CurrentSize(),1,file);
	fclose(file);
	sc.FreeSerializeMemory();
}

bool DesignObj_Generator::ReadRegenVersion(string sFilename)
{
	SerializeClass sc;
	size_t s;
	char *pPtr = FileUtils::ReadFileIntoBuffer(sFilename,s);
	if( !pPtr )
		return false;
	sc.StartReading((unsigned long) s,pPtr);
	size_t count=sc.Read_long();
	for(size_t s=0;s<count;++s)
	{
		string str;
		sc.Read_string(str);
		m_vectRegenMonitor.push_back(str);
	}
	sc.FreeSerializeMemory();
	return true;
}

bool DesignObj_Generator::ReadFloorplanInfo(string sFilename)
{
	SerializeClass sc;
	size_t s;
	char *pPtr = FileUtils::ReadFileIntoBuffer(sFilename,s);
	if( !pPtr )
		return false;
	sc.StartReading((unsigned long) s,pPtr);
	int count=sc.Read_long();
	string str;
	sc.Read_string(str);
	m_pOrbiterGenerator->m_iNumFloorplanItems+=count;
	m_pOrbiterGenerator->m_sFloorPlanData+=str;

	sc.FreeSerializeMemory();
	return true;
}


void DesignObj_Generator::WriteFloorplanInfo(string sFilename)
{
	SerializeClass sc;
	sc.StartWriting();
	sc.Write_long(m_iNumFloorplanItems);
	sc.Write_string(m_sFloorPlanData);

	FILE *file = fopen(sFilename.c_str(),"wb");
	fwrite(sc.m_pcDataBlock,sc.CurrentSize(),1,file);
	fclose(file);
	sc.FreeSerializeMemory();
}

string DesignObj_Generator::GetText(int PK_Text)
{
	Row_Text_LS *pRow_Text_LS = m_mds->Text_LS_get()->GetRow(PK_Text,m_pOrbiterGenerator->m_pRow_Language->PK_Language_get());
	if( !pRow_Text_LS && m_pOrbiterGenerator->m_pRow_Language->PK_Language_get()!=1 )
		pRow_Text_LS = m_mds->Text_LS_get()->GetRow(PK_Text,1);
	return pRow_Text_LS ? pRow_Text_LS->Description_get() : "";
}

void DesignObj_Generator::AddDataGridObjects()
{
	// If there are objects specified for to be placed in the cells (ie DESIGNOBJPARAMETER_PK_DesignObj_Row1_CONST, DESIGNOBJPARAMETER_PK_DesignObj_Coll1_CONST or DESIGNOBJPARAMETER_PK_DesignObj_Cell_CONST)
	// then we'll place these as child objects within the datagrid and update the m_mapChildDgObjects within the datagrid object to point to 
	// the grid of objects.  The user may have specified a different object/height+width for the first row/column.  Iterate from the top row
	// down, starting with the custom first row, then repeat for all additional rows
	int RowHeight = atoi(GetParm(DESIGNOBJPARAMETER_Fixed_Row_Height_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());
	int NumRows = atoi(GetParm(DESIGNOBJPARAMETER_Num_of_Rows_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());
	int ColWidth = atoi(GetParm(DESIGNOBJPARAMETER_Fixed_Column_Width_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());
	int NumCols = atoi(GetParm(DESIGNOBJPARAMETER_Num_of_Columns_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());
	int FirstRowHeight = atoi(GetParm(DESIGNOBJPARAMETER_First_Row_Height_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());
	int FirstColWidth = atoi(GetParm(DESIGNOBJPARAMETER_First_Column_Width_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());

	if( (!RowHeight && !NumRows) || (!ColWidth && !NumCols) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DesignObj_Generator::AddDataGridObjects %d hasn't a width/height",m_pRow_DesignObj->PK_DesignObj_get());
		return;
	}

	if( !NumRows )
	{
		if( FirstRowHeight && NumRows>1 )
			NumRows = ( (m_rPosition.Height-FirstRowHeight) / RowHeight ) + 1;
		else
			NumRows = m_rPosition.Height / RowHeight;
	}
	if( !RowHeight )
	{
		if( FirstRowHeight && NumRows>1 )
			RowHeight = (m_rPosition.Height-FirstRowHeight) / (NumRows-1);
		else
			RowHeight = m_rPosition.Height / NumRows;
	}
	if( !NumCols )
	{
		if( FirstColWidth && NumCols>1 )
			NumCols = ( (m_rPosition.Width-FirstColWidth) / ColWidth ) + 1;
		else
			NumCols = m_rPosition.Width / ColWidth;
	}
	if( !ColWidth )
	{
		if( FirstColWidth && NumCols>1 )
			ColWidth = (m_rPosition.Width-FirstColWidth) / (NumCols-1);
		else
			ColWidth = m_rPosition.Width / NumCols;
	}

	Row_DesignObj *pRow_DesignObj_FirstRow=NULL,*pRow_DesignObj_FirstCol=NULL,*pRow_DesignObj_Cell=NULL;
	int PK_DesignObj_FirstRow = atoi(GetParm(DESIGNOBJPARAMETER_PK_DesignObj_Row1_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());
	int PK_DesignObj_FirstCol = atoi(GetParm(DESIGNOBJPARAMETER_PK_DesignObj_Coll1_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());
	int PK_DesignObj_Cell = atoi(GetParm(DESIGNOBJPARAMETER_PK_DesignObj_Cell_CONST,0 != m_pOrbiterGenerator->m_pRow_Skin->MergeStandardVariation_get()).c_str());
	if( PK_DesignObj_FirstRow )
		pRow_DesignObj_FirstRow = m_mds->DesignObj_get()->GetRow(PK_DesignObj_FirstRow);
	if( PK_DesignObj_FirstCol )
		pRow_DesignObj_FirstCol = m_mds->DesignObj_get()->GetRow(PK_DesignObj_FirstCol);
	if( PK_DesignObj_Cell )
		pRow_DesignObj_Cell = m_mds->DesignObj_get()->GetRow(PK_DesignObj_Cell);

	int RowPos=0;

	if( pRow_DesignObj_FirstRow )  // We've got a custom first row for special handling
	{
		int ColPos = 0, StartCol = 0;  // If there's a special first column and a special first row, cell 0,0 must be blank, and StartCol will be 1
		if( pRow_DesignObj_FirstCol )
		{
			StartCol = 1;
			ColPos = FirstColWidth ? FirstColWidth : ColWidth;  // Start at the first position
		}
		for( int iCol = StartCol; iCol<NumCols; ++iCol )
		{
			PlutoRectangle rectangle(m_rPosition.X+ColPos,m_rPosition.Y,ColWidth,FirstRowHeight ? FirstRowHeight : RowHeight);  
			DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,pRow_DesignObj_FirstRow,rectangle,this,false,false);
				pDesignObj_Generator->m_bCanBeHidden=pDesignObj_Generator->m_bHideByDefault=true;
			if( pDesignObj_Generator->m_pRow_DesignObjVariation ) // Be sure it's a valid object
			{
				m_mapChildDgObjects[ make_pair (iCol,0) ] = pDesignObj_Generator;  // m_mapChildDgObjects is col/row to object
				// Reduce this grid by the size of the up/down
				m_alChildDesignObjs.push_back(pDesignObj_Generator);
			}
			else
				delete pDesignObj_Generator;
			ColPos += ColWidth;
		}
		RowPos = FirstRowHeight ? FirstRowHeight : RowHeight;
	}

	// Add all the rest of the rows
	for(int iRow = pRow_DesignObj_FirstRow ? 1 : 0; iRow<NumRows; ++iRow)
	{
		int ColPos = 0, StartCol = 0;  // If there's a special first column and a special first row, cell 0,0 must be blank, and StartCol will be 1
		if( pRow_DesignObj_FirstCol )
		{
			PlutoRectangle rectangle(m_rPosition.X,m_rPosition.Y+RowPos,FirstColWidth ? FirstColWidth : ColWidth,RowHeight);  
			DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,pRow_DesignObj_FirstCol,rectangle,this,false,false);
				pDesignObj_Generator->m_bCanBeHidden=pDesignObj_Generator->m_bHideByDefault=true;
			if( pDesignObj_Generator->m_pRow_DesignObjVariation ) // Be sure it's a valid object
			{
				m_mapChildDgObjects[ make_pair (0,iRow) ] = pDesignObj_Generator;  // m_mapChildDgObjects is col/row to object
				// Reduce this grid by the size of the up/down
				m_alChildDesignObjs.push_back(pDesignObj_Generator);
			}
			else
				delete pDesignObj_Generator;

			StartCol = 1;
			ColPos = FirstColWidth ? FirstColWidth : ColWidth;  // Start at the first position
		}
		if( pRow_DesignObj_Cell )
		{
			for( int iCol = StartCol; iCol<NumCols; ++iCol )
			{
				PlutoRectangle rectangle(m_rPosition.X+ColPos,m_rPosition.Y+RowPos,ColWidth,RowHeight);  
				DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,pRow_DesignObj_Cell,rectangle,this,false,false);
				pDesignObj_Generator->m_bCanBeHidden=pDesignObj_Generator->m_bHideByDefault=true;
				if( pDesignObj_Generator->m_pRow_DesignObjVariation ) // Be sure it's a valid object
				{
					m_mapChildDgObjects[ make_pair (iCol,iRow) ] = pDesignObj_Generator;  // m_mapChildDgObjects is col/row to object
					// Reduce this grid by the size of the up/down
					m_alChildDesignObjs.push_back(pDesignObj_Generator);
				}
				else
					delete pDesignObj_Generator;
				ColPos += ColWidth;
			}
			RowPos += RowHeight;
		}
	}
}

void DesignObj_Generator::HandleAdjustments()
{
	// Each adjustment is separated with a : and is in the format [criteria]![what to adjust]![operator]![adjusted value]
	// criteria can include: UI=[PK_UI], Skin=[PK_Skin], Aspect=[16_9 | 3_4], and each can be separated with & or | for and or or
	// what to adjust can be: x, y, w, h, r (right), b (bottom), or pX (parameter #X)
	// operator can be +, - or =
	// adjusted value can be an actual value or <%=token%> where token can be:
		// screen_width, screen_height, Ox.y where x is a PK_DesignObj and y is the same as 'what to adjust' values

	string::size_type pos=0;
	while(pos<m_sAdjustments.size())
	{
		string sToken = StringUtils::Tokenize(m_sAdjustments,":",pos);
		HandleAdjustment(sToken);	
	}
}

void DesignObj_Generator::HandleAdjustment(string sToken)
{
	string::size_type pos=0;
	string sCriteria = StringUtils::Tokenize(sToken,"!",pos);
	string sWhat = StringUtils::Tokenize(sToken,"!",pos);
	string sOperator = StringUtils::Tokenize(sToken,"!",pos);
	string sValue = StringUtils::Tokenize(sToken,"!",pos);

	if( sWhat.empty() || sOperator.empty() || sValue.empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DesignObj_Generator::HandleAdjustment values are empth");
		return;
	}
	if( ProcessCriteria(sCriteria)==false )
		return;

	string sCalculatedValue = CalculateValue(sValue);
	if( sOperator=="-" || sOperator=="+" )
		ProcessAddition(sWhat[0],atoi(sCalculatedValue.c_str()),sOperator=="+");
	else if( sOperator=="=" )
		ProcessAssignment(sWhat,sValue);
}

bool DesignObj_Generator::ProcessCriteria(string sCriteria)
{
	return true;
}

string DesignObj_Generator::CalculateValue(string sValue)
{
	if( sValue=="screen_width" )
		return StringUtils::itos(m_pOrbiterGenerator->m_Width);
	else if( sValue=="screen_height" )
		return StringUtils::itos(m_pOrbiterGenerator->m_Height);
	else if( sValue.size()>1 && sValue[0]=='O' )
	{
		string::size_type pos = sValue.find('.');
		int PK_DesignObj = atoi(sValue.substr(1).c_str());
		DesignObj_Generator *pDesignObj_Generator=NULL;
		CGArray *pCGArray=NULL;
		m_pDesignObj_TopMost->FindChild(PK_DesignObj,&pDesignObj_Generator,&pCGArray);
		if( pDesignObj_Generator && pos!=string::npos && sValue.size()>pos+1 )
		{
			char cValue = sValue[pos+1];
			switch(cValue)
			{
			case 'x':
				return StringUtils::itos(pDesignObj_Generator->m_rPosition.X);
			case 'y':
				return StringUtils::itos(pDesignObj_Generator->m_rPosition.Y);
			case 'w':
				return StringUtils::itos(pDesignObj_Generator->m_rPosition.Width);
			case 'h':
				return StringUtils::itos(pDesignObj_Generator->m_rPosition.Height);
			case 'r':
				return StringUtils::itos(pDesignObj_Generator->m_rPosition.Right());
			case 'b':
				return StringUtils::itos(pDesignObj_Generator->m_rPosition.Bottom());
			case 'p':
				if( sValue.size()>pos+2 )
				{
					int PK_DesignObjParameter = atoi( sValue.substr(pos+2).c_str() );
					return pDesignObj_Generator->GetParm( PK_DesignObjParameter );
				}
			}
		}
		if( pCGArray && pos!=string::npos && sValue.size()>pos+1 )
		{
			char cValue = sValue[pos+1];
			switch(cValue)
			{
			case 'x':
				return StringUtils::itos(pCGArray->m_rBounds.X);
			case 'y':
				return StringUtils::itos(pCGArray->m_rBounds.Y);
			case 'w':
				return StringUtils::itos(pCGArray->m_rBounds.Width);
			case 'h':
				return StringUtils::itos(pCGArray->m_rBounds.Height);
			case 'r':
				return StringUtils::itos(pCGArray->m_rBounds.Right());
			case 'b':
				return StringUtils::itos(pCGArray->m_rBounds.Bottom());
			}
		}
	}

	return "";
}

void DesignObj_Generator::ProcessAddition(char cValue,int iValue,bool bAdd)
{
	switch(cValue)
	{
	case 'x':
		m_rPosition.X += iValue * (bAdd ? 1 : -1);
		return;
	case 'y':
		m_rPosition.Y += iValue * (bAdd ? 1 : -1);
		return;
	case 'w':
		m_rPosition.Width += iValue * (bAdd ? 1 : -1);
		return;
	case 'h':
		m_rPosition.Height += iValue * (bAdd ? 1 : -1);
		return;
	case 'r':
		m_rPosition.Right(  m_rPosition.Right() + (iValue * (bAdd ? 1 : -1)) );
		return;
	case 'b':
		m_rPosition.Bottom(  m_rPosition.Bottom() + (iValue * (bAdd ? 1 : -1)) );
		return;
	}
}

void DesignObj_Generator::ProcessAssignment(string sValue,string sValueToAssign)
{
	if( sValue.size()<1 )
		return;
	char cValue = sValue[0];
	switch(cValue)
	{
	case 'x':
		m_rPosition.X = atoi(sValueToAssign.c_str());
		return;
	case 'y':
		m_rPosition.Y = atoi(sValueToAssign.c_str());
		return;
	case 'w':
		m_rPosition.Width = atoi(sValueToAssign.c_str());
		return;
	case 'h':
		m_rPosition.Height = atoi(sValueToAssign.c_str());
		return;
	case 'r':
		m_rPosition.Right( atoi(sValueToAssign.c_str()) );
		return;
	case 'b':
		m_rPosition.Bottom( atoi(sValueToAssign.c_str()) );
		return;
	case 'p':
		if( sValue.size()>1 )
		{
			int PK_DesignObjParameter = atoi( sValue.substr(1).c_str() );
			m_mapParameterOverides[ PK_DesignObjParameter ] = sValueToAssign;
		}
		return;
	}
}

void DesignObj_Generator::FindChild(int PK_DesignObj,DesignObj_Generator **ppDesignObj_Generator,CGArray **ppCGArray)
{
	for(vector<class DesignObj_Generator *>::iterator it=m_alChildDesignObjs.begin();it!=m_alChildDesignObjs.end();++it)
	{
		DesignObj_Generator *pDesignObj_Generator = *it;
		if( pDesignObj_Generator->m_pRow_DesignObj->PK_DesignObj_get()==PK_DesignObj )
		{
			*ppDesignObj_Generator = pDesignObj_Generator;
			return;
		}
		else
		{
			 pDesignObj_Generator->FindChild(PK_DesignObj,ppDesignObj_Generator,ppCGArray);
			 if( *ppDesignObj_Generator || *ppCGArray )
				 return;
		}
	}

	for(vector<class CGArray *>::iterator it=m_alNonMPArrays.begin();it!=m_alNonMPArrays.end();++it)
	{
		CGArray *pCGArray = *it;
		if( pCGArray->m_pRow_DesignObjVariation_DesignObj_Skin_Language->FK_DesignObjVariation_DesignObj_getrow()->FK_DesignObj_Child_get()==PK_DesignObj )
		{
			*ppCGArray=pCGArray;
			return;
		}
	}
	for(vector<class CGArray *>::iterator it=m_alMPArray.begin();it!=m_alMPArray.end();++it)
	{
		CGArray *pCGArray = *it;
		if( pCGArray->m_pRow_DesignObjVariation_DesignObj_Skin_Language->FK_DesignObjVariation_DesignObj_getrow()->FK_DesignObj_Child_get()==PK_DesignObj )
		{
			*ppCGArray=pCGArray;
			return;
		}
	}
}
