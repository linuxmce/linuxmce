//definition of a Sprite class for the animation demo
#include <PocketFrog.h>

using namespace Frog;

class Sprite
{
public:
    void SetDisplayDevice(DisplayDevice* display);
    void LoadSpriteData(unsigned resourceID,int _f);//sprite frames in resource
	void InitSprite(int _x,int _y,int _xm,int _ym,int _xmax,int _xmin,int _s,bool _on);//init
    void Draw();//Blit to display (backbuffer usually)
	void Move();// update x, y & frame counter etc
	bool Hit(Sprite& _sp);//Intersect check
//    void SetSpriteData(Surface* sprites);//unused sample
    Sprite();//constructor
    virtual ~Sprite();//destructor
	int Xpos;//   variables accessible publicly
	int Ypos;//
	bool On;// on or off
	int FrameCount;//which frame to draw
	int FrameTot;//_f
	int Alpha;//amount of alpha blending
	Rect *FrameArray;//an empty Rect array pointer
	Rect *Collide;// Rect for collision check
//Note  the	Rect FrameArray[FrameCount] is created in Sprite()

private:
    DisplayDevice* m_display;
    Surface* SpriteData;
	int Width;//width of 1 frame of loaded image
	int Depth;//height of the loaded image/bitmap
	int Speed;//how often to draw sprite
	int Xmove;//amount to add or subtract from Xpos
	int Ymove;//amount to ...Ypos
	int Xmax;//furthest right
	int Xmin;//furthest left
	int Cnt1;//counter one
	int Cnt2;//counter two
};