/*************************************************************************************************************

                                        -= Andamiro's Midas hardware =-

                                            driver by Luca Elia

    a reengineered Neo-Geo, with a few differences: no Z80, better sound chip, serial eeprom and 256 color tiles.
    Plus a PIC12C508A microcontroller, probably for the protection checks (I've patched them out for now).

    This driver should be merged with neogeo.c
    Hardware description:

    http://web.archive.org/web/20041018094226/http://www.andamiro.com/kor/business/hard_05.html

    CPU         MC68000

    VRAM        256kbyte (4Display/Access bank)

    PaletteRAM  96kbyte

    Display     320(x)*224(y)

    Sprite      16(x)*240(y(max))*380(max) (96 sprite/line(max))
                128 level y-axis scaling (or line control effect)
                16 level x-axis scale-down x,y fip
                255color/sprite(of 256 palette set)

    Text        8dot*8dot, 40(x)*28(y),
                255color/text(of 16 palette set)

    Color       32640 of 24bit True Color
                (255 colors/sprite)

    Sound       8 channel 44.1KHz(max) stereo
                4bit ADPCM, 8bit PCM, 16bit PCM

    Controller  4 direction,
                6 button joystick * 2 player (max. 4 playersupport)
                light-gun*2 player
                trackball*2 player

    Maximum ROM 2Gbit

    size  220(x)mm * 210(y)mm


    Notes:

    - hammer: keep test button pressed during boot for hardware tests

*************************************************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "sound/ymz280b.h"
#include "machine/eeprom.h"
#include "machine/ticket.h"


class midas_state : public driver_device
{
public:
	midas_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_gfxregs(*this, "gfxregs"){ }

	UINT16 *m_gfxram;
	required_shared_ptr<UINT16> m_gfxregs;
	tilemap_t *m_tmap;
	DECLARE_READ16_MEMBER(ret_ffff);
	DECLARE_WRITE16_MEMBER(midas_gfxregs_w);
	DECLARE_WRITE16_MEMBER(livequiz_coin_w);
	DECLARE_READ16_MEMBER(hammer_sensor_r);
	DECLARE_WRITE16_MEMBER(hammer_coin_w);
	DECLARE_WRITE16_MEMBER(hammer_motor_w);
	DECLARE_WRITE16_MEMBER(hammer_led_w);
	DECLARE_WRITE16_MEMBER(midas_eeprom_w);
	DECLARE_DRIVER_INIT(livequiz);
	TILE_GET_INFO_MEMBER(get_tile_info);
	virtual void video_start();
	UINT32 screen_update_midas(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};






TILE_GET_INFO_MEMBER(midas_state::get_tile_info)
{
	UINT16 code = m_gfxram[ tile_index + 0x7000 ];
	SET_TILE_INFO_MEMBER(1, code & 0xfff, (code >> 12) & 0xf, TILE_FLIPXY( 0 ));
}

void midas_state::video_start()
{
	m_gfxram = auto_alloc_array(machine(), UINT16, 0x20000/2);

	m_tmap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(midas_state::get_tile_info),this), TILEMAP_SCAN_COLS,8,8,0x80,0x20);

	m_tmap->set_transparent_pen(0);
}

static void draw_sprites(running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	midas_state *state = machine.driver_data<midas_state>();
	UINT16 *s       =   state->m_gfxram + 0x8000;
	UINT16 *codes   =   state->m_gfxram;

	int sx_old = 0, sy_old = 0, ynum_old = 0, xzoom_old = 0;
	int xdim, ydim, xscale, yscale;
	int i,y;

	for (i = 0; i < 0x180; i++,s++,codes+=0x40)
	{
		int zoom    =   s[0x000];
		int sy      =   s[0x200];
		int sx      =   s[0x400];

		int xzoom   =   ((zoom >> 8) & 0x0f) + 1;
		int yzoom   =   ((zoom >> 0) & 0x7f) + 1;

		int ynum;

		if (sy & 0x40)
		{
			ynum    =   ynum_old;

			sx      =   sx_old + xzoom_old;
			sy      =   sy_old;

			if (sx >= 0x1f0)
				sx -= 0x200;
		}
		else
		{
			ynum    =   sy & 0x3f;

			sx      =   (sx >> 7);
			sy      =   0x200 - (sy >> 7);

			if (sx >= 0x1f0)
				sx -= 0x200;

			if (ynum > 0x20)
				ynum = 0x20;
		}

		ynum_old    =   ynum;
		sx_old      =   sx;
		sy_old      =   sy;
		xzoom_old   =   xzoom;

		// Use fixed point values (16.16), for accuracy

		sx          <<= 16;
		sy          <<= 16;

		xdim    =   ( xzoom << 16 ) * 16 / 16;
		ydim    =   ( yzoom << 16 ) * 16 / 0x80;

		xscale  =   xdim / 16;
		yscale  =   ydim / 16;

		// Let's approximate to the nearest greater integer value
		// to avoid holes in between tiles

		if (xscale & 0xffff)    xscale += (1<<16) / 16;
		if (yscale & 0xffff)    yscale += (1<<16) / 16;

		// Draw the tiles

		for (y = 0; y < ynum; y++)
		{
			UINT16 code     =   codes[y*2];
			UINT16 attr     =   codes[y*2+1];

			drawgfxzoom_transpen(   bitmap, cliprect, machine.gfx[0],
							code,
							attr >> 8,
							attr & 1, attr & 2,
							sx / 0x10000, ((sy + y * ydim) / 0x10000)&0x1ff,
							xscale, yscale, 0           );
		}
	}
}

UINT32 midas_state::screen_update_midas(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int layers_ctrl = -1;

#ifdef MAME_DEBUG
	if ( machine().input().code_pressed(KEYCODE_Z) )
	{
		int msk = 0;
		if (machine().input().code_pressed(KEYCODE_Q))  msk |= 1 << 0;  // for m_tmap
		if (machine().input().code_pressed(KEYCODE_A))  msk |= 1 << 1;  // for sprites
		if (msk != 0) layers_ctrl &= msk;
	}
#endif

	bitmap.fill(4095, cliprect);

	if (layers_ctrl & 2)    draw_sprites(machine(), bitmap,cliprect);
	if (layers_ctrl & 1)    m_tmap->draw(bitmap, cliprect, 0, 0);

	return 0;
}

WRITE16_MEMBER(midas_state::midas_eeprom_w)
{
	device_t *device = machine().device("eeprom");
	if (ACCESSING_BITS_0_7)
	{
		// latch the bit
		eeprom_device *eeprom = downcast<eeprom_device *>(device);
		eeprom->write_bit(data & 0x04);

		// reset line asserted: reset.
		eeprom->set_cs_line((data & 0x01) ? CLEAR_LINE : ASSERT_LINE );

		// clock line asserted: write latch or select next bit to read
		eeprom->set_clock_line((data & 0x02) ? ASSERT_LINE : CLEAR_LINE );
	}
}

READ16_MEMBER(midas_state::ret_ffff)
{
	return 0xffff;
}

WRITE16_MEMBER(midas_state::midas_gfxregs_w)
{
	COMBINE_DATA( m_gfxregs + offset );

	switch( offset )
	{
		case 1:
		{
			UINT16 addr = m_gfxregs[0];
			m_gfxram[addr] = data;
			m_gfxregs[0] += m_gfxregs[2];

			if ( addr >= 0x7000 && addr <= 0x7fff ) m_tmap->mark_tile_dirty(addr - 0x7000);

			break;
		}
	}
}

/***************************************************************************************
                                       Live Quiz Show
***************************************************************************************/

