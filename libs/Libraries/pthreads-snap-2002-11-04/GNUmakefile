#
# --------------------------------------------------------------------------
#
#      Pthreads-win32 - POSIX Threads Library for Win32
#      Copyright(C) 1998 John E. Bossom
#      Copyright(C) 1999,2002 Pthreads-win32 contributors
# 
#      Contact Email: rpj@ise.canberra.edu.au
# 
#      The current list of contributors is contained
#      in the file CONTRIBUTORS included with the source
#      code distribution. The list can also be seen at the
#      following World Wide Web location:
#      http://sources.redhat.com/pthreads-win32/contributors.html
# 
#      This library is free software; you can redistribute it and/or
#      modify it under the terms of the GNU Lesser General Public
#      License as published by the Free Software Foundation; either
#      version 2 of the License, or (at your option) any later version.
# 
#      This library is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#      Lesser General Public License for more details.
# 
#      You should have received a copy of the GNU Lesser General Public
#      License along with this library in the file COPYING.LIB;
#      if not, write to the Free Software Foundation, Inc.,
#      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
#

DEVROOT	= C:\PTHREADS

DLLDEST	= $(DEVROOT)\DLL
LIBDEST	= $(DEVROOT)\DLL

#RM	= rm
#MV	= mv
#CP	= cp

RM	= erase
MV	= rename
CP	= copy

CC	= gcc
CXX	= g++

AR	= ar

#OPT	= -g
#OPT	= -O3 -DTEST_ICE
OPT	= -O3 -finline-functions

LFLAGS		= -lwsock32

GC_CFLAGS	= -D__CLEANUP_C
GCE_CFLAGS	= -D__CLEANUP_CXX -x c++ -mthreads

## Mingw32
MAKE		= make
CFLAGS	= $(OPT) -I. -D_WIN32_WINNT=0x400 -DHAVE_CONFIG_H -Wall

## Cygwin G++
#CFLAGS	= $(OPT) -x $(GLANG) -fhandle-exceptions -D_WIN32_WINNT=0x400 -I. -DHAVE_CONFIG_H -Wall

DLL_INLINED_OBJS	= \
		pthread.o

# Agregate modules for inlinability
DLL_OBJS	= \
		attr.o \
		barrier.o \
		cancel.o \
		cleanup.o \
		condvar.o \
		create.o \
		dll.o \
		errno.o \
		exit.o \
		fork.o \
		global.o \
		misc.o \
		mutex.o \
		nonportable.o \
		private.o \
		rwlock.o \
		sched.o \
		semaphore.o \
		signal.o \
		spin.o \
		sync.o \
		tsd.o

