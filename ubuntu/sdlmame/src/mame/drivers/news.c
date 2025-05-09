/*

News

from the program ROM:
PROGRAMED BY KWANG-HO CHO
COPYRIGHT(C) 1993
ALL RIGHTS RESERVED BY POBY
Hi-tel ID:poby:


driver by David Haywood

*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "includes/news.h"
#include "sound/okim6295.h"


static ADDRESS_MAP_START( news_map, AS_PROGRAM, 8, news_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM     /* 4000-7fff is written to during startup, probably leftover code */
	AM_RANGE(0x8000, 0x87ff) AM_RAM_WRITE(news_fgram_w) AM_SHARE("fgram")
	AM_RANGE(0x8800, 0x8fff) AM_RAM_WRITE(news_bgram_w) AM_SHARE("bgram")
	AM_RANGE(0x9000, 0x91ff) AM_RAM_WRITE(paletteram_xxxxRRRRGGGGBBBB_byte_be_w) AM_SHARE("paletteram")
	AM_RANGE(0xc000, 0xc000) AM_READ_PORT("DSW")
	AM_RANGE(0xc001, 0xc001) AM_READ_PORT("INPUTS")
	AM_RANGE(0xc002, 0xc002) AM_DEVREADWRITE("oki", okim6295_device, read, write)
	AM_RANGE(0xc003, 0xc003) AM_WRITE(news_bgpic_w)
	AM_RANGE(0xe000, 0xffff) AM_RAM
ADDRESS_MAP_END


static INPUT_PORTS_START( news )
	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SWA:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWA:3,4")
	PORT_DIPSETTING(    0x0c, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x10, 0x10, "Helps" ) PORT_DIPLOCATION("SWA:5")
	PORT_DIPSETTING(    0x10, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPNAME( 0x20, 0x00, "Copyright" ) PORT_DIPLOCATION("SWA:6")
	PORT_DIPSETTING(    0x00, "Poby" )
	PORT_DIPSETTING(    0x20, "Virus" )
	PORT_DIPUNUSED_DIPLOC( 0x40, IP_ACTIVE_LOW, "SWA:7" )
	PORT_DIPUNUSED_DIPLOC( 0x80, IP_ACTIVE_LOW, "SWA:8" )

	PORT_START("INPUTS")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2 )
INPUT_PORTS_END

static INPUT_PORTS_START( newsa )
	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SWA:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWA:3,4")
	PORT_DIPSETTING(    0x0c, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x10, 0x10, "Helps" ) PORT_DIPLOCATION("SWA:5")
	PORT_DIPSETTING(    0x10, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWA:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x40, IP_ACTIVE_LOW, "SWA:7" )
	PORT_DIPUNUSED_DIPLOC( 0x80, IP_ACTIVE_LOW, "SWA:8" )

	PORT_START("INPUTS")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2 )
INPUT_PORTS_END

static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0, 4, 8, 12, 16, 20, 24, 28 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static GFXDECODE_START( news )
	GFXDECODE_ENTRY( "gfx1", 0, tiles8x8_layout, 0, 16 )
GFXDECODE_END



void news_state::machine_start()
{

	save_item(NAME(m_bgpic));
}

void news_state::machine_reset()
{

	m_bgpic = 0;
}

static MACHINE_CONFIG_START( news, news_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80,8000000)         /* ? MHz */
	MCFG_CPU_PROGRAM_MAP(news_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", news_state,  irq0_line_hold)


	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(256, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 256-1, 16, 256-16-1)
	MCFG_SCREEN_UPDATE_DRIVER(news_state, screen_update_news)

	MCFG_GFXDECODE(news)
	MCFG_PALETTE_LENGTH(0x100)


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_OKIM6295_ADD("oki", 1056000, OKIM6295_PIN7_HIGH) // clock frequency & pin 7 not verified
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_CONFIG_END



ROM_START( news )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "virus.4", 0x00000, 0x08000, BAD_DUMP CRC(aa005dfb) SHA1(52f4dd399a30568851d43d052b83cfaa6682665d)  ) /* The Original was too short, I padded it with 0xFF */

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "virus.2", 0x00000, 0x40000, CRC(b5af58d8) SHA1(5dd8c6ab8b53df695463bd0c3620adf8c08daaec) )
	ROM_LOAD16_BYTE( "virus.3", 0x00001, 0x40000, CRC(a4b1c175) SHA1(b1ac0da4d91bc3a3454ea80aa4cdbbc68bbdf7f1) )

	ROM_REGION( 0x40000, "oki", 0 )
	ROM_LOAD( "virus.1", 0x00000, 0x40000, CRC(41f5935a) SHA1(1566d243f165019660cd4dd69df9f049e0130f15) )
ROM_END

ROM_START( newsa )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "newsa_virus.4", 0x00000, 0x10000, CRC(74a257da) SHA1(f1f6db521312f152ec3b1e6efa45a514433b2ffc) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "virus.2", 0x00000, 0x40000, CRC(b5af58d8) SHA1(5dd8c6ab8b53df695463bd0c3620adf8c08daaec) )
	ROM_LOAD16_BYTE( "virus.3", 0x00001, 0x40000, CRC(a4b1c175) SHA1(b1ac0da4d91bc3a3454ea80aa4cdbbc68bbdf7f1) )

	ROM_REGION( 0x40000, "oki", 0 )
	ROM_LOAD( "virus.1", 0x00000, 0x40000, CRC(41f5935a) SHA1(1566d243f165019660cd4dd69df9f049e0130f15) )
ROM_END

GAME( 1993, news,  0,    news, news, driver_device,  0, ROT0, "Poby / Virus", "News (set 1)", GAME_SUPPORTS_SAVE )
GAME( 1993, newsa, news, news, newsa, driver_device, 0, ROT0, "Poby",         "News (set 2)", GAME_SUPPORTS_SAVE )
