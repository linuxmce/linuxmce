// Here we avoid loading the header multiple times
#ifndef Activateorbiter_HEADER
#define Activateorbiter_HEADER
// We need the Plasma Applet headers
#include <KIcon>
 
#include <Plasma/Applet>
#include <Plasma/Svg>
#include "plasma/widgets/pushbutton.h"
#include "plasma/widgets/icon.h" 
#include "DB.h"
// Defines
#define PLUTO_CONFIG "/etc/pluto.conf"

// Alarm Callbacks
#define AO_KEEP_ALIVE 1

using namespace std;

class QSizeF;
 
// Define our plasma Applet
class PlasmaActivateorbiter : public Plasma::Applet
{
    Q_OBJECT
    public:
        // Basic Create/Destroy
        PlasmaActivateorbiter(QObject *parent, const QVariantList &args);
        ~PlasmaActivateorbiter();
 
        // The paintInterface procedure paints the applet to screen
        void paintInterface(QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);
	void init();

	bool AOinitialize();
	bool AOdeinitialize();
	void AODeviceKeepAlive();
	void reportDeviceUp();
	void reportDeviceDown();
	void AlarmCallback(int id, void* param);
	void jumpToOrbiter();
	bool openDB();
	bool closeDB();
	void AOReadConfig();

	string m_sDeviceID, m_sCoreDeviceID, m_sOrbiterID, m_sMDID, m_sCoreIP, m_sMySQLHost, m_sMySQLUser, m_sMySQLPass, m_sMySQLPort, m_sDCERouterPort;

        DB m_dbPlutoDatabase;
	MYSQL m_mysqlMysql;
	MYSQL_RES *m_pResult;
	MYSQL_ROW m_mysqlRow;

    private:
        Plasma::PushButton m_pushButton;
	Plasma::Icon *m_icon;

    private Q_SLOTS:
	void jumpToOrbiter(bool pressed);
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(activateorbiter, PlasmaActivateorbiter)
#endif