# Separate modules for minimum size statically linked images
SMALL_STATIC_OBJS	= \
		pthread_attr_init.o \
		pthread_attr_destroy.o \
		pthread_attr_getdetachstate.o \
		pthread_attr_setdetachstate.o \
		pthread_attr_getstackaddr.o \
		pthread_attr_setstackaddr.o \
		pthread_attr_getstacksize.o \
		pthread_attr_setstacksize.o \
		pthread_attr_getscope.o \
		pthread_attr_setscope.o \
		pthread_attr_setschedpolicy.o \
		pthread_attr_getschedpolicy.o \
		pthread_attr_setschedparam.o \
		pthread_attr_getschedparam.o \
		pthread_attr_setinheritsched.o \
		pthread_attr_getinheritsched.o \
		pthread_barrier_init.o \
		pthread_barrier_destroy.o \
		pthread_barrier_wait.o \
		pthread_barrierattr_init.o \
		pthread_barrierattr_destroy.o \
		pthread_barrierattr_setpshared.o \
		pthread_barrierattr_getpshared.o \
		pthread_setcancelstate.o \
		pthread_setcanceltype.o \
		pthread_testcancel.o \
		pthread_cancel.o \
		cleanup.o \
		pthread_condattr_destroy.o \
		pthread_condattr_getpshared.o \
		pthread_condattr_init.o \
		pthread_condattr_setpshared.o \
		pthread_cond_destroy.o \
		pthread_cond_init.o \
		pthread_cond_signal.o \
		pthread_cond_wait.o \
		create.o \
		dll.o \
		errno.o \
		pthread_exit.o \
		fork.o \
		global.o \
		pthread_mutex_init.o \
		pthread_mutex_destroy.o \
		pthread_mutexattr_init.o \
		pthread_mutexattr_destroy.o \
		pthread_mutexattr_getpshared.o \
		pthread_mutexattr_setpshared.o \
		pthread_mutexattr_settype.o \
		pthread_mutexattr_gettype.o \
		pthread_mutex_lock.o \
		pthread_mutex_timedlock.o \
		pthread_mutex_unlock.o \
		pthread_mutex_trylock.o \
		pthread_mutexattr_setkind_np.o \
		pthread_mutexattr_getkind_np.o \
		pthread_getw32threadhandle_np.o \
		pthread_delay_np.o \
		pthread_num_processors_np.o \
		pthread_win32_attach_detach_np.o \
		pthread_equal.o \
		pthread_getconcurrency.o \
		pthread_once.o \
		pthread_self.o \
		pthread_setconcurrency.o \
		pthread_rwlock_init.o \
		pthread_rwlock_destroy.o \
		pthread_rwlockattr_init.o \
		pthread_rwlockattr_destroy.o \
		pthread_rwlockattr_getpshared.o \
		pthread_rwlockattr_setpshared.o \
		pthread_rwlock_rdlock.o \
		pthread_rwlock_wrlock.o \
		pthread_rwlock_unlock.o \
		pthread_rwlock_tryrdlock.o \
		pthread_rwlock_trywrlock.o \
		pthread_setschedparam.o \
		pthread_getschedparam.o \
		pthread_timechange_handler_np.o \
		ptw32_is_attr.o \
		ptw32_cond_check_need_init.o \
		ptw32_mutex_check_need_init.o \
		ptw32_processInitialize.o \
		ptw32_processTerminate.o \
		ptw32_threadStart.o \
		ptw32_threadDestroy.o \
		ptw32_tkAssocCreate.o \
		ptw32_tkAssocDestroy.o \
		ptw32_callUserDestroyRoutines.o \
		ptw32_timespec.o \
		ptw32_throw.o \
		ptw32_InterlockedCompareExchange.o \
		ptw32_getprocessors.o \
		ptw32_calloc.o \
		ptw32_new.o \
		ptw32_rwlock_check_need_init.o \
		sched_get_priority_max.o \
		sched_get_priority_min.o \
		sched_setscheduler.o \
		sched_getscheduler.o \
		sched_yield.o \
		sem_init.o \
		sem_destroy.o \
		sem_trywait.o \
		sem_timedwait.o \
		sem_wait.o \
		sem_post.o \
		sem_post_multiple.o \
		sem_getvalue.o \
		sem_open.o \
		sem_close.o \
		sem_unlink.o \
		ptw32_increase_semaphore.o \
		ptw32_decrease_semaphore.o \
		signal.o \
		ptw32_spinlock_check_need_init.o \
		pthread_spin_init.o \
		pthread_spin_destroy.o \
		pthread_spin_lock.o \
		pthread_spin_unlock.o \
		pthread_spin_trylock.o \
		pthread_detach.o \
		pthread_join.o \
		pthread_key_create.o \
		pthread_key_delete.o \
		pthread_setspecific.o \
		pthread_getspecific.o \
		w32_CancelableWait.o

INCL	= \
		config.h \
		implement.h \
		semaphore.h \
		pthread.h \
		need_errno.h

ATTR_SRCS	= \
		pthread_attr_init.c \
		pthread_attr_destroy.c \
		pthread_attr_getdetachstate.c \
		pthread_attr_setdetachstate.c \
		pthread_attr_getstackaddr.c \
		pthread_attr_setstackaddr.c \
		pthread_attr_getstacksize.c \
		pthread_attr_setstacksize.c \
		pthread_attr_getscope.c \
		pthread_attr_setscope.c

BARRIER_SRCS = \
		pthread_barrier_init.c \
		pthread_barrier_destroy.c \
		pthread_barrier_wait.c \
		pthread_barrierattr_init.c \
		pthread_barrierattr_destroy.c \
		pthread_barrierattr_setpshared.c \
		pthread_barrierattr_getpshared.c

CANCEL_SRCS	= \
		pthread_setcancelstate.c \
		pthread_setcanceltype.c \
		pthread_testcancel.c \
		pthread_cancel.c 

CONDVAR_SRCS	= \
		ptw32_cond_check_need_init.c \
		pthread_condattr_destroy.c \
		pthread_condattr_getpshared.c \
		pthread_condattr_init.c \
		pthread_condattr_setpshared.c \
		pthread_cond_destroy.c \
		pthread_cond_init.c \
		pthread_cond_signal.c \
		pthread_cond_wait.c

EXIT_SRCS	= \
		pthread_exit.c

MISC_SRCS	= \
		pthread_equal.c \
		pthread_getconcurrency.c \
		pthread_once.c \
		pthread_self.c \
		pthread_setconcurrency.c \
		ptw32_calloc.c \
		ptw32_new.c \
		w32_CancelableWait.c

