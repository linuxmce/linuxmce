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
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __PENDING_GL_EFFECTS_H__
#define __PENDING_GL_EFFECTS_H__

struct PendingGLEffects //inline
{
	int m_nOnScreenTransitionEffectID;
	int m_nOffScreenTransitionEffectID;
	int m_nOnSelectWithChangeEffectID;

	PendingGLEffects() 
	{
		Reset();
	}

	void Reset()
	{
		m_nOnScreenTransitionEffectID = 0;
		m_nOffScreenTransitionEffectID = 0;
		m_nOnSelectWithChangeEffectID = 0;
	}
};

#endif
