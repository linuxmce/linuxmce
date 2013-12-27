
//using namespace std;
//#include "LMCE_Launch_Manager.h"
#include "LM.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCERouter.h"

#include "version.h"

//using namespace LM;


int main(int argc, char* argv[]) 
{
	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);

	cout << "LMCE_Launch_Manager, v." << VERSION << endl
		<< "Visit www.plutohome.com for source code and license information" << endl << endl;

	
	LM LM;
	LM.Initialize();
	LM.Run();
	//LM.Draw();
	//cout << "ENDING" << endl;
		return 0;
}
//<-dceag-main-e->
