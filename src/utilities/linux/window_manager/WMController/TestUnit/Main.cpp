#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "../WMController.h"

#include <string>
#include <vector>
using namespace std;

void Tokenize(string &Input, string Tokens, vector<string> &vect_strings);

int main(int argc, char *argv[])
{
 	int opt;


	string sWindow; //-r win : window id
	bool bListWindows = false;	//-l : list windows
	bool bActivateWindow = false; //-a : activate window
	string sPosition; //-p x,y,h,w : position
	string sVisible; //-s 1 or 0 : show/hide
	string sVerbose;//-v 1 or 0 : verbose
	string sFullScreen;	 //-f 1 or 0 : fullscreen?
    string sMaximized; // -m 1 or 0 : maximized
    string sLayer; //-z above|below|normal : layer

    while ((opt = getopt(argc, argv, "alr:p:s:z:f:v:m:")) != -1)
	{
        switch (opt)
		{
            case 'r':
				sWindow = optarg;
                break;
            case 'p':
				sPosition = optarg;
                break;
			case 'l':
				bListWindows = true;
				break;
			case 'a':
				bActivateWindow = true;
				break;
			case 's':
				sVisible = optarg;
				break;
            case 'm':
				sMaximized = optarg;
				break;
            case 'v':
				sVerbose = optarg;
				break;
			case 'z':
				sLayer = optarg;
				break;
			case 'f':
				sFullScreen = optarg;
				break;

			default:
				printf("Unknown option\n\n");
				printf("Usage: plutowmctrl <options>\n"
					"\nOptions: \n"
					"\t-l\t\t\t list all windows]\n"
					"\t-r\t\t\t specify the window class name\n"
					"\t-p x,y,h,w\t\t the position\n"
					"\t-s 0|1\t\t\t show|hide\n"
					"\t-v 0|1\t\t\t verbose\n"
					"\t-m 0|1\t\t\t maximize\n"
					"\t-a \t\t\t activate window \n"
					"\t-f 0|1\t\t\t fullscreen or not\n"
					"\t-z a[bove]|n[ormal]|b[elow]\t the layer (z order)\n"
				);
				return -1;
		}
	}

    WMControllerImpl *pWMController = new WMControllerImpl();

	if(bListWindows)
    {
        list<WinInfo> dummy;
		pWMController->ListWindows(dummy);
    }

	if(sWindow != "")
	{
		if(bActivateWindow)
		{
			pWMController->ActivateWindow(sWindow, true);
		}

		if(sPosition != "")
		{
			vector<string> vectItems;
			Tokenize(sPosition, ",", vectItems);
			if(vectItems.size() == 4)
			{
				pWMController->SetPosition(sWindow, atoi(vectItems[0].c_str()),
					atoi(vectItems[1].c_str()), atoi(vectItems[2].c_str()), atoi(vectItems[3].c_str()), true);
			}
			else
				printf("ERROR: You need 4 params for position!\n");
		}

		if(sVisible == "1" || sVisible == "0")
		{
			pWMController->SetVisible(sWindow, sVisible == "1", true);
		}

		if(sMaximized == "1" || sMaximized == "0")
		{
			pWMController->SetMaximized(sWindow, sMaximized == "1", true);
		}

		if(sFullScreen == "1" || sFullScreen == "0")
		{
			pWMController->SetFullScreen(sWindow, sFullScreen == "1", true);
		}

		if(sLayer != "")
		{
			pWMController->SetLayer(
                sWindow,
                ( sLayer == "above" || sLayer == "a" ) ? LayerAbove :
                ( sLayer == "below" || sLayer == "b" ) ? LayerBelow :
                ( sLayer == "normal" || sLayer == "n" ) ? LayerNormal :
                LayerUnknown,
				true
                );
		}
	}
    delete pWMController;
    pWMController = NULL;

	return 0;
}

void Tokenize(string &Input, string Tokens, vector<string> &vect_strings)
{
	const char *pTokens = Tokens.c_str();
	int Size = int(Tokens.size());
	if( Size<1 || Size>3 )
		return; // Not supported no tokens or more than 3

	const char *pcPtr = Input.c_str();
	char *pPtr_dup = strdup(pcPtr);
	char *pPtr = pPtr_dup;
	const char *pPtr_Start = pPtr;

	while( *pPtr )
    {
		while( *pPtr && *pPtr!=pTokens[0]
			&& (Size<2 || *pPtr!=pTokens[1])
			&& (Size<3 || *pPtr!=pTokens[2]) )
				pPtr++;

		// We're stopped on a token, terminate this string and skip over any more tokens
		while( *pPtr &&
			( *pPtr==pTokens[0] ||
			  (Size>1 && *pPtr==pTokens[1]) ||
			  (Size>2 && *pPtr==pTokens[2]) ) )
		{
			(*pPtr)=0;
			pPtr++;
		}

		if( *pPtr_Start && pPtr_Start!=pPtr )
            vect_strings.push_back(pPtr_Start);
		pPtr_Start=pPtr;
    }
	free(pPtr_dup);
}
