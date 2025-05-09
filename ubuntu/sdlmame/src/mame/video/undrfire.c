#include "emu.h"
#include "video/taitoic.h"
#include "includes/undrfire.h"



/******************************************************************/

void undrfire_state::video_start()
{
	int i;

	m_spritelist = auto_alloc_array(machine(), struct tempsprite, 0x4000);

	for (i = 0; i < 16384; i++) /* Fix later - some weird colours in places */
		palette_set_color(machine(), i, MAKE_RGB(0,0,0));
}

/***************************************************************
            SPRITE DRAW ROUTINES

We draw a series of small tiles ("chunks") together to
create each big sprite. The spritemap rom provides the lookup
table for this. The game hardware looks up 16x16 sprite chunks
from the spritemap rom, creating a 64x64 sprite like this:

     0  1  2  3
     4  5  6  7
     8  9 10 11
    12 13 14 15

(where the number is the word offset into the spritemap rom).
It can also create 32x32 sprites.

NB: unused portions of the spritemap rom contain hex FF's.
It is a useful coding check to warn in the log if these
are being accessed. [They can be inadvertently while
spriteram is being tested, take no notice of that.]

Heavy use is made of sprite zooming.

        ***

    Sprite table layout (4 long words per entry)

    ------------------------------------------
     0 | ........ x....... ........ ........ | Flip X
     0 | ........ .xxxxxxx ........ ........ | ZoomX
     0 | ........ ........ .xxxxxxx xxxxxxxx | Sprite Tile
       |                                     |
     2 | ........ ....xx.. ........ ........ | Sprite/tile priority [*]
     2 | ........ ......xx xxxxxx.. ........ | Palette bank
     2 | ........ ........ ......xx xxxxxxxx | X position
       |                                     |
     3 | ........ .....x.. ........ ........ | Sprite size (0=32x32, 1=64x64)
     3 | ........ ......x. ........ ........ | Flip Y
     3 | ........ .......x xxxxxx.. ........ | ZoomY
     3 | ........ ........ ......xx xxxxxxxx | Y position
    ------------------------------------------

    [*  00=over BG0, 01=BG1, 10=BG2, 11=BG3 ]
    [or 00=over BG1, 01=BG2, 10=BG3, 11=BG3 ]

***************************************************************/

