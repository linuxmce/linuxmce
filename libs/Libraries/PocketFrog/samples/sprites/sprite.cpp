// Sprite class implementation for demo
#include <PocketFrog.h>
#include "sprite.h"
using namespace Frog;
//Sprite::SpriteData(){};
	Sprite::Sprite()
	{

	};//constructor
	Sprite::~Sprite()
	{
	delete Collide;//release memory
	}//destructor
void Sprite::SetDisplayDevice(DisplayDevice *display)
{
    m_display = display;
}//end set display device

void Sprite::LoadSpriteData(unsigned resourceID,int _f)
{
    SpriteData = LoadImage(m_display, resourceID);//load from resource
	SpriteData->SetColorMask( Color( 0,0,0) );//transparent color assumed black
	FrameTot = _f;//noof frames in the btmap
	FrameCount = FrameTot-1;//for 0123 instead of 1234
	FrameArray = new Rect[_f] ;//array of Rects for screen drawing
	Width = SpriteData->GetWidth() / FrameTot;//Width of 1 Sprite
	Depth = SpriteData->GetHeight();//always 1 high in bitmap

	for (int x = 0; x < FrameTot; ++x)
	{// make a Rect for each frame for drawing & collision testing
		FrameArray[x].Set(0+(x*Width), 0,Width+(x*Width)-1,Depth-1);
	}
}//end load data
void Sprite::InitSprite(int _x,int _y,int _xm,int _ym,int _xmax,int _xmin,int _s, bool _on)
{
	Xpos = _x;//default init
	Ypos = _y;//default init
	Xmin = _xmin;//makes sprite start just off screen
	Xmax = _xmax;//and end just off the screen
	// Ymin & Ymax would also be needed for most uses
	Speed = _s;// rate at which we animate frames
	Cnt1 = Speed;// init animate timer
	Xmove = _xm;//neg or pos offset (how far we move sprite each frame)
	Ymove = _ym;
	On = _on;// true or false = on or off
	Collide = new Rect;//to maintain onscreen position for collision detection
	Collide->Set(Xpos,Ypos,Xpos+(Width-1),Ypos+(Depth-1));// for Sprite intersect testing
	Alpha = 0xFF;//default no blending

}// end init sprite
//***************************************
void Sprite::Draw()
{
	if(this->On)// if state is on then draw
	{
		m_display->SetBlending(Alpha);// level of alpha blending
		m_display->Blit(Xpos,Ypos,SpriteData,&FrameArray[FrameCount]);//draw 1 frame
		m_display->SetBlending(0xFF);//no blend
	}
	}//end sprite draw
//***************************************
void Sprite::Move()
{
	if(Xmove > 0 && Xpos > Xmax)//moving right
	{
		Xpos = Xmin;
//		Ypos;
	}
	if(Xmove < 0 && Xpos < Xmin)//moving left
	{
		Xpos = Xmax;
//		Ypos;
	}

	Cnt1--;				//decrement movement counter
	if(Cnt1 == 0)		//when it reaches zero we move the Sprite
	{
		Xpos += Xmove;	//Xmove could be positive or negative
		Ypos += Ymove;	//Ymove only used with heart
		Cnt1 = Speed;	//countdown counter
		FrameCount--;	//decrement movement counter
		if(FrameCount < 0)//when it reaches zero we change the frame
		{
			FrameCount = FrameTot -1;//eg: if 4 frames draw 0,1,2,3
		}//endif
	}//end if Cnt1
// update the collision Rect
	Collide->Move(Xpos,Ypos);//update Rect position

}//end move
//****************************************************
/*Note: The Hit function makes use of 'this' to indicate that we are dealing with
variables of the calling object (which in this demo is always cavegirl)
While you can mostly get away without it it's good to know it actually exists*/

bool Sprite::Hit(Sprite& _sp)//_sp is sprite to be checked for intersect
{
/* this method uses the source sprite's collision Rect and a chosen Point
on the target Sprite to test for collision using 'Contains'.
If you want to use this method you could maintain the Point in SpriteMove	
*/
	int Xm = _sp.Xpos + (_sp.Width/2);// half width of sprite
	int Ym = _sp.Ypos + (_sp.Depth/2);// half height of sprite
	Point HotSpot(Xm,Ym);//hotspot on target Sprite

	if( Collide->Contains(HotSpot))
	{				//** To try the other collision code below
		return true;//** Comment out this line & remove // from the return true below
	}//end if intersects

/* this method uses the Collide Rects & a temp Rect to find the intersection.
	The temp Rect has the coordinates of the intersecting area.This demo is 'tuned'
	for the above contains method so you will see a wrong effect with the heart in this one
*/
Rect R1(this->Xpos,this->Ypos,this->Xpos+this->Width-1,this->Ypos+this->Depth-1);
Rect R2(_sp.Xpos,_sp.Ypos,_sp.Xpos+_sp.Width-1,_sp.Ypos+_sp.Depth-1);
// Rect constructor does not like Rect R1(this.Collide)???
Rect intersection(R1);//make a temp rect to hold intersect info
intersection.Intersect(R2);// test for an intersect

if (!intersection.IsEmpty())// if temp Rect intersection is not empty
{
//   return true;// intersect has occured
}
}//end hit
//*************************************************************************
