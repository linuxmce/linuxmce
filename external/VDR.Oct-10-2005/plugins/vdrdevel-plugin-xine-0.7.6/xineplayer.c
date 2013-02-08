
#include "xineCommon.h"

#include "xineExternal.h"

static int fdControl = -1;
static int fdResult = -1;

bool writeString(char *s)
{
  int l = ::strlen(s);

  while (l)
  {
    int r = ::write(fdControl, s, l);
    if (r < 0)
    {
      ::perror("xineplayer: writeString() failed");
      return false;
    }

    l -= r;
    s += r;
  }

  return true;
}

bool cmdPlay(char *const mrl)
{
  if (!::writeString("play "))
    return false;

  if (!::writeString(mrl))
    return false;

  if (!::writeString("\n"))
    return false;

  return true;
}

bool waitResult()
{
  char s;
  
  int r = ::read(fdResult, &s, 1);
  if (r < 0)
  {
    ::perror("xineplayer: waitResult() failed");
    return false;
  }

  return (1 == r);
}

bool communicate(char *const mrl)
{
  if (!::cmdPlay(mrl))
    return false;

  if (!::waitResult())
    return false;
  
  return true;
}

#define ARG_VDR_XINE_INSTANCE "--vdr-xine-instance="

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
  usage:    
    ::fprintf(stderr, "usage: xineplayer [ " ARG_VDR_XINE_INSTANCE "N ] [ options ] mrl\n");
    return 1;
  }

  int instanceNo = -1;
  
  if (0 == ::strncmp(argv[ 1 ], ARG_VDR_XINE_INSTANCE, ::strlen(ARG_VDR_XINE_INSTANCE)))
  {
    instanceNo = ::atoi(argv[ 1 ] + ::strlen(ARG_VDR_XINE_INSTANCE));

    if (instanceNo < 0)
      goto usage;

    if (argc < 3)
      goto usage;
  }

  string fifoDir = FIFO_DIR;

  if (instanceNo >= 0)
  {
    char s[ 20 ];
    ::sprintf(s, "%d", instanceNo);
    
    fifoDir += s;
  }

  string fifoNameExtControl = fifoDir + FIFO_NAME_EXT_CONTROL;
  string fifoNameExtResult  = fifoDir + FIFO_NAME_EXT_RESULT;
  
  fdResult = ::open(fifoNameExtResult.c_str(), O_RDONLY);
  if (-1 == fdResult)
  {
    ::perror(("xineplayer: opening '" + fifoNameExtResult + "' failed").c_str());

    ::close(fdControl);
    return 1;
  }
  
  fdControl = ::open(fifoNameExtControl.c_str(), O_WRONLY);
  if (-1 == fdControl)
  {
    ::perror(("xineplayer: opening '" + fifoNameExtControl + "' failed").c_str());
    return 1;
  }
  
  bool result = ::communicate(argv[ argc - 1 ]);
  
  ::close(fdControl);
  ::close(fdResult);
  
  return !result;
}

