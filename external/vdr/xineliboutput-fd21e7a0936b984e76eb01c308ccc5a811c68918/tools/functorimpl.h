/*
 * functorimpl.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: functorimpl.h,v 1.1 2006-08-24 23:25:07 phintuka Exp $
 *
 */

#ifndef __XINELIB_FUNCTORIMPL_H

#ifndef __XINELIB_FUNCTOR_H
#  error functorimpl.h should not be included, use functor.h instead
#endif

#if 1 /* gcc 3.3.x (?) does not accept class TRESULT=void */
template <class TCLASS>
class cFunctor0 : public cFunctor {

  public:
  protected:

    typedef void (TCLASS::*TFUNC)(void);

    cFunctor0(TCLASS *obj, TFUNC f) : m_obj(obj), m_f(f) {}
    virtual ~cFunctor0() {};

    virtual void Execute(void)
    { 
      (*m_obj.*m_f)(); 
    }

  private:
    TCLASS *m_obj;
    TFUNC   m_f;

    friend cFunctor *CreateFunctor<TCLASS>(TCLASS*,TFUNC);
};

template <class TCLASS, class TARG1>
class cFunctor1 : public cFunctor {

  public:

  protected:
    typedef void (TCLASS::*TFUNC)(TARG1);

    cFunctor1(TCLASS *obj, TFUNC f, TARG1 arg1) :
      m_obj(obj), m_f(f), m_arg1(arg1) {}
    virtual ~cFunctor1() {};

    virtual void Execute(void)
    { 
      (*m_obj.*m_f)(m_arg1); 
    }

  private:
    TCLASS *m_obj;
    TFUNC   m_f;
    TARG1   m_arg1;

    friend cFunctor *CreateFunctor<TCLASS,TARG1>(TCLASS*,TFUNC,TARG1);
};
#endif

template <class TCLASS, class TRESULT>
class cFunctorR0 : public cFunctor {

  public:
  protected:

    typedef TRESULT (TCLASS::*TFUNC)(void);

    cFunctorR0(TCLASS *obj, TFUNC f) : m_obj(obj), m_f(f) {}
    virtual ~cFunctorR0() {};

    virtual void Execute(void)
    { 
      // TODO: use future to pass back value
      (void) (*m_obj.*m_f)(); 
    }

  private:
    TCLASS *m_obj;
    TFUNC   m_f;

    friend cFunctor *CreateFunctor<TCLASS,TRESULT>(TCLASS*,TFUNC);
};

template <class TCLASS, class TRESULT, class TARG1>
class cFunctorR1 : public cFunctor {

  public:
  protected:

    typedef TRESULT (TCLASS::*TFUNC)(TARG1);

    cFunctorR1(TCLASS *obj, TFUNC f, TARG1 arg1) : 
      m_obj(obj), m_f(f), m_arg1(arg1) {}
    virtual ~cFunctorR1() {};

    virtual void Execute(void)
    { 
      // TODO: use future to pass back value
      (void) (*m_obj.*m_f)(m_arg1); 
    }

  private:
    TCLASS *m_obj;
    TFUNC   m_f;
    TARG1   m_arg1;

    friend cFunctor *CreateFunctor<TCLASS,TRESULT>(TCLASS*,TFUNC,TARG1);
};

#if 1 /* gcc 3.3.x (?) does not accept class TRESULT=void */
template<class TCLASS>
cFunctor *CreateFunctor(TCLASS *c,
			void (TCLASS::*fp)(void))
{
  return new cFunctor0<TCLASS>(c, fp);
}

template<class TCLASS, class TARG1>
cFunctor *CreateFunctor(TCLASS *c, 
			void (TCLASS::*fp)(TARG1),
			TARG1 arg1)
{
  return new cFunctor1<TCLASS,TARG1>(c, fp, arg1);
}
#endif

template<class TCLASS, class TRESULT>
cFunctor *CreateFunctor(TCLASS *c,
			TRESULT (TCLASS::*fp)(void))
{
  return new cFunctorR0<TCLASS,TRESULT>(c, fp);
}

template<class TCLASS, class TRESULT, class TARG1>
cFunctor *CreateFunctor(TCLASS *c,
			TRESULT (TCLASS::*fp)(TARG1), 
			TARG1 arg1)
{
  return new cFunctorR1<TCLASS,TRESULT,TARG1>(c, fp, arg1);
}


#endif
