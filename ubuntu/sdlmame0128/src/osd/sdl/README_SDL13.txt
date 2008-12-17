WIP: sdl 1.3

The following modes are working:

Soft:
=====

./mamed -mt -video soft  -ym none -numscreens 2 mario

OpenGL:
=======

Plain opengl does work. Anything more advanced like pbo, fbo or glsl will 
most probably not.

	./mamed -mt -video opengl mario -ym none -nogl_pbo -numscreens 2

YUV - modes:
============

	./mamed -mt -video soft -rd software -ym yuy2 -numscreens 2 mario

The "-rd" overwrites the default which is built-in opengl. This renderer
does not support yuv modes. The software driver does support them non-accelarated.
This has been left in for the time Xv is once again implemented in SDL.

Once my new directfb driver has made it into SDL, you may use

	./mamed -mt -video soft -sm yuy2 -vd directfb -rd directfb mario
	
for accelerated blitting on the framebuffer - provided directfb supports it. 
At least my Radeon R480 is supported.

-video soft and -scale_mode (-sm)
=================================

sdlmame supports 7 scale modes using textures in -video soft:

none: All rendering/scaling in software.

hwblit: Rendering in software/scaling with hardware (if supported)

hwbest: Rendering in software/antialiased scaling with hardware (if supported)

yv12, yv12x2, yuy2, yuy2x2: 
Rendering in software / scaling with hardware (if supported)

Whether these are actually hardware accelerated depends on the SDL driver
and the hardware. The current SDL X11 driver needs opengl for rendering but
does not support yuv textures. The "to-be-submitted" SDL directfb driver 
supports all above if the hardware supports it. However, only one YUV-texture
per display is supported. The second window consequently will get "software" 
YUV blitting.

	  