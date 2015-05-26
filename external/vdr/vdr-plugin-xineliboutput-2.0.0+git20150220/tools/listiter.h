/*
 * listiter.h: 
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: listiter.h,v 1.3 2010-03-15 14:13:43 phintuka Exp $
 *
 */


#ifndef _LISTITER_H_
#define _LISTITER_H_

//------------------------------ list ----------------------------------------

template <class LIST,class ITEM, class RESULT>
void ForEach(LIST& List, RESULT (ITEM::*f)())
{
  for(ITEM *it = List.First(); it; it = List.Next(it))
    (*it.*f)();
}

template <class LIST,class ITEM, class ARG1, class RESULT>
void ForEach(LIST& List, RESULT (ITEM::*f)(ARG1), ARG1 arg1)
{
  for(ITEM *it = List.First(); it; it = List.Next(it))
    (*it.*f)(arg1);
}

template <class LIST,class ITEM, class ARG1, class ARG2>
void ForEach(LIST& List, void (ITEM::*f)(ARG1,ARG2), ARG1 arg1, ARG2 arg2)
{
  for(ITEM *it = List.First(); it; it = List.Next(it))
    (*it.*f)(arg1,arg2);
}

template <class LIST,class ITEM, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
  void ForEach(LIST& List, void (ITEM::*f)(ARG1,ARG2,ARG3,ARG4,ARG5), ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)
{
  for(ITEM *it = List.First(); it; it = List.Next(it))
    (*it.*f)(arg1,arg2,arg3,arg4,arg5);
}

template <class LIST,class ITEM, class ARG1, class RESULT>
RESULT ForEach(LIST& List, RESULT (ITEM::*f)(ARG1), ARG1 arg1, 
	       RESULT (*combiner)(RESULT,RESULT), RESULT def)
{
  RESULT result = def;
  for(ITEM *it = List.First(); it; it = List.Next(it))
    result = (*combiner)((*it.*f)(arg1),result);
  return result;
}

template <class LIST,class ITEM, class ARG1, class ARG2, class RESULT>
RESULT ForEach(LIST& List, RESULT (ITEM::*f)(ARG1,ARG2), 
	       ARG1 arg1, ARG2 arg2, 
	       RESULT (*combiner)(RESULT,RESULT), RESULT def)
{
  RESULT result = def;
  for(ITEM *it = List.First(); it; it = List.Next(it))
    result = (*combiner)((*it.*f)(arg1,arg2),result);
  return result;
}

template <class LIST,class ITEM, class ARG1, class ARG2, class ARG3, 
	  class RESULT>
RESULT ForEach(LIST& List, RESULT (ITEM::*f)(ARG1,ARG2,ARG3), 
	       ARG1 arg1, ARG2 arg2, ARG3 arg3, 
	       RESULT (*combiner)(RESULT,RESULT), RESULT def)
{
  RESULT result = def;
  for(ITEM *it = List.First(); it; it = List.Next(it))
    result = (*combiner)((*it.*f)(arg1,arg2,arg3),result);
  return result;
}

template <class LIST, class ITEM, class ARG1, class ARG2, class ARG3, class ARG4,
          class RESULT>
  RESULT ForEach(LIST& List, RESULT (ITEM::*f)(ARG1,ARG2,ARG3,ARG4),
                 ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4,
                 RESULT (*combiner)(RESULT,RESULT), RESULT def)
{
  RESULT result = def;
  for(ITEM *it = List.First(); it; it = List.Next(it))
    result = (*combiner)((*it.*f)(arg1,arg2,arg3,arg4),result);
  return result;
}

template<class T>
T mmin(T a, T b) {return a<b ? a : b;}

template<class T>
T mmax(T a, T b) {return a>b ? a : b;}

template<class T>
T mand(T a, T b) {return a&&b;}

template<class T>
T mor(T a, T b)  {return a||b;}

#endif
