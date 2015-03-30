#include "CApp.h"

int main(int argc, char * argv[])
{
	bool clock_running;
	pthread_t clock_thread;

	int ret = pthread_create( &clock_thread, NULL, CApp::OnExecute, NULL );
	if( 0 != ret )
	{
		clock_running = false;
		return false;
	}
	clock_running = true;

	if( 0 != pthread_join( clock_thread, NULL ) )
	{
	}

	clock_running = false;


//	CApp theApp;
//	return theApp.OnExecute();
}
