

#ifndef _LMCE_LAUNCH_MANAGER_H_
#define _LMCE_LAUNCH_MANAGER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>

/**
 * @short Application Main Window
 * @author Paul Webber <webpaul01@gmail.com>
 * @version 0.1
 */

struct lmce_launch_managerWidget;

class lmce_launch_manager : public KMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    lmce_launch_manager();

    /**
     * Default Destructor
     */
    virtual ~lmce_launch_manager();

    // initialize data
    void initializeLMCE();
    
    void scheduleAutostart();
    
private:
		lmce_launch_managerWidget *pWidget;

protected:
		void closeEvent ( QCloseEvent * e );

};

#endif // _LMCE_LAUNCH_MANAGER_H_
