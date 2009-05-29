/*
 * i18n.h: Internationalization
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: i18n.h,v 1.3 2007/09/18 13:52:52 phintuka Exp $
 *
 */

#ifndef __XINELIBOUTPUT_I18N_H
#define __XINELIBOUTPUT_I18N_H

#include <vdr/i18n.h>

#if VDRVERSNUM < 10507

extern const tI18nPhrase Phrases[];

#ifndef trNOOP
#  define trNOOP(s) (s)
#endif

#ifndef trVDR
#  define trVDR(s) tr(s)
#endif

#endif // VDRVERSNUM < 10507

#endif //__XINELIBOUTPUT_I18N_H
