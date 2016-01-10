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
#include "OrbiterRendererFactory.h"
#include "OrbiterRenderer.h"
#include "Orbiter.h"

#if defined(PROXY_ORBITER) 
	#include "../Proxy_Orbiter/Proxy_OrbiterRenderer_SDL.h"
#elif defined(BLUETOOTH_DONGLE)
	#include "../Bluetooth_Dongle/OrbiterRenderer_SDL_Bluetooth.h"	
#elif defined(MOXI_ORBITER)
	#include "Moxi_Orbiter/OrbiterRenderer_Moxi.h"	
#else
	#ifdef WIN32
		#if defined(POCKETFROG)
			#include "PocketFrog/OrbiterRenderer_PocketFrog.h"
		#else
			#include "SDL/Win32/OrbiterRenderer_SDL_Win32.h"
		#endif
	#else
		#include "Linux/OrbiterRenderer_Linux.h"
	#endif
#endif

/*static*/ OrbiterRenderer *OrbiterRendererFactory::CreateRenderer(Orbiter *pOrbiter)
{
#if defined(PROXY_ORBITER)
	return new Proxy_OrbiterRenderer_SDL(pOrbiter);
#elif defined(BLUETOOTH_DONGLE)
	return new OrbiterRenderer_SDL_Bluetooth(pOrbiter);
#elif defined(MOXI_ORBITER)
	return new OrbiterRenderer_Moxi(pOrbiter);
#else
	#ifdef WIN32
		#if defined(POCKETFROG)
			return new OrbiterRenderer_PocketFrog(pOrbiter);
		#elif defined(ORBITER_OPENGL)
			return new OrbiterRenderer_SDL_Win32(pOrbiter);
		#else
			return new OrbiterRenderer_SDL_Win32(pOrbiter);
		#endif
	#else
		return new OrbiterRenderer_Linux(pOrbiter);
	#endif
#endif
}

