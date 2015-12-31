//
// Small wrapper arround some administrative functions which have to be
// executed set-root-uid
//

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
  setuid(geteuid());
  setgid(getegid());
  if (argc == 2)
  {
    if (strcmp(argv[1], "--start") == 0)
    {
      return system("/etc/init.d/vdr start");
    }
    else if (strcmp(argv[1], "--restart") == 0)
    {
      return system("/etc/init.d/vdr restart");
    }
    else if (strcmp(argv[1], "--stop") == 0)
    {
      return system("/etc/init.d/vdr stop");
    }
    else if (strcmp(argv[1], "--switch") == 0)
    {
      return system("/etc/init.d/vdrdevel stop && "
                    "sleep 5 && "
                    "/etc/init.d/vdr start");
    }
    else if (strcmp(argv[1], "--switchback") == 0)
    {
      return system("/etc/init.d/vdr stop && "
                    "sleep 5 && "
                    "/etc/init.d/vdrdevel start");
    }
  }
  return 1;
}
