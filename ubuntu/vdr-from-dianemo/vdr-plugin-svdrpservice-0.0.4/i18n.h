/*
 * i18n.h: translations
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _SVDRPSERVICE_I18N__H
#define _SVDRPSERVICE_I18N__H

#include <vdr/config.h>
#include <vdr/i18n.h>

#if VDRVERSNUM < 10507
#define trNOOP(s) (s)
#define trVDR(s) tr(s)
extern const tI18nPhrase Phrases[];
#endif

#endif //_SVDRPSERVICE_I18N__H
