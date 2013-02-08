
//using namespace std;
//#include "LMCE_Launch_Manager.h"
#include "LM.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCERouter.h"

// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
#define  VERSION "<=version=>"
const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/


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
