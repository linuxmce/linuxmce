#include <cstdlib>
#include <iostream>

#include <signal.h>
#include <attr/attributes.h>
#include <stdint.h>

using namespace std;

#include "Controller.h"
#include "ServerSocket.h"
#include "SocketException.h"

#include "Logger.h"

#include "MediaAttributes.h"

#define PORT 3442

#define VERSION "<=VERSION=>"

Controller* g_control = NULL;
MediaAttributes *g_mediaAttributes = NULL;

bool g_done = false;

namespace DCE
{
    Logger *g_pPlutoLogger;
}

static void sigrt_handler(int sig, siginfo_t *si, void *data)
{
    if (g_control && si && si->si_fd)
    {
        g_control->rescanDirectory(si->si_fd);
    }
}

static void sig_handler(int signum)
{
    g_done = true;
}

int main(int argc, char **argv)
{
    cout << "Media info updater daemon, v." << VERSION << endl
         << "Visit www.plutohome.com for source code and license information" << endl << endl;

    string DBHost="dcerouter",DBUser="root",DBPassword="",DBName="pluto_media";
    int DBPort=3306,ListenPort=3450;
    string sLogger="stdout";


    vector<string> vectDirectories;

    bool bError=false; // An error parsing the command line
    char c;
    for(int optnum=1;optnum<argc;++optnum)
    {
        if( argv[optnum][0]!='-' )
        {
            cerr << "Unknown option " << argv[optnum] << endl;
            bError=true;
            break;
        }

        c=argv[optnum][1];
        switch (c)
        {
        case 's':
            while ( argv[++optnum] && argv[optnum][0] != '-')
                vectDirectories.push_back(argv[optnum]);
            break;
        case 'h':
            DBHost = argv[++optnum];
            break;
        case 'u':
            DBUser = argv[++optnum];
            break;
        case 'p':
            DBPassword = argv[++optnum];
            break;
        case 'D':
            DBName = argv[++optnum];
            break;
        case 'P':
            DBPort = atoi(argv[++optnum]);
            break;
        case 'l':
            sLogger = argv[++optnum];
            break;
        default:
            bError = true;
        };
    }

    if (bError)
    {
        cout << "A directory scanner for the pluto system.  See www.plutohome.com for details." << endl
            << "Usage: MediaUpdater [-r Router's IP] [stdout|null|filename] -s <list of folders to scan>" << endl
            << "-h hostname  -- address or DNS of database host, default is `dcerouter`" << endl
            << "-u username  -- username for database connection" << endl
            << "-p password  -- password for database connection, default is `` (empty)" << endl
            << "-D database  -- database name" << endl
            << "-P port      -- port for database connection, default is 3306" << endl
            << "-s <list of directories to watch>" << endl;
            exit(1);
    }

    try
    {
        if( sLogger=="null" )
            LoggerWrapper::SetType(LT_LOGGER_NULL);
        else if( sLogger=="stdout" )
            g_pPlutoLogger = new FileLogger(stdout);
        else
            g_pPlutoLogger = new FileLogger(sLogger.c_str());
    }
    catch(...)
    {
        cerr << "Unable to create logger" << endl;
    }
//
    try
    {
        g_control = new Controller;

        g_mediaAttributes = new MediaAttributes(DBHost, DBUser, DBPassword, DBName, DBPort);

        g_control->setMediaAttributeObject(g_mediaAttributes);

        struct sigaction act;
        ServerSocket server (PORT);

        signal(SIGINT, sig_handler);
        signal(SIGTERM, sig_handler);
        signal(SIGIO, SIG_IGN);

        /* setup signal handler for kernel directory notification */
        act.sa_sigaction = sigrt_handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO;
        sigaction(SIGRTMIN+1, &act, NULL);

        for (int i = 0; i < vectDirectories.size() ; i ++)
        {
           string strDirectory = vectDirectories[i];
           int nSize = strDirectory.size();

           if ( strDirectory[nSize-1] == '/' )
            strDirectory = strDirectory.substr(0, nSize - 1);

            g_control->addDirectory(strDirectory);
        }

        /* daemonize */
        // 19-Mar-04: It doesn't work with the fork anymore.  Removing it and will call medialibd with & - Matt
        //if (fork())
        //  return 0;
        setsid();

        /* process signals until interrupted/terminated */
        while (!g_done)
        {
            if (server.poll() > 0)
            {
                ServerSocket sock;
                server.accept(sock);
                try
                {
                    while (!g_done)
                    {
                        string::size_type pos;
                        string data;
                        uint64_t key;

                        if (sock.poll() > 0)
                        {
                            sock >> data;
                            if (data.length() > 3)
                            {
                                string cmd  = data.substr(0,3);
                                string operand = data.substr(4);
                                if (cmd == "bye")
                                {
                                    break;
                                }
                                else if (cmd == "qui")
                                {
                                    g_done = true;
                                }
                                else if (cmd == "get")
                                {
                                    key = (uint64_t) atoll(operand.c_str());
                                    if (key > 0)
                                        sock << g_control->getFile(key);
                                    sock << "\n";
                                }
                                else if (cmd == "set" && (pos = operand.find('=')) != string::npos)
                                {
                                    string filename = operand.substr(0,pos);
                                    string value = operand.substr(pos+1);
                                    key = (uint64_t) atoll(value.c_str());
                                    if (!filename.empty())
                                    {
                                        if (!value.empty())
                                            attr_set(filename.c_str(), "ID", value.c_str(), value.length(), 0);
                                        else
                                            attr_remove(filename.c_str(), "ID", 0);
                                        g_control->updateFileKey(filename, key);
                                    }
                                    sock << "OK\n";
                                }
                            }
                        }
                    }
                }
                catch (SocketException&) {} // connection closed
            }
        }
        delete g_control;
        return 0;
    }
    catch (SocketException &e)
    {
        cout << "Error on socket:" << e.description() << "\nExiting.\n";
    }
    catch (string s)
    {
        cout << "Error: " << s << endl;
        delete g_control;
        return 1;
    }
}
