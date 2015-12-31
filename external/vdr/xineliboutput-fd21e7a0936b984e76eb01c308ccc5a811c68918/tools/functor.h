/*
 * functor.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: functor.h,v 1.1 2006-08-24 23:25:07 phintuka Exp $
 *
 */

#ifndef __XINELIB_FUNCTOR_H
#define __XINELIB_FUNCTOR_H

#include <vdr/tools.h>


class cFunctor : public cListObject 
{
  public:
    cFunctor() : cListObject() {}
    virtual ~cFunctor() {}
    virtual void Execute(void) = 0;
};

#if 1 /* gcc 3.3.x (?) does not accept class TRESULT=void */
template<class TCLASS>
cFunctor *CreateFunctor(TCLASS *c, 
			void (TCLASS::*fp)(void));

template<class TCLASS, class TARG1>
cFunctor *CreateFunctor(TCLASS *c, 
			void (TCLASS::*fp)(TARG1), 
			TARG1 arg1);
#endif

template<class TCLASS, class TRESULT>
cFunctor *CreateFunctor(TCLASS *c, 
			TRESULT (TCLASS::*fp)(void));

template<class TCLASS, class TRESULT, class TARG1>
cFunctor *CreateFunctor(TCLASS *c, 
			TRESULT (TCLASS::*fp)(TARG1), 
			TARG1 arg1);

#include "functorimpl.h"

#endif
