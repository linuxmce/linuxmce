#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "LIRCD_Thread.h"

#define LIRCD_SOCKET "/tmp/hulu_lircd"

#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"
using namespace DCE;

/* mutex (dunno if we really need it, but it's there.) */
pluto_pthread_mutex_t LIRCD_mutex ("lircd");

/* Private Methods */
static void LIRCD_SignalHandler (int ExitStatus);
static bool LIRCD_Open ();
static int setNonblocking(int fd);

/* Public bits */
bool LIRCD_bQuit = false;
bool LIRCD_bConnectionActive = false;
int lircd_sockfd;		/* file descriptor for lirc socket      */
int lircd_clientfd;		/* file descriptor for the initial client conn */
sockaddr_un lircd_address;	/* Address to the lirc socket           */
sockaddr_un lircd_client_address;	/* Address to the lirc socket from the client end */
socklen_t lircd_len;
socklen_t lircd_client_len;

static void
LIRCD_SignalHandler (int ExitStatus)
{
  LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					"Hulu_Player: Terminated by signal %d",
					ExitStatus);
  LIRCD_bQuit = true;
}


void *
LIRCD_Thread (void *arg)
{

  // Attach signal handlers to handle premature termination.
  signal (SIGTERM, LIRCD_SignalHandler);
  signal (SIGINT, LIRCD_SignalHandler);

  // Initialize the mutex
  LIRCD_mutex.Init (NULL);

  LIRCD_Open ();

  lircd_clientfd = -1;  // No client connected yet.

  while (!LIRCD_bQuit)
    {

	LoggerWrapper::GetInstance ()->Write(LV_CRITICAL,
						"LIRCD_Thread: Listening for Hulu Desktop");
      do
      {
	// This call blocks.
	lircd_clientfd = accept (lircd_sockfd,
				(sockaddr *) & lircd_client_address,
				&lircd_client_len);
	Sleep(100);	// XXX HACK!
      } while (!LIRCD_bQuit && lircd_clientfd == -1);

      if (lircd_clientfd != -1)
      {

	LoggerWrapper::GetInstance()->Write (LV_CRITICAL, 
					      "LIRCD_Thread: Accepted Socket Connection: %d",
					      lircd_clientfd);
	LIRCD_bConnectionActive = true;

      }

      while (LIRCD_bConnectionActive)
	{
	  // While the connection is active, we basically sit here.
	  // Since we do not have to process anything from hulu at all
	  // but we do need to prevent the accept connection from running
	  // prematurely.
		
		if (LIRCD_bQuit)
		{
			LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
								"LIRCD_Thread: Got Quit while connection active. Quitting.");
			LIRCD_bConnectionActive = false;
		}
	  Sleep (20);
	}

	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
						"LIRCD_Thread: Closing Socket connection: %d",
						lircd_clientfd);
      if (lircd_clientfd != -1)
	close (lircd_clientfd);	// Drop the FD

    }

  LIRCD_Close ();


  // Restore the signal handlers to their defaults.
  signal (SIGTERM, SIG_DFL);
  signal (SIGINT, SIG_DFL);
  return NULL;

}

/*********************************************************************************/
/** Socket functions								**/
/*********************************************************************************/

inline int
write_socket (int fd, const char *buf, int len)
{
  int done, todo = len;

  while (todo)
    {
#ifdef SIM_REC
      do
	{
	  done = write (fd, buf, todo);
	}
      while (done < 0 && errno == EAGAIN);
#else
      done = write (fd, buf, todo);
#endif
      if (done <= 0)
	return (done);
      buf += done;
      todo -= done;
    }
  return (len);
}

inline int
write_socket_len (int fd, const char *buf)
{
  int len;

  len = strlen (buf);
  if (write_socket (fd, buf, len) < len)
    return (0);
  return (1);
}


/*********************************************************************************/
/** Fake LIRCD stuff                                                  		**/
/*********************************************************************************/

bool
LIRCD_Open ()
{

  unlink (LIRCD_SOCKET);
  lircd_sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
  setNonblocking(lircd_sockfd);
  lircd_address.sun_family = AF_UNIX;
  strcpy (lircd_address.sun_path, LIRCD_SOCKET);
  lircd_len = sizeof (lircd_address);
  bind (lircd_sockfd, (sockaddr *) & lircd_address, lircd_len);
  listen (lircd_sockfd, 0);	// Should we set this to 1 ???

  return true;
}

bool
LIRCD_Close ()
{
  // Close the open client file descriptor.
  close (lircd_clientfd);
  LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					"Closed File Descriptor: %d",
					lircd_sockfd);
  return true;
}

bool
LIRCD_SendCommand (string sCommand)
{
  // Write a command to the server socket.
  PLUTO_SAFETY_LOCK (sl, LIRCD_mutex);
  LoggerWrapper::GetInstance ()->Write (LV_STATUS, "Sending %s",
					sCommand.c_str ());
  write_socket_len (lircd_clientfd, sCommand.c_str ());
  return true;
}

int setNonblocking(int fd)
{
    int flags;

    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}
