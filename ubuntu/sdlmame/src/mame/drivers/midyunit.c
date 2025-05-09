/*************************************************************************

    Williams/Midway Y/Z-unit system

    driver by Alex Pasadyn, Zsolt Vasvari, Kurt Mahan, Ernesto Corvi,
    and Aaron Giles

    Games supported (prototype and release versions):
        * Narc
        * Trog
        * Strike Force
        * Smash TV
        * High Impact Football
        * Super High Impact
        * Terminator 2
        * Mortal Kombat (Y-unit versions)
        * Total Carnage

    Known bugs:
        * when the Porsche spins in Narc, the wheels are missing for
            a single frame (may be an original bug)
        * Terminator 2 freezes while playing the movies after destroying
            skynet. Currently we have a hack in which prevents the freeze,
            but we really should eventually figure it out for real

    Notes:
        * Super Hi Impact Proto V4.0: You need to at least reset the high score
            table from the UTILITIES menu.  It's best to do a FULL FACTORY RESTORE


Super High Impact
Midway, 1991

This game runs on (typical) Midway Y-Unit hardware. The PCB is a base
board 'system' that can run other Y-Unit games by swapping ROMs and
the protection chip.

PCB Layout
----------

5770-12555-00 REV. - A
|-------------------------------------------------------------------|
|                     6264  PAL    53461 53461 53461 48MHZ 24MHz  J6|
|J4                                53461 53461 53461                |
|                                  53461 53461 53461                |
|                                  53461 53461 53461                |
|J2           MAX691  6264  PAL                         |-----|     |
|                         PAL PAL                       |TMS  |   J7|
|RESET_SW                                               |34010|     |
|  LED1                                                 |-----|     |
|    LED2             6264 PAL PAL                                  |
|                                                                   |
|J       41464 41464                                              J8|
|A  PAL  41464 41464                                                |
|M                                                                  |
|M                                                                  |
|A           U89  U90  U91  U92  U93     U95  U96  U97  U98         |
|                                                     |--------|    |
|                                                     |L1A3787 |    |
|                                                     |WILLIAMS|    |
|                                                     |5410-12239   |
|                                                     |--------|    |
|DSW1 DSW2                                                          |
|BATTERY                                             *A-5346-40017-8|
|                                                                   |
| J13   J12  U105 U106 U107 U108 U109    U111 U112 U113 U114        |
|-------------------------------------------------------------------|
Notes:
      *    - Intel P5C090-50 protection chip labelled 'A-5346-40017-8'
             clocks on pin1 6.00MHz, pin4 6.00MHz, pin21 6.00MHz
     J*    - multi-pin connectors for additional controls, cabinet switches
             and power input and output
     J8    - used to connect external sound PCB via a flat cable.
     J2    - Used to supply power to external sound PCB
     34010 - clock 6.000MHz [48/8]
     LED1  - power active
     LED2  - data active, flickers while PCB is working
     U89/105  - main program EPROMs 27C010
     Other U* - 27C020 EPROMs


Sound PCB
---------

5766-12702-00 REV. B
|----------------------------------------|
|    3.579545MHz 6116  U4  U19  U20      |
|  YM2151                                |
|YM3012                                  |
|      458                         6809  |
| 458                                    |
|         MC1408                         |
|J1            6821                      |
|                                        |
| 458    55536                  8MHz     |
|     458                                |
|                                        |
|J2                                     J|
|         J3     J4       TDA2002 TDA2002|
|----------------------------------------|
Notes:
      6809   - clock 2.000MHz [8/4]
      YM2151 - clock 3.579545MHz
      55536  - Harris HC-55536 Continuously Variable Slope Delta Modulator
      6116   - 2k x8 SRAM
      U*     - 27C010 EPROMs
      J4     - flat cable connector from main board J8
      J3     - power input connector
      J2     - connector for volume pot
      J      - speakers output connector

**************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "cpu/tms34010/tms34010.h"
#include "sound/okim6295.h"
#include "includes/midyunit.h"


/* master clocks vary based on game */
#define SLOW_MASTER_CLOCK       XTAL_40MHz      /* "slow" == smashtv, trog, hiimpact */
#define FAST_MASTER_CLOCK       XTAL_48MHz      /* "fast" == narc, mk, totcarn, strkforc */
#define FASTER_MASTER_CLOCK     XTAL_50MHz      /* "faster" == term2 */

/* pixel clocks are 48MHz (narc) or 24MHz (all others) regardless */
#define MEDRES_PIXEL_CLOCK      (XTAL_48MHz / 6)
#define STDRES_PIXEL_CLOCK      (XTAL_24MHz / 6)



/*************************************
 *
 *  Yawdim sound banking
 *
 *************************************/

WRITE8_MEMBER(midyunit_state::yawdim_oki_bank_w)
{
	device_t *device = machine().device("oki");
	if (data & 4)
		downcast<okim6295_device *>(device)->set_bank_base(0x40000 * (data & 3));
}



/*************************************
 *
 *  Sound board comms
 *
 *************************************/

CUSTOM_INPUT_MEMBER(midyunit_state::narc_talkback_strobe_r)
{
	return (m_narc_sound->read(machine().driver_data()->generic_space(), 0) >> 8) & 1;
}


CUSTOM_INPUT_MEMBER(midyunit_state::narc_talkback_data_r)
{
	return m_narc_sound->read(machine().driver_data()->generic_space(), 0) & 0xff;
}


CUSTOM_INPUT_MEMBER(midyunit_state::adpcm_irq_state_r)
{
	return m_adpcm_sound->irq_read() & 1;
}



/*************************************
 *
 *  Memory maps
 *
 *************************************/

static ADDRESS_MAP_START( main_map, AS_PROGRAM, 16, midyunit_state )
	AM_RANGE(0x00000000, 0x001fffff) AM_READWRITE(midyunit_vram_r, midyunit_vram_w)
	AM_RANGE(0x01000000, 0x010fffff) AM_RAM
	AM_RANGE(0x01400000, 0x0140ffff) AM_READWRITE(midyunit_cmos_r, midyunit_cmos_w)
	AM_RANGE(0x01800000, 0x0181ffff) AM_RAM_WRITE(midyunit_paletteram_w) AM_SHARE("paletteram")
	AM_RANGE(0x01a80000, 0x01a8009f) AM_MIRROR(0x00080000) AM_READWRITE(midyunit_dma_r, midyunit_dma_w)
	AM_RANGE(0x01c00000, 0x01c0005f) AM_READ(midyunit_input_r)
	AM_RANGE(0x01c00060, 0x01c0007f) AM_READWRITE(midyunit_protection_r, midyunit_cmos_enable_w)
	AM_RANGE(0x01e00000, 0x01e0001f) AM_WRITE(midyunit_sound_w)
	AM_RANGE(0x01f00000, 0x01f0001f) AM_WRITE(midyunit_control_w)
	AM_RANGE(0x02000000, 0x05ffffff) AM_READ(midyunit_gfxrom_r) AM_SHARE("gfx_rom")
	AM_RANGE(0xc0000000, 0xc00001ff) AM_READWRITE_LEGACY(tms34010_io_register_r, tms34010_io_register_w)
	AM_RANGE(0xff800000, 0xffffffff) AM_ROM AM_REGION("user1", 0)
ADDRESS_MAP_END


static ADDRESS_MAP_START( yawdim_sound_map, AS_PROGRAM, 8, midyunit_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0x9000, 0x97ff) AM_WRITE(yawdim_oki_bank_w)
	AM_RANGE(0x9800, 0x9fff) AM_DEVREADWRITE("oki", okim6295_device, read, write)
	AM_RANGE(0xa000, 0xa7ff) AM_READ(soundlatch_byte_r)
ADDRESS_MAP_END



/*************************************
 *
 *  Input ports
 *
 *************************************/

static INPUT_PORTS_START( narc )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P1 Crouch") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Fire") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Jump") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 Rocket Bomb") PORT_PLAYER(1)
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P2 Crouch") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 Fire") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Jump") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P2 Rocket Bomb") PORT_PLAYER(2)

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) /* Slam Switch */
	PORT_SERVICE_NO_TOGGLE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNUSED ) /* Video Freeze */
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Vault Switch") PORT_CODE(KEYCODE_9)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0400, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, midyunit_state,narc_talkback_strobe_r, NULL)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNUSED ) /* memory protect interlock */
	PORT_BIT( 0x3000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0xc000, IP_ACTIVE_LOW, IPT_UNUSED )
/*
    Test mode indicates "Cut for French" and "Cut for German", hinting that these
    are jumpers or wires that can be modified on the PCB. However, there are no
    French or German strings in the ROMs, and this "feature" was clearly never
    actually implemented
    PORT_DIPNAME( 0xc000, 0xc000, DEF_STR( Language ) )
    PORT_DIPSETTING(      0xc000, DEF_STR( English ) )
    PORT_DIPSETTING(      0x8000, DEF_STR( French ) )
    PORT_DIPSETTING(      0x4000, DEF_STR( German ) )
*/

	PORT_START("IN2")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, midyunit_state,narc_talkback_data_r, NULL)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK0")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK1")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END


static INPUT_PORTS_START( trog )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Punch") PORT_PLAYER(1)
	PORT_BIT( 0x00e0, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 Punch") PORT_PLAYER(2)
	PORT_BIT( 0xe000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) /* Slam Switch */
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BILL1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNUSED ) /* video freeze */
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P3 Punch") PORT_PLAYER(3)

	PORT_START("IN2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P4 Punch") PORT_PLAYER(4)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0xffc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0038, 0x0038, DEF_STR( Coinage ))
	PORT_DIPSETTING(      0x0038, "1" )
	PORT_DIPSETTING(      0x0018, "2" )
	PORT_DIPSETTING(      0x0028, "3" )
	PORT_DIPSETTING(      0x0008, "4" )
	PORT_DIPSETTING(      0x0030, "ECA" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ))
	PORT_DIPNAME( 0x0040, 0x0000, "Coinage Source" )
	PORT_DIPSETTING(      0x0040, "Dipswitch" )
	PORT_DIPSETTING(      0x0000, "CMOS" )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Cabinet ))
	PORT_DIPSETTING(      0x0080, DEF_STR( Upright ))
	PORT_DIPSETTING(      0x0000, DEF_STR( Cocktail ))
	PORT_DIPNAME( 0x0100, 0x0100, "Test Switch" )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0200, 0x0200, "Video Freeze" )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Players ) )
	PORT_DIPSETTING(      0x0c00, "4 Players" )
	PORT_DIPSETTING(      0x0400, "3 Players" )
	PORT_DIPSETTING(      0x0800, "2 Players" )
	PORT_DIPSETTING(      0x0000, "1 Player" )
	PORT_DIPNAME( 0x1000, 0x0000, "Coin Counters" )
	PORT_DIPSETTING(      0x1000, "One Counter" )
	PORT_DIPSETTING(      0x0000, "Two Counters" )
	PORT_DIPNAME( 0x2000, 0x0000, "Powerup Test" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x2000, DEF_STR( On ))
	PORT_DIPNAME( 0xc000, 0xc000, "Country" )
	PORT_DIPSETTING(      0xc000, DEF_STR( USA ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( French ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( German ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( Unused ))

	PORT_START("UNK0")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK1")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END


static INPUT_PORTS_START( smashtv )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_NAME("P1 Move Up") PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_NAME("P1 Move Down") PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_NAME("P1 Move Left") PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_NAME("P1 Move Right") PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_NAME("P1 Fire Up") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_NAME("P1 Fire Down") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_NAME("P1 Fire Left")PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_NAME("P1 Fire Right") PORT_PLAYER(1)
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_NAME("P2 Move Up") PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_NAME("P2 Move Down") PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_NAME("P2 Move Left") PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_NAME("P2 Move Right") PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_NAME("P2 Fire Up") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_NAME("P2 Fire Down") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_NAME("P2 Fire Left") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_NAME("P2 Fire Right") PORT_PLAYER(2)


	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) /* Slam Switch */
	PORT_SERVICE_NO_TOGGLE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BILL1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNUSED ) /* video freeze */
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0xf800, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0001, 0x0001, "1-8" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0100, 0x0100, "2-8" ) /* Coinage? */
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown )) /* Coinage? */
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown )) /* Coinage? */
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown )) /* Coinage? */
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown )) /* Coinage? */
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown )) /* Coinage? */
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown )) /* Rotary Joystick enable? */
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Service_Mode ))
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
/*  I cannot figure out how to enable dip coinage
Does the Rotary Joystick Dip do anything?  */

	PORT_START("UNK0")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK1")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END


static INPUT_PORTS_START( strkforc )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Fire") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 Weapon") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Weapon Select") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Fire") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P2 Weapon") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Weapon Select") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 ) PORT_NAME("P1 Start / Transform")
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) /* Slam Switch */
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 ) PORT_NAME("P2 Start / Transform")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0xfc00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0001, 0x0001, "Coin Meter" )
	PORT_DIPSETTING(      0x0001, "Shared" )
	PORT_DIPSETTING(      0x0000, "Independent" )
	PORT_DIPNAME( 0x0002, 0x0002, "Credits to Start" )
	PORT_DIPSETTING(      0x0002, "1" )
	PORT_DIPSETTING(      0x0000, "2" )
	PORT_DIPNAME( 0x000c, 0x000c, "Points for Ship" )
	PORT_DIPSETTING(      0x0008, "40000" )
	PORT_DIPSETTING(      0x000c, "50000" )
	PORT_DIPSETTING(      0x0004, "75000" )
	PORT_DIPSETTING(      0x0000, "100000" )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Lives ))
	PORT_DIPSETTING(      0x0010, "3" )
	PORT_DIPSETTING(      0x0000, "4" )
	PORT_DIPNAME( 0x00e0, 0x00e0, DEF_STR( Difficulty ))
	PORT_DIPSETTING(      0x0080, "Level 1" )
	PORT_DIPSETTING(      0x00c0, "Level 2" )
	PORT_DIPSETTING(      0x00a0, "Level 3" )
	PORT_DIPSETTING(      0x00e0, "Level 4" )
	PORT_DIPSETTING(      0x0060, "Level 5" )
	PORT_DIPSETTING(      0x0040, "Level 6" )
	PORT_DIPSETTING(      0x0020, "Level 7" )
	PORT_DIPSETTING(      0x0000, "Level 8" )
	PORT_DIPNAME( 0x0700, 0x0700, DEF_STR( Coin_B ))
	PORT_DIPSETTING(      0x0700, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(      0x0600, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(      0x0500, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(      0x0400, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(      0x0300, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(      0x0200, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(      0x0100, "U.K. Elect." )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ))
	PORT_DIPNAME( 0x7800, 0x7800, DEF_STR( Coin_A ))
	PORT_DIPSETTING(      0x3000, DEF_STR( 5C_1C ))
	PORT_DIPSETTING(      0x3800, DEF_STR( 4C_1C ))
	PORT_DIPSETTING(      0x4000, DEF_STR( 3C_1C ))
	PORT_DIPSETTING(      0x4800, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(      0x7800, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(      0x7000, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(      0x6800, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(      0x6000, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(      0x5800, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(      0x5000, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(      0x2800, "1 Coin/1 Credit - 2 Coins/3 Credits" )
	PORT_DIPSETTING(      0x2000, "1 Coin/1 Credit - 3 Coins/4 Credits" )
	PORT_DIPSETTING(      0x1800, "1 Coin/1 Credit - 4 Coins/5 Credits" )
	PORT_DIPSETTING(      0x1000, "1 Coin/1 Credit - 5 Coins/6 Credits" )
	PORT_DIPSETTING(      0x0800, "3 Coin/1 Credit - 5 Coins/2 Credits" )
	PORT_DIPSETTING(      0x0000, "1 Coin/2 Credits - 2 Coins/5 Credits" )
	PORT_DIPNAME( 0x8000, 0x8000, "Test Switch" )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))

	PORT_START("UNK0")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK1")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END


static INPUT_PORTS_START( mkla2 )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 High Punch") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Block") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 High Kick") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 High Punch") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Block") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 High Kick") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) /* Slam Switch */
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BILL1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P2 Low Punch") PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("P2 Low Kick") PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("P2 Block 2") PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P1 Low Punch") PORT_PLAYER(1)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("P1 Low Kick") PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, midyunit_state,adpcm_irq_state_r, NULL)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("P1 Block 2") PORT_PLAYER(1)

	PORT_START("IN2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_BIT( 0x000f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0010, 0x0010, "Attract Sound" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0010, DEF_STR( On ))
	PORT_DIPNAME( 0x0020, 0x0020, "Low Blows" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0020, DEF_STR( On ))
	PORT_DIPNAME( 0x0040, 0x0040, "Blood" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0040, DEF_STR( On ))
	PORT_DIPNAME( 0x0080, 0x0080, "Violence" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0080, DEF_STR( On ))
	PORT_DIPNAME( 0x0100, 0x0100, "Test Switch" )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0200, 0x0000, "Counters" )
	PORT_DIPSETTING(      0x0200, "One" )
	PORT_DIPSETTING(      0x0000, "Two" )
	PORT_DIPNAME( 0x7c00, 0x7c00, DEF_STR( Coinage ))
	PORT_DIPSETTING(      0x7c00, "USA-1" )
	PORT_DIPSETTING(      0x3c00, "USA-2" )
	PORT_DIPSETTING(      0x5c00, "USA-3" )
	PORT_DIPSETTING(      0x1c00, "USA-4" )
	PORT_DIPSETTING(      0x6c00, "USA-ECA" )
	PORT_DIPSETTING(      0x0c00, "USA-Free Play" )
	PORT_DIPSETTING(      0x7400, "German-1" )
	PORT_DIPSETTING(      0x3400, "German-2" )
	PORT_DIPSETTING(      0x5400, "German-3" )
	PORT_DIPSETTING(      0x1400, "German-4" )
	PORT_DIPSETTING(      0x6400, "German-5" )
	PORT_DIPSETTING(      0x2400, "German-ECA" )
	PORT_DIPSETTING(      0x0400, "German-Free Play" )
	PORT_DIPSETTING(      0x7800, "French-1" )
	PORT_DIPSETTING(      0x3800, "French-2" )
	PORT_DIPSETTING(      0x5800, "French-3" )
	PORT_DIPSETTING(      0x1800, "French-4" )
	PORT_DIPSETTING(      0x6800, "French-ECA" )
	PORT_DIPSETTING(      0x0800, "French-Free Play" )
	PORT_DIPNAME( 0x8000, 0x0000, "Coinage Source" )
	PORT_DIPSETTING(      0x8000, "Dipswitch" )
	PORT_DIPSETTING(      0x0000, "CMOS" )

	PORT_START("UNK0")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK1")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END


static INPUT_PORTS_START( mkla4 )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 High Punch") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Block") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 High Kick") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 High Punch") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Block") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P2 High Kick") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) /* Slam Switch */
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BILL1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P2 Low Punch") PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("P2 Low Kick") PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("P2 Block 2") PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P1 Low Punch") PORT_PLAYER(1)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("P1 Low Kick") PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, midyunit_state,adpcm_irq_state_r, NULL)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("P1 Block 2") PORT_PLAYER(1)

	PORT_START("IN2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_BIT( 0x0007, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0008, 0x0008, "Comic Book Offer" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0008, DEF_STR( On ))
	PORT_DIPNAME( 0x0010, 0x0010, "Attract Sound" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0010, DEF_STR( On ))
	PORT_DIPNAME( 0x0020, 0x0020, "Low Blows" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0020, DEF_STR( On ))
	PORT_DIPNAME( 0x0040, 0x0040, "Blood" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0040, DEF_STR( On ))
	PORT_DIPNAME( 0x0080, 0x0080, "Violence" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0080, DEF_STR( On ))
	PORT_DIPNAME( 0x0100, 0x0100, "Test Switch" )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0200, 0x0000, "Counters" )
	PORT_DIPSETTING(      0x0200, "One" )
	PORT_DIPSETTING(      0x0000, "Two" )
	PORT_DIPNAME( 0x7c00, 0x7c00, DEF_STR( Coinage ))
	PORT_DIPSETTING(      0x7c00, "USA-1" )
	PORT_DIPSETTING(      0x3c00, "USA-2" )
	PORT_DIPSETTING(      0x5c00, "USA-3" )
	PORT_DIPSETTING(      0x1c00, "USA-4" )
	PORT_DIPSETTING(      0x6c00, "USA-ECA" )
	PORT_DIPSETTING(      0x0c00, "USA-Free Play" )
	PORT_DIPSETTING(      0x7400, "German-1" )
	PORT_DIPSETTING(      0x3400, "German-2" )
	PORT_DIPSETTING(      0x5400, "German-3" )
	PORT_DIPSETTING(      0x1400, "German-4" )
	PORT_DIPSETTING(      0x6400, "German-5" )
	PORT_DIPSETTING(      0x2400, "German-ECA" )
	PORT_DIPSETTING(      0x0400, "German-Free Play" )
	PORT_DIPSETTING(      0x7800, "French-1" )
	PORT_DIPSETTING(      0x3800, "French-2" )
	PORT_DIPSETTING(      0x5800, "French-3" )
	PORT_DIPSETTING(      0x1800, "French-4" )
	PORT_DIPSETTING(      0x6800, "French-ECA" )
	PORT_DIPSETTING(      0x0800, "French-Free Play" )
	PORT_DIPNAME( 0x8000, 0x0000, "Coinage Source" )
	PORT_DIPSETTING(      0x8000, "Dipswitch" )
	PORT_DIPSETTING(      0x0000, "CMOS" )

	PORT_START("UNK0")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK1")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( mkyawdim )
	PORT_INCLUDE( mkla4 )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNUSED ) // no ADPCM in these bootlegs
INPUT_PORTS_END

