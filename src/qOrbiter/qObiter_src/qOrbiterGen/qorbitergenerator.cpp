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

        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Regen Success for qOrbiter %d, starting!", m_iPK_Orbiter);
            return true;


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

QHash <QString, int> qOrbiterGenerator::get_users()
{
    Row_Users *drUsers_Default = NULL;
    Row_Device_DeviceData *drD_C_DP_DefaultUser = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Orbiter->PK_Orbiter_get(),DEVICEDATA_PK_Users_CONST);

    QHash <QString, int> userHash;
    if( drD_C_DP_DefaultUser )
    {
            Row_Installation_Users *pRow_Installation_Users=m_spDatabase_pluto_main->Installation_Users_get()->GetRow(m_pRow_Device->FK_Installation_get(),atoi(drD_C_DP_DefaultUser->IK_DeviceData_get().c_str()));
            if( pRow_Installation_Users )
                    drUsers_Default = pRow_Installation_Users->FK_Users_getrow();
    }



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


    if( drUsers_Default )
            m_dwPK_Users_Default = drUsers_Default->PK_Users_get();

   return userHash;

}

QHash <QString, int> qOrbiterGenerator::get_locations()
{
    int i=0;
    QHash <QString, int> returnHash;

    /*  This is tricky with Mysql since there are no full joins or sub-queries.  We have to do it in multiple passes
    string sql = string("SELECT PK_Room,Room.Description AS RoomDescription,") \
    +	"PK_Device,X.Description AS DeviceDescription,FK_Icon FROM Room " \
    +	"FULL OUTER JOIN " \
    +	"(SELECT * FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(DeviceTemplate_Entertain_Unit_CONST) + ") AS X " \
    +	"ON FK_Room=PK_Room " \
    +	"WHERE (Room.FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + " Or Room.FK_Installation IS NULL) " \
    +	"AND (X.FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + " Or X.FK_Installation IS NULL)";
    */

    // First put this in a list.  We really need a deque so we can access the members by their numeric
    // position, but we also need to sort it, and deque's don't implement sorting
    list<qLocationInfo *> listLocationInfo;

    // If a User has the flag 'requires pin' set to true, then unless the user is specifically allowed
    // to use this orbiter by having a record in Room_Users with FK_Room=NULL, then this user requires a pin
    vector<Row_Users *> vectRow_Users;
    m_spDatabase_pluto_main->Users_get()->GetRows("LEFT JOIN Room_Users ON FK_Users=PK_Users AND FK_Orbiter=" + StringUtils::itos(m_pRow_Device->PK_Device_get()) + " WHERE RequirePinToSelect=1 AND FK_Users IS NULL",
            &vectRow_Users);
    for(size_t s=0;s<vectRow_Users.size();++s)
            m_vectPK_Users_RequiringPIN.push_back( vectRow_Users[s]->PK_Users_get() );

    vector<Row_Room *> vectRow_Room;
    m_spDatabase_pluto_main->Room_get()->GetRows("HideFromOrbiter=0 AND FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()),&vectRow_Room);
    for(size_t s=0;s<vectRow_Room.size();++s)
    {
            Row_Room *pRow_Room = vectRow_Room[s];
            vector<Row_Room_Users *> vectRow_Room_Users;
            pRow_Room->Room_Users_FK_Room_getrows(&vectRow_Room_Users);

            returnHash.insert(QString::fromStdString(pRow_Room->Description_get()), (int)pRow_Room->PK_Room_get());

            // If there are records in Room_Users for this room, then this room has restrictions.  There must be a
            // record in the database with FK_Orbiter=this to explicitly allow this orbiter, or with
            // FK_Orbiter=NULL to allow all orbiters
            if(vectRow_Room_Users.size())
            {
                    size_t s;
                    for(s=0;s<vectRow_Room_Users.size();++s)
                    {
                            Row_Room_Users *pRow_Room_Users = vectRow_Room_Users[s];
                            if( pRow_Room_Users->FK_Orbiter_isNull() ||
                                            pRow_Room_Users->FK_Orbiter_get() == m_pRow_Device->PK_Device_get() )
                                    break; // this one is good
                    }
                    if( s>=vectRow_Room_Users.size() )
                            continue;  // we didn't find anything matching
            }

            qLocationInfo *li = new qLocationInfo();
            li->Description = pRow_Room->Description_get();
            li->PK_EntertainArea = 0;
            li->PK_Room = pRow_Room->PK_Room_get();
            li->drIcon=NULL;
            if( !pRow_Room->FK_Icon_isNull() )
                    li->drIcon = pRow_Room->FK_Icon_getrow();

            // If there are record in Room_Users for this room, and users are specified, then
            // only those users can access this room
            for(size_t s=0;s<vectRow_Room_Users.size();++s)
            {
                    Row_Room_Users *pRow_Room_Users = vectRow_Room_Users[s];
                    if( !pRow_Room_Users->FK_Users_isNull() )
                            li->m_vectAllowedUsers.push_back(pRow_Room_Users->FK_Users_get());
            }

            listLocationInfo.push_back(li);
    }
return returnHash;

}
