# Makefile for the pthreads test suite.
# If all of the .pass files can be created, the test suite has passed.
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


CP	= copy
RM	= erase
MKDIR	= mkdir
TOUCH	= echo Passed >
ECHO	= @echo

CPHDR	= pthread.h semaphore.h sched.h

OPTIM	= /O2 /Ob0

# C++ Exceptions
VCEFLAGS	= /GX /TP /DPtW32NoCatchWarn /D__CLEANUP_CXX
VCELIB	= pthreadVCE.lib
VCEDLL	= pthreadVCE.dll
# Structured Exceptions
VSEFLAGS	= /D__CLEANUP_SEH
VSELIB	= pthreadVSE.lib
VSEDLL	= pthreadVSE.dll
# C cleanup code
VCFLAGS	= /D__CLEANUP_C
VCLIB	= pthreadVC.lib
VCDLL	= pthreadVC.dll
# C++ Exceptions in application - using VC version of pthreads dll
VCXFLAGS	= /GX /TP /D__CLEANUP_C

CFLAGS= $(OPTIM) /W3 /WX /MD /nologo /Yd /Zi -D_WIN32_WINNT=0x400
LFLAGS= /INCREMENTAL:NO
INCLUDES=-I.
BUILD_DIR=..

COPYFILES	= $(CPHDR) $(CPLIB) $(CPDLL)

TEST		=
EHFLAGS	=

# If a test case returns a non-zero exit code to the shell, make will
# stop.

PASSES= loadfree.pass \
	  semaphore1.pass  semaphore2.pass  self1.pass  mutex5.pass  \
	  mutex1.pass  mutex1n.pass  mutex1e.pass  mutex1r.pass  mutex2.pass  mutex3.pass  \
	  condvar1.pass  condvar1_1.pass  condvar1_2.pass  condvar2.pass  condvar2_1.pass  \
	  exit1.pass  create1.pass  equal1.pass  \
	  exit2.pass  exit3.pass  \
	  join0.pass  join1.pass  join2.pass  \
	  mutex4.pass  mutex6.pass  mutex6n.pass  mutex6e.pass  mutex6r.pass  \
	  mutex7.pass  mutex7n.pass  mutex7e.pass  mutex7r.pass  \
	  mutex8.pass  mutex8n.pass  mutex8e.pass  mutex8r.pass  \
	  count1.pass  once1.pass  tsd1.pass  \
	  self2.pass  \
	  cancel1.pass  cancel2.pass  \
	  delay1.pass  delay2.pass  eyal1.pass  \
	  condvar3.pass  condvar3_1.pass  condvar3_2.pass  condvar3_3.pass  \
	  condvar4.pass  condvar5.pass  condvar6.pass  \
	  condvar7.pass  condvar8.pass  condvar9.pass  \
	  errno1.pass  \
	  rwlock1.pass  rwlock2.pass  rwlock3.pass  rwlock4.pass  rwlock5.pass  rwlock6.pass  rwlock7.pass  \
	  rwlock2_t.pass  rwlock3_t.pass  rwlock4_t.pass  rwlock5_t.pass  rwlock6_t.pass  rwlock6_t2.pass  \
	  context1.pass  \
	  cancel3.pass  cancel4.pass  cancel5.pass  cancel6a.pass  cancel6d.pass  \
	  cleanup0.pass  cleanup1.pass  cleanup2.pass  cleanup3.pass  \
	  priority1.pass priority2.pass inherit1.pass  \
	  spin1.pass  spin2.pass  spin3.pass  spin4.pass  \
	  barrier1.pass  barrier2.pass  barrier3.pass  barrier4.pass  barrier5.pass  \
	  exception1.pass  exception2.pass  exception3.pass

BENCHRESULTS = \
	  benchtest1.bench benchtest2.bench benchtest3.bench benchtest4.bench benchtest5.bench