static INPUT_PORTS_START( term2 )
	PORT_START("IN0")
	PORT_BIT( 0x000f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Trigger") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Bomb") PORT_PLAYER(1)
	PORT_BIT( 0x00c0, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0f00, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 Trigger") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Bomb") PORT_PLAYER(2)
	PORT_BIT( 0xc000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) /* Slam Switch */
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNUSED ) /* video freeze */
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x3000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, midyunit_state,adpcm_irq_state_r, NULL)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0007, 0x0003, "Credits" )
	PORT_DIPSETTING(      0x0007, "2 Start/1 Continue" )
	PORT_DIPSETTING(      0x0006, "4 Start/1 Continue" )
	PORT_DIPSETTING(      0x0005, "2 Start/2 Continue" )
	PORT_DIPSETTING(      0x0004, "4 Start/2 Continue" )
	PORT_DIPSETTING(      0x0003, "1 Start/1 Continue" )
	PORT_DIPSETTING(      0x0002, "3 Start/2 Continue" )
	PORT_DIPSETTING(      0x0001, "3 Start/1 Continue" )
	PORT_DIPSETTING(      0x0000, "3 Start/3 Continue" )
	PORT_DIPNAME( 0x0038, 0x0038, DEF_STR( Coinage ))
	PORT_DIPSETTING(      0x0038, "1" )
	PORT_DIPSETTING(      0x0018, "2" )
	PORT_DIPSETTING(      0x0028, "3" )
	PORT_DIPSETTING(      0x0008, "4" )
	PORT_DIPSETTING(      0x0030, "ECA" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ))
	PORT_DIPNAME( 0x0040, 0x0040, "Dipswitch Coinage" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0040, DEF_STR( On ))
	PORT_DIPNAME( 0x0080, 0x0080, "Normal Display" )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0100, 0x0100, "Test Switch" )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0200, 0x0200, "Video Freeze" )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Players ) )
	PORT_DIPSETTING(      0x0800, "2 Players" )
	PORT_DIPSETTING(      0x0000, "1 Player" )
	PORT_DIPNAME( 0x1000, 0x0000, "Two Counters" )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x2000, 0x0000, "Powerup Test" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x2000, DEF_STR( On ))
	PORT_DIPNAME( 0xc000, 0xc000, "Country" )
	PORT_DIPSETTING(      0xc000, DEF_STR( USA ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( French ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( German ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( Unused ))

	PORT_START("UNK0")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK1")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("STICK0_X")
	PORT_BIT( 0x00ff, 0x0080, IPT_AD_STICK_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_REVERSE PORT_PLAYER(1)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("STICK0_Y")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_PLAYER(1)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("STICK1_X")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_REVERSE PORT_PLAYER(2)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("STICK1_Y")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(20) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_PLAYER(2)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


static INPUT_PORTS_START( totcarn )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_NAME("P1 Move Up") PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_NAME("P1 Move Down") PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_NAME("P1 Move Left") PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_NAME("P1 Move Right") PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_NAME("P1 Fire Up") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_NAME("P1 Fire Down") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_NAME("P1 Fire Left") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_NAME("P1 Fire Right") PORT_PLAYER(1)
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_NAME("P2 Move Up") PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_NAME("P2 Move Down") PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_NAME("P2 Move Left") PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_NAME("P2 Move Right") PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_NAME("P2 Fire Up") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_NAME("P2 Fire Down") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_NAME("P2 Fire Left") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_NAME("P2 Fire Right") PORT_PLAYER(2)

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 ) PORT_NAME("P1 Start / Bomb")
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) /* Slam Switch */
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 ) PORT_NAME("P2 Start / Bomb")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BILL1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNUSED ) /* video freeze */
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x3c00, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, midyunit_state,adpcm_irq_state_r, NULL)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x001f, 0x001f, DEF_STR( Coinage ))
	PORT_DIPSETTING(      0x001f, "USA 1" )
	PORT_DIPSETTING(      0x001e, "USA 2" )
	PORT_DIPSETTING(      0x001d, "USA 3" )
	PORT_DIPSETTING(      0x001c, "German 1" )
	PORT_DIPSETTING(      0x001b, "German 2" )
	PORT_DIPSETTING(      0x001a, "German 3" )
	PORT_DIPSETTING(      0x0019, "France 2" )
	PORT_DIPSETTING(      0x0018, "France 3" )
	PORT_DIPSETTING(      0x0017, "France 4" )
	PORT_DIPSETTING(      0x0016, "Swiss 1" )
	PORT_DIPSETTING(      0x0015, "Italy" )
	PORT_DIPSETTING(      0x0014, "U.K. 1" )
	PORT_DIPSETTING(      0x0013, "U.K. 2" )
	PORT_DIPSETTING(      0x0012, "U.K. ECA" )
	PORT_DIPSETTING(      0x0011, "Spain 1" )
	PORT_DIPSETTING(      0x0010, "Australia 1" )
	PORT_DIPSETTING(      0x000f, "Japan 1" )
	PORT_DIPSETTING(      0x000e, "Japan 2" )
	PORT_DIPSETTING(      0x000d, "Austria 1" )
	PORT_DIPSETTING(      0x000c, "Belgium 1" )
	PORT_DIPSETTING(      0x000b, "Belgium 2" )
	PORT_DIPSETTING(      0x000a, "Sweden" )
	PORT_DIPSETTING(      0x0009, "New Zealand 1" )
	PORT_DIPSETTING(      0x0008, "Netherlands" )
	PORT_DIPSETTING(      0x0007, "Finland" )
	PORT_DIPSETTING(      0x0006, "Norway" )
	PORT_DIPSETTING(      0x0005, "Denmark" )
//  PORT_DIPSETTING(      0x0004, DEF_STR( Unused ))
//  PORT_DIPSETTING(      0x0003, DEF_STR( Unused ))
//  PORT_DIPSETTING(      0x0002, DEF_STR( Unused ))
//  PORT_DIPSETTING(      0x0001, DEF_STR( Unused ))
//  PORT_DIPSETTING(      0x0000, DEF_STR( Unused ))
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0040, 0x0040, "Dipswitch Coinage" )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0080, 0x0080, "Keys for Pleasure Dome" )
	PORT_DIPSETTING(      0x0080, "220" )
	PORT_DIPSETTING(      0x0000, "200" )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Service_Mode ))
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ))
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))

	PORT_START("UNK0")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("UNK1")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



/*************************************
 *
 *  34010 configuration
 *
 *************************************/

static const tms34010_config zunit_tms_config =
{
	FALSE,                          /* halt on reset */
	"screen",                       /* the screen operated on */
	MEDRES_PIXEL_CLOCK,             /* pixel clock */
	2,                              /* pixels per clock */
	midyunit_scanline_update,       /* scanline updater (indexed16) */
	NULL,                           /* scanline updater (rgb32) */
	NULL,                           /* generate interrupt */
	midyunit_to_shiftreg,           /* write to shiftreg function */
	midyunit_from_shiftreg          /* read from shiftreg function */
};

static const tms34010_config yunit_tms_config =
{
	FALSE,                          /* halt on reset */
	"screen",                       /* the screen operated on */
	STDRES_PIXEL_CLOCK,             /* pixel clock */
	2,                              /* pixels per clock */
	midyunit_scanline_update,       /* scanline updater (indexed16) */
	NULL,                           /* scanline updater (rgb32) */
	NULL,                           /* generate interrupt */
	midyunit_to_shiftreg,           /* write to shiftreg function */
	midyunit_from_shiftreg          /* read from shiftreg function */
};



/*************************************
 *
 *  Z-unit machine driver
 *
 *************************************/

static MACHINE_CONFIG_START( zunit, midyunit_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", TMS34010, FAST_MASTER_CLOCK)
	MCFG_CPU_CONFIG(zunit_tms_config)
	MCFG_CPU_PROGRAM_MAP(main_map)

	MCFG_MACHINE_RESET_OVERRIDE(midyunit_state,midyunit)
	MCFG_NVRAM_ADD_0FILL("nvram")

	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_ALWAYS_UPDATE)
	MCFG_PALETTE_LENGTH(8192)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_RAW_PARAMS(MEDRES_PIXEL_CLOCK*2, 673, 0, 511, 433, 0, 399)
	MCFG_SCREEN_UPDATE_STATIC(tms340x0_ind16)

	MCFG_VIDEO_START_OVERRIDE(midyunit_state,midzunit)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_WILLIAMS_NARC_SOUND_ADD("narcsnd")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 1.0)
MACHINE_CONFIG_END



/*************************************
 *
 *  Y-unit machine drivers
 *
 *************************************/

