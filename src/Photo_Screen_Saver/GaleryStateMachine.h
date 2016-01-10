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
#ifndef GaleryStateMachine_H_
#define GaleryStateMachine_H_

#define STATUS_STARTUP 0
#define STATUS_LOADED_IMAGES 1
#define STATUS_ANIMATE_CURRENT_IMAGE 2
#define STATUS_FADE_IMAGES 3
#define STATUS_PAINTING_STATIC_IMAGE 4

class GaleryStateMachine
{
	int Status;
public:
	GaleryStateMachine(void);
	~GaleryStateMachine(void);
	int GetStatus(void);
	void LoadedImages(void);
	void AnimateCurrentImage(void);
	void FadeImages(void);
	void AboutToPaintStaticImage(void);
};

#endif 