static void draw_sprites(running_machine &machine, bitmap_ind16 &bitmap,const rectangle &cliprect,const int *primasks,int x_offs,int y_offs)
{
	undrfire_state *state = machine.driver_data<undrfire_state>();
	UINT32 *spriteram32 = state->m_spriteram;
	UINT16 *spritemap = (UINT16 *)state->memregion("user1")->base();
	int offs, data, tilenum, color, flipx, flipy;
	int x, y, priority, dblsize, curx, cury;
	int sprites_flipscreen = 0;
	int zoomx, zoomy, zx, zy;
	int sprite_chunk,map_offset,code,j,k,px,py;
	int dimension,total_chunks,bad_chunks;

	/* pdrawgfx() needs us to draw sprites front to back, so we have to build a list
	   while processing sprite ram and then draw them all at the end */
	struct tempsprite *sprite_ptr = state->m_spritelist;

	for (offs = (state->m_spriteram.bytes()/4-4);offs >= 0;offs -= 4)
	{
		data = spriteram32[offs+0];
		flipx =    (data & 0x00800000) >> 23;
		zoomx =    (data & 0x007f0000) >> 16;
		tilenum =  (data & 0x00007fff);

		data = spriteram32[offs+2];
		priority = (data & 0x000c0000) >> 18;
		color =    (data & 0x0003fc00) >> 10;
		x =        (data & 0x000003ff);

		data = spriteram32[offs+3];
		dblsize =  (data & 0x00040000) >> 18;
		flipy =    (data & 0x00020000) >> 17;
		zoomy =    (data & 0x0001fc00) >> 10;
		y =        (data & 0x000003ff);

		color |= (0x100 + (priority << 6));     /* priority bits select color bank */
		color /= 2;     /* as sprites are 5bpp */
		flipy = !flipy;
		y = (-y &0x3ff);

		if (!tilenum) continue;

		flipy = !flipy;
		zoomx += 1;
		zoomy += 1;

		y += y_offs;

		/* treat coords as signed */
		if (x>0x340) x -= 0x400;
		if (y>0x340) y -= 0x400;

		x -= x_offs;

		bad_chunks = 0;
		dimension = ((dblsize*2) + 2);  // 2 or 4
		total_chunks = ((dblsize*3) + 1) << 2;  // 4 or 16
		map_offset = tilenum << 2;

		{
			for (sprite_chunk=0;sprite_chunk<total_chunks;sprite_chunk++)
			{
				j = sprite_chunk / dimension;   /* rows */
				k = sprite_chunk % dimension;   /* chunks per row */

				px = k;
				py = j;
				/* pick tiles back to front for x and y flips */
				if (flipx)  px = dimension-1-k;
				if (flipy)  py = dimension-1-j;

				code = spritemap[map_offset + px + (py<<(dblsize+1))];

				if (code==0xffff)
				{
					bad_chunks += 1;
					continue;
				}

				curx = x + ((k*zoomx)/dimension);
				cury = y + ((j*zoomy)/dimension);

				zx= x + (((k+1)*zoomx)/dimension) - curx;
				zy= y + (((j+1)*zoomy)/dimension) - cury;

				if (sprites_flipscreen)
				{
					/* -zx/y is there to fix zoomed sprite coords in screenflip.
					   drawgfxzoom does not know to draw from flip-side of sprites when
					   screen is flipped; so we must correct the coords ourselves. */

					curx = 320 - curx - zx;
					cury = 256 - cury - zy;
					flipx = !flipx;
					flipy = !flipy;
				}

				sprite_ptr->gfx = 0;
				sprite_ptr->code = code;
				sprite_ptr->color = color;
				sprite_ptr->flipx = !flipx;
				sprite_ptr->flipy = flipy;
				sprite_ptr->x = curx;
				sprite_ptr->y = cury;
				sprite_ptr->zoomx = zx << 12;
				sprite_ptr->zoomy = zy << 12;

				if (primasks)
				{
					sprite_ptr->primask = primasks[priority];

					sprite_ptr++;
				}
				else
				{
					drawgfxzoom_transpen(bitmap,cliprect,machine.gfx[sprite_ptr->gfx],
							sprite_ptr->code,
							sprite_ptr->color,
							sprite_ptr->flipx,sprite_ptr->flipy,
							sprite_ptr->x,sprite_ptr->y,
							sprite_ptr->zoomx,sprite_ptr->zoomy,0);
				}
			}
		}

		if (bad_chunks)
logerror("Sprite number %04x had %02x invalid chunks\n",tilenum,bad_chunks);
	}

	/* this happens only if primsks != NULL */
	while (sprite_ptr != state->m_spritelist)
	{
		sprite_ptr--;

		pdrawgfxzoom_transpen(bitmap,cliprect,machine.gfx[sprite_ptr->gfx],
				sprite_ptr->code,
				sprite_ptr->color,
				sprite_ptr->flipx,sprite_ptr->flipy,
				sprite_ptr->x,sprite_ptr->y,
				sprite_ptr->zoomx,sprite_ptr->zoomy,
				machine.priority_bitmap,sprite_ptr->primask,0);
	}
}


