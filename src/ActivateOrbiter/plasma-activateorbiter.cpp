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

}
 
 
PlasmaActivateorbiter::~PlasmaActivateorbiter()
{

   AOdeinitialize();

  closeDB();

}

bool PlasmaActivateorbiter::AOdeinitialize()
{

}

void PlasmaActivateorbiter::AODeviceKeepAlive() 
{
}

void PlasmaActivateorbiter::reportDeviceUp()
{
}

void PlasmaActivateorbiter::reportDeviceDown()
{
}


void PlasmaActivateorbiter::AlarmCallback(int id, void* param)
{
}

bool PlasmaActivateorbiter::AOinitialize()
{

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

       if (m_sDeviceID == m_sCoreDeviceID)
        {
                m_sMDID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + m_sDeviceID + " AND FK_DeviceCategory IN (8)");

                if (m_sMDID != "")
                {
                }
        }
        else
        {
                m_sMDID = m_sDeviceID;
        }

        if (m_sDeviceID!="")
        {
                string localOrbiterID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + m_sMDID + " AND FK_DeviceCategory IN (2, 3, 5)");
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
		string sMessageSendCmd = "/usr/pluto/bin/MessageSend localhost 0 "+m_sDeviceID+" 1 912 119 0";
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
