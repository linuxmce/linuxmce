#include <QtGui/QApplication>
#include <switchmanager.h>

#include "qmlapplicationviewer.h"


int main(int argc, char *argv[])
{
  string  g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
    string  g_sBinaryPath = FileUtils::BasePath(argv[0]);

    cout << "QML_Light_Switch, v." << "1.1" << endl
            << "Visit www.plutohome.com for source code and license information" << endl << endl;

    string sRouter_IP="dcerouter";
    int PK_Device=0;
    string sLogger="stdout";

    bool bLocalMode=false,bError=false; // An error parsing the command line
    char c;
    for(int optnum=1;optnum<argc;++optnum)
    {
            if( argv[optnum][0]!='-' )
            {
                    cerr << "Unknown option " << argv[optnum] << endl;
                    bError=true;
            }

            c=argv[optnum][1];
            switch (c)
            {
            case 'r':
                    sRouter_IP = argv[++optnum];
                    break;
            case 'd':
                    PK_Device = atoi(argv[++optnum]);
                    break;
    case 'L':
        bLocalMode = true;
        break;
            case 'l':
                    sLogger = argv[++optnum];
                    break;
            default:
                    bError=true;
                    break;
            };
    }

    if (bError)
    {
            cout << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
                    << "Usage: QML_Light_Switch [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]" << endl
                    << "-r -- the IP address of the DCE Router  Defaults to 'dcerouter'." << endl
                    << "-d -- This device's ID number.  If not specified, it will be requested from the router based on our IP address." << endl
                    << "-l -- Where to save the log files.  Specify 'dcerouter' to have the messages logged to the DCE Router.  Defaults to stdout." << endl;
            exit(1);
    }
    QApplication app(argc, argv);


   switchManager *dceLightSwitch = new switchManager(PK_Device);
   dceLightSwitch->initialize_Start();
   dceLightSwitch->initialize_LMdevice(true);
   dceLightSwitch->loadUI();

   dceLightSwitch->show();




    return app.exec();
}

