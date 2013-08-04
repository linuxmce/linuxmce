#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

static int quit = 0;

static void
daemonize (void)
{
  pid_t pid, sid;

  if (getppid () == 1)
    return;

  pid = fork ();
  if (pid < 0)
    {
      exit (1);
    }
  if (pid > 0)
    {
      exit (1);
    }

  umask (0);

  sid = setsid ();
  if (sid < 0)
    {
      exit (1);
    }

  if ((chdir ("/")) < 0)
    {
      exit (1);
    }

  freopen ("/dev/null", "r", stdin);
  freopen ("/dev/null", "w", stdout);
  freopen ("/dev/null", "w", stderr);
}

void
default_handler (int sig)
{
  quit = 1;
}

int
file_exists (char *fileName)
{
  struct stat buf;
  int i = stat (fileName, &buf);
  /*
   * File found 
   */
  if (i == 0)
    {
      return 1;
    }
  return 0;

}

int
main (int argc, char *argv[])
{
  unsigned char buf[5];
  int fd, fd2;
  union i2c_smbus_data smbus_data;
  struct i2c_smbus_ioctl_data smbus_ioctl_data;
  unsigned int sensor_val;
  unsigned int brightness_val;
  unsigned int old_brightness_val;
  char cBrightness_val[5];

  if (!file_exists ("/dev/i2c-0"))
    {
      perror ("/dev/i2c-0");
      exit (1);
    }

  if (!file_exists ("/sys/class/backlight/openframe-bl/brightness"))
    {
      perror ("/sys/class/backlight/openframe-bl/brightness");
      exit (1);
    }

  daemonize ();

  signal (SIGINT, default_handler);
  signal (SIGTERM, default_handler);
  signal (SIGKILL, default_handler);
  signal (SIGHUP, default_handler);

  fd = open ("/dev/i2c-0", O_RDWR);
  if (fd < 0)
    {
      perror ("Open i2c bus\n");
      exit (1);
    }

  fd2 = open ("/sys/class/backlight/openframe-bl/brightness", O_WRONLY);
  if (fd < 0)
    {
      perror ("Open backlight power control.");
      exit (1);
    }
  // set slave device to 8051 microprocessor
  if (ioctl (fd, I2C_SLAVE, 0x34))
    {
      perror ("Set slave i2c address\n");
      exit (1);
    }

  old_brightness_val = brightness_val = 0;

  while (quit == 0)
    {

      // send the initial request
      smbus_data.block[0] = 2;	// count of bytes
      smbus_data.block[1] = 0;
      smbus_data.block[2] = 0;
      smbus_ioctl_data.read_write = 0;
      smbus_ioctl_data.command = 0x27;
      smbus_ioctl_data.size = I2C_SMBUS_BLOCK_DATA;
      smbus_ioctl_data.data = &smbus_data;
      if (ioctl (fd, I2C_SMBUS, &smbus_ioctl_data))
	{
	  perror ("Send initial request\n");
	  exit (1);
	}
      // wait for response
      smbus_data.block[0] = 4;
      smbus_ioctl_data.read_write = 1;
      while (1)
	{
	  if (ioctl (fd, I2C_SMBUS, &smbus_ioctl_data))
	    {
	      perror ("Read result\n");
	      exit (1);
	    }
	  if (smbus_data.block[1] == 0x87)
	    break;
	  usleep (1000);
	}
      if (smbus_data.block[1] != 0x87)
	{
	  fprintf (stderr, "Failed to get response\n");
	  exit (1);
	}

      sensor_val = (smbus_data.block[3] << 8 | smbus_data.block[2]);
      brightness_val = ((sensor_val * 20) / 32);

      printf ("n: %u  o: %u\n", brightness_val, old_brightness_val);
      // Simple jitter correction.
      if (abs (brightness_val - old_brightness_val) > 1)
	{
	  sprintf (cBrightness_val, "%u", brightness_val);
	  printf ("%s\n", cBrightness_val);
	  write (fd2, cBrightness_val, strlen (cBrightness_val));
	}

      old_brightness_val = brightness_val;
      usleep (1000);

    }

  close (fd);
  close (fd2);
  return 0;
}
