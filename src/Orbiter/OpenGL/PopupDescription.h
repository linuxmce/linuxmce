/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/

#ifndef PopupDescription_H_
#define PopupDescription_H_

#include <string>
using namespace std;

class MeshFrame;
class OpenGL3DEngine;
class PlutoRectangle;

/**
 *	Class that keeps popup management
 */
class PopupDescription
{
	string ObjectID;
	string ObjectHash;
	OpenGL3DEngine* Engine;
	PlutoRectangle* HighLight;
    MeshFrame *PopupFrame;

public:
	/**
	 *	Default constructor 
	 */
	PopupDescription(OpenGL3DEngine* Engine, string ID, string ObjHash, PlutoRectangle* HighLight);
	virtual ~PopupDescription(void);

	/**
	 *	Removes the popup from screne tree
	 */
	void Hide();
	/**
	 *	Adds to scene the current popup
	 */
	void Show();
	/**
	 *	Returns the complete geometry of the popup
	 */
	MeshFrame *Frame() { return PopupFrame; }
};

#endif
