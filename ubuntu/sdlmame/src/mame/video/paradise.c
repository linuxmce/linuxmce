/***************************************************************************

               -= Paradise / Target Ball / Torus =-

               driver by   Luca Elia (l.elia@tin.it)


Note:   if MAME_DEBUG is defined, pressing Z with:

        Q       shows the background layer
        W       shows the midground layer
        E       shows the foreground layer
        R       shows the pixmap layer
        A       shows sprites

        There are 4 Fixed 256 x 256 Layers.

        Background tiles are 8x8x4 with a register selecting which
        color code to use.

        midground and foreground tiles are 8x8x8 with no color code.
        Then there's a 16 color pixel layer.

        Bog standard 16x16x8 sprites, apparently with no color code nor flipping.

***************************************************************************/

#include "emu.h"
#include "includes/paradise.h"

WRITE8_MEMBER(paradise_state::paradise_flipscreen_w)
{
	flip_screen_set(data ? 0 : 1);
}

WRITE8_MEMBER(paradise_state::tgtball_flipscreen_w)
{
	flip_screen_set(data ? 1 : 0);
}

/* Note: Penky updates pixel palette bank register BEFORE actually writing to the paletteram. */
static void update_pix_palbank(running_machine &machine)
{
	paradise_state *state = machine.driver_data<paradise_state>();
	int i;

	for (i = 0; i < 15; i++)
		palette_set_color_rgb(machine, 0x800 + i, state->m_paletteram[0x200 + state->m_pixbank + i + 0x800 * 0], state->m_paletteram[0x200 + state->m_pixbank + i + 0x800 * 1],
								state->m_paletteram[0x200 + state->m_pixbank + i + 0x800 * 2]);
}

/* 800 bytes for red, followed by 800 bytes for green & 800 bytes for blue */
WRITE8_MEMBER(paradise_state::paradise_palette_w)
{
	m_paletteram[offset] = data;
	offset %= 0x800;
	palette_set_color_rgb(machine(), offset, m_paletteram[offset + 0x800 * 0], m_paletteram[offset + 0x800 * 1],
		m_paletteram[offset + 0x800 * 2]);

	update_pix_palbank(machine());
}

/***************************************************************************

                                    Tilemaps

    Offset:

    $000.b      Code (Low  Bits)
    $400.b      Code (High Bits)

***************************************************************************/

/* Background */
WRITE8_MEMBER(paradise_state::paradise_vram_0_w)
{
	m_vram_0[offset] = data;
	m_tilemap_0->mark_tile_dirty(offset % 0x400);
}

/* 16 color tiles with paradise_palbank as color code */
WRITE8_MEMBER(paradise_state::paradise_palbank_w)
{
	int bank1 = (data & 0x0e) | 1;
	int bank2 = (data & 0xf0);

	m_pixbank = bank2;

	update_pix_palbank(machine());

	if (m_palbank != bank1)
	{
		m_palbank = bank1;
		m_tilemap_0->mark_all_dirty();
	}
}

TILE_GET_INFO_MEMBER(paradise_state::get_tile_info_0)
{
	int code = m_vram_0[tile_index] + (m_vram_0[tile_index + 0x400] << 8);
	SET_TILE_INFO_MEMBER(1, code, m_palbank, 0);
}


/* Midground */
WRITE8_MEMBER(paradise_state::paradise_vram_1_w)
{
	m_vram_1[offset] = data;
	m_tilemap_1->mark_tile_dirty(offset % 0x400);
}

TILE_GET_INFO_MEMBER(paradise_state::get_tile_info_1)
{
	int code = m_vram_1[tile_index] + (m_vram_1[tile_index + 0x400] << 8);
	SET_TILE_INFO_MEMBER(2, code, 0, 0);
}


/* Foreground */
WRITE8_MEMBER(paradise_state::paradise_vram_2_w)
{
	m_vram_2[offset] = data;
	m_tilemap_2->mark_tile_dirty(offset % 0x400);
}

TILE_GET_INFO_MEMBER(paradise_state::get_tile_info_2)
{
	int code = m_vram_2[tile_index] + (m_vram_2[tile_index + 0x400] << 8);
	SET_TILE_INFO_MEMBER(3, code, 0, 0);
}

/* 256 x 256 bitmap. 4 bits per pixel so every byte encodes 2 pixels */

WRITE8_MEMBER(paradise_state::paradise_pixmap_w)
{
	int x, y;

	m_videoram[offset] = data;

	x = (offset & 0x7f) << 1;
	y = (offset >> 7);

	m_tmpbitmap.pix16(y, x + 0) = 0x80f - (data >> 4);
	m_tmpbitmap.pix16(y, x + 1) = 0x80f - (data & 0x0f);
}


/***************************************************************************

                            Vide Hardware Init

***************************************************************************/

