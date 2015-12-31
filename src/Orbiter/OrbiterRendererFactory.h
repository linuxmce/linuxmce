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
#ifndef __ORBITER_RENDERER_FACTORY__
#define __ORBITER_RENDERER_FACTORY__

namespace DCE
{
	class Orbiter;
}

using namespace DCE;
class OrbiterRenderer;

class OrbiterRendererFactory
{
public:
	static OrbiterRenderer *CreateRenderer(Orbiter *pOrbiter);
};

#endif //__ORBITER_RENDERER_FACTORY__

