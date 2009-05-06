

#include "lmce_launch_manager.h"
#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

//<-mkr_B_etch_b->
#define USE_UNIQUE_APP
//<-mkr_B_etch_e->

static const char description[] =
    I18N_NOOP("A KDE KPart Application");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

void run_as_root(char **argv) {
	if (getuid() == 0 ) {
		return;
	}

	printf("Try to gain root ...");
	execl("/usr/bin/gksu","gksu", argv[0], NULL);
//	execl("/usr/bin/kdesu","kdesu","-u","root","-c",argv[0],NULL);
//	execl("/bin/su","su","-u","root","-c","./mce-installer", NULL);
	exit(1);
}

int main(int argc, char **argv)
{
	run_as_root(argv);
	KAboutData about("Linux MCE Launch Manager", I18N_NOOP("Linux MCE Launch Manager"), version, description,
                     KAboutData::License_Custom, "(C) 2007 Paul Webber", 0, 0, "webpaul01@gmail.com");
    about.addAuthor( "Paul Webber", 0, "webpaul01@gmail.com" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );

#ifdef USE_UNIQUE_APP
    KUniqueApplication::addCmdLineOptions();

    if (!KUniqueApplication::start()) {
	    fprintf(stderr, "lmce_launch_manager is already running!\n");
	    exit(0);
    }

    KUniqueApplication app;
#else
    KApplication app;
#endif

    lmce_launch_manager *mainWin = 0;

    if (app.isRestored())
    {
        RESTORE(lmce_launch_manager);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        /// @todo do something with the command line args here

        mainWin = new lmce_launch_manager();
        app.setMainWidget( mainWin );
        mainWin->show();
	// initialize LMCE stuff
	mainWin->initializeLMCE();	
	mainWin->scheduleAutostart();

        args->clear();
    }

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

