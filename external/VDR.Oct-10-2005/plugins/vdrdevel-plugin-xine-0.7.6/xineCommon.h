
#ifndef __XINECOMMON_H
#define __XINECOMMON_H



#include <assert.h>
#include <math.h>
#include <signal.h>

#include <string>
using namespace std;

#include <xine.h>
#include <xine/input_vdr.h>

#include <vdr/config.h>  // poisened

namespace PluginXine
{
  extern bool beQuiet;
};

#define xfprintf(fh, fmt, args...)  \
  while (!PluginXine::beQuiet)      \
  {                                 \
    fprintf(fh, fmt, ##args);       \
    break;                          \
  }



#endif //__XINECOMMON_H
