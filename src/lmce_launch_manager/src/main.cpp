

#include "lmce_launch_manager.h"


//<-mkr_B_etch_b->
#define USE_UNIQUE_APP
//<-mkr_B_etch_e->

static const char version[] = "0.2";

void run_as_root(char **argv) {
	if (getuid() == 0 ) {
		return;
	}

	printf("Try to gain root ...");
	execl("/usr/bin/gksu","gksu", argv[0], NULL);
	exit(1);
}

int main(int argc, char **argv)
{
	run_as_root(argv);
	

      	?initializeLMCE();	
	?scheduleAutostart();

        args->clear();


  
}

