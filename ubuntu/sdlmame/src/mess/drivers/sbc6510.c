/***************************************************************************

        SBC6510 from Josip Perusanec

        18/12/2009 Skeleton driver.
        2012-08-12 Working [Robbbert]

    CPU MOS 6510 (1MHz)
    ROM 4KB
    RAM 128KB
    CIA 6526 - for interrupt gen and scan of keyboard
    YM2149/AY-3-8910 - sound + HDD/CF IDE
    GAL16V8 - address decoder
    ATMEGA8 - gen. of PAL video signal (modified TellyMate)
    keyboard of C64 computer used

Commands:
A - (unknown)
C - (unknown)
E - (unknown)
F - (unknown)
G - (unknown)
L - (unknown)
R - (unknown)
S - (unknown)

Some commands expect a filename enclosed in double quotes. If the quotes
are not there, it loops forever looking for them. Good example of bad
programming. There is no help and no error messages.

ToDo:

- The ATMEGA8 is a CPU with Timer, 4 IO ports, UART, ADC, Watchdog all
  built in. This enormously complex device needs to be emulated. It also
  contains some (4k?) RAM, of which certain addresses have special meaning.
  For example bytes 0 and 1 control the serial video stream of bits.

- When the system boots, there is a Y in the top corner. This is actually
  Esc-Y, which our terminal does not understand.

- IDE interface.

- Find out the proper way to use the monitor, there are no instructions
  and the slightest mistake can freeze the system.

- No software to test with.


****************************************************************************/

#include "emu.h"
#include "cpu/m6502/m6510.h"
#include "machine/6526cia.h"
#include "sound/ay8910.h"
#include "machine/terminal.h"


class sbc6510_state : public driver_device
{
public:
	sbc6510_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
	m_terminal(*this, TERMINAL_TAG) { }

	DECLARE_READ8_MEMBER(a2_r);
	DECLARE_WRITE8_MEMBER(a2_w);
	DECLARE_READ8_MEMBER(psg_a_r);
	DECLARE_READ8_MEMBER(psg_b_r);
	DECLARE_WRITE8_MEMBER(key_w);
	DECLARE_READ8_MEMBER(key_r);
	UINT8 m_term_data;
	UINT8 m_key_row;
	UINT8 m_2;
	required_device<generic_terminal_device> m_terminal;
	virtual void machine_reset();
};


static ADDRESS_MAP_START( sbc6510_mem, AS_PROGRAM, 8, sbc6510_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x0001) AM_RAM
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(a2_r,a2_w)
	AM_RANGE(0x0003, 0xdfff) AM_RAM
	AM_RANGE(0xE000, 0xE00F) AM_MIRROR(0x1f0) AM_DEVREADWRITE_LEGACY("cia6526", mos6526_r, mos6526_w)
	AM_RANGE(0xE800, 0xE800) AM_MIRROR(0x1ff) AM_DEVWRITE_LEGACY("ay8910", ay8910_address_w)
	AM_RANGE(0xEA00, 0xEA00) AM_MIRROR(0x1ff) AM_DEVREADWRITE_LEGACY("ay8910", ay8910_r, ay8910_data_w)
	AM_RANGE(0xf000, 0xffff) AM_ROM
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( sbc6510 ) // cbm keyboard
	PORT_START( "X0" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Crsr Down Up") PORT_CODE(KEYCODE_RALT) PORT_CHAR(UCHAR_MAMEKEY(DOWN)) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F3)        PORT_CHAR(UCHAR_MAMEKEY(F5)) PORT_CHAR(UCHAR_MAMEKEY(F6))
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F2)        PORT_CHAR(UCHAR_MAMEKEY(F3)) PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F1)        PORT_CHAR(UCHAR_MAMEKEY(F1)) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F4)        PORT_CHAR(UCHAR_MAMEKEY(F7)) PORT_CHAR(UCHAR_MAMEKEY(F8))
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Crsr Right Left") PORT_CODE(KEYCODE_RCONTROL) PORT_CHAR(UCHAR_MAMEKEY(RIGHT)) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Return") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Del  Inst") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8) PORT_CHAR(UCHAR_MAMEKEY(INSERT))

	PORT_START( "X1" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Shift (Left)") PORT_CODE(KEYCODE_LSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E)         PORT_CHAR('E')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S)         PORT_CHAR('S')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z)     PORT_CHAR('Z')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4)     PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A)         PORT_CHAR('A')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W)         PORT_CHAR('W')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3)     PORT_CHAR('3') PORT_CHAR('#')

	PORT_START( "X2" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X)         PORT_CHAR('X')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T)         PORT_CHAR('T')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F)         PORT_CHAR('F')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C)         PORT_CHAR('C')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6)     PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D)         PORT_CHAR('D')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R)         PORT_CHAR('R')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5)     PORT_CHAR('5') PORT_CHAR('%')

	PORT_START( "X3" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V)         PORT_CHAR('V')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U)         PORT_CHAR('U')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H)         PORT_CHAR('H')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B)         PORT_CHAR('B')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8)     PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G)         PORT_CHAR('G')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y)         PORT_CHAR('Y')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7)     PORT_CHAR('7') PORT_CHAR('\'')

	PORT_START( "X4" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N)         PORT_CHAR('N')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O)         PORT_CHAR('O')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K)         PORT_CHAR('K')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M)         PORT_CHAR('M')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0)     PORT_CHAR('0')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J)         PORT_CHAR('J')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I)         PORT_CHAR('I')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9)     PORT_CHAR('9') PORT_CHAR(')')

	PORT_START( "X5" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COMMA)     PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('@')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COLON)     PORT_CHAR(':') PORT_CHAR('[')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_STOP)      PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_EQUALS)    PORT_CHAR('-')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L)         PORT_CHAR('L')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P)         PORT_CHAR('P')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS)     PORT_CHAR('+')

	PORT_START( "X6" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH)     PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("\xE2\x86\x91  Pi") PORT_CODE(KEYCODE_DEL) PORT_CHAR(0x2191) PORT_CHAR(0x03C0)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('=')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Shift (Right)") PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Home  Clr") PORT_CODE(KEYCODE_INSERT)     PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_QUOTE)     PORT_CHAR(';') PORT_CHAR(']')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_CLOSEBRACE)    PORT_CHAR('*')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_BACKSLASH2)    PORT_CHAR('\xA3')

	PORT_START( "X7" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Stop Run") PORT_CODE(KEYCODE_HOME)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q)     PORT_CHAR('Q')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("CBM") PORT_CODE(KEYCODE_LALT)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SPACE)     PORT_CHAR(' ')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2)     PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_TAB)       PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("\xE2\x86\x90") PORT_CODE(KEYCODE_TILDE)   PORT_CHAR(0x2190)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1)     PORT_CHAR('1') PORT_CHAR('!')

	PORT_START( "X8" )  /* unused */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Restore") PORT_CODE(KEYCODE_PRTSCR)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Shift Lock (switch)") PORT_CODE(KEYCODE_CAPSLOCK) PORT_TOGGLE PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END