void paradise_state::video_start()
{

	m_tilemap_0 = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(paradise_state::get_tile_info_0),this), TILEMAP_SCAN_ROWS, 8, 8, 0x20, 0x20);
	m_tilemap_1 = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(paradise_state::get_tile_info_1),this), TILEMAP_SCAN_ROWS, 8, 8, 0x20, 0x20);
	m_tilemap_2 = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(paradise_state::get_tile_info_2),this), TILEMAP_SCAN_ROWS, 8, 8, 0x20, 0x20);

	/* pixmap */
	machine().primary_screen->register_screen_bitmap(m_tmpbitmap);

	m_tilemap_0->set_transparent_pen(0x0f);
	m_tilemap_1->set_transparent_pen(0xff);
	m_tilemap_2->set_transparent_pen(0xff);

	save_item(NAME(m_tmpbitmap));
}


/***************************************************************************

                            Sprites Drawing

***************************************************************************/

/* Sprites / Layers priority */
WRITE8_MEMBER(paradise_state::paradise_priority_w)
{
	m_priority = data;
}

static void draw_sprites( running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	paradise_state *state = machine.driver_data<paradise_state>();
	UINT8 *spriteram = state->m_spriteram;
	int i;
	for (i = 0; i < state->m_spriteram.bytes() ; i += state->m_sprite_inc)
	{
		int code = spriteram[i + 0];
		int x    = spriteram[i + 1];
		int y    = spriteram[i + 2] - 2;
		int attr = spriteram[i + 3];

		int flipx = 0;  // ?
		int flipy = 0;

		if (state->flip_screen())
		{
			x = 0xf0 - x;   flipx = !flipx;
			y = 0xf0 - y;   flipy = !flipy;
		}

		drawgfx_transpen(bitmap,cliprect,machine.gfx[0],
				code + (attr << 8),
				0,
				flipx, flipy,
				x,y, 0xff );

		/* wrap around x */
		drawgfx_transpen(bitmap,cliprect,machine.gfx[0],
				code + (attr << 8),
				0,
				flipx, flipy,
				x - 256,y, 0xff );

		drawgfx_transpen(bitmap,cliprect,machine.gfx[0],
				code + (attr << 8),
				0,
				flipx, flipy,
				x + 256,y, 0xff );
	}
}


/***************************************************************************

                                Screen Drawing

***************************************************************************/

UINT32 paradise_state::screen_update_paradise(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int layers_ctrl = -1;

#ifdef MAME_DEBUG
if (machine().input().code_pressed(KEYCODE_Z))
{
	int mask = 0;
	if (machine().input().code_pressed(KEYCODE_Q))  mask |= 1;
	if (machine().input().code_pressed(KEYCODE_W))  mask |= 2;
	if (machine().input().code_pressed(KEYCODE_E))  mask |= 4;
	if (machine().input().code_pressed(KEYCODE_R))  mask |= 8;
	if (machine().input().code_pressed(KEYCODE_A))  mask |= 16;
	if (mask != 0) layers_ctrl &= mask;
}
#endif

	bitmap.fill(get_black_pen(machine()), cliprect);

	if (!(m_priority & 4))  /* Screen blanking */
		return 0;

	if (m_priority & 1)
		if (layers_ctrl & 16)
			draw_sprites(screen.machine(), bitmap, cliprect);

	if (layers_ctrl & 1)    m_tilemap_0->draw(bitmap, cliprect, 0, 0);
	if (layers_ctrl & 2)    m_tilemap_1->draw(bitmap, cliprect, 0, 0);
	if (layers_ctrl & 4)    copybitmap_trans(bitmap, m_tmpbitmap, flip_screen(), flip_screen(), 0, 0, cliprect, 0x80f);

	if (m_priority & 2)
	{
		if (!(m_priority & 1))
			if (layers_ctrl & 16)
				draw_sprites(screen.machine(), bitmap, cliprect);
		if (layers_ctrl & 8)
			m_tilemap_2->draw(bitmap, cliprect, 0, 0);
	}
	else
	{
		if (layers_ctrl & 8)
			m_tilemap_2->draw(bitmap, cliprect, 0, 0);
		if (!(m_priority & 1))
			if (layers_ctrl & 16)
				draw_sprites(screen.machine(), bitmap, cliprect);
	}
	return 0;
}

/* no pix layer, no tilemap_0, different priority bits */
UINT32 paradise_state::screen_update_torus(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{

	bitmap.fill(get_black_pen(machine()), cliprect);

	if (!(m_priority & 2))  /* Screen blanking */
		return 0;

	if (m_priority & 1)
		draw_sprites(machine(), bitmap, cliprect);

	m_tilemap_1->draw(bitmap, cliprect, 0,0);

	if (m_priority & 4)
	{
		if (!(m_priority & 1))
			draw_sprites(machine(), bitmap, cliprect);

		m_tilemap_2->draw(bitmap, cliprect, 0, 0);
	}
	else
	{
		m_tilemap_2->draw(bitmap, cliprect, 0, 0);

		if (!(m_priority & 1))
			draw_sprites(machine(), bitmap,cliprect);
	}
	return 0;
}

/* I don't know how the priority bits work on this one */
UINT32 paradise_state::screen_update_madball(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{

	bitmap.fill(get_black_pen(machine()), cliprect);
	m_tilemap_0->draw(bitmap, cliprect, 0, 0);
	m_tilemap_1->draw(bitmap, cliprect, 0, 0);
	m_tilemap_2->draw(bitmap, cliprect, 0, 0);
	draw_sprites(machine(), bitmap, cliprect);
	return 0;
}
