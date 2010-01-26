/*
    Copyright 2004-2008 Chris Tallon

    This file is part of VOMP.

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <signal.h>

class Thread
{
  protected:
    // Override this method in derived classes
    virtual void threadMethod()=0;
    virtual void threadPostStopCleanup() {};

    // Methods to use from outside the thread
    int threadStart();    // start the thread. threadMethod() will be called in derived class
    void threadStop();    // stop the thread nicely. thread will be stopped when it next calls threadCheckExit()
    void threadCancel();  // stop thread immediately. thread will be stopped at the next cancellation point
    void threadSignal();  // releases a thread that has called threadWaitForSignal
    void threadSignalNoLock();  // same as above but without locking guarantees. probably not a good idea.
    char threadIsActive(); // returns 1 if thread has been started but not stop() or cancel() 'd

    // Methods to use from inside the thread
    void threadCheckExit();      // terminates thread if threadStop() has been called
    void threadWaitForSignal();  // pauses thread until threadSignal() is called
    void threadDetach();         // Detaches the thread
    void threadLock();           // locks the mutex used for internal cond/signal stuff
    void threadUnlock();         // unlocks.

    // Internal bits and pieces

  private:
    char threadActive;
    pthread_t pthread;
    pthread_cond_t threadCond;
    pthread_mutex_t threadCondMutex;

  public:
    Thread();
    virtual ~Thread() {};
    void threadInternalStart2();
};

#endif
