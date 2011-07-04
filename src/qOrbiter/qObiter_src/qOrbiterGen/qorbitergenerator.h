#ifndef QORBITERGENERATOR_H
#define QORBITERGENERATOR_H

#include <stdlib.h>
#include <auto_ptr.h>

#include <QWidget>
#include <QTimer>
#include <QDeclarativeView>
#include <qOrbiterData.h>
#include <QDebug>
#include <datamodels/usermodel.h>
#include <datamodels/lightingscenariomodel.h>
#include <datamodels/locationmodel.h>


#include "PlutoUtils/DatabaseUtils.h"
#include "DCE/DCEConfig.h"


#include "pluto_main/Database_pluto_main.h"
#include "pluto_media/Database_pluto_media.h"
#include "PlutoUtils/DBHelper.h"
#include <Logger.h>

/*
  this class is for the purposes of generating a blob of data that can be utilized by the qt based
  orbiter and qml. It is a temporary measure to get us to a basic qOrbiter implementation to look further
  at this and other parts of the system that need to be updated. It is based on orbitergen

  this header file is a mess! beware!
  */
class qOrbiterGenerator : public QWidget, public DBHelper ,public qOrbiterData
{
Q_OBJECT
public:
    /* databse variables */
    string m_sDBHost,m_sDBUser,m_sDBPass,m_sDBDBName;
    int m_iDBPort;
    std::auto_ptr<Database_pluto_main> m_spDatabase_pluto_main;
    std::auto_ptr<Database_pluto_media> m_spDatabase_pluto_media;

   //class Database_pluto_main *m_spDatabase_pluto_main;
 /* Database variables, representative of rows with accompanying defines in pluto_main/ */
   class Row_Orbiter * m_pRow_Orbiter;
   class Row_Device * m_pRow_Device;
   class Row_Room * m_pRow_Room;
   class Row_EntertainArea * m_pRow_EntertainArea;
   class Row_Screen *m_pRow_Screen_MainMenu,*m_pRow_Screen_Sleeping,*m_pRow_Screen_ScreenSaver;
   class Row_DesignObj *m_pRow_DesignObj_MainMenu,*m_pRow_DesignObj_Sleeping,*m_pRow_DesignObj_ScreenSaver;
   class Row_Skin *m_pRow_Skin;
   class Row_Language *m_pRow_Language;
   class Row_Users * userList;
   class RegenMonitor *m_pRegenMonitor;
   class Row_CommandGroup_Room *m_pRowCommandGroup_Room;


    /* Orbiter Specific Variables */

    bool m_bNoEffects,m_bUseOCG;
    bool m_bIgnoreSelected, m_bIgnoreHighlighted, m_bIgnoreAlt;
    int m_dwMediaType;
    int m_iLocation;
    bool m_bOrbiterChanged,m_bIsMobilePhone;
    int m_iPK_Orbiter;
    int m_iPK_CommandGroup; // If this is an array and we are supposed to execute an action group, this will be non-zero.  Arrays are always customer created command groups
     string m_sDesc_Array;

    /* FloorPlan Variables */
    int m_iFloorplanPage;
    string m_sFloorPlanData;
    int m_iNumFloorplanItems;

    /* design obj related */
    map<int,int> m_htUsedStyleVariation;
    map<int,int> m_mapUsedOrbiterCriteria;
    map<int,int> m_mapSkinTranslated;
    //map<string,listDesignObj_Generator *> m_htGeneratedScreens;
    map<int,class TextStyle *> m_htStylesUsedInOrbiterText;
    map<int,bool> m_mapDesignObj_WithArrays;
    map<int,bool> m_mapPopups;  
    string m_iID_Array;
    string m_GraphicsBasePath;
    string m_sSize_Regen_Data;
    int m_iPK_DesignObj_Screen;    
    map<int,bool> m_map_PK_DesignObj_SoleScreenToGen; // Only re-generate this one screen
    map<int, int> m_mapEffects; //The map with effects    
    int m_iScreensTotal,m_iScreensToRender,m_iLastReportedPercentage;  // Keep track of screens we're going to need to render
    //PlutoSize m_sScale,m_sScale_Orig;  // orig is the value without reduction for the borders in case the object has DESIGNOBJPARAMETER_Dont_Scale_CONST==1
    bool m_bDontAutoRegenArrays;    
    class Row_Icon * m_pRow_Icon;
    PlutoSize *m_sizeScreen;
    class Row_Size * m_pRow_Size;
    class Row_UI * m_pRow_UI;
     string m_sOutputPath,m_sGraphicsBasePath,m_sFontPath;

