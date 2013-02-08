#ifndef LIRCD_THREAD_H
#define LIRCD_THREAD_H

#include "Hulu_Player.h"

using namespace DCE;

/* Forward declaration of c object for pthread */
void * LIRCD_Thread(void * arg);

/* The LIRCD Methods to be called from main thread */
bool LIRCD_SendCommand(string Command);
bool LIRCD_Close();

/* The C forward declaration for the main thread. */

extern bool LIRCD_bQuit;		// Do we need to quit the thread?
extern bool LIRCD_bConnectionActive;	// Is a connection active?

#endif /* LIRCD_THREAD_H */