static void draw_sprites_cbombers(running_machine &machine, bitmap_ind16 &bitmap,const rectangle &cliprect,const int *primasks,int x_offs,int y_offs)
{
	undrfire_state *state = machine.driver_data<undrfire_state>();
	UINT32 *spriteram32 = state->m_spriteram;
	UINT16 *spritemap = (UINT16 *)state->memregion("user1")->base();
	UINT8 *spritemapHibit = (UINT8 *)state->memregion("user2")->base();

	int offs, data, tilenum, color, flipx, flipy;
	int x, y, priority, dblsize, curx, cury;
	int sprites_flipscreen = 0;
	int zoomx, zoomy, zx, zy;
	int sprite_chunk,map_offset,code,j,k,px,py;
	int dimension,total_chunks;

	/* pdrawgfx() needs us to draw sprites front to back, so we have to build a list
	   while processing sprite ram and then draw them all at the end */
	struct tempsprite *sprite_ptr = state->m_spritelist;

	for (offs = (state->m_spriteram.bytes()/4-4);offs >= 0;offs -= 4)
	{
		data = spriteram32[offs+0];
		flipx =    (data & 0x00800000) >> 23;
		zoomx =    (data & 0x007f0000) >> 16;
		tilenum =  (data & 0x0000ffff);

		data = spriteram32[offs+2];
		priority = (data & 0x000c0000) >> 18;
		color =    (data & 0x0003fc00) >> 10;
		x =        (data & 0x000003ff);

		data = spriteram32[offs+3];
		dblsize =  (data & 0x00040000) >> 18;
		flipy =    (data & 0x00020000) >> 17;
		zoomy =    (data & 0x0001fc00) >> 10;
		y =        (data & 0x000003ff);

		color |= (/*0x100 +*/ (priority << 6));     /* priority bits select color bank */

		color /= 2;     /* as sprites are 5bpp */
		flipy = !flipy;

		if (!tilenum) continue;

		zoomx += 1;
		zoomy += 1;

		y += y_offs;

		/* treat coords as signed */
		if (x>0x340) x -= 0x400;
		if (y>0x340) y -= 0x400;

		x -= x_offs;

		dimension = ((dblsize*2) + 2);  // 2 or 4
		total_chunks = ((dblsize*3) + 1) << 2;  // 4 or 16
		map_offset = tilenum << 2;

		for (sprite_chunk = 0; sprite_chunk < total_chunks; sprite_chunk++)
		{
			int map_addr;

			j = sprite_chunk / dimension;   /* rows */
			k = sprite_chunk % dimension;   /* chunks per row */

			px = k;
			py = j;
			/* pick tiles back to front for x and y flips */
			if (flipx)  px = dimension-1-k;
			if (flipy)  py = dimension-1-j;

			map_addr = map_offset + px + (py << (dblsize + 1));
			code =  (spritemapHibit[map_addr] << 16) | spritemap[map_addr];

			curx = x + ((k*zoomx)/dimension);
			cury = y + ((j*zoomy)/dimension);

			zx= x + (((k+1)*zoomx)/dimension) - curx;
			zy= y + (((j+1)*zoomy)/dimension) - cury;

			if (sprites_flipscreen)
			{
				/* -zx/y is there to fix zoomed sprite coords in screenflip.
				       drawgfxzoom does not know to draw from flip-side of sprites when
				       screen is flipped; so we must correct the coords ourselves. */

				curx = 320 - curx - zx;
				cury = 256 - cury - zy;
				flipx = !flipx;
				flipy = !flipy;
			}

			sprite_ptr->gfx = 0;
			sprite_ptr->code = code;
			sprite_ptr->color = color;
			sprite_ptr->flipx = !flipx;
			sprite_ptr->flipy = flipy;
			sprite_ptr->x = curx;
			sprite_ptr->y = cury;
			sprite_ptr->zoomx = zx << 12;
			sprite_ptr->zoomy = zy << 12;

			if (primasks)
			{
				sprite_ptr->primask = primasks[priority];
				sprite_ptr++;
			}
			else
			{
				drawgfxzoom_transpen(bitmap,cliprect,machine.gfx[sprite_ptr->gfx],
						sprite_ptr->code,
						sprite_ptr->color,
						sprite_ptr->flipx,sprite_ptr->flipy,
						sprite_ptr->x,sprite_ptr->y,
						sprite_ptr->zoomx,sprite_ptr->zoomy,0);
			}
		}
	}

	/* this happens only if primsks != NULL */
	while (sprite_ptr != state->m_spritelist)
	{
		sprite_ptr--;

		pdrawgfxzoom_transpen(bitmap,cliprect,machine.gfx[sprite_ptr->gfx],
				sprite_ptr->code,
				sprite_ptr->color,
				sprite_ptr->flipx,sprite_ptr->flipy,
				sprite_ptr->x,sprite_ptr->y,
				sprite_ptr->zoomx,sprite_ptr->zoomy,
				machine.priority_bitmap,sprite_ptr->primask,0);
	}
}


/**************************************************************
                SCREEN REFRESH
**************************************************************/

