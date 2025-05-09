/*
World Cup 90 ( Tecmo ) driver
-----------------------------

Ernesto Corvi
(ernesto@imagina.com)

TODO:
- Dip switches mapping is not complete. ( Anyone has the manual handy? )
- Hook up trackball controls in wc90t.

CPU #1 : Handles background & foreground tiles, controllers, dipswitches.
CPU #2 : Handles sprites and palette
CPU #3 : Audio.

Memory Layout:

CPU #1
0000-8000 ROM
8000-9000 RAM
a000-a800 Color Ram for background #1 tiles
a800-b000 Video Ram for background #1 tiles
c000-c800 Color Ram for background #2 tiles
c800-d000 Video Ram for background #2 tiles
e000-e800 Color Ram for foreground tiles
e800-f000 Video Ram for foreground tiles
f800-fc00 Common Ram with CPU #2
fc00-fc00 Stick 1 input port
fc02-fc02 Stick 2 input port
fc05-fc05 Start buttons and Coins input port
fc06-fc06 Dip Switch A
fc07-fc07 Dip Switch B

CPU #2
0000-c000 ROM
c000-d000 RAM
d000-d800 RAM Sprite Ram
e000-e800 RAM Palette Ram
f800-fc00 Common Ram with CPU #1

CPU #3
0000-0xc000 ROM
???????????


To enter into input test mode:
-keep pressed one of the start buttons during P.O.S.T.(in wc90 & wc90a).
-keep pressed both start buttons during P.O.S.T. until the cross hatch test fade out(in wc90t).
Press one of the start buttons to exit.


*****************************
*                           *
*  Pacman on WC90 Hardware  *
*                           *
*****************************

There is known to be a Pacman hack running on this hardware.  It was done by Mike C. and isn't ment
for inclusion in MAME.  However the roms with checksums are listed below to prevent it being added
as a newly "found" game:

Set is known as pac90 (rom names may differ, but the CRC32/SHA1 values are listed):

rom1.ic87      CRC32 8af34306   SHA1 1a98adca74f46da36e3648d37bfcb56a328a031e
rom2.ic67      CRC32 bc9bfdf2   SHA1 869e4012e5c577e501143cbfd75cce8cef919c86
rom3.ic54      CRC32 1c4d17fd   SHA1 5abebf867de452cc3e85331e91b9110c26a8b050
char.ic85      CRC32 70941a50   SHA1 283583743c21774d0097dc935ae7bc7009b5b633
char.ic85      CRC32 0b906dae   SHA1 0d14d6a7bbe0b8772143afb4c6c94c62313e4b9c <-- An alternate version...
sprite1.ic50   CRC32 190852ea   SHA1 fad7eb3aa53d03917173dd5a040655cfd329db32
sprite2.ic60   CRC32 33effbea   SHA1 dbf6b735f3c8bacb695caf5d15ac8b7961bffc74
voice.ic82     CRC32 abc61f3d   SHA1 c6f123d16a26c4d77c635617dd97bb4b906c463a


*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "sound/2608intf.h"
#include "includes/wc90.h"


WRITE8_MEMBER(wc90_state::wc90_bankswitch_w)
{
	int bankaddress;
	UINT8 *RAM = memregion("maincpu")->base();


	bankaddress = 0x10000 + ( ( data & 0xf8 ) << 8 );
	membank("bank1")->set_base(&RAM[bankaddress] );
}

WRITE8_MEMBER(wc90_state::wc90_bankswitch1_w)
{
	int bankaddress;
	UINT8 *RAM = memregion("sub")->base();


	bankaddress = 0x10000 + ( ( data & 0xf8 ) << 8 );
	membank("bank2")->set_base(&RAM[bankaddress] );
}

WRITE8_MEMBER(wc90_state::wc90_sound_command_w)
{
	soundlatch_byte_w(space, offset, data);
	machine().device("audiocpu")->execute().set_input_line(INPUT_LINE_NMI, PULSE_LINE);
}



static ADDRESS_MAP_START( wc90_map_1, AS_PROGRAM, 8, wc90_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x9fff) AM_RAM     /* Main RAM */
	AM_RANGE(0xa000, 0xafff) AM_RAM_WRITE(wc90_fgvideoram_w) AM_SHARE("fgvideoram") /* fg video ram */
	AM_RANGE(0xb000, 0xbfff) AM_RAM
	AM_RANGE(0xc000, 0xcfff) AM_RAM_WRITE(wc90_bgvideoram_w) AM_SHARE("bgvideoram")
	AM_RANGE(0xd000, 0xdfff) AM_RAM
	AM_RANGE(0xe000, 0xefff) AM_RAM_WRITE(wc90_txvideoram_w) AM_SHARE("txvideoram") /* tx video ram */
	AM_RANGE(0xf000, 0xf7ff) AM_ROMBANK("bank1")
	AM_RANGE(0xf800, 0xfbff) AM_RAM AM_SHARE("share1")
	AM_RANGE(0xfc00, 0xfc00) AM_READ_PORT("P1")
	AM_RANGE(0xfc02, 0xfc02) AM_READ_PORT("P2")
	AM_RANGE(0xfc05, 0xfc05) AM_READ_PORT("SYSTEM")
	AM_RANGE(0xfc06, 0xfc06) AM_READ_PORT("DSW1")
	AM_RANGE(0xfc07, 0xfc07) AM_READ_PORT("DSW2")
	AM_RANGE(0xfc02, 0xfc02) AM_WRITEONLY AM_SHARE("scroll0ylo")
	AM_RANGE(0xfc03, 0xfc03) AM_WRITEONLY AM_SHARE("scroll0yhi")
	AM_RANGE(0xfc06, 0xfc06) AM_WRITEONLY AM_SHARE("scroll0xlo")
	AM_RANGE(0xfc07, 0xfc07) AM_WRITEONLY AM_SHARE("scroll0xhi")
	AM_RANGE(0xfc22, 0xfc22) AM_WRITEONLY AM_SHARE("scroll1ylo")
	AM_RANGE(0xfc23, 0xfc23) AM_WRITEONLY AM_SHARE("scroll1yhi")
	AM_RANGE(0xfc26, 0xfc26) AM_WRITEONLY AM_SHARE("scroll1xlo")
	AM_RANGE(0xfc27, 0xfc27) AM_WRITEONLY AM_SHARE("scroll1xhi")
	AM_RANGE(0xfc42, 0xfc42) AM_WRITEONLY AM_SHARE("scroll2ylo")
	AM_RANGE(0xfc43, 0xfc43) AM_WRITEONLY AM_SHARE("scroll2yhi")
	AM_RANGE(0xfc46, 0xfc46) AM_WRITEONLY AM_SHARE("scroll2xlo")
	AM_RANGE(0xfc47, 0xfc47) AM_WRITEONLY AM_SHARE("scroll2xhi")
	AM_RANGE(0xfcc0, 0xfcc0) AM_WRITE(wc90_sound_command_w)
	AM_RANGE(0xfcd0, 0xfcd0) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0xfce0, 0xfce0) AM_WRITE(wc90_bankswitch_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( wc90_map_2, AS_PROGRAM, 8, wc90_state )
	AM_RANGE(0x0000, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xcfff) AM_RAM
	AM_RANGE(0xd000, 0xd7ff) AM_RAM AM_SHARE("spriteram")
	AM_RANGE(0xd800, 0xdfff) AM_RAM
	AM_RANGE(0xe000, 0xe7ff) AM_RAM_WRITE(paletteram_xxxxBBBBRRRRGGGG_byte_be_w) AM_SHARE("paletteram")
	AM_RANGE(0xf000, 0xf7ff) AM_ROMBANK("bank2")
	AM_RANGE(0xf800, 0xfbff) AM_RAM AM_SHARE("share1")
	AM_RANGE(0xfc00, 0xfc00) AM_WRITE(wc90_bankswitch1_w)
	AM_RANGE(0xfc01, 0xfc01) AM_WRITE(watchdog_reset_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_map, AS_PROGRAM, 8, wc90_state )
	AM_RANGE(0x0000, 0xbfff) AM_ROM
	AM_RANGE(0xf000, 0xf7ff) AM_RAM
	AM_RANGE(0xf800, 0xf803) AM_DEVREADWRITE_LEGACY("ymsnd", ym2608_r, ym2608_w)
	AM_RANGE(0xfc00, 0xfc00) AM_READNOP /* ??? adpcm ??? */
	AM_RANGE(0xfc10, 0xfc10) AM_READ(soundlatch_byte_r)
ADDRESS_MAP_END


static INPUT_PORTS_START( wc90 )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coinage ) )      PORT_DIPLOCATION("SW1:8,7,6,5")
	PORT_DIPSETTING(    0x00, "10 Coins/1 Credit" )
	PORT_DIPSETTING(    0x08, DEF_STR( 9C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 8C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 7C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )   PORT_DIPLOCATION("SW1:4,3")
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x40, 0x40, "Count Down" )            PORT_DIPLOCATION("SW1:2")
	PORT_DIPSETTING(    0x40, "1 Count - 1 Second" )
	PORT_DIPSETTING(    0x00, "1 Count - 56/60 Second" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW1:1")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x03, "1 Player Game Time" )    PORT_DIPLOCATION("SW2:8,7")
	PORT_DIPSETTING(    0x01, "1:00" )
	PORT_DIPSETTING(    0x02, "1:30" )
	PORT_DIPSETTING(    0x03, "2:00" )
	PORT_DIPSETTING(    0x00, "2:30" )
	PORT_DIPNAME( 0x1c, 0x1c, "2 Players Game Time" )   PORT_DIPLOCATION("SW2:6,5,4")
	PORT_DIPSETTING(    0x0c, "1:00" )
	PORT_DIPSETTING(    0x14, "1:30" )
	PORT_DIPSETTING(    0x04, "2:00" )
	PORT_DIPSETTING(    0x18, "2:30" )
	PORT_DIPSETTING(    0x1c, "3:00" )
	PORT_DIPSETTING(    0x08, "3:30" )
	PORT_DIPSETTING(    0x10, "4:00" )
	PORT_DIPSETTING(    0x00, "5:00" )

	/* the following 3 switches are listed as "don't touch" */
	PORT_DIPUNUSED_DIPLOC( 0x20, 0x20, "SW2:3" )
	PORT_DIPUNUSED_DIPLOC( 0x40, 0x40, "SW2:2" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Language ) )     PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )  /* ON by default */
	PORT_DIPSETTING(    0x80, DEF_STR( Japanese ) )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout tilelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4,
			32*8+0*4, 32*8+1*4, 32*8+2*4, 32*8+3*4, 32*8+4*4, 32*8+5*4, 32*8+6*4, 32*8+7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32 },
	128*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, RGN_FRAC(1,2)+0*4, RGN_FRAC(1,2)+1*4, 2*4, 3*4, RGN_FRAC(1,2)+2*4, RGN_FRAC(1,2)+3*4,
			16*8+0*4, 16*8+1*4, RGN_FRAC(1,2)+16*8+0*4, RGN_FRAC(1,2)+16*8+1*4, 16*8+2*4, 16*8+3*4, RGN_FRAC(1,2)+16*8+2*4, RGN_FRAC(1,2)+16*8+3*4 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			16*16, 17*16, 18*16, 19*16, 20*16, 21*16, 22*16, 23*16 },
	64*8
};

