#include "qorbitergenerator.h"
#include <QDebug>
#include <QTime>

#include <iostream>
#include <fstream>

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Screen.h"
#include "pluto_main/Table_Screen_DesignObj.h"
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_Image.h"
#include "pluto_main/Table_CachedScreens.h"
#include "pluto_main/Table_Criteria.h"

#include "pluto_main/Table_Size.h"
#include "pluto_main/Table_Skin.h"
#include "pluto_main/Table_EffectType_Effect_Skin.h"
#include "pluto_main/Table_Language.h"
#include "pluto_main/Table_UI.h"
#include "pluto_main/Table_Text.h"
#include "pluto_main/Table_Style.h"
#include "pluto_main/Table_Room_Users.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_StyleVariation.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Orbiter_Variable.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_Icon.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_Installation_Users.h"
#include "pluto_main/Table_DesignObjType.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EventHandler.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_media/Table_AttributeType.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "DCE/DCEConfig.h"


bool qOrbiterGenerator::initializeRegen()
{

    if (!connectDB())
    {
    QTime dieTime = QTime::currentTime().addSecs(5);
    while( QTime::currentTime() < dieTime )
        {
        //qDebug() << "Waiting for:" << QTime::currentTime() ; just a timer to test things

        }
    }



    return true;
}

bool qOrbiterGenerator::connectDB()
{
    if( !m_spDatabase_pluto_main->Connect(m_sDBHost,m_sDBUser,m_sDBPass,m_sDBDBName,m_iDBPort) )
    {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to connect to database");
            return false;
    }
    else
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Connected to Database");

    }

    if( !m_spDatabase_pluto_media->Connect(m_sDBHost,m_sDBUser,m_sDBPass,"pluto_media",m_iDBPort) )
    {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to connect to pluto_media database");
            return false;
    }
    else
       {
           LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Connected to Pluto_Media");

       }


    m_pRow_Device = m_spDatabase_pluto_main->Device_get()->GetRow(m_iPK_Orbiter);
    if( !m_pRow_Device )
    {
            LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %d doesn't exist",m_iPK_Orbiter);
            return false;
    }
    else
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device %d Found!", m_iPK_Orbiter);

    }
    return true;
}
