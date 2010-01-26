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

#include "thread.h"

// Undeclared functions, only for use in this file to start the thread
void threadInternalStart(void *arg)
{
  // I don't want signals
  sigset_t sigs;
  sigfillset(&sigs);
  pthread_sigmask(SIG_BLOCK, &sigs, NULL);

  Thread *t = (Thread *)arg;
  t->threadInternalStart2();
}

void Thread::threadInternalStart2()
{
  threadMethod();
  this->threadPostStopCleanup();
}

Thread::Thread()
{
  threadActive = 0;
}

int Thread::threadStart()
{
  pthread_cond_init(&threadCond, NULL);
  pthread_mutex_init(&threadCondMutex, NULL);

  threadActive = 1;
  if (pthread_create(&pthread, NULL, (void*(*)(void*))threadInternalStart, (void *)this) == -1) return 0;
  return 1;
}

void Thread::threadStop()
{
  threadActive = 0;
  // Signal thread here in case it's waiting
  threadSignal();
  pthread_join(pthread, NULL);
}

void Thread::threadCancel()
{
  threadActive = 0;
  pthread_cancel(pthread);
  pthread_join(pthread, NULL);
  this->threadPostStopCleanup(); // thread was cancelled, did not run post-stop above
}

void Thread::threadCheckExit()
{
  if (!threadActive) pthread_exit(NULL);
}

char Thread::threadIsActive()
{
  return threadActive;
}

void Thread::threadSignal()
{
  pthread_mutex_lock(&threadCondMutex);
  pthread_cond_signal(&threadCond);
  pthread_mutex_unlock(&threadCondMutex);
}

void Thread::threadSignalNoLock()
{
  pthread_cond_signal(&threadCond);
}

void Thread::threadWaitForSignal()
{
  pthread_cond_wait(&threadCond, &threadCondMutex);
}

void Thread::threadDetach()
{
  pthread_detach(pthread);
}

void Thread::threadLock()
{
  pthread_mutex_lock(&threadCondMutex);
}

void Thread::threadUnlock()
{
  pthread_mutex_unlock(&threadCondMutex);
}