help:
	@ $(ECHO) Run one of the following command lines:
	@ $(ECHO) nmake clean VC    (to test using VC dll with VC (no EH) applications)
	@ $(ECHO) nmake clean VCX   (to test using VC dll with VC++ (EH) applications)
	@ $(ECHO) nmake clean VCE   (to test using the VCE dll with VC++ EH applications)
	@ $(ECHO) nmake clean VSE   (to test using VSE dll with VC (SEH) applications)
	@ $(ECHO) nmake clean VC-bench    (to benchtest using VC dll with C bench app)
	@ $(ECHO) nmake clean VCX-bench   (to benchtest using VC dll with C++ bench app)
	@ $(ECHO) nmake clean VCE-bench   (to benchtest using VCE dll with C++ bench app)
	@ $(ECHO) nmake clean VSE-bench   (to benchtest using VSE dll with SEH bench app)

all:
	@ nmake clean VC
	@ nmake clean VCX
	@ nmake clean VCE
	@ nmake clean VSE
	@ nmake clean VC-bench

tests: $(CPLIB) $(CPDLL) $(CPHDR) $(PASSES)
	@ $(ECHO) ALL TESTS PASSED! Congratulations!

benchtests: $(CPLIB) $(CPDLL) $(CPHDR) $(BENCHRESULTS)
	@ $(ECHO) ALL BENCH TESTS DONE.

$(PASSES): $*.exe
	@ $(ECHO) ... Running $(TEST) test: $*.exe
	@ .\$*.exe
	@ $(ECHO) ...... Passed
	@ $(TOUCH) $*.pass

$(BENCHRESULTS): $*.exe
	@ $(ECHO) ... Running $(TEST) benchtest: $*.exe
	@ .\$*.exe
	@ $(ECHO) ...... Done
	@ $(TOUCH) $*.bench

VCE:
	@ nmake TEST="$@" CPLIB="$(VCELIB)" CPDLL="$(VCEDLL)" EHFLAGS="$(VCEFLAGS)" tests

VSE:	
	@ nmake TEST="$@" CPLIB="$(VSELIB)" CPDLL="$(VSEDLL)" EHFLAGS="$(VSEFLAGS)" tests

VC:
	@ nmake TEST="$@" CPLIB="$(VCLIB)" CPDLL="$(VCDLL)" EHFLAGS="$(VCFLAGS)" tests

VCX:
	@ nmake TEST="$@" CPLIB="$(VCLIB)" CPDLL="$(VCDLL)" EHFLAGS="$(VCXFLAGS)" tests

VCE-bench:
	@ nmake TEST="$@" CPLIB="$(VCELIB)" CPDLL="$(VCEDLL)" EHFLAGS="$(VCEFLAGS)" benchtests

VSE-bench:
	@ nmake TEST="$@" CPLIB="$(VSELIB)" CPDLL="$(VSEDLL)" EHFLAGS="$(VSEFLAGS)" benchtests

VC-bench:
	@ nmake TEST="$@" CPLIB="$(VCLIB)" CPDLL="$(VCDLL)" EHFLAGS="$(VCFLAGS)" benchtests

VCX-bench:
	@ nmake TEST="$@" CPLIB="$(VCLIB)" CPDLL="$(VCDLL)" EHFLAGS="$(VCXFLAGS)" benchtests

.c.exe:
	@ $(ECHO) $(CC) $(EHFLAGS) $(CFLAGS) $(INCLUDES) $< /Fe$@ /link $(LFLAGS) $(CPLIB)
	@ $(CC) $(EHFLAGS) $(CFLAGS) $(INCLUDES) $< /Fe$@ /link $(LFLAGS) $(CPLIB)

.c.i:
	@ $(CC) /P $(EHFLAGS) $(CFLAGS) $(INCLUDES) $<

$(COPYFILES):
	@ $(ECHO) Copying $@
	@ $(CP) $(BUILD_DIR)\$@ .

pthread.dll:
	@ $(CP) $(CPDLL) $*.dll
	@ $(CP) $(CPLIB) $*.lib

clean:
	- $(RM) *.dll
	- $(RM) *.lib
	- $(RM) pthread.h
	- $(RM) semaphore.h
	- $(RM) sched.h
	- $(RM) *.e
	- $(RM) *.i
	- $(RM) *.obj
	- $(RM) *.pdb
	- $(RM) *.o
	- $(RM) *.asm
	- $(RM) *.exe
	- $(RM) *.pass
	- $(RM) *.bench
	- $(RM) *.log

