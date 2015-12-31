/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubyStdStreamRedirector.h"

#include <unistd.h>

namespace EMBRUBY {

RubyStdStreamRedirector::RubyStdStreamRedirector() {
	savstream_ = rstream_ = ostream_ = -1;
}

RubyStdStreamRedirector::RubyStdStreamRedirector(int stdstream) {
	Open(stdstream);
}

void 
RubyStdStreamRedirector::Open(int stdstream) {
	int pipes[2];
	pipe(pipes);
	
	savstream_ = dup(stdstream);
	
	switch(stdstream) {
	case HF_STDIN:
		rstream_ = pipes[1];
		ostream_ = pipes[0];
		dup2(ostream_, HF_STDIN);
		break;
	case HF_STDOUT:
		rstream_ = pipes[0];
		ostream_ = pipes[1];
		dup2(ostream_, HF_STDOUT);
		break;
	case HF_STDERR:
		rstream_ = pipes[0];
		ostream_ = pipes[1];
		dup2(ostream_, HF_STDERR);
		break;
	default:
		rstream_ = pipes[0];
		ostream_ = pipes[1];
		break;
	}
}

RubyStdStreamRedirector::~RubyStdStreamRedirector() {
	Close();
}

void 
RubyStdStreamRedirector::Close() {
	close(rstream_);
	close(ostream_);
}

};
