/**
 * @file LinuxSoundcard.h
 * Header file for LinuxSoundcard
 */

#ifndef LINUXSOUNDCARD_H
#define LINUXSOUNDCARD_H

#include "FileUtils.h"
#include "DCE/Logger.h"

using namespace DCE;
string TranslateSoundCard(string sInput,string sIPAddress="");

#endif
