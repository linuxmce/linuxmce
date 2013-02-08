/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

     Author: Nosilla99
     Name  : CM15A
     Descr : Active Home CM15A USB X10 PC Interface and RF transceiver
             DCE Device Handler
     Date  : 11.11.2009
     File  : CM15A/mutex.h


*/

#ifndef CM15AMUTEX_H
#define CM15AMUTEX_H

#include <pthread.h>

namespace CM15ADEV {

    class Mutex{
       public:
          /** Standard constructor */
          Mutex();
          /** Standard destructor */
          ~Mutex();

       public:
          int Lock();
          void Unlock();

          pthread_mutex_t getHandle() const {
		return mutexid;
          }

       public:
	  pthread_mutex_t mutexid;
    };

};

#endif
