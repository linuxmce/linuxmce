#include "plasma-activateorbiter.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QStyleOptionGraphicsItem>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsLinearLayout>

#include <plasma/svg.h>
#include <plasma/widgets/pushbutton.h>
#include <plasma/widgets/icon.h>
#include <plasma/theme.h>
#include <plasma/containment.h>

#include "DCE/DCEConfig.h"
#include "DCE/Logger.h"
#include "DCE/ClientSocket.h"
#include "pluto_main/Define_DeviceData.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "DB.h"

PlasmaActivateorbiter::PlasmaActivateorbiter(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);
    resize(200, 200);

    m_icon = new Plasma::Icon (QString(), this);
    m_icon->setFlag(ItemIsMovable, false);

    // DCE bits
    m_pActivate_Orbiter_Plasmoid = NULL;
    m_pAlarmManager = NULL;

}
 
 
PlasmaActivateorbiter::~PlasmaActivateorbiter()
{

   AOdeinitialize();

   delete m_pAlarmManager;
   m_pAlarmManager = NULL;

   closeDB();

}

bool PlasmaActivateorbiter::AOdeinitialize()
{
	if (m_pActivate_Orbiter_Plasmoid)
	{
		m_pActivate_Orbiter_Plasmoid->OnQuit();
		pthread_join(m_pActivate_Orbiter_Plasmoid->m_RequestHandlerThread, NULL);
		delete m_pActivate_Orbiter_Plasmoid;
		m_pActivate_Orbiter_Plasmoid = NULL;
		return true;
	} else 
	{
		return false;
	}

}

void PlasmaActivateorbiter::AODeviceKeepAlive() 
{
	if (m_pActivate_Orbiter_Plasmoid) 
	{
		if (m_pActivate_Orbiter_Plasmoid->m_bTerminate) 
		{
			// Check for a reload.
			AOdeinitialize();
			if (m_pActivate_Orbiter_Plasmoid->m_bReload)
			{
				// We need to reload
				if (!AOinitialize()) 
				{
					// Add a trigger to retry in 5 seconds.
					m_pAlarmManager->CancelAlarmByType(AO_KEEP_ALIVE);
					m_pAlarmManager->AddRelativeAlarm(5,this,AO_KEEP_ALIVE,NULL);
				} else 
				{
					// we're up, let's tell the router we're up.
					reportDeviceUp();
				}
			} else 
			{
				// We DON'T need to reload.
				m_pAlarmManager->CancelAlarmByType(AO_KEEP_ALIVE);
			}

		} else 
		{
			m_pAlarmManager->CancelAlarmByType(AO_KEEP_ALIVE);
			m_pAlarmManager->AddRelativeAlarm(5,this,AO_KEEP_ALIVE,NULL);
		}
	}
}

void PlasmaActivateorbiter::reportDeviceUp()
{
	DCE::Message *pMessage = new DCE::Message(atoi(m_sDeviceID.c_str()), DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_DEVICE_UP, 0);

	if (m_pActivate_Orbiter_Plasmoid) 
	{
		m_pActivate_Orbiter_Plasmoid->QueueMessageToRouter(pMessage);
	}
}

void PlasmaActivateorbiter::reportDeviceDown()
{
	DCE::Message *pMessage = new DCE::Message(atoi(m_sDeviceID.c_str()), DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_DEVICE_DOWN, 0);

	if (m_pActivate_Orbiter_Plasmoid)
	{
		m_pActivate_Orbiter_Plasmoid->QueueMessageToRouter(pMessage);
	}

}


void PlasmaActivateorbiter::AlarmCallback(int id, void* param)
{
	if (id==AO_KEEP_ALIVE) {
		AODeviceKeepAlive();
	}
}