WRITE16_MEMBER(midas_state::livequiz_coin_w)
{
	if (ACCESSING_BITS_0_7)
	{
		coin_counter_w(machine(), 0, data & 0x0001);
	}
#ifdef MAME_DEBUG
//  popmessage("coin %04X", data);
#endif
}

static ADDRESS_MAP_START( livequiz_map, AS_PROGRAM, 16, midas_state )
	AM_RANGE(0x000000, 0x1fffff) AM_ROM

	AM_RANGE(0x900000, 0x900001) AM_READ_PORT("DSW_PLAYER1")
	AM_RANGE(0x920000, 0x920001) AM_READ_PORT("SERVICE")
	AM_RANGE(0x940000, 0x940001) AM_READ_PORT("PLAYER2")
	AM_RANGE(0x980000, 0x980001) AM_READ_PORT("START")

	AM_RANGE(0x980000, 0x980001) AM_WRITE(livequiz_coin_w )

	AM_RANGE(0x9a0000, 0x9a0001) AM_WRITE(midas_eeprom_w )

	AM_RANGE(0x9c0000, 0x9c0005) AM_WRITE(midas_gfxregs_w ) AM_SHARE("gfxregs")

	AM_RANGE(0xa00000, 0xa3ffff) AM_RAM_WRITE(paletteram_xrgb_word_be_w ) AM_SHARE("paletteram")
	AM_RANGE(0xa40000, 0xa7ffff) AM_RAM

	AM_RANGE(0xb00000, 0xb00001) AM_READ(ret_ffff )
	AM_RANGE(0xb20000, 0xb20001) AM_READ(ret_ffff )
	AM_RANGE(0xb40000, 0xb40001) AM_READ(ret_ffff )
	AM_RANGE(0xb60000, 0xb60001) AM_READ(ret_ffff )

	AM_RANGE(0xb80008, 0xb8000b) AM_DEVREADWRITE8_LEGACY("ymz", ymz280b_r, ymz280b_w, 0x00ff )

	AM_RANGE(0xba0000, 0xba0001) AM_READ_PORT("START3")
	AM_RANGE(0xbc0000, 0xbc0001) AM_READ_PORT("PLAYER3")

	AM_RANGE(0xd00000, 0xd1ffff) AM_RAM // zoom table?

	AM_RANGE(0xe00000, 0xe3ffff) AM_RAM