static MACHINE_CONFIG_START( yunit_core, midyunit_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", TMS34010, SLOW_MASTER_CLOCK)
	MCFG_CPU_CONFIG(yunit_tms_config)
	MCFG_CPU_PROGRAM_MAP(main_map)

	MCFG_MACHINE_RESET_OVERRIDE(midyunit_state,midyunit)
	MCFG_NVRAM_ADD_0FILL("nvram")

	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_ALWAYS_UPDATE)
	MCFG_PALETTE_LENGTH(256)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_RAW_PARAMS(STDRES_PIXEL_CLOCK*2, 505, 0, 399, 289, 0, 253)
	MCFG_SCREEN_UPDATE_STATIC(tms340x0_ind16)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( yunit_cvsd_4bit_slow, yunit_core )

	/* basic machine hardware */
	MCFG_WILLIAMS_CVSD_SOUND_ADD("cvsd")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	/* video hardware */
	MCFG_PALETTE_LENGTH(256)
	MCFG_VIDEO_START_OVERRIDE(midyunit_state,midyunit_4bit)
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( yunit_cvsd_4bit_fast, yunit_core )

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK(FAST_MASTER_CLOCK)

	MCFG_WILLIAMS_CVSD_SOUND_ADD("cvsd")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	/* video hardware */
	MCFG_PALETTE_LENGTH(256)
	MCFG_VIDEO_START_OVERRIDE(midyunit_state,midyunit_4bit)
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( yunit_cvsd_6bit_slow, yunit_core )

	/* basic machine hardware */
	MCFG_WILLIAMS_CVSD_SOUND_ADD("cvsd")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	/* video hardware */
	MCFG_PALETTE_LENGTH(4096)
	MCFG_VIDEO_START_OVERRIDE(midyunit_state,midyunit_6bit)
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( yunit_adpcm_6bit_fast, yunit_core )

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK(FAST_MASTER_CLOCK)

	MCFG_WILLIAMS_ADPCM_SOUND_ADD("adpcm")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	/* video hardware */
	MCFG_PALETTE_LENGTH(4096)
	MCFG_VIDEO_START_OVERRIDE(midyunit_state,midyunit_6bit)
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( yunit_adpcm_6bit_faster, yunit_core )

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK(FASTER_MASTER_CLOCK)

	MCFG_WILLIAMS_ADPCM_SOUND_ADD("adpcm")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	/* video hardware */
	MCFG_PALETTE_LENGTH(4096)
	MCFG_VIDEO_START_OVERRIDE(midyunit_state,midyunit_6bit)
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( mkyawdim, yunit_core )

	/* basic machine hardware */

	MCFG_CPU_ADD("audiocpu", Z80, 5000000)
	MCFG_CPU_PROGRAM_MAP(yawdim_sound_map)

	/* video hardware */
	MCFG_PALETTE_LENGTH(4096)
	MCFG_VIDEO_START_OVERRIDE(midyunit_state,mkyawdim)

	/* sound hardware */
	MCFG_OKIM6295_ADD("oki", 1056000, OKIM6295_PIN7_HIGH) // clock frequency & pin 7 not verified
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_CONFIG_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( narc )
	ROM_REGION( 0x90000, "narcsnd:cpu0", 0 )    /* sound CPU */
	ROM_LOAD( "narcrev2.u4", 0x50000, 0x10000, CRC(450a591a) SHA1(bbda8061262738e5866f2707f69483a0a51d2910) )
	ROM_RELOAD(              0x60000, 0x10000 )
	ROM_LOAD( "narcrev2.u5", 0x70000, 0x10000, CRC(e551e5e3) SHA1(c8b4f53dbd4c534abb77d4dc07c4d12653b79894) )
	ROM_RELOAD(              0x80000, 0x10000 )

	ROM_REGION( 0x90000, "narcsnd:cpu1", 0 )    /* slave sound CPU */
	ROM_LOAD( "narcrev2.u35", 0x10000, 0x10000, CRC(81295892) SHA1(159664e5ee03c88d6e940e70e87e2150dc5b8b25) )
	ROM_RELOAD(               0x20000, 0x10000 )
	ROM_LOAD( "narcrev2.u36", 0x30000, 0x10000, CRC(16cdbb13) SHA1(2dfd961a5d909c1804f4fda34de33ee2664c4bc6) )
	ROM_RELOAD(               0x40000, 0x10000 )
	ROM_LOAD( "narcrev2.u37", 0x50000, 0x10000, CRC(29dbeffd) SHA1(4cbdc619db34f9c552de1ed3d034f8c079987e03) )
	ROM_RELOAD(               0x60000, 0x10000 )
	ROM_LOAD( "narcrev2.u38", 0x70000, 0x10000, CRC(09b03b80) SHA1(a45782d29a426fac38299b56af0815e844e35ae4) )
	ROM_RELOAD(               0x80000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "narcrev7.u42",  0x80000, 0x20000, CRC(d1111b76) SHA1(9700261aaba6a1ac0415362874817499f90b142a) )
	ROM_LOAD16_BYTE( "narcrev7.u24",  0x80001, 0x20000, CRC(aa0d3082) SHA1(7da59098319c49842406e7daf06aceae80fbd0ed) )
	ROM_LOAD16_BYTE( "narcrev7.u41",  0xc0000, 0x20000, CRC(3903191f) SHA1(1ad89cb03956f6625d9403e98951383fc9219478) )
	ROM_LOAD16_BYTE( "narcrev7.u23",  0xc0001, 0x20000, CRC(7a316582) SHA1(f640966c79bab70b536f2f92d4f46475a021b5b1) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD( "u94",  0x000000, 0x10000, CRC(ca3194e4) SHA1(d6aa6a09e4353a1dddd502abf85acf48e6e94cef) )
	ROM_LOAD( "u93",  0x010000, 0x10000, CRC(0ed7f7f5) SHA1(913d0dc81531adc6a7e6ffabfe681150aa4638a3) )
	ROM_LOAD( "u92",  0x020000, 0x10000, CRC(40d2fc66) SHA1(95b8d90e6abe336ad05dc3746d02b38823d2b8cd) )
	ROM_LOAD( "u91",  0x030000, 0x10000, CRC(f39325e0) SHA1(c1179825c76ed2934dfeff263a9296c2c1a5abe4) )
	ROM_LOAD( "u90",  0x040000, 0x10000, CRC(0132aefa) SHA1(9bf11ebc06f1069ea056427750902c204facbd3d) )
	ROM_LOAD( "u89",  0x050000, 0x10000, CRC(f7260c9e) SHA1(5a3fd88c7c0fa01ec2eb6fdef380ccee9d7da3a8) )
	ROM_LOAD( "u88",  0x060000, 0x10000, CRC(edc19f42) SHA1(b7121b3df743e5744ae72de2216b679fe71a2049) )
	ROM_LOAD( "u87",  0x070000, 0x10000, CRC(d9b42ff9) SHA1(cab05a5f8aadff010fba1107eb2000cc128063ff) )
	ROM_LOAD( "u86",  0x080000, 0x10000, CRC(af7daad3) SHA1(e2635a0acd6a238159ef91c1c3c9dfe8de8ae18f) )
	ROM_LOAD( "u85",  0x090000, 0x10000, CRC(095fae6b) SHA1(94f1df799142990a559e54cd949d9723481806b1) )
	ROM_LOAD( "u84",  0x0a0000, 0x10000, CRC(3fdf2057) SHA1(25ac6263a4eb962d90a305572fb95b75cb9f4138) )
	ROM_LOAD( "u83",  0x0b0000, 0x10000, CRC(f2d27c9f) SHA1(de30c7e0191adf62b11b2f2fbdf80687e653de12) )
	ROM_LOAD( "u82",  0x0c0000, 0x10000, CRC(962ce47c) SHA1(ea32f7f58a5ec1d941b372db5378d14fd850a2a7) )
	ROM_LOAD( "u81",  0x0d0000, 0x10000, CRC(00fe59ec) SHA1(85efd623b9cd75b249e19b2e97440a47718da728) )
	ROM_LOAD( "u80",  0x0e0000, 0x10000, CRC(147ba8e9) SHA1(1065b57082e0198025fe6f0bb3548f37c6a715e4) )

	ROM_LOAD( "u76",  0x200000, 0x10000, CRC(1cd897f4) SHA1(80414c3718ac6719abcca83f483302fc16fcfef3) )
	ROM_LOAD( "u75",  0x210000, 0x10000, CRC(78abfa01) SHA1(1523f537491b901f2d987d4443077b92e24b969d) )
	ROM_LOAD( "u74",  0x220000, 0x10000, CRC(66d2a234) SHA1(290b3051fa9d35e24a9d00fcc2b72d2751f3e7f1) )
	ROM_LOAD( "u73",  0x230000, 0x10000, CRC(efa5cd4e) SHA1(7aca6058d644a025c6799d55ffa082ba8eb5d76f) )
	ROM_LOAD( "u72",  0x240000, 0x10000, CRC(70638eb5) SHA1(fbafb354fca7c3c402be5073fa03060de569f536) )
	ROM_LOAD( "u71",  0x250000, 0x10000, CRC(61226883) SHA1(09a366df0603cc0afc8c6c5547ec6ae3a02724b2) )
	ROM_LOAD( "u70",  0x260000, 0x10000, CRC(c808849f) SHA1(bd3f69c4641331738e415d6d72fafe0eeeb2e56b) )
	ROM_LOAD( "u69",  0x270000, 0x10000, CRC(e7f9c34f) SHA1(f65aed012f1d575a63690222b8c8f2c56bc196c3) )
	ROM_LOAD( "u68",  0x280000, 0x10000, CRC(88a634d5) SHA1(9ddf86ca8cd91965348bc311cc722151f831db21) )
	ROM_LOAD( "u67",  0x290000, 0x10000, CRC(4ab8b69e) SHA1(4320407c78864edc7876ad3604405414a3e7762d) )
	ROM_LOAD( "u66",  0x2a0000, 0x10000, CRC(e1da4b25) SHA1(c81ed1ffc0a4bf64e794a1313559453f9455c312) )
	ROM_LOAD( "u65",  0x2b0000, 0x10000, CRC(6df0d125) SHA1(37392cc917e73cfa09970fd24503b45ced399976) )
	ROM_LOAD( "u64",  0x2c0000, 0x10000, CRC(abab1b16) SHA1(2913a94e1fcf8df52e29d0fb6e373aa64d23c019) )
	ROM_LOAD( "u63",  0x2d0000, 0x10000, CRC(80602f31) SHA1(f1c5c4476dbf80382f33c0776c103cff9bed8346) )
	ROM_LOAD( "u62",  0x2e0000, 0x10000, CRC(c2a476d1) SHA1(ffde1784548050d87f1404aaca3689417e6f7a81) )

	ROM_LOAD( "u58",  0x400000, 0x10000, CRC(8a7501e3) SHA1(dcd87c464fcb88180cc1c24ec82586440a197a5c) )
	ROM_LOAD( "u57",  0x410000, 0x10000, CRC(a504735f) SHA1(2afe58e576eea2e0326c6b42adb621358a270881) )
	ROM_LOAD( "u56",  0x420000, 0x10000, CRC(55f8cca7) SHA1(0b0a0d50be4401e4ac4e75d8040f18540f9ddc61) )
	ROM_LOAD( "u55",  0x430000, 0x10000, CRC(d3c932c1) SHA1(1a7ffc04e796ba355506bf9037c21aef18fe01a3) )
	ROM_LOAD( "u54",  0x440000, 0x10000, CRC(c7f4134b) SHA1(aea523e17f95c27d1f2c1f69884f626d96c8cb3b) )
	ROM_LOAD( "u53",  0x450000, 0x10000, CRC(6be4da56) SHA1(35a93a259be04a644ca70df4922f6915274c3932) )
	ROM_LOAD( "u52",  0x460000, 0x10000, CRC(1ea36a4a) SHA1(78e5437d46c1ecff5e221bc301925b10f00c5269) )
	ROM_LOAD( "u51",  0x470000, 0x10000, CRC(9d4b0324) SHA1(80fb38a9ac81a0383112df680b9755d7cccbd50b) )
	ROM_LOAD( "u50",  0x480000, 0x10000, CRC(6f9f0c26) SHA1(be77d99fb37fa31c3824725b28ee74206c584b90) )
	ROM_LOAD( "u49",  0x490000, 0x10000, CRC(80386fce) SHA1(f182ed0f1a3753dedc56cb120cb8d10e1556e966) )
	ROM_LOAD( "u48",  0x4a0000, 0x10000, CRC(05c16185) SHA1(429910c5b1f1fe47fdec6cfcba765ee9f10749f0) )
	ROM_LOAD( "u47",  0x4b0000, 0x10000, CRC(4c0151f1) SHA1(b526066fc594f3ec83bb4866986e3b73cdae3992) )
	ROM_LOAD( "u46",  0x4c0000, 0x10000, CRC(5670bfcb) SHA1(b20829b715c6421894c10c02aebb08d22b5109c9) )
	ROM_LOAD( "u45",  0x4d0000, 0x10000, CRC(27f10d98) SHA1(b027ade2b4a52977d9c40c9549b9067d37fab41c) )
	ROM_LOAD( "u44",  0x4e0000, 0x10000, CRC(93b8eaa4) SHA1(b786f3286c5443cf08e556e9fb030b3444288f3c) )

	ROM_LOAD( "u40",  0x600000, 0x10000, CRC(7fcaebc7) SHA1(b951d63c072d693f7dfc7e362a12513eb9bd6bab) )
	ROM_LOAD( "u39",  0x610000, 0x10000, CRC(7db5cf52) SHA1(478aefc1126493378d22c857646e2fce221c7d21) )
	ROM_LOAD( "u38",  0x620000, 0x10000, CRC(3f9f3ef7) SHA1(5315e8c372bb63d95f814d8eafe0f41e4d95ba1a) )
	ROM_LOAD( "u37",  0x630000, 0x10000, CRC(ed81826c) SHA1(afe1c0fc692a802279c1f7f31143d33028d35ce4) )
	ROM_LOAD( "u36",  0x640000, 0x10000, CRC(e5d855c0) SHA1(3fa0f765238ad2a27c0c65805bf56ebfbe50bf05) )
	ROM_LOAD( "u35",  0x650000, 0x10000, CRC(3a7b1329) SHA1(e8b547a3b8f85cd13e12cfe0bf3949acc1486e6b) )
	ROM_LOAD( "u34",  0x660000, 0x10000, CRC(fe982b0e) SHA1(a03e7e348186339fd93ce119f65e8f0ea7b7bb7a) )
	ROM_LOAD( "u33",  0x670000, 0x10000, CRC(6bc7eb0f) SHA1(6964ef63d0daf1bc7fa9585567659cfc198b6cc3) )
	ROM_LOAD( "u32",  0x680000, 0x10000, CRC(5875a6d3) SHA1(ae64aa786239be39c3c99bbe019bdc91003c1691) )
	ROM_LOAD( "u31",  0x690000, 0x10000, CRC(2fa4b8e5) SHA1(8e4e4abd60d20e0ef955ac4b1f300cfd157e50ca) )
	ROM_LOAD( "u30",  0x6a0000, 0x10000, CRC(7e4bb8ee) SHA1(7166bd56a569329e01ed0c03579a403d659a4a7b) )
	ROM_LOAD( "u29",  0x6b0000, 0x10000, CRC(45136fd9) SHA1(44388e16d02a8c55fed0dbbcd842c941fa4b11b1) )
	ROM_LOAD( "u28",  0x6c0000, 0x10000, CRC(d6cdac24) SHA1(d4bbe3a1be89be7d21769bfe476b50c05cd0c357) )
	ROM_LOAD( "u27",  0x6d0000, 0x10000, CRC(4d33bbec) SHA1(05a3bd66ff91c824e841ca3943585f6aa383c5c2) )
	ROM_LOAD( "u26",  0x6e0000, 0x10000, CRC(cb19f784) SHA1(1e4d85603c940e247fdc45f0366dfb484285e588) )
ROM_END


ROM_START( narc3 )
	ROM_REGION( 0x90000, "narcsnd:cpu0", 0 )    /* sound CPU */
	ROM_LOAD( "narcrev2.u4", 0x50000, 0x10000, CRC(450a591a) SHA1(bbda8061262738e5866f2707f69483a0a51d2910) )
	ROM_RELOAD(              0x60000, 0x10000 )
	ROM_LOAD( "narcrev2.u5", 0x70000, 0x10000, CRC(e551e5e3) SHA1(c8b4f53dbd4c534abb77d4dc07c4d12653b79894) )
	ROM_RELOAD(              0x80000, 0x10000 )

	ROM_REGION( 0x90000, "narcsnd:cpu1", 0 )    /* slave sound CPU */
	ROM_LOAD( "narcrev2.u35", 0x10000, 0x10000, CRC(81295892) SHA1(159664e5ee03c88d6e940e70e87e2150dc5b8b25) )
	ROM_RELOAD(               0x20000, 0x10000 )
	ROM_LOAD( "narcrev2.u36", 0x30000, 0x10000, CRC(16cdbb13) SHA1(2dfd961a5d909c1804f4fda34de33ee2664c4bc6) )
	ROM_RELOAD(               0x40000, 0x10000 )
	ROM_LOAD( "narcrev2.u37", 0x50000, 0x10000, CRC(29dbeffd) SHA1(4cbdc619db34f9c552de1ed3d034f8c079987e03) )
	ROM_RELOAD(               0x60000, 0x10000 )
	ROM_LOAD( "narcrev2.u38", 0x70000, 0x10000, CRC(09b03b80) SHA1(a45782d29a426fac38299b56af0815e844e35ae4) )
	ROM_RELOAD(               0x80000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "narcrev3.u78",  0x00000, 0x10000, CRC(388581b0) SHA1(9d3c718c7bee8f3db9b87ae08ec3bcc1bf65084b) )
	ROM_RELOAD(                       0x20000, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev3.u60",  0x00001, 0x10000, CRC(f273bc04) SHA1(d4a75c1d6fa706f582ac8131387042a3c9abd08e) )
	ROM_RELOAD(                       0x20001, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev3.u77",  0x40000, 0x10000, CRC(bdafaccc) SHA1(9e0607d2a2a939847e95489970969df5af1fb708) )
	ROM_RELOAD(                       0x60000, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev3.u59",  0x40001, 0x10000, CRC(96314a99) SHA1(917cde404b325d0689a2c5848a145eedfd31fc57) )
	ROM_RELOAD(                       0x60001, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev3.u42",  0x80000, 0x10000, CRC(56aebc81) SHA1(5177ea0121e1b742934ffdcf85795b2c9595b5de) )
	ROM_RELOAD(                       0xa0000, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev3.u24",  0x80001, 0x10000, CRC(11d7e143) SHA1(c58bc9615d480a97443cc5d4fb2f8ce9fba9db63) )
	ROM_RELOAD(                       0xa0001, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev3.u41",  0xc0000, 0x10000, CRC(6142fab7) SHA1(e1cc5b088bf2fb9be51d4620b3ff3e50e0fd3117) )
	ROM_RELOAD(                       0xe0000, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev3.u23",  0xc0001, 0x10000, CRC(98cdd178) SHA1(dd46a957462f2a9dc6de89379fe3e21664873a3c) )
	ROM_RELOAD(                       0xe0001, 0x10000 )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD( "u94",  0x000000, 0x10000, CRC(ca3194e4) SHA1(d6aa6a09e4353a1dddd502abf85acf48e6e94cef) )
	ROM_LOAD( "u93",  0x010000, 0x10000, CRC(0ed7f7f5) SHA1(913d0dc81531adc6a7e6ffabfe681150aa4638a3) )
	ROM_LOAD( "u92",  0x020000, 0x10000, CRC(40d2fc66) SHA1(95b8d90e6abe336ad05dc3746d02b38823d2b8cd) )
	ROM_LOAD( "u91",  0x030000, 0x10000, CRC(f39325e0) SHA1(c1179825c76ed2934dfeff263a9296c2c1a5abe4) )
	ROM_LOAD( "u90",  0x040000, 0x10000, CRC(0132aefa) SHA1(9bf11ebc06f1069ea056427750902c204facbd3d) )
	ROM_LOAD( "u89",  0x050000, 0x10000, CRC(f7260c9e) SHA1(5a3fd88c7c0fa01ec2eb6fdef380ccee9d7da3a8) )
	ROM_LOAD( "u88",  0x060000, 0x10000, CRC(edc19f42) SHA1(b7121b3df743e5744ae72de2216b679fe71a2049) )
	ROM_LOAD( "u87",  0x070000, 0x10000, CRC(d9b42ff9) SHA1(cab05a5f8aadff010fba1107eb2000cc128063ff) )
	ROM_LOAD( "u86",  0x080000, 0x10000, CRC(af7daad3) SHA1(e2635a0acd6a238159ef91c1c3c9dfe8de8ae18f) )
	ROM_LOAD( "u85",  0x090000, 0x10000, CRC(095fae6b) SHA1(94f1df799142990a559e54cd949d9723481806b1) )
	ROM_LOAD( "u84",  0x0a0000, 0x10000, CRC(3fdf2057) SHA1(25ac6263a4eb962d90a305572fb95b75cb9f4138) )
	ROM_LOAD( "u83",  0x0b0000, 0x10000, CRC(f2d27c9f) SHA1(de30c7e0191adf62b11b2f2fbdf80687e653de12) )
	ROM_LOAD( "u82",  0x0c0000, 0x10000, CRC(962ce47c) SHA1(ea32f7f58a5ec1d941b372db5378d14fd850a2a7) )
	ROM_LOAD( "u81",  0x0d0000, 0x10000, CRC(00fe59ec) SHA1(85efd623b9cd75b249e19b2e97440a47718da728) )
	ROM_LOAD( "u80",  0x0e0000, 0x10000, CRC(147ba8e9) SHA1(1065b57082e0198025fe6f0bb3548f37c6a715e4) )

	ROM_LOAD( "u76",  0x200000, 0x10000, CRC(1cd897f4) SHA1(80414c3718ac6719abcca83f483302fc16fcfef3) )
	ROM_LOAD( "u75",  0x210000, 0x10000, CRC(78abfa01) SHA1(1523f537491b901f2d987d4443077b92e24b969d) )
	ROM_LOAD( "u74",  0x220000, 0x10000, CRC(66d2a234) SHA1(290b3051fa9d35e24a9d00fcc2b72d2751f3e7f1) )
	ROM_LOAD( "u73",  0x230000, 0x10000, CRC(efa5cd4e) SHA1(7aca6058d644a025c6799d55ffa082ba8eb5d76f) )
	ROM_LOAD( "u72",  0x240000, 0x10000, CRC(70638eb5) SHA1(fbafb354fca7c3c402be5073fa03060de569f536) )
	ROM_LOAD( "u71",  0x250000, 0x10000, CRC(61226883) SHA1(09a366df0603cc0afc8c6c5547ec6ae3a02724b2) )
	ROM_LOAD( "u70",  0x260000, 0x10000, CRC(c808849f) SHA1(bd3f69c4641331738e415d6d72fafe0eeeb2e56b) )
	ROM_LOAD( "u69",  0x270000, 0x10000, CRC(e7f9c34f) SHA1(f65aed012f1d575a63690222b8c8f2c56bc196c3) )
	ROM_LOAD( "u68",  0x280000, 0x10000, CRC(88a634d5) SHA1(9ddf86ca8cd91965348bc311cc722151f831db21) )
	ROM_LOAD( "u67",  0x290000, 0x10000, CRC(4ab8b69e) SHA1(4320407c78864edc7876ad3604405414a3e7762d) )
	ROM_LOAD( "u66",  0x2a0000, 0x10000, CRC(e1da4b25) SHA1(c81ed1ffc0a4bf64e794a1313559453f9455c312) )
	ROM_LOAD( "u65",  0x2b0000, 0x10000, CRC(6df0d125) SHA1(37392cc917e73cfa09970fd24503b45ced399976) )
	ROM_LOAD( "u64",  0x2c0000, 0x10000, CRC(abab1b16) SHA1(2913a94e1fcf8df52e29d0fb6e373aa64d23c019) )
	ROM_LOAD( "u63",  0x2d0000, 0x10000, CRC(80602f31) SHA1(f1c5c4476dbf80382f33c0776c103cff9bed8346) )
	ROM_LOAD( "u62",  0x2e0000, 0x10000, CRC(c2a476d1) SHA1(ffde1784548050d87f1404aaca3689417e6f7a81) )

	ROM_LOAD( "u58",  0x400000, 0x10000, CRC(8a7501e3) SHA1(dcd87c464fcb88180cc1c24ec82586440a197a5c) )
	ROM_LOAD( "u57",  0x410000, 0x10000, CRC(a504735f) SHA1(2afe58e576eea2e0326c6b42adb621358a270881) )
	ROM_LOAD( "u56",  0x420000, 0x10000, CRC(55f8cca7) SHA1(0b0a0d50be4401e4ac4e75d8040f18540f9ddc61) )
	ROM_LOAD( "u55",  0x430000, 0x10000, CRC(d3c932c1) SHA1(1a7ffc04e796ba355506bf9037c21aef18fe01a3) )
	ROM_LOAD( "u54",  0x440000, 0x10000, CRC(c7f4134b) SHA1(aea523e17f95c27d1f2c1f69884f626d96c8cb3b) )
	ROM_LOAD( "u53",  0x450000, 0x10000, CRC(6be4da56) SHA1(35a93a259be04a644ca70df4922f6915274c3932) )
	ROM_LOAD( "u52",  0x460000, 0x10000, CRC(1ea36a4a) SHA1(78e5437d46c1ecff5e221bc301925b10f00c5269) )
	ROM_LOAD( "u51",  0x470000, 0x10000, CRC(9d4b0324) SHA1(80fb38a9ac81a0383112df680b9755d7cccbd50b) )
	ROM_LOAD( "u50",  0x480000, 0x10000, CRC(6f9f0c26) SHA1(be77d99fb37fa31c3824725b28ee74206c584b90) )
	ROM_LOAD( "u49",  0x490000, 0x10000, CRC(80386fce) SHA1(f182ed0f1a3753dedc56cb120cb8d10e1556e966) )
	ROM_LOAD( "u48",  0x4a0000, 0x10000, CRC(05c16185) SHA1(429910c5b1f1fe47fdec6cfcba765ee9f10749f0) )
	ROM_LOAD( "u47",  0x4b0000, 0x10000, CRC(4c0151f1) SHA1(b526066fc594f3ec83bb4866986e3b73cdae3992) )
	ROM_LOAD( "u46",  0x4c0000, 0x10000, CRC(5670bfcb) SHA1(b20829b715c6421894c10c02aebb08d22b5109c9) )
	ROM_LOAD( "u45",  0x4d0000, 0x10000, CRC(27f10d98) SHA1(b027ade2b4a52977d9c40c9549b9067d37fab41c) )
	ROM_LOAD( "u44",  0x4e0000, 0x10000, CRC(93b8eaa4) SHA1(b786f3286c5443cf08e556e9fb030b3444288f3c) )

	ROM_LOAD( "u40",  0x600000, 0x10000, CRC(7fcaebc7) SHA1(b951d63c072d693f7dfc7e362a12513eb9bd6bab) )
	ROM_LOAD( "u39",  0x610000, 0x10000, CRC(7db5cf52) SHA1(478aefc1126493378d22c857646e2fce221c7d21) )
	ROM_LOAD( "u38",  0x620000, 0x10000, CRC(3f9f3ef7) SHA1(5315e8c372bb63d95f814d8eafe0f41e4d95ba1a) )
	ROM_LOAD( "u37",  0x630000, 0x10000, CRC(ed81826c) SHA1(afe1c0fc692a802279c1f7f31143d33028d35ce4) )
	ROM_LOAD( "u36",  0x640000, 0x10000, CRC(e5d855c0) SHA1(3fa0f765238ad2a27c0c65805bf56ebfbe50bf05) )
	ROM_LOAD( "u35",  0x650000, 0x10000, CRC(3a7b1329) SHA1(e8b547a3b8f85cd13e12cfe0bf3949acc1486e6b) )
	ROM_LOAD( "u34",  0x660000, 0x10000, CRC(fe982b0e) SHA1(a03e7e348186339fd93ce119f65e8f0ea7b7bb7a) )
	ROM_LOAD( "u33",  0x670000, 0x10000, CRC(6bc7eb0f) SHA1(6964ef63d0daf1bc7fa9585567659cfc198b6cc3) )
	ROM_LOAD( "u32",  0x680000, 0x10000, CRC(5875a6d3) SHA1(ae64aa786239be39c3c99bbe019bdc91003c1691) )
	ROM_LOAD( "u31",  0x690000, 0x10000, CRC(2fa4b8e5) SHA1(8e4e4abd60d20e0ef955ac4b1f300cfd157e50ca) )
	ROM_LOAD( "u30",  0x6a0000, 0x10000, CRC(7e4bb8ee) SHA1(7166bd56a569329e01ed0c03579a403d659a4a7b) )
	ROM_LOAD( "u29",  0x6b0000, 0x10000, CRC(45136fd9) SHA1(44388e16d02a8c55fed0dbbcd842c941fa4b11b1) )
	ROM_LOAD( "u28",  0x6c0000, 0x10000, CRC(d6cdac24) SHA1(d4bbe3a1be89be7d21769bfe476b50c05cd0c357) )
	ROM_LOAD( "u27",  0x6d0000, 0x10000, CRC(4d33bbec) SHA1(05a3bd66ff91c824e841ca3943585f6aa383c5c2) )
	ROM_LOAD( "u26",  0x6e0000, 0x10000, CRC(cb19f784) SHA1(1e4d85603c940e247fdc45f0366dfb484285e588) )
ROM_END

ROM_START( narc2 )
	ROM_REGION( 0x90000, "narcsnd:cpu0", 0 )    /* sound CPU */
	ROM_LOAD( "narcrev2.u4", 0x50000, 0x10000, CRC(450a591a) SHA1(bbda8061262738e5866f2707f69483a0a51d2910) )
	ROM_RELOAD(              0x60000, 0x10000 )
	ROM_LOAD( "narcrev2.u5", 0x70000, 0x10000, CRC(e551e5e3) SHA1(c8b4f53dbd4c534abb77d4dc07c4d12653b79894) )
	ROM_RELOAD(              0x80000, 0x10000 )

	ROM_REGION( 0x90000, "narcsnd:cpu1", 0 )    /* slave sound CPU */
	ROM_LOAD( "narcrev2.u35", 0x10000, 0x10000, CRC(81295892) SHA1(159664e5ee03c88d6e940e70e87e2150dc5b8b25) )
	ROM_RELOAD(               0x20000, 0x10000 )
	ROM_LOAD( "narcrev2.u36", 0x30000, 0x10000, CRC(16cdbb13) SHA1(2dfd961a5d909c1804f4fda34de33ee2664c4bc6) )
	ROM_RELOAD(               0x40000, 0x10000 )
	ROM_LOAD( "narcrev2.u37", 0x50000, 0x10000, CRC(29dbeffd) SHA1(4cbdc619db34f9c552de1ed3d034f8c079987e03) )
	ROM_RELOAD(               0x60000, 0x10000 )
	ROM_LOAD( "narcrev2.u38", 0x70000, 0x10000, CRC(09b03b80) SHA1(a45782d29a426fac38299b56af0815e844e35ae4) )
	ROM_RELOAD(               0x80000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "narcrev2.u78",  0x00000, 0x10000, CRC(150c2dc4) SHA1(c7e3f90f5fed08f2a6951779166cbc6d2dbcd380) )
	ROM_RELOAD(                       0x20000, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev2.u60",  0x00001, 0x10000, CRC(9720ddea) SHA1(27f0182799f14c1c7c8dc48f7cf4160768b24662) )
	ROM_RELOAD(                       0x20001, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev2.u77",  0x40000, 0x10000, CRC(75ba4c74) SHA1(8713c22d30107d01612571d3a42aa9edda795fb0) )
	ROM_RELOAD(                       0x60000, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev2.u59",  0x40001, 0x10000, CRC(f7c6c104) SHA1(1b57a95f2232a9433831b99b689802ef185ff203) )
	ROM_RELOAD(                       0x60001, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev2.u42",  0x80000, 0x10000, CRC(3db20bb8) SHA1(688844bd573e5d0c5225fccbc12ae91b88b95bd8) )
	ROM_RELOAD(                       0xa0000, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev2.u24",  0x80001, 0x10000, CRC(91bae451) SHA1(549ad5938ae9ae4e320d0c5f8f30f23f5de2c802) )
	ROM_RELOAD(                       0xa0001, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev2.u41",  0xc0000, 0x10000, CRC(b0d463e1) SHA1(f6f1a9088aab838f3efe21f71616374ffec35a05) )
	ROM_RELOAD(                       0xe0000, 0x10000 )
	ROM_LOAD16_BYTE( "narcrev2.u23",  0xc0001, 0x10000, CRC(a9eb4825) SHA1(9c0b98451f1a240a3cb7ed4c1aab6c7c4abd27e6) )
	ROM_RELOAD(                       0xe0001, 0x10000 )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD( "u94",  0x000000, 0x10000, CRC(ca3194e4) SHA1(d6aa6a09e4353a1dddd502abf85acf48e6e94cef) )
	ROM_LOAD( "u93",  0x010000, 0x10000, CRC(0ed7f7f5) SHA1(913d0dc81531adc6a7e6ffabfe681150aa4638a3) )
	ROM_LOAD( "u92",  0x020000, 0x10000, CRC(40d2fc66) SHA1(95b8d90e6abe336ad05dc3746d02b38823d2b8cd) )
	ROM_LOAD( "u91",  0x030000, 0x10000, CRC(f39325e0) SHA1(c1179825c76ed2934dfeff263a9296c2c1a5abe4) )
	ROM_LOAD( "u90",  0x040000, 0x10000, CRC(0132aefa) SHA1(9bf11ebc06f1069ea056427750902c204facbd3d) )
	ROM_LOAD( "u89",  0x050000, 0x10000, CRC(f7260c9e) SHA1(5a3fd88c7c0fa01ec2eb6fdef380ccee9d7da3a8) )
	ROM_LOAD( "u88",  0x060000, 0x10000, CRC(edc19f42) SHA1(b7121b3df743e5744ae72de2216b679fe71a2049) )
	ROM_LOAD( "u87",  0x070000, 0x10000, CRC(d9b42ff9) SHA1(cab05a5f8aadff010fba1107eb2000cc128063ff) )
	ROM_LOAD( "u86",  0x080000, 0x10000, CRC(af7daad3) SHA1(e2635a0acd6a238159ef91c1c3c9dfe8de8ae18f) )
	ROM_LOAD( "u85",  0x090000, 0x10000, CRC(095fae6b) SHA1(94f1df799142990a559e54cd949d9723481806b1) )
	ROM_LOAD( "u84",  0x0a0000, 0x10000, CRC(3fdf2057) SHA1(25ac6263a4eb962d90a305572fb95b75cb9f4138) )
	ROM_LOAD( "u83",  0x0b0000, 0x10000, CRC(f2d27c9f) SHA1(de30c7e0191adf62b11b2f2fbdf80687e653de12) )
	ROM_LOAD( "u82",  0x0c0000, 0x10000, CRC(962ce47c) SHA1(ea32f7f58a5ec1d941b372db5378d14fd850a2a7) )
	ROM_LOAD( "u81",  0x0d0000, 0x10000, CRC(00fe59ec) SHA1(85efd623b9cd75b249e19b2e97440a47718da728) )
	ROM_LOAD( "u80",  0x0e0000, 0x10000, CRC(147ba8e9) SHA1(1065b57082e0198025fe6f0bb3548f37c6a715e4) )

	ROM_LOAD( "u76",  0x200000, 0x10000, CRC(1cd897f4) SHA1(80414c3718ac6719abcca83f483302fc16fcfef3) )
	ROM_LOAD( "u75",  0x210000, 0x10000, CRC(78abfa01) SHA1(1523f537491b901f2d987d4443077b92e24b969d) )
	ROM_LOAD( "u74",  0x220000, 0x10000, CRC(66d2a234) SHA1(290b3051fa9d35e24a9d00fcc2b72d2751f3e7f1) )
	ROM_LOAD( "u73",  0x230000, 0x10000, CRC(efa5cd4e) SHA1(7aca6058d644a025c6799d55ffa082ba8eb5d76f) )
	ROM_LOAD( "u72",  0x240000, 0x10000, CRC(70638eb5) SHA1(fbafb354fca7c3c402be5073fa03060de569f536) )
	ROM_LOAD( "u71",  0x250000, 0x10000, CRC(61226883) SHA1(09a366df0603cc0afc8c6c5547ec6ae3a02724b2) )
	ROM_LOAD( "u70",  0x260000, 0x10000, CRC(c808849f) SHA1(bd3f69c4641331738e415d6d72fafe0eeeb2e56b) )
	ROM_LOAD( "u69",  0x270000, 0x10000, CRC(e7f9c34f) SHA1(f65aed012f1d575a63690222b8c8f2c56bc196c3) )
	ROM_LOAD( "u68",  0x280000, 0x10000, CRC(88a634d5) SHA1(9ddf86ca8cd91965348bc311cc722151f831db21) )
	ROM_LOAD( "u67",  0x290000, 0x10000, CRC(4ab8b69e) SHA1(4320407c78864edc7876ad3604405414a3e7762d) )
	ROM_LOAD( "u66",  0x2a0000, 0x10000, CRC(e1da4b25) SHA1(c81ed1ffc0a4bf64e794a1313559453f9455c312) )
	ROM_LOAD( "u65",  0x2b0000, 0x10000, CRC(6df0d125) SHA1(37392cc917e73cfa09970fd24503b45ced399976) )
	ROM_LOAD( "u64",  0x2c0000, 0x10000, CRC(abab1b16) SHA1(2913a94e1fcf8df52e29d0fb6e373aa64d23c019) )
	ROM_LOAD( "u63",  0x2d0000, 0x10000, CRC(80602f31) SHA1(f1c5c4476dbf80382f33c0776c103cff9bed8346) )
	ROM_LOAD( "u62",  0x2e0000, 0x10000, CRC(c2a476d1) SHA1(ffde1784548050d87f1404aaca3689417e6f7a81) )

	ROM_LOAD( "u58",  0x400000, 0x10000, CRC(8a7501e3) SHA1(dcd87c464fcb88180cc1c24ec82586440a197a5c) )
	ROM_LOAD( "u57",  0x410000, 0x10000, CRC(a504735f) SHA1(2afe58e576eea2e0326c6b42adb621358a270881) )
	ROM_LOAD( "u56",  0x420000, 0x10000, CRC(55f8cca7) SHA1(0b0a0d50be4401e4ac4e75d8040f18540f9ddc61) )
	ROM_LOAD( "u55",  0x430000, 0x10000, CRC(d3c932c1) SHA1(1a7ffc04e796ba355506bf9037c21aef18fe01a3) )
	ROM_LOAD( "u54",  0x440000, 0x10000, CRC(c7f4134b) SHA1(aea523e17f95c27d1f2c1f69884f626d96c8cb3b) )
	ROM_LOAD( "u53",  0x450000, 0x10000, CRC(6be4da56) SHA1(35a93a259be04a644ca70df4922f6915274c3932) )
	ROM_LOAD( "u52",  0x460000, 0x10000, CRC(1ea36a4a) SHA1(78e5437d46c1ecff5e221bc301925b10f00c5269) )
	ROM_LOAD( "u51",  0x470000, 0x10000, CRC(9d4b0324) SHA1(80fb38a9ac81a0383112df680b9755d7cccbd50b) )
	ROM_LOAD( "u50",  0x480000, 0x10000, CRC(6f9f0c26) SHA1(be77d99fb37fa31c3824725b28ee74206c584b90) )
	ROM_LOAD( "u49",  0x490000, 0x10000, CRC(80386fce) SHA1(f182ed0f1a3753dedc56cb120cb8d10e1556e966) )
	ROM_LOAD( "u48",  0x4a0000, 0x10000, CRC(05c16185) SHA1(429910c5b1f1fe47fdec6cfcba765ee9f10749f0) )
	ROM_LOAD( "u47",  0x4b0000, 0x10000, CRC(4c0151f1) SHA1(b526066fc594f3ec83bb4866986e3b73cdae3992) )
	ROM_LOAD( "u46",  0x4c0000, 0x10000, CRC(5670bfcb) SHA1(b20829b715c6421894c10c02aebb08d22b5109c9) )
	ROM_LOAD( "u45",  0x4d0000, 0x10000, CRC(27f10d98) SHA1(b027ade2b4a52977d9c40c9549b9067d37fab41c) )
	ROM_LOAD( "u44",  0x4e0000, 0x10000, CRC(93b8eaa4) SHA1(b786f3286c5443cf08e556e9fb030b3444288f3c) )

	ROM_LOAD( "u40",  0x600000, 0x10000, CRC(7fcaebc7) SHA1(b951d63c072d693f7dfc7e362a12513eb9bd6bab) )
	ROM_LOAD( "u39",  0x610000, 0x10000, CRC(7db5cf52) SHA1(478aefc1126493378d22c857646e2fce221c7d21) )
	ROM_LOAD( "u38",  0x620000, 0x10000, CRC(3f9f3ef7) SHA1(5315e8c372bb63d95f814d8eafe0f41e4d95ba1a) )
	ROM_LOAD( "u37",  0x630000, 0x10000, CRC(ed81826c) SHA1(afe1c0fc692a802279c1f7f31143d33028d35ce4) )
	ROM_LOAD( "u36",  0x640000, 0x10000, CRC(e5d855c0) SHA1(3fa0f765238ad2a27c0c65805bf56ebfbe50bf05) )
	ROM_LOAD( "u35",  0x650000, 0x10000, CRC(3a7b1329) SHA1(e8b547a3b8f85cd13e12cfe0bf3949acc1486e6b) )
	ROM_LOAD( "u34",  0x660000, 0x10000, CRC(fe982b0e) SHA1(a03e7e348186339fd93ce119f65e8f0ea7b7bb7a) )
	ROM_LOAD( "u33",  0x670000, 0x10000, CRC(6bc7eb0f) SHA1(6964ef63d0daf1bc7fa9585567659cfc198b6cc3) )
	ROM_LOAD( "u32",  0x680000, 0x10000, CRC(5875a6d3) SHA1(ae64aa786239be39c3c99bbe019bdc91003c1691) )
	ROM_LOAD( "u31",  0x690000, 0x10000, CRC(2fa4b8e5) SHA1(8e4e4abd60d20e0ef955ac4b1f300cfd157e50ca) )
	ROM_LOAD( "u30",  0x6a0000, 0x10000, CRC(7e4bb8ee) SHA1(7166bd56a569329e01ed0c03579a403d659a4a7b) )
	ROM_LOAD( "u29",  0x6b0000, 0x10000, CRC(45136fd9) SHA1(44388e16d02a8c55fed0dbbcd842c941fa4b11b1) )
	ROM_LOAD( "u28",  0x6c0000, 0x10000, CRC(d6cdac24) SHA1(d4bbe3a1be89be7d21769bfe476b50c05cd0c357) )
	ROM_LOAD( "u27",  0x6d0000, 0x10000, CRC(4d33bbec) SHA1(05a3bd66ff91c824e841ca3943585f6aa383c5c2) )
	ROM_LOAD( "u26",  0x6e0000, 0x10000, CRC(cb19f784) SHA1(1e4d85603c940e247fdc45f0366dfb484285e588) )
ROM_END


ROM_START( trog )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "trogu4.bin", 0x10000, 0x10000, CRC(759d0bf4) SHA1(c4c3fa51c43cf7fd241ac1f33d7d220aa9f9edb3) )
	ROM_RELOAD(                0x20000, 0x10000 )
	ROM_LOAD (  "trogu19.bin", 0x30000, 0x10000, CRC(960c333d) SHA1(da8ce8dfffffe7a2d60b3f75cc5aa88e5e2be659) )
	ROM_RELOAD(                0x40000, 0x10000 )
	ROM_LOAD (  "trogu20.bin", 0x50000, 0x10000, CRC(67f1658a) SHA1(c85dc920ff4b292afa9f6681f31918a200799cc9) )
	ROM_RELOAD(                0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la5",  0xc0000, 0x20000, CRC(d62cc51a) SHA1(a63ed5b0e08dd89a1392e04cd88c9d83d75810c6) )
	ROM_LOAD16_BYTE( "u89-la5",   0xc0001, 0x20000, CRC(edde0bc8) SHA1(95389b75c438c0f0cad668a35570fcb4f7790a02) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "trogu111.bin",  0x000000, 0x20000, CRC(9ded08c1) SHA1(bbf069f218e3f3d67e45fa2229a471451b3a8f96) )
	ROM_LOAD ( "trogu112.bin",  0x020000, 0x20000, CRC(42293843) SHA1(cae77eeddd784573beccb79e54573da0e4ccdd8a) )
	ROM_LOAD ( "trogu113.bin",  0x040000, 0x20000, CRC(77f50cbb) SHA1(5f2df3aedd90871ac02bca07c66387f6cda0dfdf) )

	ROM_LOAD ( "trogu106.bin",  0x080000, 0x20000, CRC(af2eb0d8) SHA1(3767e6f3853b092b40664c2b6c6a838f0243514b) )
	ROM_LOAD ( "trogu107.bin",  0x0a0000, 0x20000, CRC(88a7b3f6) SHA1(ba55f66929841a915d7b96aabf4b11e50ba6cfbd) )

	ROM_LOAD (  "trogu95.bin",  0x200000, 0x20000, CRC(f3ba2838) SHA1(2bee6c783c84a9f3f9309d802f42983857190ece) )
	ROM_LOAD (  "trogu96.bin",  0x220000, 0x20000, CRC(cfed2e77) SHA1(7fc0f52ac844c9efcbcc3004c40f9f4fc7e1c346) )
	ROM_LOAD (  "trogu97.bin",  0x240000, 0x20000, CRC(3262d1f8) SHA1(754e3e8223edd11398b2db77fd5db619dad1577b) )

	ROM_LOAD (  "trogu90.bin",  0x280000, 0x20000, CRC(16e06753) SHA1(62ec2b18e6b965ea0792d655d7878b4225da3aca) )
	ROM_LOAD (  "trogu91.bin",  0x2a0000, 0x20000, CRC(880a02c7) SHA1(ab1b2d24be4571a183b230d267c6c8167d4a42a4) )
ROM_END


ROM_START( trog4 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "trogu4.bin", 0x10000, 0x10000, CRC(759d0bf4) SHA1(c4c3fa51c43cf7fd241ac1f33d7d220aa9f9edb3) )
	ROM_RELOAD(                0x20000, 0x10000 )
	ROM_LOAD (  "trogu19.bin", 0x30000, 0x10000, CRC(960c333d) SHA1(da8ce8dfffffe7a2d60b3f75cc5aa88e5e2be659) )
	ROM_RELOAD(                0x40000, 0x10000 )
	ROM_LOAD (  "trogu20.bin", 0x50000, 0x10000, CRC(67f1658a) SHA1(c85dc920ff4b292afa9f6681f31918a200799cc9) )
	ROM_RELOAD(                0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la4",  0xc0000, 0x20000, CRC(e6095189) SHA1(a2caaf64e371050b37c63d9608ba5d289cf3cd91) )
	ROM_LOAD16_BYTE( "u89-la4",   0xc0001, 0x20000, CRC(fdd7cc65) SHA1(bfc4339953c122bca968f9cfa3a82df3584a3727) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "trogu111.bin",  0x000000, 0x20000, CRC(9ded08c1) SHA1(bbf069f218e3f3d67e45fa2229a471451b3a8f96) )
	ROM_LOAD ( "trogu112.bin",  0x020000, 0x20000, CRC(42293843) SHA1(cae77eeddd784573beccb79e54573da0e4ccdd8a) )
	ROM_LOAD ( "trogu113.bin",  0x040000, 0x20000, CRC(77f50cbb) SHA1(5f2df3aedd90871ac02bca07c66387f6cda0dfdf) )

	ROM_LOAD ( "trogu106.bin",  0x080000, 0x20000, CRC(af2eb0d8) SHA1(3767e6f3853b092b40664c2b6c6a838f0243514b) )
	ROM_LOAD ( "trogu107.bin",  0x0a0000, 0x20000, CRC(88a7b3f6) SHA1(ba55f66929841a915d7b96aabf4b11e50ba6cfbd) )

	ROM_LOAD (  "trogu95.bin",  0x200000, 0x20000, CRC(f3ba2838) SHA1(2bee6c783c84a9f3f9309d802f42983857190ece) )
	ROM_LOAD (  "trogu96.bin",  0x220000, 0x20000, CRC(cfed2e77) SHA1(7fc0f52ac844c9efcbcc3004c40f9f4fc7e1c346) )
	ROM_LOAD (  "trogu97.bin",  0x240000, 0x20000, CRC(3262d1f8) SHA1(754e3e8223edd11398b2db77fd5db619dad1577b) )

	ROM_LOAD (  "trogu90.bin",  0x280000, 0x20000, CRC(16e06753) SHA1(62ec2b18e6b965ea0792d655d7878b4225da3aca) )
	ROM_LOAD (  "trogu91.bin",  0x2a0000, 0x20000, CRC(880a02c7) SHA1(ab1b2d24be4571a183b230d267c6c8167d4a42a4) )
ROM_END


ROM_START( trog3 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "trogu4.bin", 0x10000, 0x10000, CRC(759d0bf4) SHA1(c4c3fa51c43cf7fd241ac1f33d7d220aa9f9edb3) )
	ROM_RELOAD(                0x20000, 0x10000 )
	ROM_LOAD (  "trogu19.bin", 0x30000, 0x10000, CRC(960c333d) SHA1(da8ce8dfffffe7a2d60b3f75cc5aa88e5e2be659) )
	ROM_RELOAD(                0x40000, 0x10000 )
	ROM_LOAD (  "trogu20.bin", 0x50000, 0x10000, CRC(67f1658a) SHA1(c85dc920ff4b292afa9f6681f31918a200799cc9) )
	ROM_RELOAD(                0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la3",  0xc0000, 0x20000, CRC(d09cea97) SHA1(0c1384be2af8abbaf1c5c7f86f31ec605c18e798) )
	ROM_LOAD16_BYTE( "u89-la3",   0xc0001, 0x20000, CRC(a61e3572) SHA1(5366f4c9592dc9e23ffe867a16cbf51d1811a622) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "trogu111.bin",  0x000000, 0x20000, CRC(9ded08c1) SHA1(bbf069f218e3f3d67e45fa2229a471451b3a8f96) )
	ROM_LOAD ( "trogu112.bin",  0x020000, 0x20000, CRC(42293843) SHA1(cae77eeddd784573beccb79e54573da0e4ccdd8a) )
	ROM_LOAD ( "trogu113.bin",  0x040000, 0x20000, CRC(77f50cbb) SHA1(5f2df3aedd90871ac02bca07c66387f6cda0dfdf) )

	ROM_LOAD ( "trogu106.bin",  0x080000, 0x20000, CRC(af2eb0d8) SHA1(3767e6f3853b092b40664c2b6c6a838f0243514b) )
	ROM_LOAD ( "trogu107.bin",  0x0a0000, 0x20000, CRC(88a7b3f6) SHA1(ba55f66929841a915d7b96aabf4b11e50ba6cfbd) )

	ROM_LOAD (  "trogu95.bin",  0x200000, 0x20000, CRC(f3ba2838) SHA1(2bee6c783c84a9f3f9309d802f42983857190ece) )
	ROM_LOAD (  "trogu96.bin",  0x220000, 0x20000, CRC(cfed2e77) SHA1(7fc0f52ac844c9efcbcc3004c40f9f4fc7e1c346) )
	ROM_LOAD (  "trogu97.bin",  0x240000, 0x20000, CRC(3262d1f8) SHA1(754e3e8223edd11398b2db77fd5db619dad1577b) )

	ROM_LOAD (  "trogu90.bin",  0x280000, 0x20000, CRC(16e06753) SHA1(62ec2b18e6b965ea0792d655d7878b4225da3aca) )
	ROM_LOAD (  "trogu91.bin",  0x2a0000, 0x20000, CRC(880a02c7) SHA1(ab1b2d24be4571a183b230d267c6c8167d4a42a4) )
ROM_END


ROM_START( trogpa6 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "trogu4.bin", 0x10000, 0x10000, CRC(759d0bf4) SHA1(c4c3fa51c43cf7fd241ac1f33d7d220aa9f9edb3) )
	ROM_RELOAD(                0x20000, 0x10000 )
	ROM_LOAD (  "trogu19.bin", 0x30000, 0x10000, CRC(960c333d) SHA1(da8ce8dfffffe7a2d60b3f75cc5aa88e5e2be659) )
	ROM_RELOAD(                0x40000, 0x10000 )
	ROM_LOAD (  "trogu20.bin", 0x50000, 0x10000, CRC(67f1658a) SHA1(c85dc920ff4b292afa9f6681f31918a200799cc9) )
	ROM_RELOAD(                0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-pa6",  0xc0000, 0x20000, CRC(71ad1903) SHA1(e7ff1344a7bdc3b90f09ce8251ebcd25012be602) )
	ROM_LOAD16_BYTE( "u89-pa6",   0xc0001, 0x20000, CRC(04473da8) SHA1(47d9e918fba93b4af1e3cacbac9df843e6a10091) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "trogu111.bin",  0x000000, 0x20000, CRC(9ded08c1) SHA1(bbf069f218e3f3d67e45fa2229a471451b3a8f96) )
	ROM_LOAD ( "trogu112.bin",  0x020000, 0x20000, CRC(42293843) SHA1(cae77eeddd784573beccb79e54573da0e4ccdd8a) )
	ROM_LOAD ( "trogu113.pa6",  0x040000, 0x20000, CRC(ae50e5ea) SHA1(915b76f76e7ccbf2c4c28829cea15feaafea498b) )

	ROM_LOAD ( "trogu106.bin",  0x080000, 0x20000, CRC(af2eb0d8) SHA1(3767e6f3853b092b40664c2b6c6a838f0243514b) )
	ROM_LOAD ( "trogu107.bin",  0x0a0000, 0x20000, CRC(88a7b3f6) SHA1(ba55f66929841a915d7b96aabf4b11e50ba6cfbd) )

	ROM_LOAD (  "trogu95.bin",  0x200000, 0x20000, CRC(f3ba2838) SHA1(2bee6c783c84a9f3f9309d802f42983857190ece) )
	ROM_LOAD (  "trogu96.bin",  0x220000, 0x20000, CRC(cfed2e77) SHA1(7fc0f52ac844c9efcbcc3004c40f9f4fc7e1c346) )
	ROM_LOAD (  "trogu97.pa6",  0x240000, 0x20000, CRC(354b1cb3) SHA1(88400e39f0476d32a0798c50855a8ff9dc0a6617) )

	ROM_LOAD (  "trogu90.bin",  0x280000, 0x20000, CRC(16e06753) SHA1(62ec2b18e6b965ea0792d655d7878b4225da3aca) )
	ROM_LOAD (  "trogu91.bin",  0x2a0000, 0x20000, CRC(880a02c7) SHA1(ab1b2d24be4571a183b230d267c6c8167d4a42a4) )
ROM_END


ROM_START( trogpa4 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "trogu4.bin", 0x10000, 0x10000, CRC(759d0bf4) SHA1(c4c3fa51c43cf7fd241ac1f33d7d220aa9f9edb3) )
	ROM_RELOAD(                0x20000, 0x10000 )
	ROM_LOAD (  "trogu19.bin", 0x30000, 0x10000, CRC(960c333d) SHA1(da8ce8dfffffe7a2d60b3f75cc5aa88e5e2be659) )
	ROM_RELOAD(                0x40000, 0x10000 )
	ROM_LOAD (  "trogu20.bin", 0x50000, 0x10000, CRC(67f1658a) SHA1(c85dc920ff4b292afa9f6681f31918a200799cc9) )
	ROM_RELOAD(                0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-pa4",  0xc0000, 0x20000, CRC(526a3f5b) SHA1(8ad8cb15ada527f989f774a4fb81a171697c6dad) )
	ROM_LOAD16_BYTE( "u89-pa4",   0xc0001, 0x20000, CRC(38d68685) SHA1(42b73a64641301bf2991929cf365b8f45fc1b5d8) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "trogu111.bin",  0x000000, 0x20000, CRC(9ded08c1) SHA1(bbf069f218e3f3d67e45fa2229a471451b3a8f96) )
	ROM_LOAD ( "trogu112.bin",  0x020000, 0x20000, CRC(42293843) SHA1(cae77eeddd784573beccb79e54573da0e4ccdd8a) )
	ROM_LOAD ( "trogu113.pa4",  0x040000, 0x20000, CRC(2980a56f) SHA1(1e6ab16be6071d6568149e9ba56e146e3431b5f2) )

	ROM_LOAD ( "trogu106.bin",  0x080000, 0x20000, CRC(af2eb0d8) SHA1(3767e6f3853b092b40664c2b6c6a838f0243514b) )
	ROM_LOAD ( "trogu107.bin",  0x0a0000, 0x20000, CRC(88a7b3f6) SHA1(ba55f66929841a915d7b96aabf4b11e50ba6cfbd) )

	ROM_LOAD (  "trogu95.bin",  0x200000, 0x20000, CRC(f3ba2838) SHA1(2bee6c783c84a9f3f9309d802f42983857190ece) )
	ROM_LOAD (  "trogu96.bin",  0x220000, 0x20000, CRC(cfed2e77) SHA1(7fc0f52ac844c9efcbcc3004c40f9f4fc7e1c346) )
	ROM_LOAD (  "trogu97.pa4",  0x240000, 0x20000, CRC(f94b77c1) SHA1(d4ca3d7270ea1d86cb5c53e85dc7682b0e5945ef) )

	ROM_LOAD (  "trogu90.bin",  0x280000, 0x20000, CRC(16e06753) SHA1(62ec2b18e6b965ea0792d655d7878b4225da3aca) )
	ROM_LOAD (  "trogu91.bin",  0x2a0000, 0x20000, CRC(880a02c7) SHA1(ab1b2d24be4571a183b230d267c6c8167d4a42a4) )
ROM_END


ROM_START( smashtv )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl2.u4", 0x10000, 0x10000, CRC(29d3f6c8) SHA1(8a90cdff54f59ddb7dba521504d880515a59df08) )
	ROM_RELOAD(           0x20000, 0x10000 )
	ROM_LOAD ( "sl2.u19", 0x30000, 0x10000, CRC(ac5a402a) SHA1(c476018062126dc3936caa2c328de490737165ec) )
	ROM_RELOAD(           0x40000, 0x10000 )
	ROM_LOAD ( "sl2.u20", 0x50000, 0x10000, CRC(875c66d9) SHA1(51cdad62ec57e69bba6fcf14e59841ec628dec11) )
	ROM_RELOAD(           0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la8",  0xc0000, 0x20000, CRC(48cd793f) SHA1(7d0d9edccf0610f57e40934ab33e32315369656d) )
	ROM_LOAD16_BYTE( "u89-la8",   0xc0001, 0x20000, CRC(8e7fe463) SHA1(629332be706cda26f8b170b8e2877355230119ee) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1.u111",  0x000000, 0x20000, CRC(72f0ba84) SHA1(2e925b3cdd3c8e14046b3948d82f0f3cde3c22c5) )
	ROM_LOAD ( "la1.u112",  0x020000, 0x20000, CRC(436f0283) SHA1(ec33a8942c0fc326db885e08dad9346ec5a63360) )
	ROM_LOAD ( "la1.u113",  0x040000, 0x20000, CRC(4a4b8110) SHA1(9f1881d1d2682764ab85aebd685d97eb8b4afe46) )

	ROM_LOAD (  "la1.u95",  0x200000, 0x20000, CRC(e864a44b) SHA1(40eb8e11a183f4f82dc8decb36aaeded9cd1bc26) )
	ROM_LOAD (  "la1.u96",  0x220000, 0x20000, CRC(15555ea7) SHA1(4fefc059736ca424dc05a08cb55b9acf9e31228b) )
	ROM_LOAD (  "la1.u97",  0x240000, 0x20000, CRC(ccac9d9e) SHA1(a43d70d1a0bbd377f0fc539c2e8b725f7079f463) )

	ROM_LOAD ( "la1.u106",  0x400000, 0x20000, CRC(5c718361) SHA1(6178b1d53411f24d5a5a01559727e300cd27d587) )
	ROM_LOAD ( "la1.u107",  0x420000, 0x20000, CRC(0fba1e36) SHA1(17038cf35a72678bba149a632f1ad1b80cc3a38c) )
	ROM_LOAD ( "la1.u108",  0x440000, 0x20000, CRC(cb0a092f) SHA1(33cbb87b4be1eadb1f3624ef5e218e65109fa3eb) )
ROM_END


ROM_START( smashtv6 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl2.u4", 0x10000, 0x10000, CRC(29d3f6c8) SHA1(8a90cdff54f59ddb7dba521504d880515a59df08) )
	ROM_RELOAD(           0x20000, 0x10000 )
	ROM_LOAD ( "sl2.u19", 0x30000, 0x10000, CRC(ac5a402a) SHA1(c476018062126dc3936caa2c328de490737165ec) )
	ROM_RELOAD(           0x40000, 0x10000 )
	ROM_LOAD ( "sl2.u20", 0x50000, 0x10000, CRC(875c66d9) SHA1(51cdad62ec57e69bba6fcf14e59841ec628dec11) )
	ROM_RELOAD(           0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la6",  0xc0000, 0x20000, CRC(f1666017) SHA1(2283e71ad55a7cd3bc97bd6b20aebb90ad618bf8) )
	ROM_LOAD16_BYTE( "u89-la6",   0xc0001, 0x20000, CRC(908aca5d) SHA1(c97f05ecb8d96306fecef40330331e279d29f78d) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1.u111",  0x000000, 0x20000, CRC(72f0ba84) SHA1(2e925b3cdd3c8e14046b3948d82f0f3cde3c22c5) )
	ROM_LOAD ( "la1.u112",  0x020000, 0x20000, CRC(436f0283) SHA1(ec33a8942c0fc326db885e08dad9346ec5a63360) )
	ROM_LOAD ( "la1.u113",  0x040000, 0x20000, CRC(4a4b8110) SHA1(9f1881d1d2682764ab85aebd685d97eb8b4afe46) )

	ROM_LOAD (  "la1.u95",  0x200000, 0x20000, CRC(e864a44b) SHA1(40eb8e11a183f4f82dc8decb36aaeded9cd1bc26) )
	ROM_LOAD (  "la1.u96",  0x220000, 0x20000, CRC(15555ea7) SHA1(4fefc059736ca424dc05a08cb55b9acf9e31228b) )
	ROM_LOAD (  "la1.u97",  0x240000, 0x20000, CRC(ccac9d9e) SHA1(a43d70d1a0bbd377f0fc539c2e8b725f7079f463) )

	ROM_LOAD ( "la1.u106",  0x400000, 0x20000, CRC(5c718361) SHA1(6178b1d53411f24d5a5a01559727e300cd27d587) )
	ROM_LOAD ( "la1.u107",  0x420000, 0x20000, CRC(0fba1e36) SHA1(17038cf35a72678bba149a632f1ad1b80cc3a38c) )
	ROM_LOAD ( "la1.u108",  0x440000, 0x20000, CRC(cb0a092f) SHA1(33cbb87b4be1eadb1f3624ef5e218e65109fa3eb) )
ROM_END


ROM_START( smashtv5 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 ) /* sound CPU */
	ROM_LOAD (  "sl2.u4", 0x10000, 0x10000, CRC(29d3f6c8) SHA1(8a90cdff54f59ddb7dba521504d880515a59df08) )
	ROM_RELOAD(           0x20000, 0x10000 )
	ROM_LOAD ( "sl2.u19", 0x30000, 0x10000, CRC(ac5a402a) SHA1(c476018062126dc3936caa2c328de490737165ec) )
	ROM_RELOAD(           0x40000, 0x10000 )
	ROM_LOAD ( "sl2.u20", 0x50000, 0x10000, CRC(875c66d9) SHA1(51cdad62ec57e69bba6fcf14e59841ec628dec11) )
	ROM_RELOAD(           0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la5",  0xc0000, 0x20000, CRC(81f564b9) SHA1(5bddcda054be6766b40af88ae2519b3a87c33667) )
	ROM_LOAD16_BYTE( "u89-la5",   0xc0001, 0x20000, CRC(e5017d25) SHA1(27e544efa7f5cbe6ed3fc3211b12694c15a316c7) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1.u111",  0x000000, 0x20000, CRC(72f0ba84) SHA1(2e925b3cdd3c8e14046b3948d82f0f3cde3c22c5) )
	ROM_LOAD ( "la1.u112",  0x020000, 0x20000, CRC(436f0283) SHA1(ec33a8942c0fc326db885e08dad9346ec5a63360) )
	ROM_LOAD ( "la1.u113",  0x040000, 0x20000, CRC(4a4b8110) SHA1(9f1881d1d2682764ab85aebd685d97eb8b4afe46) )

	ROM_LOAD (  "la1.u95",  0x200000, 0x20000, CRC(e864a44b) SHA1(40eb8e11a183f4f82dc8decb36aaeded9cd1bc26) )
	ROM_LOAD (  "la1.u96",  0x220000, 0x20000, CRC(15555ea7) SHA1(4fefc059736ca424dc05a08cb55b9acf9e31228b) )
	ROM_LOAD (  "la1.u97",  0x240000, 0x20000, CRC(ccac9d9e) SHA1(a43d70d1a0bbd377f0fc539c2e8b725f7079f463) )

	ROM_LOAD ( "la1.u106",  0x400000, 0x20000, CRC(5c718361) SHA1(6178b1d53411f24d5a5a01559727e300cd27d587) )
	ROM_LOAD ( "la1.u107",  0x420000, 0x20000, CRC(0fba1e36) SHA1(17038cf35a72678bba149a632f1ad1b80cc3a38c) )
	ROM_LOAD ( "la1.u108",  0x440000, 0x20000, CRC(cb0a092f) SHA1(33cbb87b4be1eadb1f3624ef5e218e65109fa3eb) )
ROM_END


ROM_START( smashtv4 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl2.u4", 0x10000, 0x10000, CRC(29d3f6c8) SHA1(8a90cdff54f59ddb7dba521504d880515a59df08) )
	ROM_RELOAD(           0x20000, 0x10000 )
	ROM_LOAD ( "sl2.u19", 0x30000, 0x10000, CRC(ac5a402a) SHA1(c476018062126dc3936caa2c328de490737165ec) )
	ROM_RELOAD(           0x40000, 0x10000 )
	ROM_LOAD ( "sl2.u20", 0x50000, 0x10000, CRC(875c66d9) SHA1(51cdad62ec57e69bba6fcf14e59841ec628dec11) )
	ROM_RELOAD(           0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la4",  0xc0000, 0x20000, CRC(a50ccb71) SHA1(414dfe355e314f6460ce07edbdd5e4b801451cf8) )
	ROM_LOAD16_BYTE( "u89-la4",   0xc0001, 0x20000, CRC(ef0b0279) SHA1(baad5a2a8d51d007e365f378f3214bbd2ea9699c) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1.u111",  0x000000, 0x20000, CRC(72f0ba84) SHA1(2e925b3cdd3c8e14046b3948d82f0f3cde3c22c5) )
	ROM_LOAD ( "la1.u112",  0x020000, 0x20000, CRC(436f0283) SHA1(ec33a8942c0fc326db885e08dad9346ec5a63360) )
	ROM_LOAD ( "la1.u113",  0x040000, 0x20000, CRC(4a4b8110) SHA1(9f1881d1d2682764ab85aebd685d97eb8b4afe46) )

	ROM_LOAD (  "la1.u95",  0x200000, 0x20000, CRC(e864a44b) SHA1(40eb8e11a183f4f82dc8decb36aaeded9cd1bc26) )
	ROM_LOAD (  "la1.u96",  0x220000, 0x20000, CRC(15555ea7) SHA1(4fefc059736ca424dc05a08cb55b9acf9e31228b) )
	ROM_LOAD (  "la1.u97",  0x240000, 0x20000, CRC(ccac9d9e) SHA1(a43d70d1a0bbd377f0fc539c2e8b725f7079f463) )

	ROM_LOAD ( "la1.u106",  0x400000, 0x20000, CRC(5c718361) SHA1(6178b1d53411f24d5a5a01559727e300cd27d587) )
	ROM_LOAD ( "la1.u107",  0x420000, 0x20000, CRC(0fba1e36) SHA1(17038cf35a72678bba149a632f1ad1b80cc3a38c) )
	ROM_LOAD ( "la1.u108",  0x440000, 0x20000, CRC(cb0a092f) SHA1(33cbb87b4be1eadb1f3624ef5e218e65109fa3eb) )
ROM_END


ROM_START( smashtv3 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl2.u4", 0x10000, 0x10000, CRC(29d3f6c8) SHA1(8a90cdff54f59ddb7dba521504d880515a59df08) )
	ROM_RELOAD(           0x20000, 0x10000 )
	ROM_LOAD ( "sl2.u19", 0x30000, 0x10000, CRC(ac5a402a) SHA1(c476018062126dc3936caa2c328de490737165ec) )
	ROM_RELOAD(           0x40000, 0x10000 )
	ROM_LOAD ( "sl2.u20", 0x50000, 0x10000, CRC(875c66d9) SHA1(51cdad62ec57e69bba6fcf14e59841ec628dec11) )
	ROM_RELOAD(           0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la3",  0xc0000, 0x20000, CRC(33b626c3) SHA1(8f0582f6fe08dc7de920aeac578ed570ca4e717f) )
	ROM_LOAD16_BYTE( "u89-la3",   0xc0001, 0x20000, CRC(5f6fbc25) SHA1(d623f5e64ff4e70e24d770ac3ac0d32ff3928ce0) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1.u111",  0x000000, 0x20000, CRC(72f0ba84) SHA1(2e925b3cdd3c8e14046b3948d82f0f3cde3c22c5) )
	ROM_LOAD ( "la1.u112",  0x020000, 0x20000, CRC(436f0283) SHA1(ec33a8942c0fc326db885e08dad9346ec5a63360) )
	ROM_LOAD ( "la1.u113",  0x040000, 0x20000, CRC(4a4b8110) SHA1(9f1881d1d2682764ab85aebd685d97eb8b4afe46) )

	ROM_LOAD (  "la1.u95",  0x200000, 0x20000, CRC(e864a44b) SHA1(40eb8e11a183f4f82dc8decb36aaeded9cd1bc26) )
	ROM_LOAD (  "la1.u96",  0x220000, 0x20000, CRC(15555ea7) SHA1(4fefc059736ca424dc05a08cb55b9acf9e31228b) )
	ROM_LOAD (  "la1.u97",  0x240000, 0x20000, CRC(ccac9d9e) SHA1(a43d70d1a0bbd377f0fc539c2e8b725f7079f463) )

	ROM_LOAD ( "la1.u106",  0x400000, 0x20000, CRC(5c718361) SHA1(6178b1d53411f24d5a5a01559727e300cd27d587) )
	ROM_LOAD ( "la1.u107",  0x420000, 0x20000, CRC(0fba1e36) SHA1(17038cf35a72678bba149a632f1ad1b80cc3a38c) )
	ROM_LOAD ( "la1.u108",  0x440000, 0x20000, CRC(cb0a092f) SHA1(33cbb87b4be1eadb1f3624ef5e218e65109fa3eb) )
ROM_END


ROM_START( hiimpact )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl1u4.bin", 0x10000, 0x20000, CRC(28effd6a) SHA1(4a839f15e1b453a22fdef7b1801b8cc5cfdf3c29) )
	ROM_LOAD ( "sl1u19.bin", 0x30000, 0x20000, CRC(0ea22c89) SHA1(6d4579f6b10cac685be01348451b3537a0626034) )
	ROM_LOAD ( "sl1u20.bin", 0x50000, 0x20000, CRC(4e747ab5) SHA1(82040f40aac7dae577376a742eadaaa9644500c1) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la5",  0xc0000, 0x20000, CRC(104c30e7) SHA1(62b48b9c20730ffbaa1810650ff55aba14b6880d) )
	ROM_LOAD16_BYTE( "u89-la5",   0xc0001, 0x20000, CRC(07aa0010) SHA1(7dfd34028afeea4444e70c40fa30c6576ff22f7d) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1u111.bin",  0x000000, 0x20000, CRC(49560560) SHA1(03d51e6019afa9a396c91a484969be4922fa4c99) )
	ROM_LOAD ( "la1u112.bin",  0x020000, 0x20000, CRC(4dd879dc) SHA1(ac4f02fcb933df38f1ebf51b109092b77563b684) )
	ROM_LOAD ( "la1u113.bin",  0x040000, 0x20000, CRC(b67aeb70) SHA1(dd1512329c46da4254712712b6f847544f4487bd) )
	ROM_LOAD ( "la1u114.bin",  0x060000, 0x20000, CRC(9a4bc44b) SHA1(309eb5214fe5e1fe64d724d515190a31fc524aae) )

	ROM_LOAD (  "la1u95.bin",  0x200000, 0x20000, CRC(e1352dc0) SHA1(7faa2cfa9ebaf2d99b243232316221b672869703) )
	ROM_LOAD (  "la1u96.bin",  0x220000, 0x20000, CRC(197d0f34) SHA1(2d544588c3241423188ac7fb7aff87043fdd063d) )
	ROM_LOAD (  "la1u97.bin",  0x240000, 0x20000, CRC(908ea575) SHA1(79802d8df4e016d178be98333d2b1d047a27eccc) )
	ROM_LOAD (  "la1u98.bin",  0x260000, 0x20000, CRC(6dcbab11) SHA1(7432172810fd4b922b61769c68d86f24769a42cf) )

	ROM_LOAD ( "la1u106.bin",  0x400000, 0x20000, CRC(7d0ead0d) SHA1(1e65b6e7e629021d70603df37db5fa89cfe93175) )
	ROM_LOAD ( "la1u107.bin",  0x420000, 0x20000, CRC(ef48e8fa) SHA1(538de37cd8342085ec27f67292a7eeb1007e3b1f) )
	ROM_LOAD ( "la1u108.bin",  0x440000, 0x20000, CRC(5f363e12) SHA1(da398c0204f785aad4c52007d2f25031ecc1c63f) )
	ROM_LOAD ( "la1u109.bin",  0x460000, 0x20000, CRC(3689fbbc) SHA1(d95c0a2e3abf977ba7a899e419c22d004020c560) )

	ROM_REGION( 0x1100, "plds", 0 )
	ROM_LOAD( "ep610.u8",    0x0000, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "pls153a.u40", 0x0400, 0x00eb, CRC(69e5143f) SHA1(1a1e7b3233f7d5a1c161564710e8e984a9b0a16c) )
	ROM_LOAD( "ep610.u51",   0x0500, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u52",   0x0900, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u65",   0x0d00, 0x032f, NO_DUMP ) /* PAL is read protected */
ROM_END


ROM_START( hiimpact4 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl1u4.bin", 0x10000, 0x20000, CRC(28effd6a) SHA1(4a839f15e1b453a22fdef7b1801b8cc5cfdf3c29) )
	ROM_LOAD ( "sl1u19.bin", 0x30000, 0x20000, CRC(0ea22c89) SHA1(6d4579f6b10cac685be01348451b3537a0626034) )
	ROM_LOAD ( "sl1u20.bin", 0x50000, 0x20000, CRC(4e747ab5) SHA1(82040f40aac7dae577376a742eadaaa9644500c1) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la4",  0xc0000, 0x20000, CRC(5f67f823) SHA1(4171b6949682d1b2180e39d44c4e0033c4c07149) )
	ROM_LOAD16_BYTE( "u89-la4",   0xc0001, 0x20000, CRC(404d260b) SHA1(46bb44b3f1895d3424dba7664f198bce7dee911d) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1u111.bin",  0x000000, 0x20000, CRC(49560560) SHA1(03d51e6019afa9a396c91a484969be4922fa4c99) )
	ROM_LOAD ( "la1u112.bin",  0x020000, 0x20000, CRC(4dd879dc) SHA1(ac4f02fcb933df38f1ebf51b109092b77563b684) )
	ROM_LOAD ( "la1u113.bin",  0x040000, 0x20000, CRC(b67aeb70) SHA1(dd1512329c46da4254712712b6f847544f4487bd) )
	ROM_LOAD ( "la1u114.bin",  0x060000, 0x20000, CRC(9a4bc44b) SHA1(309eb5214fe5e1fe64d724d515190a31fc524aae) )

	ROM_LOAD (  "la1u95.bin",  0x200000, 0x20000, CRC(e1352dc0) SHA1(7faa2cfa9ebaf2d99b243232316221b672869703) )
	ROM_LOAD (  "la1u96.bin",  0x220000, 0x20000, CRC(197d0f34) SHA1(2d544588c3241423188ac7fb7aff87043fdd063d) )
	ROM_LOAD (  "la1u97.bin",  0x240000, 0x20000, CRC(908ea575) SHA1(79802d8df4e016d178be98333d2b1d047a27eccc) )
	ROM_LOAD (  "la1u98.bin",  0x260000, 0x20000, CRC(6dcbab11) SHA1(7432172810fd4b922b61769c68d86f24769a42cf) )

	ROM_LOAD ( "la1u106.bin",  0x400000, 0x20000, CRC(7d0ead0d) SHA1(1e65b6e7e629021d70603df37db5fa89cfe93175) )
	ROM_LOAD ( "la1u107.bin",  0x420000, 0x20000, CRC(ef48e8fa) SHA1(538de37cd8342085ec27f67292a7eeb1007e3b1f) )
	ROM_LOAD ( "la1u108.bin",  0x440000, 0x20000, CRC(5f363e12) SHA1(da398c0204f785aad4c52007d2f25031ecc1c63f) )
	ROM_LOAD ( "la1u109.bin",  0x460000, 0x20000, CRC(3689fbbc) SHA1(d95c0a2e3abf977ba7a899e419c22d004020c560) )

	ROM_REGION( 0x1100, "plds", 0 )
	ROM_LOAD( "ep610.u8",    0x0000, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "pls153a.u40", 0x0400, 0x00eb, CRC(69e5143f) SHA1(1a1e7b3233f7d5a1c161564710e8e984a9b0a16c) )
	ROM_LOAD( "ep610.u51",   0x0500, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u52",   0x0900, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u65",   0x0d00, 0x032f, NO_DUMP ) /* PAL is read protected */
ROM_END


ROM_START( hiimpact3 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl1u4.bin", 0x10000, 0x20000, CRC(28effd6a) SHA1(4a839f15e1b453a22fdef7b1801b8cc5cfdf3c29) )
	ROM_LOAD ( "sl1u19.bin", 0x30000, 0x20000, CRC(0ea22c89) SHA1(6d4579f6b10cac685be01348451b3537a0626034) )
	ROM_LOAD ( "sl1u20.bin", 0x50000, 0x20000, CRC(4e747ab5) SHA1(82040f40aac7dae577376a742eadaaa9644500c1) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la3",  0xc0000, 0x20000, CRC(b9190c4a) SHA1(adcf1023d62f67fbde7a7a7aeeda068d7711f7cf) )
	ROM_LOAD16_BYTE( "u89-la3",   0xc0001, 0x20000, CRC(1cbc72a5) SHA1(ba0b4b54453fcd1888d40690848e0ee4150bb8e1) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1u111.bin",  0x000000, 0x20000, CRC(49560560) SHA1(03d51e6019afa9a396c91a484969be4922fa4c99) )
	ROM_LOAD ( "la1u112.bin",  0x020000, 0x20000, CRC(4dd879dc) SHA1(ac4f02fcb933df38f1ebf51b109092b77563b684) )
	ROM_LOAD ( "la1u113.bin",  0x040000, 0x20000, CRC(b67aeb70) SHA1(dd1512329c46da4254712712b6f847544f4487bd) )
	ROM_LOAD ( "la1u114.bin",  0x060000, 0x20000, CRC(9a4bc44b) SHA1(309eb5214fe5e1fe64d724d515190a31fc524aae) )

	ROM_LOAD (  "la1u95.bin",  0x200000, 0x20000, CRC(e1352dc0) SHA1(7faa2cfa9ebaf2d99b243232316221b672869703) )
	ROM_LOAD (  "la1u96.bin",  0x220000, 0x20000, CRC(197d0f34) SHA1(2d544588c3241423188ac7fb7aff87043fdd063d) )
	ROM_LOAD (  "la1u97.bin",  0x240000, 0x20000, CRC(908ea575) SHA1(79802d8df4e016d178be98333d2b1d047a27eccc) )
	ROM_LOAD (  "la1u98.bin",  0x260000, 0x20000, CRC(6dcbab11) SHA1(7432172810fd4b922b61769c68d86f24769a42cf) )

	ROM_LOAD ( "la1u106.bin",  0x400000, 0x20000, CRC(7d0ead0d) SHA1(1e65b6e7e629021d70603df37db5fa89cfe93175) )
	ROM_LOAD ( "la1u107.bin",  0x420000, 0x20000, CRC(ef48e8fa) SHA1(538de37cd8342085ec27f67292a7eeb1007e3b1f) )
	ROM_LOAD ( "la1u108.bin",  0x440000, 0x20000, CRC(5f363e12) SHA1(da398c0204f785aad4c52007d2f25031ecc1c63f) )
	ROM_LOAD ( "la1u109.bin",  0x460000, 0x20000, CRC(3689fbbc) SHA1(d95c0a2e3abf977ba7a899e419c22d004020c560) )

	ROM_REGION( 0x1100, "plds", 0 )
	ROM_LOAD( "ep610.u8",    0x0000, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "pls153a.u40", 0x0400, 0x00eb, CRC(69e5143f) SHA1(1a1e7b3233f7d5a1c161564710e8e984a9b0a16c) )
	ROM_LOAD( "ep610.u51",   0x0500, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u52",   0x0900, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u65",   0x0d00, 0x032f, NO_DUMP ) /* PAL is read protected */
ROM_END


ROM_START( hiimpact2 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl1u4.bin", 0x10000, 0x20000, CRC(28effd6a) SHA1(4a839f15e1b453a22fdef7b1801b8cc5cfdf3c29) )
	ROM_LOAD ( "sl1u19.bin", 0x30000, 0x20000, CRC(0ea22c89) SHA1(6d4579f6b10cac685be01348451b3537a0626034) )
	ROM_LOAD ( "sl1u20.bin", 0x50000, 0x20000, CRC(4e747ab5) SHA1(82040f40aac7dae577376a742eadaaa9644500c1) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la2",  0xc0000, 0x20000, CRC(25d83ba1) SHA1(4422a34b2957aabb3f0a26ca129e290dfc062933) )
	ROM_LOAD16_BYTE( "u89-la2",   0xc0001, 0x20000, CRC(811f1253) SHA1(125a6cca26d37fae343b78046774f54ee6e8d996) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1u111.bin",  0x000000, 0x20000, CRC(49560560) SHA1(03d51e6019afa9a396c91a484969be4922fa4c99) )
	ROM_LOAD ( "la1u112.bin",  0x020000, 0x20000, CRC(4dd879dc) SHA1(ac4f02fcb933df38f1ebf51b109092b77563b684) )
	ROM_LOAD ( "la1u113.bin",  0x040000, 0x20000, CRC(b67aeb70) SHA1(dd1512329c46da4254712712b6f847544f4487bd) )
	ROM_LOAD ( "la1u114.bin",  0x060000, 0x20000, CRC(9a4bc44b) SHA1(309eb5214fe5e1fe64d724d515190a31fc524aae) )

	ROM_LOAD (  "la1u95.bin",  0x200000, 0x20000, CRC(e1352dc0) SHA1(7faa2cfa9ebaf2d99b243232316221b672869703) )
	ROM_LOAD (  "la1u96.bin",  0x220000, 0x20000, CRC(197d0f34) SHA1(2d544588c3241423188ac7fb7aff87043fdd063d) )
	ROM_LOAD (  "la1u97.bin",  0x240000, 0x20000, CRC(908ea575) SHA1(79802d8df4e016d178be98333d2b1d047a27eccc) )
	ROM_LOAD (  "la1u98.bin",  0x260000, 0x20000, CRC(6dcbab11) SHA1(7432172810fd4b922b61769c68d86f24769a42cf) )

	ROM_LOAD ( "la1u106.bin",  0x400000, 0x20000, CRC(7d0ead0d) SHA1(1e65b6e7e629021d70603df37db5fa89cfe93175) )
	ROM_LOAD ( "la1u107.bin",  0x420000, 0x20000, CRC(ef48e8fa) SHA1(538de37cd8342085ec27f67292a7eeb1007e3b1f) )
	ROM_LOAD ( "la1u108.bin",  0x440000, 0x20000, CRC(5f363e12) SHA1(da398c0204f785aad4c52007d2f25031ecc1c63f) )
	ROM_LOAD ( "la1u109.bin",  0x460000, 0x20000, CRC(3689fbbc) SHA1(d95c0a2e3abf977ba7a899e419c22d004020c560) )

	ROM_REGION( 0x1100, "plds", 0 )
	ROM_LOAD( "ep610.u8",    0x0000, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "pls153a.u40", 0x0400, 0x00eb, CRC(69e5143f) SHA1(1a1e7b3233f7d5a1c161564710e8e984a9b0a16c) )
	ROM_LOAD( "ep610.u51",   0x0500, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u52",   0x0900, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u65",   0x0d00, 0x032f, NO_DUMP ) /* PAL is read protected */
ROM_END


ROM_START( hiimpact1 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl1u4.bin", 0x10000, 0x20000, CRC(28effd6a) SHA1(4a839f15e1b453a22fdef7b1801b8cc5cfdf3c29) )
	ROM_LOAD ( "sl1u19.bin", 0x30000, 0x20000, CRC(0ea22c89) SHA1(6d4579f6b10cac685be01348451b3537a0626034) )
	ROM_LOAD ( "sl1u20.bin", 0x50000, 0x20000, CRC(4e747ab5) SHA1(82040f40aac7dae577376a742eadaaa9644500c1) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la1",  0xc0000, 0x20000, CRC(e86228ba) SHA1(0af263e51cb65115038ee5bf508515674e05913e) )
	ROM_LOAD16_BYTE( "u89-la1",   0xc0001, 0x20000, CRC(f23e972e) SHA1(e5ae5eaf5f97ec271b92072fd674e8cd93b36778) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1u111.bin",  0x000000, 0x20000, CRC(49560560) SHA1(03d51e6019afa9a396c91a484969be4922fa4c99) ) /* All roms had a "LA 1" sticker over a "PA 1" on the label */
	ROM_LOAD ( "la1u112.bin",  0x020000, 0x20000, CRC(4dd879dc) SHA1(ac4f02fcb933df38f1ebf51b109092b77563b684) )
	ROM_LOAD ( "la1u113.bin",  0x040000, 0x20000, CRC(b67aeb70) SHA1(dd1512329c46da4254712712b6f847544f4487bd) )
	ROM_LOAD ( "la1u114.bin",  0x060000, 0x20000, CRC(9a4bc44b) SHA1(309eb5214fe5e1fe64d724d515190a31fc524aae) )

	ROM_LOAD (  "la1u95.bin",  0x200000, 0x20000, CRC(e1352dc0) SHA1(7faa2cfa9ebaf2d99b243232316221b672869703) )
	ROM_LOAD (  "la1u96.bin",  0x220000, 0x20000, CRC(197d0f34) SHA1(2d544588c3241423188ac7fb7aff87043fdd063d) )
	ROM_LOAD (  "la1u97.bin",  0x240000, 0x20000, CRC(908ea575) SHA1(79802d8df4e016d178be98333d2b1d047a27eccc) )
	ROM_LOAD (  "la1u98.bin",  0x260000, 0x20000, CRC(6dcbab11) SHA1(7432172810fd4b922b61769c68d86f24769a42cf) )

	ROM_LOAD ( "la1u106.bin",  0x400000, 0x20000, CRC(7d0ead0d) SHA1(1e65b6e7e629021d70603df37db5fa89cfe93175) )
	ROM_LOAD ( "la1u107.bin",  0x420000, 0x20000, CRC(ef48e8fa) SHA1(538de37cd8342085ec27f67292a7eeb1007e3b1f) )
	ROM_LOAD ( "la1u108.bin",  0x440000, 0x20000, CRC(5f363e12) SHA1(da398c0204f785aad4c52007d2f25031ecc1c63f) )
	ROM_LOAD ( "la1u109.bin",  0x460000, 0x20000, CRC(3689fbbc) SHA1(d95c0a2e3abf977ba7a899e419c22d004020c560) )

	ROM_REGION( 0x1100, "plds", 0 )
	ROM_LOAD( "ep610.u8",    0x0000, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "pls153a.u40", 0x0400, 0x00eb, CRC(69e5143f) SHA1(1a1e7b3233f7d5a1c161564710e8e984a9b0a16c) )
	ROM_LOAD( "ep610.u51",   0x0500, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u52",   0x0900, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u65",   0x0d00, 0x032f, NO_DUMP ) /* PAL is read protected */
ROM_END


ROM_START( hiimpactp )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sl1u4.bin", 0x10000, 0x20000, CRC(28effd6a) SHA1(4a839f15e1b453a22fdef7b1801b8cc5cfdf3c29) )
	ROM_LOAD ( "sl1u19.bin", 0x30000, 0x20000, CRC(0ea22c89) SHA1(6d4579f6b10cac685be01348451b3537a0626034) )
	ROM_LOAD ( "sl1u20.bin", 0x50000, 0x20000, CRC(4e747ab5) SHA1(82040f40aac7dae577376a742eadaaa9644500c1) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-pa1",  0xc0000, 0x20000, CRC(79ef9a35) SHA1(200d50b108401e889b6200c53c203ee5041d1423) )
	ROM_LOAD16_BYTE( "u89-pa1",   0xc0001, 0x20000, CRC(2bd3de30) SHA1(ee3615c1cc5b948731eb258887641f059b942b25) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "la1u111.bin",  0x000000, 0x20000, CRC(49560560) SHA1(03d51e6019afa9a396c91a484969be4922fa4c99) ) /* All rom were labeled "PA 1" which Williams later designated "LA 1" */
	ROM_LOAD ( "la1u112.bin",  0x020000, 0x20000, CRC(4dd879dc) SHA1(ac4f02fcb933df38f1ebf51b109092b77563b684) ) /* without changing any of the contents when they release ver LA 1 */
	ROM_LOAD ( "la1u113.bin",  0x040000, 0x20000, CRC(b67aeb70) SHA1(dd1512329c46da4254712712b6f847544f4487bd) )
	ROM_LOAD ( "la1u114.bin",  0x060000, 0x20000, CRC(9a4bc44b) SHA1(309eb5214fe5e1fe64d724d515190a31fc524aae) )

	ROM_LOAD (  "la1u95.bin",  0x200000, 0x20000, CRC(e1352dc0) SHA1(7faa2cfa9ebaf2d99b243232316221b672869703) )
	ROM_LOAD (  "la1u96.bin",  0x220000, 0x20000, CRC(197d0f34) SHA1(2d544588c3241423188ac7fb7aff87043fdd063d) )
	ROM_LOAD (  "la1u97.bin",  0x240000, 0x20000, CRC(908ea575) SHA1(79802d8df4e016d178be98333d2b1d047a27eccc) )
	ROM_LOAD (  "la1u98.bin",  0x260000, 0x20000, CRC(6dcbab11) SHA1(7432172810fd4b922b61769c68d86f24769a42cf) )

	ROM_LOAD ( "la1u106.bin",  0x400000, 0x20000, CRC(7d0ead0d) SHA1(1e65b6e7e629021d70603df37db5fa89cfe93175) )
	ROM_LOAD ( "la1u107.bin",  0x420000, 0x20000, CRC(ef48e8fa) SHA1(538de37cd8342085ec27f67292a7eeb1007e3b1f) )
	ROM_LOAD ( "la1u108.bin",  0x440000, 0x20000, CRC(5f363e12) SHA1(da398c0204f785aad4c52007d2f25031ecc1c63f) )
	ROM_LOAD ( "la1u109.bin",  0x460000, 0x20000, CRC(3689fbbc) SHA1(d95c0a2e3abf977ba7a899e419c22d004020c560) )

	ROM_REGION( 0x1100, "plds", 0 )
	ROM_LOAD( "ep610.u8",    0x0000, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "pls153a.u40", 0x0400, 0x00eb, CRC(69e5143f) SHA1(1a1e7b3233f7d5a1c161564710e8e984a9b0a16c) )
	ROM_LOAD( "ep610.u51",   0x0500, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u52",   0x0900, 0x032f, NO_DUMP ) /* PAL is read protected */
	ROM_LOAD( "ep610.u65",   0x0d00, 0x032f, NO_DUMP ) /* PAL is read protected */
ROM_END


ROM_START( shimpact )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "shiu4.bin", 0x10000, 0x20000, CRC(1e5a012c) SHA1(4077fc266799a01738b7f88e867535f1fbacd557) )
	ROM_LOAD (  "shiu19.bin", 0x30000, 0x20000, CRC(10f9684e) SHA1(1fdc5364f87fb65f4f2a438841e0fe847f765aaf) )
	ROM_LOAD (  "shiu20.bin", 0x50000, 0x20000, CRC(1b4a71c1) SHA1(74b7b4ae76ebe65f1f46b2117970bfefefbb5344) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-la1",  0xc0000, 0x20000, CRC(f2cf8de3) SHA1(97428d05208c18a9fcf8f2e3c6ed2bf6441350c3) )
	ROM_LOAD16_BYTE( "u89-la1",   0xc0001, 0x20000, CRC(f97d9b01) SHA1(d5f39d6a5db23f5efd123cf9da0d09c84893b9c4) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "shiu111.bin",  0x000000, 0x40000, CRC(80ae2a86) SHA1(1ff76e3064c7636f6877e426f4a88c094d1a6325) )
	ROM_LOAD ( "shiu112.bin",  0x040000, 0x40000, CRC(3ffc27e9) SHA1(ec337629c17daaa2445fb344e08243de7f09536e) )
	ROM_LOAD ( "shiu113.bin",  0x080000, 0x40000, CRC(01549d00) SHA1(40604e949cef056f90031850bdb91782135e7ec2) )
	ROM_LOAD ( "shiu114.bin",  0x0c0000, 0x40000, CRC(a68af319) SHA1(9ed2e620a952dce26e08d0931f52eaeb638fc14d) )

	ROM_LOAD (  "shiu95.bin",  0x200000, 0x40000, CRC(e8f56ef5) SHA1(7cb0b6bad3f0be822ef9b92e6523572e45776969) )
	ROM_LOAD (  "shiu96.bin",  0x240000, 0x40000, CRC(24ed04f9) SHA1(f4e91640713c0c376861652f3f0db33bff32656d) )
	ROM_LOAD (  "shiu97.bin",  0x280000, 0x40000, CRC(dd7f41a9) SHA1(a14a285ccc593f8f1d50b0d5574af4845a1e287e) )
	ROM_LOAD (  "shiu98.bin",  0x2c0000, 0x40000, CRC(23ef65dd) SHA1(58400c305dfad1de18b84a8c118f72529b507414) )

	ROM_LOAD ( "shiu106.bin",  0x400000, 0x40000, CRC(6f5bf337) SHA1(5b1a0d927302c7e1727976c2d8c612a80b8f1484) )
	ROM_LOAD ( "shiu107.bin",  0x440000, 0x40000, CRC(a8815dad) SHA1(627d916a4b0ab03a943d123ca0eabd514634ad30) )
	ROM_LOAD ( "shiu108.bin",  0x480000, 0x40000, CRC(d39685a3) SHA1(84e5da34a9946b954635befd37760683850d310b) )
	ROM_LOAD ( "shiu109.bin",  0x4c0000, 0x40000, CRC(36e0b2b2) SHA1(96d76698a09cd884349bf0c4c1b75423b4404432) )
ROM_END


ROM_START( shimpactp6 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "shiu4.bin", 0x10000, 0x20000, CRC(1e5a012c) SHA1(4077fc266799a01738b7f88e867535f1fbacd557) )
	ROM_LOAD (  "shiu19.bin", 0x30000, 0x20000, CRC(10f9684e) SHA1(1fdc5364f87fb65f4f2a438841e0fe847f765aaf) )
	ROM_LOAD (  "shiu20.bin", 0x50000, 0x20000, CRC(1b4a71c1) SHA1(74b7b4ae76ebe65f1f46b2117970bfefefbb5344) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-pa6",  0xc0000, 0x20000, CRC(33e1978d) SHA1(a88af5551d6b4777e0c5f5e3844b2f1d61bbb35d) )
	ROM_LOAD16_BYTE( "u89-pa6",   0xc0001, 0x20000, CRC(6c070978) SHA1(ca6657c48810d78496c51eb750f45a3e08132ce3) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "shiu111.bin",  0x000000, 0x40000, CRC(80ae2a86) SHA1(1ff76e3064c7636f6877e426f4a88c094d1a6325) )
	ROM_LOAD ( "shiu112.bin",  0x040000, 0x40000, CRC(3ffc27e9) SHA1(ec337629c17daaa2445fb344e08243de7f09536e) )
	ROM_LOAD ( "shiu113.bin",  0x080000, 0x40000, CRC(01549d00) SHA1(40604e949cef056f90031850bdb91782135e7ec2) )
	ROM_LOAD ( "shiu114.bin",  0x0c0000, 0x40000, CRC(a68af319) SHA1(9ed2e620a952dce26e08d0931f52eaeb638fc14d) )

	ROM_LOAD (  "shiu95.bin",  0x200000, 0x40000, CRC(e8f56ef5) SHA1(7cb0b6bad3f0be822ef9b92e6523572e45776969) )
	ROM_LOAD (  "shiu96.bin",  0x240000, 0x40000, CRC(24ed04f9) SHA1(f4e91640713c0c376861652f3f0db33bff32656d) )
	ROM_LOAD (  "shiu97.bin",  0x280000, 0x40000, CRC(dd7f41a9) SHA1(a14a285ccc593f8f1d50b0d5574af4845a1e287e) )
	ROM_LOAD (  "shiu98.bin",  0x2c0000, 0x40000, CRC(23ef65dd) SHA1(58400c305dfad1de18b84a8c118f72529b507414) )

	ROM_LOAD ( "shiu106.bin",  0x400000, 0x40000, CRC(6f5bf337) SHA1(5b1a0d927302c7e1727976c2d8c612a80b8f1484) )
	ROM_LOAD ( "shiu107.bin",  0x440000, 0x40000, CRC(a8815dad) SHA1(627d916a4b0ab03a943d123ca0eabd514634ad30) )
	ROM_LOAD ( "shiu108.bin",  0x480000, 0x40000, CRC(d39685a3) SHA1(84e5da34a9946b954635befd37760683850d310b) )
	ROM_LOAD ( "shiu109.bin",  0x4c0000, 0x40000, CRC(36e0b2b2) SHA1(96d76698a09cd884349bf0c4c1b75423b4404432) )
ROM_END


ROM_START( shimpactp5 )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "shiu4.bin", 0x10000, 0x20000, CRC(1e5a012c) SHA1(4077fc266799a01738b7f88e867535f1fbacd557) )
	ROM_LOAD (  "shiu19.bin", 0x30000, 0x20000, CRC(10f9684e) SHA1(1fdc5364f87fb65f4f2a438841e0fe847f765aaf) )
	ROM_LOAD (  "shiu20.bin", 0x50000, 0x20000, CRC(1b4a71c1) SHA1(74b7b4ae76ebe65f1f46b2117970bfefefbb5344) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-pa5",  0xc0000, 0x20000, CRC(4342cd45) SHA1(a8e8609efbd67a957104316a0fd4824802134290) )
	ROM_LOAD16_BYTE( "u89-pa5",   0xc0001, 0x20000, CRC(cda47b73) SHA1(9b51f7d0cd6ffa07a5880e4cc8a855c2f7616c22) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "shiu111.bin",  0x000000, 0x40000, CRC(80ae2a86) SHA1(1ff76e3064c7636f6877e426f4a88c094d1a6325) )
	ROM_LOAD ( "shiu112.bin",  0x040000, 0x40000, CRC(3ffc27e9) SHA1(ec337629c17daaa2445fb344e08243de7f09536e) )
	ROM_LOAD ( "shiu113.bin",  0x080000, 0x40000, CRC(01549d00) SHA1(40604e949cef056f90031850bdb91782135e7ec2) )
	ROM_LOAD ( "shiu114.pa3",  0x0c0000, 0x40000, CRC(56f96a67) SHA1(070ba9c34c23b3037e91c2a7e0a85093c95def69) )

	ROM_LOAD (  "shiu95.bin",  0x200000, 0x40000, CRC(e8f56ef5) SHA1(7cb0b6bad3f0be822ef9b92e6523572e45776969) )
	ROM_LOAD (  "shiu96.bin",  0x240000, 0x40000, CRC(24ed04f9) SHA1(f4e91640713c0c376861652f3f0db33bff32656d) )
	ROM_LOAD (  "shiu97.bin",  0x280000, 0x40000, CRC(dd7f41a9) SHA1(a14a285ccc593f8f1d50b0d5574af4845a1e287e) )
	ROM_LOAD (  "shiu98.pa3",  0x2c0000, 0x40000, CRC(28418723) SHA1(d4eef3131c82f1ecb65d6623b195c4f76010aa1b) )

	ROM_LOAD ( "shiu106.bin",  0x400000, 0x40000, CRC(6f5bf337) SHA1(5b1a0d927302c7e1727976c2d8c612a80b8f1484) )
	ROM_LOAD ( "shiu107.bin",  0x440000, 0x40000, CRC(a8815dad) SHA1(627d916a4b0ab03a943d123ca0eabd514634ad30) )
	ROM_LOAD ( "shiu108.bin",  0x480000, 0x40000, CRC(d39685a3) SHA1(84e5da34a9946b954635befd37760683850d310b) )
	ROM_LOAD ( "shiu109.pa3",  0x4c0000, 0x40000, CRC(58f71141) SHA1(f7143bdaa7325b88e01a1d6be3aeb1d69cf0672b) )
ROM_END


ROM_START( shimpactp4 ) /* You must manualy reset the high score table or game will hang after initial demo screen, it's best to do a "Full Factory Restore" */
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (   "shiu4.bin", 0x10000, 0x20000, CRC(1e5a012c) SHA1(4077fc266799a01738b7f88e867535f1fbacd557) )
	ROM_LOAD (  "shiu19.bin", 0x30000, 0x20000, CRC(10f9684e) SHA1(1fdc5364f87fb65f4f2a438841e0fe847f765aaf) )
	ROM_LOAD (  "shiu20.bin", 0x50000, 0x20000, CRC(1b4a71c1) SHA1(74b7b4ae76ebe65f1f46b2117970bfefefbb5344) )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105-pa4",  0xc0000, 0x20000, CRC(770b31ce) SHA1(d0bc2ed0f6134afb0dd53236377044a122e7f181) ) /* Both program roms had a "PROTO4" sticker */
	ROM_LOAD16_BYTE( "u89-pa4",   0xc0001, 0x20000, CRC(96b622a5) SHA1(6d21c69ad1b0990679b616a79ba698772c8d98ff) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "shiu111.bin",  0x000000, 0x40000, CRC(80ae2a86) SHA1(1ff76e3064c7636f6877e426f4a88c094d1a6325) ) /* All graphic roms had a "PROTO3" sticker */
	ROM_LOAD ( "shiu112.bin",  0x040000, 0x40000, CRC(3ffc27e9) SHA1(ec337629c17daaa2445fb344e08243de7f09536e) )
	ROM_LOAD ( "shiu113.bin",  0x080000, 0x40000, CRC(01549d00) SHA1(40604e949cef056f90031850bdb91782135e7ec2) )
	ROM_LOAD ( "shiu114.pa3",  0x0c0000, 0x40000, CRC(56f96a67) SHA1(070ba9c34c23b3037e91c2a7e0a85093c95def69) )

	ROM_LOAD (  "shiu95.bin",  0x200000, 0x40000, CRC(e8f56ef5) SHA1(7cb0b6bad3f0be822ef9b92e6523572e45776969) )
	ROM_LOAD (  "shiu96.bin",  0x240000, 0x40000, CRC(24ed04f9) SHA1(f4e91640713c0c376861652f3f0db33bff32656d) )
	ROM_LOAD (  "shiu97.bin",  0x280000, 0x40000, CRC(dd7f41a9) SHA1(a14a285ccc593f8f1d50b0d5574af4845a1e287e) )
	ROM_LOAD (  "shiu98.pa3",  0x2c0000, 0x40000, CRC(28418723) SHA1(d4eef3131c82f1ecb65d6623b195c4f76010aa1b) )

	ROM_LOAD ( "shiu106.bin",  0x400000, 0x40000, CRC(6f5bf337) SHA1(5b1a0d927302c7e1727976c2d8c612a80b8f1484) )
	ROM_LOAD ( "shiu107.bin",  0x440000, 0x40000, CRC(a8815dad) SHA1(627d916a4b0ab03a943d123ca0eabd514634ad30) )
	ROM_LOAD ( "shiu108.bin",  0x480000, 0x40000, CRC(d39685a3) SHA1(84e5da34a9946b954635befd37760683850d310b) )
	ROM_LOAD ( "shiu109.pa3",  0x4c0000, 0x40000, CRC(58f71141) SHA1(f7143bdaa7325b88e01a1d6be3aeb1d69cf0672b) )
ROM_END


ROM_START( strkforc )
	ROM_REGION( 0x90000, "cvsd:cpu", 0 )    /* sound CPU */
	ROM_LOAD (  "sfu4.bin", 0x10000, 0x10000, CRC(8f747312) SHA1(729929c209741e72eb83b407cf95d7709ec1b5ae) )
	ROM_RELOAD(             0x20000, 0x10000 )
	ROM_LOAD ( "sfu19.bin", 0x30000, 0x10000, CRC(afb29926) SHA1(ad904c0968a90b8187cc87d6c171fbc021d2f66f) )
	ROM_RELOAD(             0x40000, 0x10000 )
	ROM_LOAD ( "sfu20.bin", 0x50000, 0x10000, CRC(1bc9b746) SHA1(a5ad40ce7f228f30c21c5a7bdc2893c2a7fe7f58) )
	ROM_RELOAD(             0x60000, 0x10000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "sfu105.bin",  0xc0000, 0x20000, CRC(7895e0e3) SHA1(fa471af9e673a82713a590f463f87a4c59e3d5d8) )
	ROM_LOAD16_BYTE( "sfu89.bin",   0xc0001, 0x20000, CRC(26114d9e) SHA1(79906966859f0ae0884b956e7d520e3cff78fab7) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "sfu111.bin",  0x000000, 0x20000, CRC(878efc80) SHA1(94448002faff5839beab5466e5a41195869face3) )
	ROM_LOAD ( "sfu112.bin",  0x020000, 0x20000, CRC(93394399) SHA1(67ad5c27c3c82fa6055032df98c365c56c7f8b1b) )
	ROM_LOAD ( "sfu113.bin",  0x040000, 0x20000, CRC(9565a79b) SHA1(ebb90132ed8acbbed09bbcdff435cdf60a3ef8ab) )
	ROM_LOAD ( "sfu114.bin",  0x060000, 0x20000, CRC(b71152da) SHA1(784e229a5ae51776a3e984f22d1d73b2286cfc68) )

	ROM_LOAD ( "sfu106.bin",  0x080000, 0x20000, CRC(a394d4cf) SHA1(d08c5994b08dafd233a270d24b4c851bcedf5cbe) )
	ROM_LOAD ( "sfu107.bin",  0x0a0000, 0x20000, CRC(edef1419) SHA1(cda8de55355eabf8146a243f917f6d27babe5ce3) )

	ROM_LOAD (  "sfu95.bin",  0x200000, 0x20000, CRC(519cb2b4) SHA1(9059d2ca2705bd297c066a9470b756aecb395431) )
	ROM_LOAD (  "sfu96.bin",  0x220000, 0x20000, CRC(61214796) SHA1(bad32ef909f714289ee7cf2a5179a3b96678a72a) )
	ROM_LOAD (  "sfu97.bin",  0x240000, 0x20000, CRC(eb5dee5f) SHA1(9432140b4c983472fdc41f36390ee4db67896475) )
	ROM_LOAD (  "sfu98.bin",  0x260000, 0x20000, CRC(c5c079e7) SHA1(3cbd56db7d0eeaa6fb4f1cc8793cd1deff4e3c2c) )

	ROM_LOAD (  "sfu90.bin",  0x280000, 0x20000, CRC(607bcdc0) SHA1(f174a549ade75df2f86142150a1e4c3554907602) )
	ROM_LOAD (  "sfu91.bin",  0x2a0000, 0x20000, CRC(da02547e) SHA1(d29c071bd9deab2414ac0733d9a18fcf8c68b4d9) )
ROM_END


ROM_START( mkla4 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "mkg-u105.la4",  0x00000, 0x80000, CRC(29af348f) SHA1(9f8a57606647c5ea056d61aa4ab1232538539fd8) )
	ROM_LOAD16_BYTE(  "mkg-u89.la4",  0x00001, 0x80000, CRC(1ad76662) SHA1(bee4ab5371f58df799365e73ec0cc02e903f240c) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END


ROM_START( mkla3 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "mkg-u105.la3",  0x00000, 0x80000, CRC(2ce843c5) SHA1(d48efcecd6528414249f3884edc32e0dafa9677f) )
	ROM_LOAD16_BYTE(  "mkg-u89.la3",  0x00001, 0x80000, CRC(49a46e10) SHA1(c63c00531b29c01ee864acc141b1713507d25c69) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END


ROM_START( mkla2 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "mkg-u105.la2",  0x00000, 0x80000, CRC(8531d44e) SHA1(652c7946cc725e11815f852af8891511b87de186) )
	ROM_LOAD16_BYTE(  "mkg-u89.la2",  0x00001, 0x80000, CRC(b88dc26e) SHA1(bf34a03bdb70b67fd9c0b6d636b038a63827151e) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END


ROM_START( mkla1 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "mkg-u105.la1",  0x00000, 0x80000, CRC(e1f7b4c9) SHA1(dc62e67e03b54460494bd94a50347327c19b72ec) )
	ROM_LOAD16_BYTE(  "mkg-u89.la1",  0x00001, 0x80000, CRC(9d38ac75) SHA1(86ff581cd3546f6b1be75e1d0744a8d767b22f5a) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END


ROM_START( mkprot9 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "mkprot9.105",  0x00000, 0x80000, CRC(20772bbd) SHA1(d5b400700b91c7a70bd2441c5254300cf1f743d7) )
	ROM_LOAD16_BYTE(  "mkprot9.89",  0x00001, 0x80000, CRC(3238d45b) SHA1(8a4e827994d0d20feda3785a5f8f0f77b737052b) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END


ROM_START( mkprot8 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "mkprot8.105",  0x00000, 0x80000, CRC(2f3c095d) SHA1(f6e9ac0fc0f997f4b323ba48590b042eae079a16) )
	ROM_LOAD16_BYTE(  "mkprot8.89",  0x00001, 0x80000, CRC(edcf217f) SHA1(29e17bd20844a3e666e794c2fc068a011ccff2e8) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END

ROM_START( mkprot4 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "mkprot4.105",  0x00000, 0x80000, CRC(d7f8d78b) SHA1(736f16d8c0407ee6dc8d3e40df08d1c926147a16) )
	ROM_LOAD16_BYTE(  "mkprot4.89",  0x00001, 0x80000, CRC(a6b5d6d2) SHA1(917dbcff6d601d3fb015c8e26c6f0768290cd64a) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END

ROM_START( mkyturbo )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	/* A 'NIBBLE BOARD' daughtercard holding a GAL16V8A-2SP, 27C040 EPROM and a 9.8304MHz XTAL plugs into the U89 socket */
	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "kombo-rom-u105.bin", 0x00000, 0x80000, CRC(80d5618c) SHA1(9bdfddbc70b61c94c1871abac1de153b8b728761) )
	ROM_LOAD16_BYTE(  "kombo-rom-u89.bin", 0x00001, 0x80000, CRC(450788e3) SHA1(34e4fa9c2ede66799301c3d1755df25edc432539) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END

ROM_START( mkyturboe )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	/* A 'NIBBLE BOARD' daughtercard holding a GAL16V8A-2SP, 27C040 EPROM and a 9.8304MHz XTAL plugs into the U89 socket */
	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "turbo30.u105", 0x00000, 0x80000, CRC(59747c59) SHA1(69e1450a6b2b41b8939ce84903cb35c1906b81e2) )
	ROM_LOAD16_BYTE(  "turbo30.u89", 0x00001, 0x80000, CRC(84d66a75) SHA1(11ee7ae7fc1c13cafa8312f101878393ae6fd8b7) )
		ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END

ROM_START( mknifty )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "nifty.105", 0x00000, 0x80000, CRC(c66fd38d) SHA1(92e99f7c46422e47f503057398385168f63814cc) )
	ROM_LOAD16_BYTE(  "nifty.89", 0x00001, 0x80000, CRC(bbf8738d) SHA1(38acaf7c29e59b5c3ba32d5cb950d0fe8852ff51) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END

ROM_START( mknifty666 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "mks-u3.rom", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "mks-u12.rom", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(               0x40000, 0x40000 )
	ROM_LOAD ( "mks-u13.rom", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(               0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "mortall_kombo_rom_u105-j4.u105.bin", 0x00000, 0x80000, CRC(243d8009) SHA1(e275f93d2d4b3a454303ce106641707a98bae084) )
	ROM_LOAD16_BYTE( "kombo-u89.u89", 0x00001, 0x80000, CRC(7b26a6b1) SHA1(378bd54fcc5c801ad8cc10ed94157a1e60572199))

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END

ROM_START( mkyawdim )
	ROM_REGION( 0x10000, "audiocpu", 0 )    /* sound CPU */
	ROM_LOAD (  "1.u67", 0x00000, 0x10000, CRC(b58d229e) SHA1(3ed14ef650dfa7f9d460611b19e9233a022cbea6) )

	ROM_REGION( 0x100000, "oki", 0 )    /* ADPCM */
	ROM_LOAD( "2.u59",  0x00000, 0x20000, CRC(a72ad81e) SHA1(7be4285b28755bd48acce670f34d6a7f043dda96) )
	ROM_CONTINUE(       0x40000, 0x20000 )
	ROM_CONTINUE(       0x80000, 0x20000 )
	ROM_CONTINUE(       0xc0000, 0x20000 )
	ROM_LOAD( "3.u60",  0x20000, 0x20000, CRC(6e68e0b0) SHA1(edb7aa6507452ffa5ce7097e3b1855a69542971c) )
	ROM_CONTINUE(       0x60000, 0x20000 )
	ROM_CONTINUE(       0xa0000, 0x20000 )
	ROM_CONTINUE(       0xe0000, 0x20000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "4.u25",  0x00000, 0x80000, CRC(b12b3bf2) SHA1(deb7755e8407d9de25124b3fdbc4c834a25d8252) )
	ROM_LOAD16_BYTE( "5.u26",  0x00001, 0x80000, CRC(7a37dc5c) SHA1(c4fc6933d8b990c5c56c65282b1f72b90b5d5435) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "mkg-u111.rom",  0x000000, 0x80000, CRC(d17096c4) SHA1(01ef390a372c9d94adf138f9543ebb88b89f4c38) )
	ROM_LOAD ( "mkg-u112.rom",  0x080000, 0x80000, CRC(993bc2e4) SHA1(7791edbec2b4b8971a3e790346dd7564ecf16d5c) )
	ROM_LOAD ( "mkg-u113.rom",  0x100000, 0x80000, CRC(6fb91ede) SHA1(a3735b49f93b08c44fbc97e2b5aad394628fbe90) )
	ROM_LOAD ( "mkg-u114.rom",  0x180000, 0x80000, CRC(ed1ff88a) SHA1(6b090b658ee6148af953bd0c9216f37162b6460f) )

	ROM_LOAD (  "mkg-u95.rom",  0x200000, 0x80000, CRC(a002a155) SHA1(3cf7909e92bcd428063596fc5b9953e0000d6eca) )
	ROM_LOAD (  "mkg-u96.rom",  0x280000, 0x80000, CRC(dcee8492) SHA1(a912b74d3b26ebd1b1613cc631080f83ececeaf8) )
	ROM_LOAD (  "mkg-u97.rom",  0x300000, 0x80000, CRC(de88caef) SHA1(a7927b504dc56ca5c9048373977fe5743b0a3f0b) )
	ROM_LOAD (  "mkg-u98.rom",  0x380000, 0x80000, CRC(37eb01b4) SHA1(06092460bd137e08d0f8df8560942ed877d40e09) )

	ROM_LOAD ( "mkg-u106.rom",  0x400000, 0x80000, CRC(45acaf21) SHA1(5edd36c55f4e5d3c74fb85171728ec0a58284b12) )
	ROM_LOAD ( "mkg-u107.rom",  0x480000, 0x80000, CRC(2a6c10a0) SHA1(cc90923c44f2961b945a0fd0f85ecc2ba04af2cb) )
	ROM_LOAD ( "mkg-u108.rom",  0x500000, 0x80000, CRC(23308979) SHA1(0b36788624a1cf0d3f4c895be5ba967b8dfcf85e) )
	ROM_LOAD ( "mkg-u109.rom",  0x580000, 0x80000, CRC(cafc47bb) SHA1(8610af6e52f7089ff4acd850c53ab8b4119e4445) )
ROM_END

ROM_START( mkyawdim2 )
	ROM_REGION( 0x10000, "audiocpu", 0 )    /* sound CPU */
		// Differs from other yawdim set - sound doesn't want to work
	ROM_LOAD ( "yawdim.u167", 0x00000, 0x10000, CRC(16da7efb) SHA1(ac1db81a55aca36136b94977a91a1fc778b7b164) )

	ROM_REGION( 0x100000, "oki", 0 )    /* ADPCM */
		// Half size as other yawdim set
	ROM_LOAD( "yawdim.u159",  0x00000, 0x20000, CRC(95b120af) SHA1(41b6fb384e5048926b87959a2c58d96b95698aba) )
	ROM_CONTINUE(       0x40000, 0x20000 )
	ROM_LOAD( "mw-15.u160",  0x20000, 0x20000, CRC(6e68e0b0) SHA1(edb7aa6507452ffa5ce7097e3b1855a69542971c) )
	ROM_CONTINUE(       0x60000, 0x20000 )
	ROM_CONTINUE(       0xa0000, 0x20000 )
	ROM_CONTINUE(       0xe0000, 0x20000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "4.u25",  0x00000, 0x80000, CRC(b12b3bf2) SHA1(deb7755e8407d9de25124b3fdbc4c834a25d8252) )
	ROM_LOAD16_BYTE( "5.u26",  0x00001, 0x80000, CRC(7a37dc5c) SHA1(c4fc6933d8b990c5c56c65282b1f72b90b5d5435) )

	ROM_REGION( 0x800000, "gfx1", 0 ) /* 8mbit dumps */
	ROM_LOAD ( "b-1.bin",  0x000000, 0x100000, CRC(f41e61c6) SHA1(7dad38839d5c9aa0cfa7b2f7199f14e0f2c4494b) )
	ROM_LOAD ( "b-2.bin",  0x100000, 0x100000, CRC(8052740b) SHA1(f1b7fd536966d9d0ce690cdec635069c340d678e) )

	ROM_LOAD ( "a-1.bin",  0x200000, 0x100000, CRC(7da3cb93) SHA1(23b9053b3241b69988f7f2e6a9d1353dac4fc8ab) )
	ROM_LOAD ( "a-2.bin",  0x300000, 0x100000, CRC(1eedb0f8) SHA1(27c056c469c17bb176325b91cf92296c89681ac6) )

	ROM_LOAD ( "c-1.bin",  0x400000, 0x100000, CRC(de27c4c3) SHA1(a7760d239749c7463808adec72795f9785f553ec) )
	ROM_LOAD ( "c-2.bin",  0x500000, 0x100000, CRC(d99203f3) SHA1(46ea21cbedfd42838562594b9bdc5d80360b7e5e) )
ROM_END


ROM_START( term2 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "t2_snd.3", 0x10000, 0x20000, CRC(73c3f5c4) SHA1(978dd974590e77294dbe9a647aebd3d24af6397f) )
	ROM_RELOAD (            0x30000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "t2_snd.12", 0x00000, 0x40000, CRC(e192a40d) SHA1(1f7a0e282c0c8eb66cbe514128bd104433e53b7a) )
	ROM_RELOAD(             0x40000, 0x40000 )
	ROM_LOAD ( "t2_snd.13", 0x80000, 0x40000, CRC(956fa80b) SHA1(02ab504627f4b25a394fa4192bb134138cbf6a4f) )
	ROM_RELOAD(             0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "t2-la4.105",  0x00000, 0x80000, CRC(d4d8d884) SHA1(3209e131b128f12af30b3c6056fd63df497f93eb) )
	ROM_LOAD16_BYTE( "t2-la4.89",   0x00001, 0x80000, CRC(25359415) SHA1(ca8b7e1b5a363b78499f92c979a11ace6f1dceab) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "t2.111",  0x000000, 0x80000, CRC(916d0197) SHA1(3b53d3770955b10cc6002e3d3bf0f71429667af0) )
	ROM_LOAD ( "t2.112",  0x080000, 0x80000, CRC(39ae1c87) SHA1(a9d332dffc77c1e0dc50549825f5b403cf19c41d) )
	ROM_LOAD ( "t2.113",  0x100000, 0x80000, CRC(cb5084e5) SHA1(58cab00d8ebc72792f6c29899013ae6a0d2278b9) )
	ROM_LOAD ( "t2.114",  0x180000, 0x80000, CRC(53c516ec) SHA1(2a33639bc5bb4e7f7b3e341ddb59173260461d20) )

	ROM_LOAD (  "t2.95",  0x200000, 0x80000, CRC(dd39cf73) SHA1(53bb54e66e4dfbe58385915004b0ad57583a7543) )
	ROM_LOAD (  "t2.96",  0x280000, 0x80000, CRC(31f4fd36) SHA1(766fca7d5e5043fc9e68bd3dc15e6a7830279d88) )
	ROM_LOAD (  "t2.97",  0x300000, 0x80000, CRC(7f72e775) SHA1(7e2369c6b4ed5d653700b041df58355a0960193a) )
	ROM_LOAD (  "t2.98",  0x380000, 0x80000, CRC(1a20ce29) SHA1(9089b7f77da5d67ad46ed249d72de8b8e0e5d807) )

	ROM_LOAD ( "t2.106",  0x400000, 0x80000, CRC(f08a9536) SHA1(fbac314bd52f23c7a704acd3c707e75cdf204c07) )
	ROM_LOAD ( "t2.107",  0x480000, 0x80000, CRC(268d4035) SHA1(89a310830be9fbc91794fcccc57053d0933c42a3) )
	ROM_LOAD ( "t2.108",  0x500000, 0x80000, CRC(379fdaed) SHA1(408df6702c8ea8d3dce0b231955c6a60f3f5f22b) )
	ROM_LOAD ( "t2.109",  0x580000, 0x80000, CRC(306a9366) SHA1(b94c23c033221f7f7fddd2911b8cec9549929768) )
ROM_END


ROM_START( term2la3 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "t2_snd.3", 0x10000, 0x20000, CRC(73c3f5c4) SHA1(978dd974590e77294dbe9a647aebd3d24af6397f) )
	ROM_RELOAD (            0x30000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "t2_snd.12", 0x00000, 0x40000, CRC(e192a40d) SHA1(1f7a0e282c0c8eb66cbe514128bd104433e53b7a) )
	ROM_RELOAD(             0x40000, 0x40000 )
	ROM_LOAD ( "t2_snd.13", 0x80000, 0x40000, CRC(956fa80b) SHA1(02ab504627f4b25a394fa4192bb134138cbf6a4f) )
	ROM_RELOAD(             0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "t2-la3.105",  0x00000, 0x80000, CRC(34142b28) SHA1(985fd169b3d62c4197fe4c6f11055a6c17872899) )
	ROM_LOAD16_BYTE( "t2-la3.89",   0x00001, 0x80000, CRC(5ffea427) SHA1(c6f65bc57b33ae1a123f610c635e0d65663e54da) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "t2.111",  0x000000, 0x80000, CRC(916d0197) SHA1(3b53d3770955b10cc6002e3d3bf0f71429667af0) )
	ROM_LOAD ( "t2.112",  0x080000, 0x80000, CRC(39ae1c87) SHA1(a9d332dffc77c1e0dc50549825f5b403cf19c41d) )
	ROM_LOAD ( "t2.113",  0x100000, 0x80000, CRC(cb5084e5) SHA1(58cab00d8ebc72792f6c29899013ae6a0d2278b9) )
	ROM_LOAD ( "t2.114",  0x180000, 0x80000, CRC(53c516ec) SHA1(2a33639bc5bb4e7f7b3e341ddb59173260461d20) )

	ROM_LOAD (  "t2.95",  0x200000, 0x80000, CRC(dd39cf73) SHA1(53bb54e66e4dfbe58385915004b0ad57583a7543) )
	ROM_LOAD (  "t2.96",  0x280000, 0x80000, CRC(31f4fd36) SHA1(766fca7d5e5043fc9e68bd3dc15e6a7830279d88) )
	ROM_LOAD (  "t2.97",  0x300000, 0x80000, CRC(7f72e775) SHA1(7e2369c6b4ed5d653700b041df58355a0960193a) )
	ROM_LOAD (  "t2.98",  0x380000, 0x80000, CRC(1a20ce29) SHA1(9089b7f77da5d67ad46ed249d72de8b8e0e5d807) )

	ROM_LOAD ( "t2.106",  0x400000, 0x80000, CRC(f08a9536) SHA1(fbac314bd52f23c7a704acd3c707e75cdf204c07) )
	ROM_LOAD ( "t2.107",  0x480000, 0x80000, CRC(268d4035) SHA1(89a310830be9fbc91794fcccc57053d0933c42a3) )
	ROM_LOAD ( "t2.108",  0x500000, 0x80000, CRC(379fdaed) SHA1(408df6702c8ea8d3dce0b231955c6a60f3f5f22b) )
	ROM_LOAD ( "t2.109",  0x580000, 0x80000, CRC(306a9366) SHA1(b94c23c033221f7f7fddd2911b8cec9549929768) )
ROM_END


ROM_START( term2la2 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "t2_snd.3", 0x10000, 0x20000, CRC(73c3f5c4) SHA1(978dd974590e77294dbe9a647aebd3d24af6397f) )
	ROM_RELOAD (            0x30000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "t2_snd.12", 0x00000, 0x40000, CRC(e192a40d) SHA1(1f7a0e282c0c8eb66cbe514128bd104433e53b7a) )
	ROM_RELOAD(             0x40000, 0x40000 )
	ROM_LOAD ( "t2_snd.13", 0x80000, 0x40000, CRC(956fa80b) SHA1(02ab504627f4b25a394fa4192bb134138cbf6a4f) )
	ROM_RELOAD(             0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "t2-la2.105",  0x00000, 0x80000, CRC(7177de98) SHA1(0987be413d6cb5ded7059ad6ebbca49331b046b2) )
	ROM_LOAD16_BYTE( "t2-la2.89",   0x00001, 0x80000, CRC(14d7b9f5) SHA1(b8676d21d53fd3c8492d8911e749d74df1c66b1d) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "t2.111",  0x000000, 0x80000, CRC(916d0197) SHA1(3b53d3770955b10cc6002e3d3bf0f71429667af0) )
	ROM_LOAD ( "t2.112",  0x080000, 0x80000, CRC(39ae1c87) SHA1(a9d332dffc77c1e0dc50549825f5b403cf19c41d) )
	ROM_LOAD ( "t2.113",  0x100000, 0x80000, CRC(cb5084e5) SHA1(58cab00d8ebc72792f6c29899013ae6a0d2278b9) )
	ROM_LOAD ( "t2.114",  0x180000, 0x80000, CRC(53c516ec) SHA1(2a33639bc5bb4e7f7b3e341ddb59173260461d20) )

	ROM_LOAD (  "t2.95",  0x200000, 0x80000, CRC(dd39cf73) SHA1(53bb54e66e4dfbe58385915004b0ad57583a7543) )
	ROM_LOAD (  "t2.96",  0x280000, 0x80000, CRC(31f4fd36) SHA1(766fca7d5e5043fc9e68bd3dc15e6a7830279d88) )
	ROM_LOAD (  "t2.97",  0x300000, 0x80000, CRC(7f72e775) SHA1(7e2369c6b4ed5d653700b041df58355a0960193a) )
	ROM_LOAD (  "t2.98",  0x380000, 0x80000, CRC(1a20ce29) SHA1(9089b7f77da5d67ad46ed249d72de8b8e0e5d807) )

	ROM_LOAD ( "t2.106",  0x400000, 0x80000, CRC(f08a9536) SHA1(fbac314bd52f23c7a704acd3c707e75cdf204c07) )
	ROM_LOAD ( "t2.107",  0x480000, 0x80000, CRC(268d4035) SHA1(89a310830be9fbc91794fcccc57053d0933c42a3) )
	ROM_LOAD ( "t2.108",  0x500000, 0x80000, CRC(379fdaed) SHA1(408df6702c8ea8d3dce0b231955c6a60f3f5f22b) )
	ROM_LOAD ( "t2.109",  0x580000, 0x80000, CRC(306a9366) SHA1(b94c23c033221f7f7fddd2911b8cec9549929768) )
ROM_END


ROM_START( term2la1 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) /* sound CPU */
	ROM_LOAD ( "t2_snd.3", 0x10000, 0x20000, CRC(73c3f5c4) SHA1(978dd974590e77294dbe9a647aebd3d24af6397f) )
	ROM_RELOAD ( 0x30000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 ) /* ADPCM */
	ROM_LOAD ( "t2_snd.12", 0x00000, 0x40000, CRC(e192a40d) SHA1(1f7a0e282c0c8eb66cbe514128bd104433e53b7a) )
	ROM_RELOAD(             0x40000, 0x40000 )
	ROM_LOAD ( "t2_snd.13", 0x80000, 0x40000, CRC(956fa80b) SHA1(02ab504627f4b25a394fa4192bb134138cbf6a4f) )
	ROM_RELOAD(             0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "t2-la1.105", 0x00000, 0x80000, CRC(ca52a8b0) SHA1(20b91bdd9fe8e7be6a3c3cb9684769733d66d401) )
	ROM_LOAD16_BYTE( "t2-la1.89",  0x00001, 0x80000, CRC(08535210) SHA1(a7986541bc504294bd6523ce691e19e496f8be7c) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "t2.111", 0x000000, 0x80000, CRC(916d0197) SHA1(3b53d3770955b10cc6002e3d3bf0f71429667af0) )
	ROM_LOAD ( "t2.112", 0x080000, 0x80000, CRC(39ae1c87) SHA1(a9d332dffc77c1e0dc50549825f5b403cf19c41d) )
	ROM_LOAD ( "t2.113", 0x100000, 0x80000, CRC(cb5084e5) SHA1(58cab00d8ebc72792f6c29899013ae6a0d2278b9) )
	ROM_LOAD ( "t2.114", 0x180000, 0x80000, CRC(53c516ec) SHA1(2a33639bc5bb4e7f7b3e341ddb59173260461d20) )

	ROM_LOAD ( "t2.95", 0x200000, 0x80000, CRC(dd39cf73) SHA1(53bb54e66e4dfbe58385915004b0ad57583a7543) )
	ROM_LOAD ( "t2.96", 0x280000, 0x80000, CRC(31f4fd36) SHA1(766fca7d5e5043fc9e68bd3dc15e6a7830279d88) )
	ROM_LOAD ( "t2.97", 0x300000, 0x80000, CRC(7f72e775) SHA1(7e2369c6b4ed5d653700b041df58355a0960193a) )
	ROM_LOAD ( "t2.98", 0x380000, 0x80000, CRC(1a20ce29) SHA1(9089b7f77da5d67ad46ed249d72de8b8e0e5d807) )

	ROM_LOAD ( "t2.106", 0x400000, 0x80000, CRC(f08a9536) SHA1(fbac314bd52f23c7a704acd3c707e75cdf204c07) )
	ROM_LOAD ( "t2.107", 0x480000, 0x80000, CRC(268d4035) SHA1(89a310830be9fbc91794fcccc57053d0933c42a3) )
	ROM_LOAD ( "t2.108", 0x500000, 0x80000, CRC(379fdaed) SHA1(408df6702c8ea8d3dce0b231955c6a60f3f5f22b) )
	ROM_LOAD ( "t2.109", 0x580000, 0x80000, CRC(306a9366) SHA1(b94c23c033221f7f7fddd2911b8cec9549929768) )
ROM_END


ROM_START( totcarn )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "tcu3.bin", 0x10000, 0x20000, CRC(5bdb4665) SHA1(c6b90b914785b8703790957cc4bb4983a332fba6) )
	ROM_RELOAD (            0x30000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "tcu12.bin", 0x00000, 0x40000, CRC(d0000ac7) SHA1(2d476c7727462623feb2f1a23fb797eaeed5ce30) )
	ROM_RELOAD(             0x40000, 0x40000 )
	ROM_LOAD ( "tcu13.bin", 0x80000, 0x40000, CRC(e48e6f0c) SHA1(bf7d548b6b1901966f99c815129ea160ef36f024) )
	ROM_RELOAD(             0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "tcu105.bin",  0x80000, 0x40000, CRC(7c651047) SHA1(530c8b4e453778a81479d02913ffe7097903447f) )
	ROM_LOAD16_BYTE( "tcu89.bin",   0x80001, 0x40000, CRC(6761daf3) SHA1(8be881ecc5ea1121bb6cee1a34901a4d5e50dbb6) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "tcu111.bin",  0x000000, 0x40000, CRC(13f3f231) SHA1(6df0dca72e170818c260d9931477103a38864a1e) )
	ROM_LOAD ( "tcu112.bin",  0x040000, 0x40000, CRC(72e45007) SHA1(b6f5dfb844b6ff46a3594d20e85f1f20bdbfb793) )
	ROM_LOAD ( "tcu113.bin",  0x080000, 0x40000, CRC(2c8ec753) SHA1(9393179ea19cbec7ac7e4f8e912bb4f86d93e8bd) )
	ROM_LOAD ( "tcu114.bin",  0x0c0000, 0x40000, CRC(6210c36c) SHA1(607acdf024c1d36238ed19841c3ef2c96f49038f) )

	ROM_LOAD (  "tcu95.bin",  0x200000, 0x40000, CRC(579caeba) SHA1(de7d9921a210839e1db4bf54fb96833bcb073862) )
	ROM_LOAD (  "tcu96.bin",  0x240000, 0x40000, CRC(f43f1ffe) SHA1(60401092be1fed52a028dc81b7a28ade923c35ea) )
	ROM_LOAD (  "tcu97.bin",  0x280000, 0x40000, CRC(1675e50d) SHA1(1479712b03fa2b67fcd2d4694f26ce1bd1959b97) )
	ROM_LOAD (  "tcu98.bin",  0x2c0000, 0x40000, CRC(ab06c885) SHA1(09163060269fed2ebd697b71602166e906c95317) )

	ROM_LOAD ( "tcu106.bin",  0x400000, 0x40000, CRC(146e3863) SHA1(1933e62a060eb667889b1edd5002c30a37ae00a7) )
	ROM_LOAD ( "tcu107.bin",  0x440000, 0x40000, CRC(95323320) SHA1(5296206f3d84c21374968ffcacfe59eb3215ca46) )
	ROM_LOAD ( "tcu108.bin",  0x480000, 0x40000, CRC(ed152acc) SHA1(372dbc4fdb581ac00a7eb5669cc1ac7afd6033f8) )
	ROM_LOAD ( "tcu109.bin",  0x4c0000, 0x40000, CRC(80715252) SHA1(4586a259780963837ce362b526f161122d2e3cb4) )
ROM_END


ROM_START( totcarnp )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   /* sound CPU */
	ROM_LOAD (  "tcu3.bin", 0x10000, 0x20000, CRC(5bdb4665) SHA1(c6b90b914785b8703790957cc4bb4983a332fba6) )
	ROM_RELOAD (            0x30000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD ( "tcu12.bin", 0x00000, 0x40000, CRC(d0000ac7) SHA1(2d476c7727462623feb2f1a23fb797eaeed5ce30) )
	ROM_RELOAD(             0x40000, 0x40000 )
	ROM_LOAD ( "tcu13.bin", 0x80000, 0x40000, CRC(e48e6f0c) SHA1(bf7d548b6b1901966f99c815129ea160ef36f024) )
	ROM_RELOAD(             0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "user1", 0 ) /* 34010 code */
	ROM_LOAD16_BYTE( "u105",  0x80000, 0x40000, CRC(7a782cae) SHA1(806894e23876325fffcad4d707c850fbd91d973a) )
	ROM_LOAD16_BYTE( "u89",   0x80001, 0x40000, CRC(1c899a8d) SHA1(953d4def814f036969b9ecf3be16e145c2d2bf9f) )

	ROM_REGION( 0x800000, "gfx1", 0 )
	ROM_LOAD ( "tcu111.bin",  0x000000, 0x40000, CRC(13f3f231) SHA1(6df0dca72e170818c260d9931477103a38864a1e) )
	ROM_LOAD ( "tcu112.bin",  0x040000, 0x40000, CRC(72e45007) SHA1(b6f5dfb844b6ff46a3594d20e85f1f20bdbfb793) )
	ROM_LOAD ( "tcu113.bin",  0x080000, 0x40000, CRC(2c8ec753) SHA1(9393179ea19cbec7ac7e4f8e912bb4f86d93e8bd) )
	ROM_LOAD ( "tcu114.bin",  0x0c0000, 0x40000, CRC(6210c36c) SHA1(607acdf024c1d36238ed19841c3ef2c96f49038f) )

	ROM_LOAD (  "tcu95.bin",  0x200000, 0x40000, CRC(579caeba) SHA1(de7d9921a210839e1db4bf54fb96833bcb073862) )
	ROM_LOAD (  "tcu96.bin",  0x240000, 0x40000, CRC(f43f1ffe) SHA1(60401092be1fed52a028dc81b7a28ade923c35ea) )
	ROM_LOAD (  "tcu97.bin",  0x280000, 0x40000, CRC(1675e50d) SHA1(1479712b03fa2b67fcd2d4694f26ce1bd1959b97) )
	ROM_LOAD (  "tcu98.bin",  0x2c0000, 0x40000, CRC(ab06c885) SHA1(09163060269fed2ebd697b71602166e906c95317) )

	ROM_LOAD ( "tcu106.bin",  0x400000, 0x40000, CRC(146e3863) SHA1(1933e62a060eb667889b1edd5002c30a37ae00a7) )
	ROM_LOAD ( "tcu107.bin",  0x440000, 0x40000, CRC(95323320) SHA1(5296206f3d84c21374968ffcacfe59eb3215ca46) )
	ROM_LOAD ( "tcu108.bin",  0x480000, 0x40000, CRC(ed152acc) SHA1(372dbc4fdb581ac00a7eb5669cc1ac7afd6033f8) )
	ROM_LOAD ( "tcu109.bin",  0x4c0000, 0x40000, CRC(80715252) SHA1(4586a259780963837ce362b526f161122d2e3cb4) )
ROM_END



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1988, narc,     0,        zunit,                 narc, midyunit_state,     narc,     ROT0, "Williams", "Narc (rev 7.00)", GAME_SUPPORTS_SAVE )
GAME( 1988, narc3,    narc,     zunit,                 narc, midyunit_state,     narc,     ROT0, "Williams", "Narc (rev 3.20)", GAME_SUPPORTS_SAVE )
GAME( 1988, narc2,    narc,     zunit,                 narc, midyunit_state,     narc,     ROT0, "Williams", "Narc (rev 2.00)", GAME_SUPPORTS_SAVE )

GAME( 1990, trog,     0,        yunit_cvsd_4bit_slow,  trog, midyunit_state,     trog,     ROT0, "Midway",   "Trog (rev LA5 03/29/91)", GAME_SUPPORTS_SAVE )
GAME( 1990, trog4,    trog,     yunit_cvsd_4bit_slow,  trog, midyunit_state,     trog,     ROT0, "Midway",   "Trog (rev LA4 03/11/91)", GAME_SUPPORTS_SAVE )
GAME( 1990, trog3,    trog,     yunit_cvsd_4bit_slow,  trog, midyunit_state,     trog,     ROT0, "Midway",   "Trog (rev LA3 02/14/91)", GAME_SUPPORTS_SAVE )
GAME( 1990, trogpa6,  trog,     yunit_cvsd_4bit_slow,  trog, midyunit_state,     trog,     ROT0, "Midway",   "Trog (prototype, rev PA6-PAC 09/09/90)", GAME_SUPPORTS_SAVE )
GAME( 1990, trogpa4,  trog,     yunit_cvsd_4bit_slow,  trog, midyunit_state,     trog,     ROT0, "Midway",   "Trog (prototype, rev 4.00 07/27/90)", GAME_SUPPORTS_SAVE )

GAME( 1990, smashtv,  0,        yunit_cvsd_6bit_slow,  smashtv, midyunit_state,  smashtv,  ROT0, "Williams", "Smash T.V. (rev 8.00)", GAME_SUPPORTS_SAVE )
GAME( 1990, smashtv6, smashtv,  yunit_cvsd_6bit_slow,  smashtv, midyunit_state,  smashtv,  ROT0, "Williams", "Smash T.V. (rev 6.00)", GAME_SUPPORTS_SAVE )
GAME( 1990, smashtv5, smashtv,  yunit_cvsd_6bit_slow,  smashtv, midyunit_state,  smashtv,  ROT0, "Williams", "Smash T.V. (rev 5.00)", GAME_SUPPORTS_SAVE )
GAME( 1990, smashtv4, smashtv,  yunit_cvsd_6bit_slow,  smashtv, midyunit_state,  smashtv,  ROT0, "Williams", "Smash T.V. (rev 4.00)", GAME_SUPPORTS_SAVE )
GAME( 1990, smashtv3, smashtv,  yunit_cvsd_6bit_slow,  smashtv, midyunit_state,  smashtv,  ROT0, "Williams", "Smash T.V. (rev 3.01)", GAME_SUPPORTS_SAVE )

GAME( 1990, hiimpact,   0,        yunit_cvsd_6bit_slow,  trog, midyunit_state,     hiimpact, ROT0, "Williams", "High Impact Football (rev LA5 02/15/91)", GAME_SUPPORTS_SAVE )
GAME( 1990, hiimpact4,  hiimpact, yunit_cvsd_6bit_slow,  trog, midyunit_state,     hiimpact, ROT0, "Williams", "High Impact Football (rev LA4 02/04/91)", GAME_SUPPORTS_SAVE )
GAME( 1990, hiimpact3,  hiimpact, yunit_cvsd_6bit_slow,  trog, midyunit_state,     hiimpact, ROT0, "Williams", "High Impact Football (rev LA3 12/27/90)", GAME_SUPPORTS_SAVE )
GAME( 1990, hiimpact2,  hiimpact, yunit_cvsd_6bit_slow,  trog, midyunit_state,     hiimpact, ROT0, "Williams", "High Impact Football (rev LA2 12/26/90)", GAME_SUPPORTS_SAVE )
GAME( 1990, hiimpact1,  hiimpact, yunit_cvsd_6bit_slow,  trog, midyunit_state,     hiimpact, ROT0, "Williams", "High Impact Football (rev LA1 12/16/90)", GAME_SUPPORTS_SAVE )
GAME( 1990, hiimpactp,  hiimpact, yunit_cvsd_6bit_slow,  trog, midyunit_state,     hiimpact, ROT0, "Williams", "High Impact Football (prototype, rev 8.6 12/09/90)", GAME_SUPPORTS_SAVE )

GAME( 1991, shimpact,   0,        yunit_cvsd_6bit_slow,  trog, midyunit_state,     shimpact, ROT0, "Midway",   "Super High Impact (rev LA1 09/30/91)", GAME_SUPPORTS_SAVE )
GAME( 1991, shimpactp6, shimpact, yunit_cvsd_6bit_slow,  trog, midyunit_state,     shimpact, ROT0, "Midway",   "Super High Impact (prototype, rev 6.0 09/23/91)", GAME_SUPPORTS_SAVE )
GAME( 1991, shimpactp5, shimpact, yunit_cvsd_6bit_slow,  trog, midyunit_state,     shimpact, ROT0, "Midway",   "Super High Impact (prototype, rev 5.0 09/15/91)", GAME_SUPPORTS_SAVE )
GAME( 1991, shimpactp4, shimpact, yunit_cvsd_6bit_slow,  trog, midyunit_state,     shimpact, ROT0, "Midway",   "Super High Impact (prototype, rev 4.0 09/10/91)", GAME_SUPPORTS_SAVE ) /* See notes about factory restore above */

GAME( 1991, strkforc, 0,        yunit_cvsd_4bit_fast,  strkforc, midyunit_state, strkforc, ROT0, "Midway",   "Strike Force (rev 1 02/25/91)", GAME_SUPPORTS_SAVE )

GAME( 1991, term2,    0,        yunit_adpcm_6bit_faster, term2, midyunit_state,    term2,    ORIENTATION_FLIP_X, "Midway",   "Terminator 2 - Judgment Day (rev LA4 08/03/92)", GAME_SUPPORTS_SAVE )
GAME( 1991, term2la3, term2,    yunit_adpcm_6bit_faster, term2, midyunit_state,    term2la3, ORIENTATION_FLIP_X, "Midway",   "Terminator 2 - Judgment Day (rev LA3 03/27/92)", GAME_SUPPORTS_SAVE )
GAME( 1991, term2la2, term2,    yunit_adpcm_6bit_faster, term2, midyunit_state,    term2la2, ORIENTATION_FLIP_X, "Midway",   "Terminator 2 - Judgment Day (rev LA2 12/09/91)", GAME_SUPPORTS_SAVE )
GAME( 1991, term2la1, term2,    yunit_adpcm_6bit_faster, term2, midyunit_state,    term2la1, ORIENTATION_FLIP_X, "Midway",   "Terminator 2 - Judgment Day (rev LA1 11/01/91)", GAME_SUPPORTS_SAVE )

GAME( 1992, mkla4,    mk,       yunit_adpcm_6bit_fast,   mkla4, midyunit_state,    mkyunit,  ROT0, "Midway",   "Mortal Kombat (rev 4.0 09/28/92)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkla3,    mk,       yunit_adpcm_6bit_fast,   mkla4, midyunit_state,    mkyunit,  ROT0, "Midway",   "Mortal Kombat (rev 3.0 08/31/92)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkla2,    mk,       yunit_adpcm_6bit_fast,   mkla2, midyunit_state,    mkyunit,  ROT0, "Midway",   "Mortal Kombat (rev 2.0 08/18/92)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkla1,    mk,       yunit_adpcm_6bit_fast,   mkla2, midyunit_state,    mkyunit,  ROT0, "Midway",   "Mortal Kombat (rev 1.0 08/09/92)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkprot9,  mk,       yunit_adpcm_6bit_faster, mkla2, midyunit_state,    mkyunit,  ROT0, "Midway",   "Mortal Kombat (prototype, rev 9.0 07/28/92)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkprot8,  mk,       yunit_adpcm_6bit_faster, mkla2, midyunit_state,    mkyunit,  ROT0, "Midway",   "Mortal Kombat (prototype, rev 8.0 07/21/92)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkprot4,  mk,       yunit_adpcm_6bit_faster, mkla2, midyunit_state,    mkyunit,  ROT0, "Midway",   "Mortal Kombat (prototype, rev 4.0 07/14/92)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkyturbo, mk,       yunit_adpcm_6bit_fast,   mkla4, midyunit_state,    mkyturbo, ROT0, "hack",     "Mortal Kombat (Turbo 3.1 09/09/93, hack)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkyturboe,mk,       yunit_adpcm_6bit_fast,   mkla4, midyunit_state,    mkyturbo, ROT0, "hack",     "Mortal Kombat (Turbo 3.0 08/31/92, hack)", GAME_SUPPORTS_SAVE )
GAME( 1992, mknifty,  mk,       yunit_adpcm_6bit_fast,   mkla4, midyunit_state,    mkyturbo, ROT0, "hack",     "Mortal Kombat (Nifty Kombo, hack)", GAME_SUPPORTS_SAVE )
GAME( 1992, mknifty666, mk,     yunit_adpcm_6bit_fast,   mkla4, midyunit_state,    mkyturbo, ROT0, "hack",     "Mortal Kombat (Nifty Kombo 666, hack)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkyawdim, mk,       mkyawdim,                mkyawdim, midyunit_state, mkyawdim, ROT0, "bootleg (Yawdim)", "Mortal Kombat (Yawdim bootleg, set 1)", GAME_SUPPORTS_SAVE )
GAME( 1992, mkyawdim2,mk,       mkyawdim,                mkyawdim, midyunit_state, mkyawdim, ROT0, "bootleg (Yawdim)", "Mortal Kombat (Yawdim bootleg, set 2)", GAME_SUPPORTS_SAVE | GAME_NO_SOUND )

GAME( 1992, totcarn,  0,        yunit_adpcm_6bit_fast,   totcarn, midyunit_state,  totcarn,  ROT0, "Midway",   "Total Carnage (rev LA1 03/10/92)", GAME_SUPPORTS_SAVE )
GAME( 1992, totcarnp, totcarn,  yunit_adpcm_6bit_fast,   totcarn, midyunit_state,  totcarn,  ROT0, "Midway",   "Total Carnage (prototype, rev 1.0 01/25/92)", GAME_SUPPORTS_SAVE )