UINT32 undrfire_state::screen_update_undrfire(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	device_t *tc0100scn = machine().device("tc0100scn");
	device_t *tc0480scp = machine().device("tc0480scp");
	address_space &space = machine().driver_data()->generic_space();
	UINT8 layer[5];
	UINT8 pivlayer[3];
	UINT16 priority;

#ifdef MAME_DEBUG
	if (machine().input().code_pressed_once (KEYCODE_X))
	{
		m_dislayer[5] ^= 1;
		popmessage("piv text: %01x",m_dislayer[5]);
	}
	if (machine().input().code_pressed_once (KEYCODE_C))
	{
		m_dislayer[0] ^= 1;
		popmessage("bg0: %01x",m_dislayer[0]);
	}

	if (machine().input().code_pressed_once (KEYCODE_V))
	{
		m_dislayer[1] ^= 1;
		popmessage("bg1: %01x",m_dislayer[1]);
	}

	if (machine().input().code_pressed_once (KEYCODE_B))
	{
		m_dislayer[2] ^= 1;
		popmessage("bg2: %01x",m_dislayer[2]);
	}

	if (machine().input().code_pressed_once (KEYCODE_N))
	{
		m_dislayer[3] ^= 1;
		popmessage("bg3: %01x",m_dislayer[3]);
	}

	if (machine().input().code_pressed_once (KEYCODE_M))
	{
		m_dislayer[4] ^= 1;
		popmessage("sprites: %01x",m_dislayer[4]);
	}
#endif

	tc0100scn_tilemap_update(tc0100scn);
	tc0480scp_tilemap_update(tc0480scp);

	priority = tc0480scp_get_bg_priority(tc0480scp);

	layer[0] = (priority & 0xf000) >> 12;   /* tells us which bg layer is bottom */
	layer[1] = (priority & 0x0f00) >>  8;
	layer[2] = (priority & 0x00f0) >>  4;
	layer[3] = (priority & 0x000f) >>  0;   /* tells us which is top */
	layer[4] = 4;   /* text layer always over bg layers */

	pivlayer[0] = tc0100scn_bottomlayer(tc0100scn);
	pivlayer[1] = pivlayer[0] ^ 1;
	pivlayer[2] = 2;

	machine().priority_bitmap.fill(0, cliprect);
	bitmap.fill(0, cliprect);   /* wrong color? */


/* The "PIV" chip seems to be a renamed TC0100SCN. It has a
   bottom layer usually full of bright garish colors that
   vaguely mimic the structure of the layers on top. Seems
   pointless - it's always hidden by other layers. Does it
   serve some blending pupose ? */

	tc0100scn_tilemap_draw(tc0100scn, bitmap, cliprect, pivlayer[0], TILEMAP_DRAW_OPAQUE, 0);
	tc0100scn_tilemap_draw(tc0100scn, bitmap, cliprect, pivlayer[1], 0, 0);

#ifdef MAME_DEBUG
	if (m_dislayer[layer[0]]==0)
#endif
	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[0], 0, 1);

#ifdef MAME_DEBUG
	if (m_dislayer[layer[1]]==0)
#endif
	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[1], 0, 2);

#ifdef MAME_DEBUG
	if (m_dislayer[layer[2]]==0)
#endif
	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[2], 0, 4);

#ifdef MAME_DEBUG
	if (m_dislayer[layer[3]]==0)
#endif
	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[3], 0, 8);

#ifdef MAME_DEBUG
	if (m_dislayer[4]==0)
#endif
	/* Sprites have variable priority (we kludge this on road levels) */
	{
		if ((tc0480scp_pri_reg_r(tc0480scp, space, 0) & 0x3) == 3)  /* on road levels kludge sprites up 1 priority */
		{
			static const int primasks[4] = {0xfff0, 0xff00, 0x0, 0x0};
			draw_sprites(machine(), bitmap, cliprect, primasks, 44, -574);
		}
		else
		{
			static const int primasks[4] = {0xfffc, 0xfff0, 0xff00, 0x0};
			draw_sprites(machine(), bitmap, cliprect, primasks, 44, -574);
		}
	}

#ifdef MAME_DEBUG
	if (m_dislayer[5]==0)
#endif
	tc0100scn_tilemap_draw(tc0100scn, bitmap, cliprect, pivlayer[2], 0, 0); /* piv text layer */

	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[4], 0, 0);    /* TC0480SCP text layer */

	/* See if we should draw artificial gun targets */
	/* (not yet implemented...) */

	if (machine().root_device().ioport("FAKE")->read() & 0x1)   /* Fake DSW */
	{
		popmessage("Gunsights on");
	}

/* Enable this to see rotation (?) control words */
#if 0
	{
		char buf[80];
		int i;

		for (i = 0; i < 8; i += 1)
		{
			sprintf (buf, "%02x: %04x", i, m_rotate_ctrl[i]);
			ui_draw_text (buf, 0, i*8);
		}
	}
#endif
	return 0;
}


