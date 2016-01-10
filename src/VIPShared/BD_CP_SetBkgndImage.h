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
#ifndef BD_CP_SetBkgndImage_h
#define BD_CP_SetBkgndImage_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

/*
 *
 *	BD_CP_SetBkgndImage command - sets new background on smartphone
 *
 */
class BD_CP_SetBkgndImage : public BDCommand
{
	unsigned char m_iImageType;
	char *m_pImage;
	unsigned long m_ImageSize;

public:
	BD_CP_SetBkgndImage()  { m_ImageSize = 0; m_pImage = NULL; m_iImageType = 0; }
	BD_CP_SetBkgndImage( unsigned char ImageType, unsigned long ImageSize,const char *pImage );

	~BD_CP_SetBkgndImage();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SET_BKGND_IMAGE; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Set background Image"; };
};
#endif