ADDRESS_MAP_END

/***************************************************************************************
                                          Hammer
***************************************************************************************/

READ16_MEMBER(midas_state::hammer_sensor_r)
{
	if (ioport("HAMMER")->read() & 0x80)
		return 0xffff;

	return (ioport("SENSORY")->read() << 8) | ioport("SENSORX")->read();
}

WRITE16_MEMBER(midas_state::hammer_coin_w)
{
	if (ACCESSING_BITS_0_7)
	{
		coin_counter_w(machine(), 0, data & 0x0001);
		coin_counter_w(machine(), 1, data & 0x0002);
	}
#ifdef MAME_DEBUG
//  popmessage("coin %04X", data);
#endif
}

WRITE16_MEMBER(midas_state::hammer_motor_w)
{
	if (ACCESSING_BITS_0_7)
	{
		machine().device<ticket_dispenser_device>("prize1")->write(space, 0, (data & 0x0001) << 7);
		machine().device<ticket_dispenser_device>("prize2")->write(space, 0, (data & 0x0002) << 6);
		machine().device<ticket_dispenser_device>("ticket")->write(space, 0, (data & 0x0010) << 3);
		// data & 0x0080 ?
	}
#ifdef MAME_DEBUG
//  popmessage("motor %04X", data);
#endif
}

WRITE16_MEMBER(midas_state::hammer_led_w)
{
#ifdef MAME_DEBUG
//  popmessage("led %04X", data);
#endif
}

static ADDRESS_MAP_START( hammer_map, AS_PROGRAM, 16, midas_state )
	AM_RANGE(0x000000, 0x1fffff) AM_ROM

	AM_RANGE(0x900000, 0x900001) AM_READ_PORT("DSW")
	AM_RANGE(0x920000, 0x920001) AM_READ_PORT("SERVICE")
	AM_RANGE(0x940000, 0x940001) AM_READ_PORT("IN0")
	AM_RANGE(0x980000, 0x980001) AM_READ_PORT("TILT")

	AM_RANGE(0x980000, 0x980001) AM_WRITE(hammer_coin_w )
	AM_RANGE(0x9c000c, 0x9c000d) AM_WRITENOP    // IRQ Ack
	AM_RANGE(0x9c000e, 0x9c000f) AM_WRITE(hammer_led_w )

	AM_RANGE(0x9a0000, 0x9a0001) AM_WRITE(midas_eeprom_w )

	AM_RANGE(0x9c0000, 0x9c0005) AM_WRITE(midas_gfxregs_w ) AM_SHARE("gfxregs")

	AM_RANGE(0xa00000, 0xa3ffff) AM_RAM_WRITE(paletteram_xrgb_word_be_w ) AM_SHARE("paletteram")
	AM_RANGE(0xa40000, 0xa7ffff) AM_RAM

	AM_RANGE(0xb00000, 0xb00001) AM_READ(ret_ffff )
	AM_RANGE(0xb20000, 0xb20001) AM_READ(ret_ffff )
	AM_RANGE(0xb40000, 0xb40001) AM_READ(ret_ffff )
	AM_RANGE(0xb60000, 0xb60001) AM_READ(ret_ffff )

	AM_RANGE(0xb80008, 0xb8000b) AM_DEVREADWRITE8_LEGACY("ymz", ymz280b_r, ymz280b_w, 0x00ff )

	AM_RANGE(0xba0000, 0xba0001) AM_READ_PORT("IN1")
	AM_RANGE(0xbc0000, 0xbc0001) AM_READ_PORT("HAMMER")

	AM_RANGE(0xbc0002, 0xbc0003) AM_WRITE(hammer_motor_w )

	AM_RANGE(0xbc0004, 0xbc0005) AM_READ(hammer_sensor_r )

	AM_RANGE(0xd00000, 0xd1ffff) AM_RAM // zoom table?

	AM_RANGE(0xe00000, 0xe3ffff) AM_RAM