MUTEX_SRCS	= \
		ptw32_mutex_check_need_init.c \
		pthread_mutex_init.c \
		pthread_mutex_destroy.c \
		pthread_mutexattr_init.c \
		pthread_mutexattr_destroy.c \
		pthread_mutexattr_getpshared.c \
		pthread_mutexattr_setpshared.c \
		pthread_mutexattr_settype.c \
		pthread_mutexattr_gettype.c \
		pthread_mutex_lock.c \
		pthread_mutex_timedlock.c \
		pthread_mutex_unlock.c \
		pthread_mutex_trylock.c

NONPORTABLE_SRCS = \
		pthread_mutexattr_setkind_np.c \
		pthread_mutexattr_getkind_np.c \
		pthread_getw32threadhandle_np.c \
		pthread_delay_np.c \
		pthread_num_processors_np.c \
		pthread_win32_attach_detach_np.c \
		pthread_timechange_handler_np.c 

PRIVATE_SRCS	= \
		ptw32_is_attr.c \
		ptw32_processInitialize.c \
		ptw32_processTerminate.c \
		ptw32_threadStart.c \
		ptw32_threadDestroy.c \
		ptw32_tkAssocCreate.c \
		ptw32_tkAssocDestroy.c \
		ptw32_callUserDestroyRoutines.c \
		ptw32_timespec.c \
		ptw32_throw.c \
		ptw32_InterlockedCompareExchange.c \
		ptw32_getprocessors.c

RWLOCK_SRCS	= \
		ptw32_rwlock_check_need_init.c \
		ptw32_rwlock_cancelwrwait.c \
		pthread_rwlock_init.c \
		pthread_rwlock_destroy.c \
		pthread_rwlockattr_init.c \
		pthread_rwlockattr_destroy.c \
		pthread_rwlockattr_getpshared.c \
		pthread_rwlockattr_setpshared.c \
		pthread_rwlock_rdlock.c \
		pthread_rwlock_timedrdlock.c \
		pthread_rwlock_wrlock.c \
		pthread_rwlock_timedwrlock.c \
		pthread_rwlock_unlock.c \
		pthread_rwlock_tryrdlock.c \
		pthread_rwlock_trywrlock.c

SCHED_SRCS	= \
		pthread_attr_setschedpolicy.c \
		pthread_attr_getschedpolicy.c \
		pthread_attr_setschedparam.c \
		pthread_attr_getschedparam.c \
		pthread_attr_setinheritsched.c \
		pthread_attr_getinheritsched.c \
		pthread_setschedparam.c \
		pthread_getschedparam.c \
		sched_get_priority_max.c \
		sched_get_priority_min.c \
		sched_setscheduler.c \
		sched_getscheduler.c \
		sched_yield.c

SEMAPHORE_SRCS = \
		sem_init.c \
		sem_destroy.c \
		sem_trywait.c \
		sem_timedwait.c \
		sem_wait.c \
		sem_post.c \
		sem_post_multiple.c \
		sem_getvalue.c \
		sem_open.c \
		sem_close.c \
		sem_unlink.c \
		ptw32_increase_semaphore.c \
		ptw32_decrease_semaphore.c

SPIN_SRCS	= \
		ptw32_spinlock_check_need_init.c \
		pthread_spin_init.c \
		pthread_spin_destroy.c \
		pthread_spin_lock.c \
		pthread_spin_unlock.c \
		pthread_spin_trylock.c

SYNC_SRCS	= \
		pthread_detach.c \
		pthread_join.c

TSD_SRCS	= \
		pthread_key_create.c \
		pthread_key_delete.c \
		pthread_setspecific.c \
		pthread_getspecific.c


GCE_DLL	= pthreadGCE.dll
GCE_LIB	= libpthreadGCE.a
GCE_INLINED_STAMP = pthreadGCE.stamp

GC_DLL 	= pthreadGC.dll
GC_LIB	= libpthreadGC.a
GC_INLINED_STAMP = pthreadGC.stamp

PTHREAD_DEF	= pthread.def

help:
	@ echo Run one of the following command lines:
	@ echo make clean GCE           (to build the GNU C dll with C++ exception handling)
	@ echo make clean GC            (to build the GNU C dll with C cleanup code)
	@ echo make clean GCE-inlined   (to build the GNU C inlined dll with C++ exception handling)
	@ echo make clean GC-inlined    (to build the GNU C inlined dll with C cleanup code)