UINT32 undrfire_state::screen_update_cbombers(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	device_t *tc0100scn = machine().device("tc0100scn");
	device_t *tc0480scp = machine().device("tc0480scp");
	address_space &space = machine().driver_data()->generic_space();
	UINT8 layer[5];
	UINT8 pivlayer[3];
	UINT16 priority;

#ifdef MAME_DEBUG
	if (machine().input().code_pressed_once (KEYCODE_X))
	{
		m_dislayer[5] ^= 1;
		popmessage("piv text: %01x",m_dislayer[5]);
	}
	if (machine().input().code_pressed_once (KEYCODE_C))
	{
		m_dislayer[0] ^= 1;
		popmessage("bg0: %01x",m_dislayer[0]);
	}

	if (machine().input().code_pressed_once (KEYCODE_V))
	{
		m_dislayer[1] ^= 1;
		popmessage("bg1: %01x",m_dislayer[1]);
	}

	if (machine().input().code_pressed_once (KEYCODE_B))
	{
		m_dislayer[2] ^= 1;
		popmessage("bg2: %01x",m_dislayer[2]);
	}

	if (machine().input().code_pressed_once (KEYCODE_N))
	{
		m_dislayer[3] ^= 1;
		popmessage("bg3: %01x",m_dislayer[3]);
	}

	if (machine().input().code_pressed_once (KEYCODE_M))
	{
		m_dislayer[4] ^= 1;
		popmessage("sprites: %01x",m_dislayer[4]);
	}
#endif

	tc0100scn_tilemap_update(tc0100scn);
	tc0480scp_tilemap_update(tc0480scp);

	priority = tc0480scp_get_bg_priority(tc0480scp);

	layer[0] = (priority & 0xf000) >> 12;   /* tells us which bg layer is bottom */
	layer[1] = (priority & 0x0f00) >>  8;
	layer[2] = (priority & 0x00f0) >>  4;
	layer[3] = (priority & 0x000f) >>  0;   /* tells us which is top */
	layer[4] = 4;   /* text layer always over bg layers */

	pivlayer[0] = tc0100scn_bottomlayer(tc0100scn);
	pivlayer[1] = pivlayer[0] ^ 1;
	pivlayer[2] = 2;

	machine().priority_bitmap.fill(0, cliprect);
	bitmap.fill(0, cliprect);   /* wrong color? */


/* The "PIV" chip seems to be a renamed TC0100SCN. It has a
   bottom layer usually full of bright garish colors that
   vaguely mimic the structure of the layers on top. Seems
   pointless - it's always hidden by other layers. Does it
   serve some blending pupose ? */

	tc0100scn_tilemap_draw(tc0100scn, bitmap, cliprect, pivlayer[0], TILEMAP_DRAW_OPAQUE, 0);
	tc0100scn_tilemap_draw(tc0100scn, bitmap, cliprect, pivlayer[1], 0, 0);

#ifdef MAME_DEBUG
	if (m_dislayer[layer[0]]==0)
#endif
	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[0], 0, 1);

#ifdef MAME_DEBUG
	if (m_dislayer[layer[1]]==0)
#endif
	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[1], 0, 2);

#ifdef MAME_DEBUG
	if (m_dislayer[layer[2]]==0)
#endif
	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[2], 0, 4);

#ifdef MAME_DEBUG
	if (m_dislayer[layer[3]]==0)
#endif
	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[3], 0, 8);

#ifdef MAME_DEBUG
	if (m_dislayer[4]==0)
#endif
	/* Sprites have variable priority (we kludge this on road levels) */
	{
		if ((tc0480scp_pri_reg_r(tc0480scp, space, 0) & 0x3) == 3)  /* on road levels kludge sprites up 1 priority */
		{
			static const int primasks[4] = {0xfff0, 0xff00, 0x0, 0x0};
			draw_sprites_cbombers(machine(), bitmap, cliprect, primasks, 80, -208);
		}
		else
		{
			static const int primasks[4] = {0xfffc, 0xfff0, 0xff00, 0x0};
			draw_sprites_cbombers(machine(), bitmap, cliprect, primasks, 80, -208);
		}
	}

#ifdef MAME_DEBUG
	if (m_dislayer[5]==0)
#endif
	tc0100scn_tilemap_draw(tc0100scn, bitmap, cliprect, pivlayer[2], 0, 0); /* piv text layer */

	tc0480scp_tilemap_draw(tc0480scp, bitmap, cliprect, layer[4], 0, 0);    /* TC0480SCP text layer */

/* Enable this to see rotation (?) control words */
#if 0
	{
		char buf[80];
		int i;

		for (i = 0; i < 8; i += 1)
		{
			sprintf (buf, "%02x: %04x", i, m_rotate_ctrl[i]);
			ui_draw_text (buf, 0, i*8);
		}
	}
#endif
	return 0;
}