ADDRESS_MAP_END


static const gfx_layout layout16x16x8 =
{
	16,16,
	RGN_FRAC(1,4),
	8,
	{
		RGN_FRAC(3,4)+8,RGN_FRAC(3,4)+0,
		RGN_FRAC(2,4)+8,RGN_FRAC(2,4)+0,
		RGN_FRAC(1,4)+8,RGN_FRAC(1,4)+0,
		RGN_FRAC(0,4)+8,RGN_FRAC(0,4)+0
	},
	{ STEP8(8*16*2+7,-1), STEP8(7,-1) },
	{ STEP16(0,8*2) },
	16*16*2
};

static const gfx_layout layout8x8x8_2 =
{
	8,8,
	RGN_FRAC(1,1),
	8,
	{ 8,9,10,11, 0,1,2,3     },
	{ (32*2+1)*4, 32*2*4, (48*2+1)*4, 48*2*4, (0+1)*4, 0*4, (16*2+1)*4, 16*2*4 },
	{ 0*8*2, 1*8*2, 2*8*2, 3*8*2, 4*8*2, 5*8*2, 6*8*2, 7*8*2 },
	32*8*2
};

static GFXDECODE_START( midas )
	GFXDECODE_ENTRY( "sprites", 0, layout16x16x8, 0, 0x100 )
	GFXDECODE_ENTRY( "tiles",   0, layout8x8x8_2, 0,  0x80 )
GFXDECODE_END