all:
	@ $(MAKE) clean GCE
	@ $(MAKE) clean GC

GC:
		$(MAKE) CLEANUP_FLAGS="$(GC_CFLAGS)" OBJ="$(DLL_OBJS)" $(GC_DLL)

GCE:
		$(MAKE) CLEANUP_FLAGS="$(GCE_CFLAGS)" OBJ="$(DLL_OBJS)" $(GCE_DLL)

GC-inlined:
		$(MAKE) CLEANUP_FLAGS="$(GC_CFLAGS)" OBJ="$(DLL_INLINED_OBJS)" $(GC_INLINED_STAMP)

GCE-inlined:
		$(MAKE) CLEANUP_FLAGS="$(GCE_CFLAGS)" OBJ="$(DLL_INLINED_OBJS)" $(GCE_INLINED_STAMP)

tests:
	@ cd tests
	@ $(MAKE) auto

%.pre: %.c
	$(CC) -E -o $@ $(CFLAGS) $^

%.s: %.c
	$(CC) -c $(CFLAGS) -Wa,-ahl $^ > $@

.SUFFIXES: .dll .c .o

.c.o:;		 $(CC) -c -o $@ $(CFLAGS) $(CLEANUP_FLAGS) $<


$(GC_DLL): $(DLL_OBJS)
	$(CC) $(OPT) -shared -o $(GC_DLL) $(DLL_OBJS) $(LFLAGS)
	dlltool -z pthread.def $(DLL_OBJS)
	dlltool -k --dllname $@ --output-lib $(GC_LIB) --def $(PTHREAD_DEF)

$(GCE_DLL): $(DLL_OBJS)
	$(CXX) $(OPT) -mthreads -shared -o $(GCE_DLL) $(DLL_OBJS) $(LFLAGS)
	dlltool -z pthread.def $(DLL_OBJS)
	dlltool -k --dllname $@ --output-lib $(GCE_LIB) --def $(PTHREAD_DEF)

$(GC_INLINED_STAMP): $(DLL_INLINED_OBJS)
	$(CC) $(OPT) -shared -o $(GC_DLL) $(DLL_INLINED_OBJS) $(LFLAGS)
	dlltool -z pthread.def $(DLL_INLINED_OBJS)
	dlltool -k --dllname $(GC_DLL) --output-lib $(GC_LIB) --def $(PTHREAD_DEF)
	echo touched > $(GC_INLINED_STAMP)

$(GCE_INLINED_STAMP): $(DLL_INLINED_OBJS)
	$(CXX) $(OPT) -mthreads -shared -o $(GCE_DLL) $(DLL_INLINED_OBJS)  $(LFLAGS)
	dlltool -z pthread.def $(DLL_INLINED_OBJS)
	dlltool -k --dllname $(GCE_DLL) --output-lib $(GCE_LIB) --def $(PTHREAD_DEF)
	echo touched > $(GCE_INLINED_STAMP)

clean:
	-$(RM) *~
	-$(RM) *.i
	-$(RM) *.o
	-$(RM) *.obj
	-$(RM) *.exe
	-$(RM) $(PTHREAD_DEF)

realclean: clean
	-$(RM) $(GC_LIB)
	-$(RM) $(GCE_LIB)
	-$(RM) $(GC_DLL)
	-$(RM) $(GCE_DLL)
	-$(RM) $(GC_INLINED_STAMP)
	-$(RM) $(GCE_INLINED_STAMP)

attr.o:		attr.c $(ATTR_SRCS) $(INCL)
barrier.o:	barrier.c $(BARRIER_SRCS) $(INCL)
cancel.o:	cancel.c $(CANCEL_SRCS) $(INCL)
condvar.o:	condvar.c $(CONDVAR_SRCS) $(INCL)
exit.o:		exit.c $(EXIT_SRCS) $(INCL)
misc.o:		misc.c $(MISC_SRCS) $(INCL)
mutex.o:	mutex.c $(MUTEX_SRCS) $(INCL)
nonportable.o:	nonportable.c $(NONPORTABLE_SRCS) $(INCL)
private.o:	private.c $(PRIVATE_SRCS) $(INCL)
rwlock.o:	rwlock.c $(RWLOCK_SRCS) $(INCL)
sched.o:	sched.c $(SCHED_SRCS) $(INCL)
semaphore.o:	semaphore.c $(SEMAPHORE_SRCS) $(INCL)
spin.o:		spin.c $(SPIN_SRCS) $(INCL)
sync.o:		sync.c $(SYNC_SRCS) $(INCL)
tsd.o:		tsd.c $(TSD_SRCS) $(INCL)