READ8_MEMBER( sbc6510_state::a2_r )
{
	return m_2;
}

WRITE8_MEMBER( sbc6510_state::a2_w )
{
	m_2 = data;
	m_terminal->write(space, 0, data);
}

static GENERIC_TERMINAL_INTERFACE( terminal_intf )
{
	DEVCB_NULL
};


void sbc6510_state::machine_reset()
{
}

READ8_MEMBER( sbc6510_state::psg_a_r )
{
	return 0xff;
}

READ8_MEMBER( sbc6510_state::psg_b_r )
{
	return 0x7f;
}

// Ports A and B connect to the IDE socket
static const ay8910_interface sbc6510_ay_interface =
{
	AY8910_LEGACY_OUTPUT,   // flags
	AY8910_DEFAULT_LOADS,   // channel load in ohms
	DEVCB_DRIVER_MEMBER(sbc6510_state, psg_a_r),        // port A read
	DEVCB_DRIVER_MEMBER(sbc6510_state, psg_b_r),        // port B read
	DEVCB_NULL,     // port A write
	DEVCB_NULL      // port B write
};

READ8_MEMBER( sbc6510_state::key_r )
{
	UINT8 i,data=0;
	char kbdrow[6];

	for (i = 0; i < 8; i++)
	if (!BIT(m_key_row, i))
	{
		sprintf(kbdrow,"X%X",i);
		data |= ioport(kbdrow)->read();
	}
	return ~data;
}

WRITE8_MEMBER( sbc6510_state::key_w )
{
	m_key_row = data;
}

const legacy_mos6526_interface cia_intf =
{
	DEVCB_CPU_INPUT_LINE("maincpu", M6510_IRQ_LINE), // irq
	DEVCB_NULL, // pc (timer related) not connected
	DEVCB_NULL, // cnt (serial related) not connected
	DEVCB_NULL, // sp (serial related) not connected
	DEVCB_NULL, // port A in
	DEVCB_DRIVER_MEMBER(sbc6510_state, key_w),  // port A out
	DEVCB_DRIVER_MEMBER(sbc6510_state, key_r),  // port B in
	DEVCB_NULL  // port B out
};

static MACHINE_CONFIG_START( sbc6510, sbc6510_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",M6510, XTAL_1MHz)
	MCFG_CPU_PROGRAM_MAP(sbc6510_mem)


	/* video hardware */
	MCFG_GENERIC_TERMINAL_ADD(TERMINAL_TAG, terminal_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("ay8910", AY8910, XTAL_1MHz)
	MCFG_SOUND_CONFIG(sbc6510_ay_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	MCFG_LEGACY_MOS6526R1_ADD("cia6526", XTAL_1MHz, 50, cia_intf)
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( sbc6510 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "sbc6510.rom", 0xf000, 0x1000, CRC(e13a5e62) SHA1(1e7482e9b98b39d0cc456254fbe8fd0981e9377e))
	ROM_REGION( 0x2000, "videocpu", ROMREGION_ERASEFF ) // ATMEGA8 at 16MHz
	ROM_LOAD( "video.bin",   0x0000, 0x2000, CRC(809f31ce) SHA1(4639de5f7b8f6c036d74f217ba85e7e897039094))
	ROM_REGION( 0x200, "gal", ROMREGION_ERASEFF )
	ROM_LOAD( "sbc6510.gal", 0x0000, 0x0117, CRC(f78f9927) SHA1(b951163958f5722032826d0d17a07c81dbd5f68e))
ROM_END

/* Driver */

/*    YEAR  NAME      PARENT  COMPAT   MACHINE    INPUT    CLASS         INIT    COMPANY          FULLNAME       FLAGS */
COMP( 2009, sbc6510,  0,      0,       sbc6510,   sbc6510, driver_device, 0,   "Josip Perusanec", "SBC6510", 0 )
