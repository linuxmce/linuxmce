#include "../../LCDLogic/MenuLoader.h"
#include "../../LCDLogic/MenuHolder.h"
#include "DCE/Logger.h"

int main( int argc, char* argv[] )
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Usage: MenuLoader_TestUnit <menu file>\n");

	MenuLoader menu_loader;
	string sMenuFilename = "../../Docs/menu-sample.xml";

	if(argc >= 2)
		sMenuFilename = argv[1];

	MenuHolder *pMenuHolder = menu_loader.Load(sMenuFilename);
	delete pMenuHolder;
	
	return 0;
}