     //qt stuffs
     QDeclarativeView *splashScreen;
     //device variables
     int m_iDevice;
     int m_iRoom;
     int m_iUser;

     QString *tempstring;
     QMap <QString, int> devices;
     QMap <QString, int> eaList;
     QMap <QString, int> roomList;
     QMap <int, int> restrictedUser;


    qOrbiterGenerator( int PK_Orbiter,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort, QWidget *parent=0)
{
        m_sDBHost=DBHost;
        m_sDBUser=DBUser;
        m_sDBPass=DBPassword;
        m_sDBDBName=DBName;
        m_iDBPort=DBPort;

      m_spDatabase_pluto_main.reset(new Database_pluto_main(LoggerWrapper::GetInstance()));
        m_spDatabase_pluto_media.reset(new Database_pluto_media(LoggerWrapper::GetInstance()));

        m_bOrbiterChanged=false;
        //m_sGraphicsBasePath=sGraphicsFiles;

        m_iPK_Orbiter=PK_Orbiter;
        qDebug() << "Orbiter:" << m_iPK_Orbiter;
        m_pRegenMonitor = NULL;
/*		m_sMySQLHost=DBHost;
        m_sMySQLUser=DBUser;
        m_sMySQLPass=DBPassword;
        m_sMySQLDBName=DBName;
        m_iMySQLPort=DBPort;
*/

        m_sFloorPlanData="";
        m_iNumFloorplanItems=0;
        Counter=0;
        m_dwMediaType=0;
        m_iID_Array="0";
        m_bDontAutoRegenArrays=false;
        m_iPK_DesignObj_Screen=0;
        m_iPK_CommandGroup=0; // If this is an array and we are supposed to execute an action group, this will be non-zero
        m_iFloorplanPage=0;
        m_iScreensToRender=m_iScreensTotal=m_iLastReportedPercentage=0;
        m_pRow_Icon=NULL;

m_pRow_Orbiter = NULL;
}


signals:
    void regenFinished(int device);

public slots:
    bool initializeRegen();                 //manages connection to database
    UserModel* get_users();                 //returns custom list model of users
    LocationModel* get_locations(QString defEA);  //returns locations in hash map
    bool get_virtual_devices();             //returns virtual devices (plugins)
    QMap <int , LightingScenarioModel*> get_lighting_scenarios();          //lighting scenarios


private:
void get_entertain_areas();
bool CommonControlledVia(Row_Device *pRow_Device1,Row_Device *pRow_Device2);


    map<int,string> htDevicePages;
    long Counter;

    /* designer variables  */
    void OutputScreen(class DesignObj_Generator *ocDesignObj);
    void OutputDesignObjs(class DesignObj_Generator *ocDesignObj,int ArrayPage,bool bIsChild,string ParentScreen);
    void OutputText(class DesignObj_Generator *ocDesignObj,class CGText *ot,bool bPrerender,string ParentScreen);
    void OutputCriteriaNest(class Row_CriteriaParmNesting *row);
    void SearchForGotos(class DesignObj_Data *pDesignObj_Data);
    //void SearchForGotos(class DesignObj_Data *pDesignObj_Data,DesignObjCommandList *alCommands);
    void MatchChildDevicesToLocation(qLocationInfo *li,Row_Device *pRow_Device,bool bStartWithTopMost=true);
    void FixupSpecialOrbiters(list<qLocationInfo *> &listLocationInfo);
    bool connectDB();

    class Row_Size *TranslateSize(string sSize);    
    string First2Dots(string sDesignObj);
    class Row_Skin *TranslateSkin(Row_Skin *pRow_Skin);
    //void PopulateScreenMap();

    //this will map each effect type to an effect based on the skin
   // void PopulateEffects(map<int, int> &mapEffects, int FK_Skin);

};

#endif // QORBITERGENERATOR_H
