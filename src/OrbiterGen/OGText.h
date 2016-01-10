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
#ifndef CGText_h
#define CGText_h

#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "Orbiter/DesignObj_Data.h"

class CGText : public DesignObjText
{
public:
	class Row_Text_LS *m_pdrText_LS;
	class Row_DesignObjVariation_Text_Skin_Language *m_pRow_DesignObjVariation_Text_Skin_Language;
	class Row_Orbiter *m_pdrOrbiter;
	class DesignObj_Generator *m_pParent;

	CGText(class DesignObj_Generator *Parent,class Row_DesignObjVariation_Text_Skin_Language *drOVTSL,class Row_Orbiter *drOrbiter);
	static class Row_Text_LS *GetText_LS(int PK_Text,class OrbiterGenerator *pOrbiterGen);
};

#endif
