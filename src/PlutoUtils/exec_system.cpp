/* 
	exec_system
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Handy routine that replaces calls to system().
// This is better because you can check the fork yourself for errors,
// Plus you can force it to block until the command is finished by
// using the wait flag.

void exec_system(std::string cmd, bool wait) {
        char command[1024];
	char *args[512];
        int fork_res;
	int arg_count;
	std::string::size_type pos;
	std::string token;
	bool done;

        sprintf(command,cmd.c_str());

	// Parse the arguments
	pos=0;
	done=false;
	arg_count=0;

	while (!done) {
	    token=StringUtils::Tokenize(cmd," ",pos);

	    if (token!="") {
		args[arg_count]=strdup(token.c_str());
		arg_count++;
	    }

	    if (pos>=cmd.length()) {
		done=true;
	    }
	}

	args[arg_count]=0; // NULL terminte the arg list

        LoggerWrapper::GetInstance()->Write(LV_STATUS,"exec_system: args parsed like this:");
	for (int i=0; i<arg_count; i++) {
             LoggerWrapper::GetInstance()->Write(LV_STATUS,"exec_system: arg %d is %s",i,args[i]);
	}

        fork_res=fork();

        // Child thread
        if (fork_res==0) {
           int retval;
           LoggerWrapper::GetInstance()->Write(LV_STATUS,"exec_system: within child fork");
	   retval=execvp(args[0],args);
           if (retval<0) {
               LoggerWrapper::GetInstance()->Write(LV_WARNING,"exec_system: child exec returned res=%d, errno=%d, err=%s",retval,errno,strerror(errno));
           }
           exit(0);
        }

        if (fork_res<0) {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"exec_system: fork failed with result %d, errno=%d, err=%s",fork_res,errno,strerror(errno));
        } else {
            LoggerWrapper::GetInstance()->Write(LV_STATUS,"exec_system: fork ok");

            if (wait) {
                int wait_res;
                LoggerWrapper::GetInstance()->Write(LV_STATUS,"exec_system: waiting for child process %d to finish",fork_res);

                wait_res=waitpid(fork_res,NULL,0);

                if (wait_res<0) {
                    LoggerWrapper::GetInstance()->Write(LV_WARNING,"exec_system: wait_res returned res=%d, errno=%d, err=%s",wait_res,errno,strerror(errno));
                } else if (wait_res==fork_res) {
                    LoggerWrapper::GetInstance()->Write(LV_STATUS,"exec_system: detected end of child process OK");
                }
            }
        }

    }