benchtest1.bench:
benchtest2.bench:
benchtest3.bench:
benchtest4.bench:
benchtest5.bench:
barrier1.pass:
barrier2.pass: barrier1.pass
barrier3.pass: barrier2.pass
barrier4.pass: barrier3.pass
barrier5.pass: barrier4.pass
cancel1.pass: create1.pass
cancel2.pass: cancel1.pass
cancel3.pass: context1.pass
cancel4.pass: cancel3.pass
cancel5.pass: cancel3.pass
cancel6a.pass: cancel3.pass
cancel6d.pass: cancel3.pass
cleanup0.pass: cancel5.pass
cleanup1.pass: cleanup0.pass
cleanup2.pass: cleanup1.pass
cleanup3.pass: cleanup2.pass
condvar1.pass:
condvar1_1.pass: condvar1.pass
condvar1_2.pass: join2.pass
condvar2.pass: condvar1.pass
condvar2_1.pass: condvar2.pass join2.pass
condvar3.pass: create1.pass condvar2.pass
condvar3_1.pass: condvar3.pass join2.pass
condvar3_2.pass: condvar3_1.pass
condvar3_3.pass: condvar3_2.pass
condvar4.pass: create1.pass
condvar5.pass: condvar4.pass
condvar6.pass: condvar5.pass
condvar7.pass: condvar6.pass cleanup1.pass
condvar8.pass: condvar7.pass
condvar9.pass: condvar8.pass
context1.pass: cancel2.pass
count1.pass: join1.pass
create1.pass: mutex2.pass
delay1.pass:
delay2.pass: delay1.pass
equal1.pass: create1.pass
errno1.pass: mutex3.pass
exception1.pass: cancel4.pass
exception2.pass: exception1.pass
exception3.pass: exception2.pass
exit1.pass:
exit2.pass: create1.pass
exit3.pass: create1.pass
eyal1.pass: tsd1.pass
inherit1.pass: join1.pass
join0.pass: create1.pass
join1.pass: create1.pass
join2.pass: create1.pass
loadfree.pass: pthread.dll
mutex1.pass: self1.pass
mutex1n.pass: mutex1.pass
mutex1e.pass: mutex1.pass
mutex1r.pass: mutex1.pass
mutex2.pass: mutex1.pass
mutex3.pass: create1.pass
mutex4.pass: mutex3.pass
mutex5.pass:
mutex6.pass: mutex4.pass
mutex6n.pass: mutex4.pass
mutex6e.pass: mutex4.pass
mutex6r.pass: mutex4.pass
mutex7.pass: mutex6.pass
mutex7n.pass: mutex6n.pass
mutex7e.pass: mutex6e.pass
mutex7r.pass: mutex6r.pass
mutex8.pass: mutex7.pass
mutex8n.pass: mutex7n.pass
mutex8e.pass: mutex7e.pass
mutex8r.pass: mutex7r.pass
once1.pass: create1.pass
priority1.pass: join1.pass
priority2.pass: priority1.pass
rwlock1.pass: condvar6.pass
rwlock2.pass: rwlock1.pass
rwlock3.pass: rwlock2.pass
rwlock4.pass: rwlock3.pass
rwlock5.pass: rwlock4.pass
rwlock6.pass: rwlock5.pass
rwlock7.pass: rwlock6.pass
rwlock2_t.pass: rwlock2.pass
rwlock3_t.pass: rwlock2_t.pass
rwlock4_t.pass: rwlock3_t.pass
rwlock5_t.pass: rwlock4_t.pass
rwlock6_t.pass: rwlock5_t.pass
rwlock6_t2.pass: rwlock6_t.pass
self1.pass:
self2.pass: create1.pass
semaphore1.pass:
semaphore2.pass:
spin1.pass:
spin2.pass: spin1.pass
spin3.pass: spin2.pass
spin4.pass: spin3.pass
tsd1.pass: join1.pass