static INPUT_PORTS_START( livequiz )

	PORT_START("DSW_PLAYER1")   // 900000
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SERVICE")   // 920000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW,  IPT_COIN1   )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_READ_LINE_DEVICE_MEMBER("eeprom", eeprom_device, read_bit) // EEPROM
	PORT_BIT( 0x0010, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_SERVICE_NO_TOGGLE( 0x0040,   IP_ACTIVE_LOW )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("PLAYER2")   // 940000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("START") // 980000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_START1  )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_START2  )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("START3")    // ba0000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_START3  )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("PLAYER3")   // bc0000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

INPUT_PORTS_END

static INPUT_PORTS_START( hammer )

	PORT_START("DSW")   // 900000
	PORT_DIPNAME( 0x01, 0x01, "Debug Mode" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x06, 0x06, "Game Mode" )
	PORT_DIPSETTING(    0x06, "Prize Game" )
	PORT_DIPSETTING(    0x00, "Ticket Game 1" ) // not in manual, does not work (it requires a toggleable ticket dispenser)
	PORT_DIPSETTING(    0x04, "Ticket Game 2" )
	PORT_DIPSETTING(    0x02, "Generic Game" )
	PORT_DIPNAME( 0x08, 0x08, "Warning Sound" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Freeze" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SERVICE")   // 920000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW,  IPT_COIN1     )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW,  IPT_COIN2     )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW,  IPT_SERVICE1  )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_SPECIAL   ) PORT_READ_LINE_DEVICE_MEMBER("eeprom", eeprom_device, read_bit)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW,  IPT_UNKNOWN   )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW,  IPT_UNKNOWN   )
	PORT_SERVICE_NO_TOGGLE( 0x0040,   IP_ACTIVE_LOW )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW,  IPT_UNKNOWN   )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN0")   // 940000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("TILT")  // 980000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_TILT    )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")   // ba0000
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("HAMMER")    // bc0000
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_READ_LINE_DEVICE_MEMBER("prize1", ticket_dispenser_device, line_r) // prize 1 sensor ("tejisw 1")
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_READ_LINE_DEVICE_MEMBER("prize2", ticket_dispenser_device, line_r) // prize 2 sensor ("tejisw 2")
	PORT_BIT( 0x0004, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_READ_LINE_DEVICE_MEMBER("ticket", ticket_dispenser_device, line_r)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW,  IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_IMPULSE(5) PORT_NAME( "Hammer" )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SENSORX")
	PORT_BIT( 0xff, 0x20, IPT_LIGHTGUN_X ) PORT_MINMAX(0x00, 0x3f+1) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(35) PORT_KEYDELTA(8)

	PORT_START("SENSORY")
	PORT_BIT( 0xff, 0x18, IPT_LIGHTGUN_Y ) PORT_MINMAX(0x00, 0x2f+1) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(35) PORT_KEYDELTA(8)

INPUT_PORTS_END


static void livequiz_irqhandler(device_t *device, int state)
{
	logerror("YMZ280 is generating an interrupt. State=%08x\n",state);
}

static const ymz280b_interface ymz280b_config =
{
	livequiz_irqhandler
};

static MACHINE_CONFIG_START( livequiz, midas_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, XTAL_24MHz / 2)
	MCFG_CPU_PROGRAM_MAP(livequiz_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", midas_state,  irq1_line_hold)

	MCFG_EEPROM_93C46_ADD("eeprom")

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(320, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 320-1, 16, 256-16-1)
	MCFG_SCREEN_UPDATE_DRIVER(midas_state, screen_update_midas)

	MCFG_GFXDECODE(midas)
	MCFG_PALETTE_LENGTH(0x10000)


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SOUND_ADD("ymz", YMZ280B, XTAL_16_9344MHz)
	MCFG_SOUND_CONFIG(ymz280b_config)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.80)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.80)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( hammer, midas_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, XTAL_28MHz / 2)
	MCFG_CPU_PROGRAM_MAP(hammer_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", midas_state,  irq1_line_hold)

	MCFG_EEPROM_93C46_ADD("eeprom")

	MCFG_TICKET_DISPENSER_ADD("prize1", attotime::from_msec(1000*5), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_LOW )
	MCFG_TICKET_DISPENSER_ADD("prize2", attotime::from_msec(1000*5), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_LOW )
	MCFG_TICKET_DISPENSER_ADD("ticket",    attotime::from_msec(200), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_LOW )

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(320, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 320-1, 16, 256-16-1)
	MCFG_SCREEN_UPDATE_DRIVER(midas_state, screen_update_midas)

	MCFG_GFXDECODE(midas)
	MCFG_PALETTE_LENGTH(0x10000)


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SOUND_ADD("ymz", YMZ280B, XTAL_16_9344MHz)
	MCFG_SOUND_CONFIG(ymz280b_config)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.80)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.80)
MACHINE_CONFIG_END