static GFXDECODE_START( wc90 )
	GFXDECODE_ENTRY( "gfx1", 0x00000, charlayout,       1*16*16, 16*16 )
	GFXDECODE_ENTRY( "gfx2", 0x00000, tilelayout,       2*16*16, 16*16 )
	GFXDECODE_ENTRY( "gfx3", 0x00000, tilelayout,       3*16*16, 16*16 )
	GFXDECODE_ENTRY( "gfx4", 0x00000, spritelayout,     0*16*16, 16*16 ) // sprites
GFXDECODE_END



/* handler called by the 2608 emulator when the internal timers cause an IRQ */
static void irqhandler(device_t *device, int irq)
{
	device->machine().device("audiocpu")->execute().set_input_line(0, irq ? ASSERT_LINE : CLEAR_LINE);
}

static const ym2608_interface ym2608_config =
{
	{
		AY8910_LEGACY_OUTPUT | AY8910_SINGLE_OUTPUT,
		AY8910_DEFAULT_LOADS,
		DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL
	},
	irqhandler
};

static MACHINE_CONFIG_START( wc90, wc90_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80, XTAL_8MHz)     /* verified on pcb */
	MCFG_CPU_PROGRAM_MAP(wc90_map_1)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", wc90_state,  irq0_line_hold)

	MCFG_CPU_ADD("sub", Z80, XTAL_8MHz)     /* verified on pcb */
	MCFG_CPU_PROGRAM_MAP(wc90_map_2)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", wc90_state,  irq0_line_hold)

	MCFG_CPU_ADD("audiocpu", Z80, XTAL_8MHz/2)  /* verified on pcb */
	MCFG_CPU_PROGRAM_MAP(sound_map)
	/* NMIs are triggered by the main CPU */

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(59.17)         /* verified on pcb */
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(32*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(wc90_state, screen_update_wc90)

	MCFG_GFXDECODE(wc90)
	MCFG_PALETTE_LENGTH(1024)


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_SOUND_ADD("ymsnd", YM2608, XTAL_8MHz)  /* verified on pcb */
	MCFG_SOUND_CONFIG(ym2608_config)
	MCFG_SOUND_ROUTE(0, "mono", 0.50)
	MCFG_SOUND_ROUTE(1, "mono", 1.0)
	MCFG_SOUND_ROUTE(2, "mono", 1.0)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( wc90t, wc90 )
	MCFG_VIDEO_START_OVERRIDE(wc90_state, wc90t )
MACHINE_CONFIG_END

ROM_START( wc90 )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "ic87_01.bin",  0x00000, 0x08000, CRC(4a1affbc) SHA1(bc531e97ca31c66fdac194e2d79d5c6ba1300556) )  /* c000-ffff is not used */
	ROM_LOAD( "ic95_02.bin",  0x10000, 0x10000, CRC(847d439c) SHA1(eade31050da9e84feb4406e327d050a7496871b7) )  /* banked at f000-f7ff */

	ROM_REGION( 0x20000, "sub", 0 )  /* Second CPU */
	ROM_LOAD( "ic67_04.bin",  0x00000, 0x10000, CRC(dc6eaf00) SHA1(d53924070a59eee35dc0e6465702e4f04e61a073) )  /* c000-ffff is not used */
	ROM_LOAD( "ic56_03.bin",  0x10000, 0x10000, CRC(1ac02b3b) SHA1(4f8dc049d404072150342f3c2df04789a73ce244) )  /* banked at f000-f7ff */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "ic54_05.bin",  0x00000, 0x10000, CRC(27c348b3) SHA1(cf19ff4ae4f323ae3e5a905249b7af8ae342202a) )

	ROM_REGION( 0x010000, "gfx1", 0 )
	ROM_LOAD( "ic85_07v.bin", 0x00000, 0x10000, CRC(c5219426) SHA1(95e21fcd7de7d418ec287ae7087f6244c6bce5a8) )  /* characters */

	ROM_REGION( 0x040000, "gfx2", 0 )
	ROM_LOAD( "ic86_08v.bin", 0x00000, 0x20000, CRC(8fa1a1ff) SHA1(ce624617ac8c8b54e41294cf5dca7a09c91f53ba) )  /* tiles #1 */
	ROM_LOAD( "ic90_09v.bin", 0x20000, 0x20000, CRC(99f8841c) SHA1(1969b4d78ca00924a7550826e1c4f4fa0588ef02) )  /* tiles #2 */

	ROM_REGION( 0x040000, "gfx3", 0 )
	ROM_LOAD( "ic87_10v.bin", 0x00000, 0x20000, CRC(8232093d) SHA1(59bf9c9a858b47326cf0c64b1ee6ac727a15a20b) )  /* tiles #3 */
	ROM_LOAD( "ic91_11v.bin", 0x20000, 0x20000, CRC(188d3789) SHA1(35654a99a20735bae09b32f74255f8132dee9af2) )  /* tiles #4 */

	ROM_REGION( 0x080000, "gfx4", 0 )
	ROM_LOAD( "ic50_12v.bin", 0x00000, 0x20000, CRC(da1fe922) SHA1(5184053c2b7dd2bf1cd2e9f783686f2c0db7e47b) )  /* sprites  */
	ROM_LOAD( "ic54_13v.bin", 0x20000, 0x20000, CRC(9ad03c2c) SHA1(1c1947f9b51a58002e9992fc7c0c1a1c59b4d740) )  /* sprites  */
	ROM_LOAD( "ic60_14v.bin", 0x40000, 0x20000, CRC(499dfb1b) SHA1(ac67985d36fea18c82a4ea00019d9e6e4bcb5d0d) )  /* sprites  */
	ROM_LOAD( "ic65_15v.bin", 0x60000, 0x20000, CRC(d8ea5c81) SHA1(ccb3f7d565b1c1b8e874a2df91cda40dde2962ed) )  /* sprites  */

	ROM_REGION( 0x20000, "ymsnd", 0 )   /* 64k for ADPCM samples */
	ROM_LOAD( "ic82_06.bin",  0x00000, 0x20000, CRC(2fd692ed) SHA1(0273dc39181504320bec0187d074b2f86c821508) )
