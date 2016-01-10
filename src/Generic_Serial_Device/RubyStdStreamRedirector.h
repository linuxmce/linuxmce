/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyStdStreamRedirector.h
Handle ruby streams?
*/
#ifndef EMBRUBYRUBYSTDSTREAMREDIRECTOR_H
#define EMBRUBYRUBYSTDSTREAMREDIRECTOR_H

/** @namespace EMBRUBY
Embedded Ruby ???
*/
namespace EMBRUBY {

#define HF_STDIN 	0
#define HF_STDOUT 	1
#define HF_STDERR 	2

/** @class RubyStdStreamRedirector
Redirects??
*/
class RubyStdStreamRedirector{
public:
    RubyStdStreamRedirector();
    RubyStdStreamRedirector(int stdstream);
    ~RubyStdStreamRedirector();

	void Open(int stdstream);
	void Close();

public:
	operator int() {
		return rstream_;
	}

	int getOriginalStream() {
		return savstream_;
	}

private:
	int rstream_; /**< redirected tail. */
	int ostream_; /**< the other tail. */

	int savstream_;
};

};

#endif