/***************************************************************************************

Live Quiz Show
1999, Andamiro Entertainment Co. Ltd.

Main Board
----------

MIDAS
|----------------------------------------------------------|
|TDA1519A                        |-----ROM-BOARD(ABOVE)----|
|                  558   YAC516  |                         |
|    VOL     558                 |    YMZ280B              |
|SP1               558   12C508A |                         |
|                                |                         |
|         |---------|            |               16.9344MHz|
|         |         |    PAL     |    |--------|           |
|         | MIDAS-2 |            |    |TMP     |   KM681000|
|J        |         |            |    |68HC000 |           |
|A        |         |            |    |        |   KM681000|
|M        |---------|    KM681000|    |--------|           |
|M                               |                 KM681000|
|A                       KM681000|                         |
| DSW(8)                         |                         |
|                        KM681000|                         |
|                                |                         |
|         |---------|            |    |---------|          |
|PUSH_BTN |         |            |    |         |          |
|         | MIDAS-1 |            |    | MIDAS-3 |          |
|93C46    |         |            |    |         |          |
|CN1      |         |            |    |         |          |
|CN2      |---------|            |    |---------|          |
|  KM681000 341256               |                         |
|CN3                             |                    24MHz|
|  KM681000 341256               |-------------------------|
|----------------------------------------------------------|
Notes:
      TMP68HC000   - Toshiba TMP68HC000 CPU clock - 12MHz [24/2] (PLCC68)
      YMZ280 clock - 16.9344MHz
      341256       - NKK N341256SJ-16 32k x8 SRAM (SOJ28)
      KM681000     - Samsung KM681000 128k x8 SRAM (SOP32)
      SP1          - 3 pin connector for stereo sound output
      CN1/2/3      - Connectors for extra controls
      MIDAS-1/2/3  - Custom chips, probably rebadged FPGAs (QFP208)
      12C508A      - Microchip PIC12C508A Microcontroller (DIP8)
      VSync        - 60Hz
      HSync        - 15.21kHz

ROM Board
---------

MIDAS
|-------------------------|
|                         |
|     27C4096.U23         |
|                         |
|  *U21        *U22       |
|                         |
|                         |
|  *U26        *U27       |
|                         |
|                         |
|   U19         U20       |
|                     CN15|
|                     CN13|
|  *U17        *U18       |
|                     CN12|
|                         |
|  *U24        *U25       |
|                         |
|                         |
|   U15         U16       |
|                         |
|                         |
|   U1         *U7        |
|                         |
|12C508A                  |
|   U5         *U6        |
|-------------------------|
Notes:
      * Not populated
      CN15/13/12 - Connectors for extra controls
      12C508A    - Microchip PIC12C508A Microcontroller (DIP8)
      U23        - 27C4096 EPROM
      All other ROMs are MX29F1610 (SOP44)

***************************************************************************************/

ROM_START( livequiz )
	ROM_REGION( 0x200000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "flash.u1", 0x000000, 0x200000, CRC(8ec44493) SHA1(a987886cb87ac0a744f01f2e4a7cc6d12efeaa04) )

	ROM_REGION( 0x200000, "pic", 0 )
	ROM_LOAD( "main_pic12c508a.u27", 0x000000, 0x000400, CRC(a84f0a7e) SHA1(fb27c05fb27b98ca603697e1be214dc6c8d5f884) )
	ROM_LOAD( "sub_pic12c508a.u4",   0x000000, 0x000400, CRC(e52ebdc4) SHA1(0f3af66b5ea184e49188e74a873699324a3930f1) )

	ROM_REGION( 0x800000, "sprites", 0 )
	ROM_LOAD( "flash.u15", 0x000000, 0x200000, CRC(d6eb56f1) SHA1(52d67bb25dd968c79eccb05159a578516b27e557) )
	ROM_LOAD( "flash.u19", 0x200000, 0x200000, CRC(daa81532) SHA1(9e66bb4639b92c3d76b7918535f55883f22f24b2) )
	ROM_LOAD( "flash.u16", 0x400000, 0x200000, CRC(4c9fd873) SHA1(6e185304ce29771265d3c48b0ef0e840d8bed02d) )
	ROM_LOAD( "flash.u20", 0x600000, 0x200000, CRC(b540a8c7) SHA1(25b9b30c7d5ff1e410ea30580017e45590542561) )

	ROM_REGION( 0x080000, "tiles", 0 )
	ROM_LOAD( "27c4096.u23", 0x000000, 0x080000, CRC(25121de8) SHA1(edf24d87551639b871baf3243b452a4e2ba84107) )

	ROM_REGION( 0x200000, "ymz", 0 )
	ROM_LOAD( "flash.u5", 0x000000, 0x200000, CRC(dc062792) SHA1(ec415c918c47ce9d181f014cde317af5717600e4) )
ROM_END

DRIVER_INIT_MEMBER(midas_state,livequiz)
{
	UINT16 *rom = (UINT16 *) machine().root_device().memregion("maincpu")->base();

	// PROTECTION CHECKS
	rom[0x13345a/2] =   0x4e75;
}