ROM_END

ROM_START( wc90a )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "wc90-1.bin",   0x00000, 0x08000, CRC(d1804e1a) SHA1(eec7374f4d23c89843f38fffff436635adb43b63) )  /* c000-ffff is not used */
	ROM_LOAD( "ic95_02.bin",  0x10000, 0x10000, CRC(847d439c) SHA1(eade31050da9e84feb4406e327d050a7496871b7) )  /* banked at f000-f7ff */

	ROM_REGION( 0x20000, "sub", 0 )  /* Second CPU */
	ROM_LOAD( "ic67_04.bin",  0x00000, 0x10000, CRC(dc6eaf00) SHA1(d53924070a59eee35dc0e6465702e4f04e61a073) )  /* c000-ffff is not used */
	ROM_LOAD( "ic56_03.bin",  0x10000, 0x10000, CRC(1ac02b3b) SHA1(4f8dc049d404072150342f3c2df04789a73ce244) )  /* banked at f000-f7ff */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "ic54_05.bin",  0x00000, 0x10000, CRC(27c348b3) SHA1(cf19ff4ae4f323ae3e5a905249b7af8ae342202a) )

	ROM_REGION( 0x010000, "gfx1", 0 )
	ROM_LOAD( "ic85_07v.bin", 0x00000, 0x10000, CRC(c5219426) SHA1(95e21fcd7de7d418ec287ae7087f6244c6bce5a8) )  /* characters */

	ROM_REGION( 0x040000, "gfx2", 0 )
	ROM_LOAD( "ic86_08v.bin", 0x00000, 0x20000, CRC(8fa1a1ff) SHA1(ce624617ac8c8b54e41294cf5dca7a09c91f53ba) )  /* tiles #1 */
	ROM_LOAD( "ic90_09v.bin", 0x20000, 0x20000, CRC(99f8841c) SHA1(1969b4d78ca00924a7550826e1c4f4fa0588ef02) )  /* tiles #2 */

	ROM_REGION( 0x040000, "gfx3", 0 )
	ROM_LOAD( "ic87_10v.bin", 0x00000, 0x20000, CRC(8232093d) SHA1(59bf9c9a858b47326cf0c64b1ee6ac727a15a20b) )  /* tiles #3 */
	ROM_LOAD( "ic91_11v.bin", 0x20000, 0x20000, CRC(188d3789) SHA1(35654a99a20735bae09b32f74255f8132dee9af2) )  /* tiles #4 */

	ROM_REGION( 0x080000, "gfx4", 0 )
	ROM_LOAD( "ic50_12v.bin", 0x00000, 0x20000, CRC(da1fe922) SHA1(5184053c2b7dd2bf1cd2e9f783686f2c0db7e47b) )  /* sprites  */
	ROM_LOAD( "ic54_13v.bin", 0x20000, 0x20000, CRC(9ad03c2c) SHA1(1c1947f9b51a58002e9992fc7c0c1a1c59b4d740) )  /* sprites  */
	ROM_LOAD( "ic60_14v.bin", 0x40000, 0x20000, CRC(499dfb1b) SHA1(ac67985d36fea18c82a4ea00019d9e6e4bcb5d0d) )  /* sprites  */
	ROM_LOAD( "ic65_15v.bin", 0x60000, 0x20000, CRC(d8ea5c81) SHA1(ccb3f7d565b1c1b8e874a2df91cda40dde2962ed) )  /* sprites  */

	ROM_REGION( 0x20000, "ymsnd", 0 )   /* 64k for ADPCM samples */
	ROM_LOAD( "ic82_06.bin",  0x00000, 0x20000, CRC(2fd692ed) SHA1(0273dc39181504320bec0187d074b2f86c821508) )
