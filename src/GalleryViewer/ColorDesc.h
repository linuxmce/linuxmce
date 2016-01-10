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
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef COLOR_DESC_H
#define COLOR_DESC_H

class TColorDesc
{
	unsigned char R, G, B;
public:
	TColorDesc(unsigned char Red, unsigned char Green, unsigned char Blue);
	virtual ~TColorDesc(void);

	unsigned char GetRed() {
		return R;
	}

	unsigned char GetGreen() {
		return G;
	}

	unsigned char GetBlue() {
		return B;
	}

};

#endif
