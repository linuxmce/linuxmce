
#include "stdafx.h"
#include "LibraryManager.h"
#include "Log.h"

using namespace std;
using namespace PlutoITunesMediaConnector::Broadcaster;


LibraryManager::LibraryManager(void) {
	REGISTER_RUNTIME_POINT("LibraryManager::LibraryManager", __FILE__, __LINE__);
}

LibraryManager::~LibraryManager(void) {
	REGISTER_RUNTIME_POINT("LibraryManager::~LibraryManager", __FILE__, __LINE__);
}
