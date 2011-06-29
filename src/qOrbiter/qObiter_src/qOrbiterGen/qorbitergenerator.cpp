#include "qorbitergenerator.h"
#include <QDebug>
#include <QTime>

#include <iostream>
#include <fstream>

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "qorbitermanager.h"




bool qOrbiterGenerator::initializeRegen()
{

    if (connectDB())
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"qOrbiter Generator Version .000001 Starting. Yes, that new");
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Generating: #%d %s",m_pRow_Device->PK_Device_get(),m_pRow_Device->Description_get().c_str());
    if (get_users())
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Regen Success for qOrbiter %d, starting!", m_iPK_Orbiter);
            return true;
    }

    }
    else
    {
        return false;
    }


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
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Connected to Pluto Main Database");

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
    m_pRow_Orbiter = m_spDatabase_pluto_main->Orbiter_get()->GetRow(m_iPK_Orbiter);

    return true;
}

int qOrbiterGenerator::get_users()
{
    Row_Users *drUsers_Default = NULL;
    Row_Device_DeviceData *drD_C_DP_DefaultUser = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Orbiter->PK_Orbiter_get(),DEVICEDATA_PK_Users_CONST);


    if( drD_C_DP_DefaultUser )
    {
            Row_Installation_Users *pRow_Installation_Users=m_spDatabase_pluto_main->Installation_Users_get()->GetRow(m_pRow_Device->FK_Installation_get(),atoi(drD_C_DP_DefaultUser->IK_DeviceData_get().c_str()));
            if( pRow_Installation_Users )
                    drUsers_Default = pRow_Installation_Users->FK_Users_getrow();
    }

    if( !drUsers_Default )
    {
            vector<Row_Installation_Users *> vectRow_Installation_Users;
            m_spDatabase_pluto_main->Installation_Users_get()->GetRows(INSTALLATION_USERS_FK_INSTALLATION_FIELD "=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()),&vectRow_Installation_Users);
            if( vectRow_Installation_Users.size() )
            {
                    LoggerWrapper::GetInstance()->Write(LV_WARNING,"***Warning*** No default user specified.  Picking first one: %d",vectRow_Installation_Users[0]->FK_Users_get());
                    drUsers_Default = vectRow_Installation_Users[0]->FK_Users_getrow();
                    if( !drUsers_Default )
                            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database problem -- There is a user in Installation_Users that is not a valid user");
            }
            else
            {
                    vector<Row_Users *> vectRow_Users;
                    m_spDatabase_pluto_main->Users_get()->GetRows("1=1",&vectRow_Users); // Just find any user
                    if( vectRow_Users.size()>0 )
                    {
                            LoggerWrapper::GetInstance()->Write(LV_WARNING,"***Warning*** No users at all in this installation.  Picking one");
                            drUsers_Default = vectRow_Users[0];
                    }
                    else
                    {
                            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"***Error*** The users table is completely empty.");
                    }
            }
    }

    if( drUsers_Default )
            m_dwPK_Users_Default = drUsers_Default->PK_Users_get();



    return true;

}