/***************************************************************************************

Hammer
Andamiro 2000

PCB Layout
----------

|------------------------------|-------------------|
|TDA1519 17358 YAC516 YMZ280B  |        CN1        |
|   17558    16.9344MHz   ATMEL_ATF1500            |
|  VOL   17358                 |                   |
|                              |   S0.U25          |
|                        MC68HC000CFN16            |
|TD62064                       |   S1.U26          |
|              MIDAS-2         |                   |
| DSW(8)                       |   P.U22    A0L.U44|
|J                             |  LP621024         |
|A                 LP621024    |            A0H.U46|
|M                             |  LP621024         |
|M                 LP621024    |            A1L.U48|
|A                             |  LP621024         |
|                  LP621024    |            A1H.U50|
|                              |                   |
|                              |            A2L.U45|
|                              |                   |
| TESTSW                       |            A2H.U47|
| 93C46        MIDAS-1     MIDAS-3                 |
| LP621024            28MHz    |            A3L.U49|
|           HM62H256           |                   |
| LP621024          HM62H256   |            A3H.U51|
|------------------------------|-------------------|
Notes:
      68000 @ 14MHz [28/2]
      YMZ280B @ 16.9344MHz
      CN1 - connector for top board

Top board
---------

HAMMER TOP PCB VER1.2
AMO30803
|------------------------------------------|
|CN19 CN14 CN15 CN13 CN22 CN23 CN20 CN21   |
|                                          |
|                                          |
|                                 17558    |
|   TD62064  TD62064              17393    |
|                                          |
|                            24MHz      CN2|
|                     AT89C52     17558    |
|PAL                              17393 CN1|
|                                          |
|                             MC7805       |
|------------------------------------------|

***************************************************************************************/

ROM_START( hammer )
	ROM_REGION( 0x200000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "p.u22", 0x000000, 0x200000, CRC(687f1596) SHA1(3dc5fb0af1e8c4f3a42ce4aad39635b1111831d8) )

	ROM_REGION( 0x1000000, "sprites", 0 )
	ROM_LOAD( "a0l.u44", 0x000000, 0x200000, CRC(b9cafd81) SHA1(24698970d1aea0907e2963c872ce61077f44c3af) )
	ROM_LOAD( "a0h.u46", 0x200000, 0x200000, CRC(f60f188b) SHA1(486f26c473b46efb402662b2f374e361cd7b4284) )

	ROM_LOAD( "a1l.u48", 0x400000, 0x200000, CRC(82129cf9) SHA1(6d68e943854bc9e8ea555bf03107dc9e836ca4d9) )
	ROM_LOAD( "a1h.u50", 0x600000, 0x200000, CRC(76897c90) SHA1(aded60d3db834598cd54ad9140eee7be4129cb27) )

	ROM_LOAD( "a2l.u45", 0x800000, 0x200000, CRC(d8086ee5) SHA1(9d5f2b3a0f903a69cfd1108ddf5ea61b571c3fe3) )
	ROM_LOAD( "a2h.u47", 0xa00000, 0x200000, CRC(a64aa2df) SHA1(7e4eb049cd6a5971a455488a484f225763921614) )

	ROM_LOAD( "a3l.u49", 0xc00000, 0x200000, CRC(4e83cf00) SHA1(e66a0b4eae0f46bf36126be3795cfac3ad3d4282) )
	ROM_LOAD( "a3h.u51", 0xe00000, 0x200000, CRC(834de39f) SHA1(6e9867180ca20e64f60bad5cad82674ce8f45b7b) )

	ROM_REGION( 0x080000, "tiles", ROMREGION_ERASE )
	// Use the tiles rom from livequiz (not present in this set) to show some debug text
//  ROM_LOAD( "27c4096.u23", 0x000000, 0x080000, CRC(25121de8) SHA1(edf24d87551639b871baf3243b452a4e2ba84107) )

	ROM_REGION( 0x400000, "ymz", 0 )
	ROM_LOAD( "s0.u25", 0x000000, 0x200000, CRC(c049a3e0) SHA1(0c7016c3128c170a84ad3f92fad1165775210e3d) )
	ROM_LOAD( "s1.u26", 0x200000, 0x200000, CRC(9cc4b3ec) SHA1(b91a8747074a1032eb7f70a015d394fe8e896d7e) )
ROM_END

GAME( 1999, livequiz, 0, livequiz, livequiz, midas_state, livequiz, ROT0, "Andamiro", "Live Quiz Show", GAME_IMPERFECT_GRAPHICS )
GAME( 2000, hammer,   0, hammer,   hammer, driver_device,   0,        ROT0, "Andamiro", "Hammer",         0 )
