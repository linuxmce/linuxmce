Put the SGE source code here!

Should look like:

[docs]
[examples]
[VisualC]
License
Makefile
Makefile.conf
Readme
sge.h
...
...


sge_config.h should look like this if you want FreeType2 support:

/* SGE Config header (generated automatically) */
#define SGE_VER 010629


or like this to build without FreeType2:

/* SGE Config header (generated automatically) */
#define SGE_VER 010629
#define _SGE_NOTTF


You can also build with SDL_Image (http://www.libsdl.org/projects/SDL_image/index.html)
support but you will have to add this to the project yourself. In this case the header
should look like this:

/* SGE Config header (generated automatically) */
#define SGE_VER 010629
#define _SGE_HAVE_IMG


/Anders Lindström