bool PlasmaActivateorbiter::AOinitialize()
{

    if (m_sCoreIP == "localhost")
	    m_sCoreIP = "127.0.0.1";

    m_pActivate_Orbiter_Plasmoid = new DCE::Activate_Orbiter_Plasmoid(atoi(m_sDeviceID.c_str()), m_sCoreIP, true, false);

    bool bConnected = m_pActivate_Orbiter_Plasmoid->GetConfig();

	m_pAlarmManager = new DCE::AlarmManager();
	m_pAlarmManager->Start(2);	// work threads	


    if (bConnected && m_pActivate_Orbiter_Plasmoid->Connect(m_pActivate_Orbiter_Plasmoid->PK_DeviceTemplate_get())) 
    {
	m_pAlarmManager->AddRelativeAlarm(5,this,AO_KEEP_ALIVE,NULL);
    }

}

void PlasmaActivateorbiter::AOReadConfig()
{
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);

        m_sCoreIP = pConfig->m_mapParameters_Find("DCERouter");
        m_sMySQLHost = pConfig->m_mapParameters_Find("MySqlHost");
        m_sMySQLUser = pConfig->m_mapParameters_Find("MySqlUser");
        m_sMySQLPass = pConfig->m_mapParameters_Find("MySqlPassword");
        m_sMySQLPort = pConfig->m_mapParameters_Find("MySqlPort");
        if (m_sMySQLPort=="")
                m_sMySQLPort = "3306";
        m_sDCERouterPort = pConfig->m_mapParameters_Find("DCERouterPort");
        if (m_sDCERouterPort== "")
                m_sDCERouterPort = "3450";


	openDB();

	m_sDeviceID = pConfig->m_mapParameters_Find("PK_Device");

       m_sCoreDeviceID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=7");
        if (m_sCoreDeviceID=="")
        {
                m_sCoreDeviceID = "1";
        }

	string mdID;

       if (m_sDeviceID == m_sCoreDeviceID)
        {
                mdID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + m_sDeviceID + " AND FK_DeviceCategory IN (8)");

                if (mdID != "")
                {
                }
        }
        else
        {
                mdID = m_sDeviceID;
        }

        if (m_sDeviceID!="")
        {
                string localOrbiterID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + mdID + " AND FK_DeviceCategory IN (2, 3, 5)");
                m_sOrbiterID = localOrbiterID;
        }
}

void PlasmaActivateorbiter::init()
{

    AOReadConfig();
    AOinitialize();

	QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	layout->addItem(m_icon);

	m_icon->setIcon(KIcon("/usr/share/icons/ActivateOrbiter.png"));

	setAspectRatioMode(Plasma::ConstrainedSquare);

	connect(m_icon, SIGNAL(pressed(bool)), this, SLOT(jumpToOrbiter(bool)));

} 
 
void PlasmaActivateorbiter::jumpToOrbiter(bool clicked) 
{
	if (clicked)
	{	
		if (m_pActivate_Orbiter_Plasmoid)
		{
			DCE::CMD_Activate_PC_Desktop cmd(atoi(m_sDeviceID.c_str()), atoi(m_sOrbiterID.c_str()), 0);
			m_pActivate_Orbiter_Plasmoid->SendCommandNoResponse(cmd);
		}
	}
}

bool PlasmaActivateorbiter::openDB()
{
        if (m_dbPlutoDatabase.connected())
        {
                closeDB();
        }


        if(m_dbPlutoDatabase.connect(m_sMySQLHost,m_sMySQLUser,m_sMySQLPass,"pluto_main"))
        {
                return true;
        }
        else
        {
                m_dbPlutoDatabase.close();

                return false;
        }
}

bool PlasmaActivateorbiter::closeDB()
{

        if (m_dbPlutoDatabase.connected())
        {
                m_dbPlutoDatabase.close();
                return true;
        }
        else
        {
                return false;
        }
}

void PlasmaActivateorbiter::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::Antialiasing);
 
    // Now we draw the applet, starting with our svg
    // m_pushButton.resize((int)contentsRect.width(), (int)contentsRect.height());
    // m_pushButton.setText("Activate Orbiter");
    // m_pushButton.paint(p, new QStyleOptionGraphicsItem(), 0);
 
    // We place the icon and text
    p->save();
    p->setPen(Qt::white);
    p->restore();
}
 
#include "plasma-activateorbiter.moc"
