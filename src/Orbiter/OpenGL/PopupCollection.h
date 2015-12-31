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

#ifndef PopupCollection_H_
#define PopupCollection_H_

#include "PopupDescription.h"

#include <map>
#include <string>

#include "Orbiter.h"
#include "Mesh/MeshFrame.h"

class OpenGL3DEngine;
/**
 *	A list of all popups, which has unified management, for OpenGL drawing
 */
class PopupCollection
{
	std::map <std::string, PopupDescription* > Popups;
	std::string Current;
	MeshFrame* Scene;
	OpenGL3DEngine* Engine;

public:
	/**
	 *	Default constructor
	 */
	PopupCollection(OpenGL3DEngine* Engine);
	/**
	 *	Default destructor
	 */
	~PopupCollection();
	
	/**
	 *	Paints a popup with specified ID
	 */
	void PaintPopup(std::string ID, std::string ObjectHash, PlutoPopup *Popup, int EffectID);
	/**
	 *	Hide an existing showed popup
	 */
	void HidePopup(std::string ID, std::string ObjectHash, bool bMarkAsVolation = false);
	/**
	 *	Hides all popups
	 */
	void Reset();
	/**
	 *	Check if exist one popup already keeped
	 */
	bool Exists(std::string ID);
	/**
	 *	Repaints a popup
	 */
	void ResetObjectInPopup(std::string PopupID, class DesignObj_Orbiter *pObj);
};

#endif
