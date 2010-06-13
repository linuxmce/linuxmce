/*
 * i18n.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __WIRBELSCAN_I18N_H
#define __WIRBELSCAN_I18N_H

#include <vdr/i18n.h>

#if VDRVERSNUM < 10507
extern const tI18nPhrase wirbelscan_Phrases[];
#define trNOOP(s) (s)
#endif

#endif //__WIRBELSCAN_I18N_H
