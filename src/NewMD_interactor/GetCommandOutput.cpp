/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <stdio.h>

#include "GetCommandOutput.h"

bool GetCommandOutput(const char * path, char * args[], string * sOutput)
{
	int pid;
	int output[2];

	pipe(output);
	switch (pid = fork())
	{
		case 0: /* child */
			// treat stderr too someplace?
			close(output[0]);
			dup2(output[1], 1);
			close(output[1]);
			execv(path, args);
			_exit(254);
			/* Rationale for _exit instead of exit:
			 * exit calls atexit-registered functions and X registers some stuff
			 * this causes the Orbiter to lose the connection to X
			 * but this child doesn't register any atexit functions, nor does it make any output if exec fails
			 */
			break;
		case -1: /* error */
			return false;
			break;
		default: /* parent */
			close(output[1]);
			
			char buffer[4096];
			memset(buffer, 0, sizeof(buffer));
			int bytes;
			while ((bytes = read(output[0], buffer, sizeof(buffer) - 1)) > 0)
			{
				if (sOutput != NULL)
				{
					* sOutput += buffer;
				}
				else
				{
					printf("%s", buffer);
					fflush(stdout);
				}
				memset(buffer, 0, sizeof(buffer));
			}
			
			int status;
			waitpid(pid, &status, 0);
			status = WEXITSTATUS(status);

			close(output[0]);
			if (status == 254)
				return false;
	}

	return true;
}
