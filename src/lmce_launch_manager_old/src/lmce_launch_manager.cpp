#include <kmainwindow.h>
#include <klocale.h>

#include <qtimer.h>

#include "lmce_launch_manager.h"
#include "lmce_launch_managerwidget.h"

lmce_launch_manager::lmce_launch_manager()
    : KMainWindow( 0, "lmce_launch_manager" )
{
	pWidget = new lmce_launch_managerWidget( this );
	setCentralWidget(pWidget);
}

lmce_launch_manager::~lmce_launch_manager()
{
	pWidget = NULL;
}

void lmce_launch_manager::initializeLMCE()
{
	if (pWidget)
		pWidget->initialize();
}

void lmce_launch_manager::scheduleAutostart()
{
	// scheduling autostart
	QTimer::singleShot( 1000, pWidget, SLOT(doAutostart()) );
}

void lmce_launch_manager::closeEvent ( QCloseEvent * e )
{
	if ( pWidget ) 
	{
		if ( pWidget->canClose() )
			e->accept();
		else
			e->ignore();
	}
	else
		e->accept();
	
}

#include "lmce_launch_manager.moc"