ROM_END

ROM_START( wc90b )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "ic87-1b.bin",  0x00000, 0x08000, CRC(d024a971) SHA1(856c6ab7abc1cd6db42703f70930b84e3da69db0) )  /* c000-ffff is not used */
	ROM_LOAD( "ic95_02.bin",  0x10000, 0x10000, CRC(847d439c) SHA1(eade31050da9e84feb4406e327d050a7496871b7) )  /* banked at f000-f7ff */

	ROM_REGION( 0x20000, "sub", 0 )  /* Second CPU */
	ROM_LOAD( "ic67_04.bin",  0x00000, 0x10000, CRC(dc6eaf00) SHA1(d53924070a59eee35dc0e6465702e4f04e61a073) )  /* c000-ffff is not used */
	ROM_LOAD( "ic56_03.bin",  0x10000, 0x10000, CRC(1ac02b3b) SHA1(4f8dc049d404072150342f3c2df04789a73ce244) )  /* banked at f000-f7ff */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "ic54_05.bin",  0x00000, 0x10000, CRC(27c348b3) SHA1(cf19ff4ae4f323ae3e5a905249b7af8ae342202a) )

	ROM_REGION( 0x010000, "gfx1", 0 )
	ROM_LOAD( "ic85_07v.bin", 0x00000, 0x10000, CRC(c5219426) SHA1(95e21fcd7de7d418ec287ae7087f6244c6bce5a8) )  /* characters */

	ROM_REGION( 0x040000, "gfx2", 0 )
	ROM_LOAD( "ic86_08v.bin", 0x00000, 0x20000, CRC(8fa1a1ff) SHA1(ce624617ac8c8b54e41294cf5dca7a09c91f53ba) )  /* tiles #1 */
	ROM_LOAD( "ic90_09v.bin", 0x20000, 0x20000, CRC(99f8841c) SHA1(1969b4d78ca00924a7550826e1c4f4fa0588ef02) )  /* tiles #2 */

	ROM_REGION( 0x040000, "gfx3", 0 )
	ROM_LOAD( "ic87_10v.bin", 0x00000, 0x20000, CRC(8232093d) SHA1(59bf9c9a858b47326cf0c64b1ee6ac727a15a20b) )  /* tiles #3 */
	ROM_LOAD( "ic91_11v.bin", 0x20000, 0x20000, CRC(188d3789) SHA1(35654a99a20735bae09b32f74255f8132dee9af2) )  /* tiles #4 */

	ROM_REGION( 0x080000, "gfx4", 0 )
	ROM_LOAD( "ic50_12v.bin", 0x00000, 0x20000, CRC(da1fe922) SHA1(5184053c2b7dd2bf1cd2e9f783686f2c0db7e47b) )  /* sprites  */
	ROM_LOAD( "ic54_13v.bin", 0x20000, 0x20000, CRC(9ad03c2c) SHA1(1c1947f9b51a58002e9992fc7c0c1a1c59b4d740) )  /* sprites  */
	ROM_LOAD( "ic60_14v.bin", 0x40000, 0x20000, CRC(499dfb1b) SHA1(ac67985d36fea18c82a4ea00019d9e6e4bcb5d0d) )  /* sprites  */
	ROM_LOAD( "ic65_15v.bin", 0x60000, 0x20000, CRC(d8ea5c81) SHA1(ccb3f7d565b1c1b8e874a2df91cda40dde2962ed) )  /* sprites  */

	ROM_REGION( 0x20000, "ymsnd", 0 )   /* 64k for ADPCM samples */
	ROM_LOAD( "ic82_06.bin",  0x00000, 0x20000, CRC(2fd692ed) SHA1(0273dc39181504320bec0187d074b2f86c821508) )
ROM_END

ROM_START( wc90t )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "wc90a-1.bin",  0x00000, 0x08000, CRC(b6f51a68) SHA1(e0263dee35bf99cb4288a1df825bbbca17c85d36) )  /* c000-ffff is not used */
	ROM_LOAD( "wc90a-2.bin",  0x10000, 0x10000, CRC(c50f2a98) SHA1(0fbeabadebfa75515d5e35bfcc565ecfa4d6e693) )  /* banked at f000-f7ff */

	ROM_REGION( 0x20000, "sub", 0 )  /* Second CPU */
	ROM_LOAD( "ic67_04.bin",  0x00000, 0x10000, CRC(dc6eaf00) SHA1(d53924070a59eee35dc0e6465702e4f04e61a073) )  /* c000-ffff is not used */
	ROM_LOAD( "wc90a-3.bin",  0x10000, 0x10000, CRC(8c7a9542) SHA1(a06a7cd40d41692c4cc2a35d9c69b944c5baf163) )  /* banked at f000-f7ff */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "ic54_05.bin",  0x00000, 0x10000, CRC(27c348b3) SHA1(cf19ff4ae4f323ae3e5a905249b7af8ae342202a) )

	ROM_REGION( 0x010000, "gfx1", 0 )
	ROM_LOAD( "ic85_07v.bin", 0x00000, 0x10000, CRC(c5219426) SHA1(95e21fcd7de7d418ec287ae7087f6244c6bce5a8) )  /* characters */

	ROM_REGION( 0x040000, "gfx2", 0 )
	ROM_LOAD( "ic86_08v.bin", 0x00000, 0x20000, CRC(8fa1a1ff) SHA1(ce624617ac8c8b54e41294cf5dca7a09c91f53ba) )  /* tiles #1 */
	ROM_LOAD( "ic90_09v.bin", 0x20000, 0x20000, CRC(99f8841c) SHA1(1969b4d78ca00924a7550826e1c4f4fa0588ef02) )  /* tiles #2 */

	ROM_REGION( 0x040000, "gfx3", 0 )
	ROM_LOAD( "ic87_10v.bin", 0x00000, 0x20000, CRC(8232093d) SHA1(59bf9c9a858b47326cf0c64b1ee6ac727a15a20b) )  /* tiles #3 */
	ROM_LOAD( "ic91_11v.bin", 0x20000, 0x20000, CRC(188d3789) SHA1(35654a99a20735bae09b32f74255f8132dee9af2) )  /* tiles #4 */

	ROM_REGION( 0x080000, "gfx4", 0 )
	ROM_LOAD( "ic50_12v.bin", 0x00000, 0x20000, CRC(da1fe922) SHA1(5184053c2b7dd2bf1cd2e9f783686f2c0db7e47b) )  /* sprites  */
	ROM_LOAD( "ic54_13v.bin", 0x20000, 0x20000, CRC(9ad03c2c) SHA1(1c1947f9b51a58002e9992fc7c0c1a1c59b4d740) )  /* sprites  */
	ROM_LOAD( "ic60_14v.bin", 0x40000, 0x20000, CRC(499dfb1b) SHA1(ac67985d36fea18c82a4ea00019d9e6e4bcb5d0d) )  /* sprites  */
	ROM_LOAD( "ic65_15v.bin", 0x60000, 0x20000, CRC(d8ea5c81) SHA1(ccb3f7d565b1c1b8e874a2df91cda40dde2962ed) )  /* sprites  */

	ROM_REGION( 0x20000, "ymsnd", 0 )   /* 64k for ADPCM samples */
	ROM_LOAD( "ic82_06.bin",  0x00000, 0x20000, CRC(2fd692ed) SHA1(0273dc39181504320bec0187d074b2f86c821508) )
ROM_END

GAME( 1989, wc90,  0,    wc90, wc90, driver_device, 0, ROT0, "Tecmo", "Tecmo World Cup '90 (World)", GAME_IMPERFECT_SOUND | GAME_NO_COCKTAIL )
GAME( 1989, wc90a, wc90, wc90, wc90, driver_device, 0, ROT0, "Tecmo", "Tecmo World Cup '90 (Euro set 1)", GAME_IMPERFECT_SOUND | GAME_NO_COCKTAIL )
GAME( 1989, wc90b, wc90, wc90, wc90, driver_device, 0, ROT0, "Tecmo", "Tecmo World Cup '90 (Euro set 2)", GAME_IMPERFECT_SOUND | GAME_NO_COCKTAIL )
GAME( 1989, wc90t, wc90, wc90t,wc90, driver_device, 0, ROT0, "Tecmo", "Tecmo World Cup '90 (trackball set 1)", GAME_IMPERFECT_SOUND | GAME_NO_COCKTAIL )
