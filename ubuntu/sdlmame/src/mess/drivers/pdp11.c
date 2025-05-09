/***************************************************************************

        PDP-11

        Unibus models
        ==================
        PDP-11/20 and PDP-11/15 ? The original, non-microprogrammed processor;
                designed by Jim O'Loughlin. Floating point was supported by
                peripheral options using various data formats.
        PDP-11/35 and PDP-11/40 ? A microprogrammed successor to the PDP-11/20;
                the design team was led by Jim O'Loughlin.
        PDP-11/45, PDP-11/50, and PDP-11/55 ? A much faster microprogrammed processor
                that could use up to 256 kB of semiconductor memory instead of or in
                addition to core memory. First model to support an optional FP11
                floating-point coprocessor, which established the format used in
                later models.
        PDP-11/70 ? The 11/45 architecture expanded to allow 4 MB of physical memory
                segregated onto a private memory bus, 2 kB of cache memory, and much
                faster I/O devices connected via the Massbus.[9]
        PDP-11/05 and PDP-11/10 ? A cost-reduced successor to the PDP-11/20.
        PDP-11/34 and PDP-11/04 ? Cost-reduced follow-on products to the 11/35
                and 11/05; the PDP-11/34 concept was created by Bob Armstrong.
                The 11/34 supported up to 256 kB of Unibus memory. The PDP-11/34a
                supported a fast floating-point option, and the 11/34c supported a
                cache memory option.
        PDP-11/60 ? A PDP-11 with user-writable microcontrol store; this was
                designed by another team led by Jim O'Loughlin.
        PDP-11/44 ? Replacement for the 11/45 and 11/70 that supported optional cache
                memory and floating-point processor, and included a sophisticated serial
                console interface and support for 4 MB of physical memory. The design
                team was managed by John Sofio.
        PDP-11/24 ? First VLSI PDP-11 for Unibus, using the "Fonz-11" (F11) chip set
                with a Unibus adapter.
        PDP-11/84 ? Using the VLSI "Jaws-11" (J11) chip set with a Unibus adapter.
        PDP-11/94 ? J11-based, faster than 11/84.

        Q-bus models
        ==============
        PDP-11/03 (also known as the LSI-11/03) ? The first LSI PDP-11, this system
                used a chipset from Western Digital and supported 60 kB of memory.
        PDP-11/23 ? Second generation of LSI (F-11). Early units supported
                only 248 kB of memory.
        PDP-11/23+/MicroPDP-11/23 ? Improved 11/23 with more functions on the
                (larger) processor card.
        MicroPDP-11/73 ? The third generation LSI-11, this system used the
                faster "Jaws-11" (J-11) chip set and supported up to 4 MB of memory.
        MicroPDP-11/53 ? Slower 11/73 with on-board memory.
        MicroPDP-11/83 ? Faster 11/73 with PMI (private memory interconnect).
        MicroPDP-11/93 ? Faster 11/83; final DEC Q-Bus PDP-11 model.
        KXJ11 - QBUS card (M7616) with PDP-11 based peripheral processor and
                DMA controller. Based on a J11 CPU equipped with 512 kB of RAM,
                64 kB of ROM, and parallel and serial interfaces.
        Mentec M100 ? Mentec redesign of the 11/93, with J-11 chipset at 19.66 MHz,
                four on-board serial ports, 1-4 MB of on-board memory, and optional FPU.
        Mentec M11 ? Processor upgrade board; microcode implementation of PDP-11
                instruction set by Mentec, using the TI 8832 ALU and TI 8818
                microsequencer from Texas Instruments.
        Mentec M1 ? Processor upgrade board; microcode implementation of
                PDP-11 instruction set by Mentec, using Atmel 0.35 ?m ASIC.[10]
        Quickware QED-993 ? High performance PDP-11/93 processor upgrade board.
        DECserver 500 and 550 LAT terminal servers DSRVS-BA using the KDJ11-SB chipset

        All PDP-11's execept the first one (11/15 and 11/20) are microprogrammed.

        23/02/2009 Skeleton driver.

****************************************************************************/

#include "emu.h"
#include "cpu/t11/t11.h"
#include "machine/terminal.h"
#include "machine/rx01.h"

class pdp11_state : public driver_device
{
public:
	pdp11_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
	m_maincpu(*this, "maincpu"),
	m_terminal(*this, TERMINAL_TAG)
	{ }

	required_device<cpu_device> m_maincpu;
	required_device<generic_terminal_device> m_terminal;
	DECLARE_READ16_MEMBER( term_r );
	DECLARE_READ16_MEMBER( term_tx_status_r );
	DECLARE_READ16_MEMBER( term_rx_status_r );
	DECLARE_WRITE16_MEMBER( term_w );
	DECLARE_WRITE8_MEMBER( kbd_put );
	UINT8 m_term_data;
	UINT16 m_term_status;
	virtual void machine_reset();
	DECLARE_MACHINE_RESET(pdp11ub2);
	DECLARE_MACHINE_RESET(pdp11qb);
};

WRITE16_MEMBER(pdp11_state::term_w)
{
	m_terminal->write(space, 0, data);
}

READ16_MEMBER(pdp11_state::term_r)
{
	m_term_status = 0x0000;
	return m_term_data;
}

READ16_MEMBER(pdp11_state::term_tx_status_r)
{   // always ready
	return 0xffff;
}

READ16_MEMBER(pdp11_state::term_rx_status_r)
{
	return m_term_status;
}

static ADDRESS_MAP_START(pdp11_mem, AS_PROGRAM, 16, pdp11_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x0000, 0xdfff ) AM_RAM  // RAM
	AM_RANGE( 0xea00, 0xfeff ) AM_ROM
	AM_RANGE( 0xff70, 0xff71 ) AM_READ(term_rx_status_r)
	AM_RANGE( 0xff72, 0xff73 ) AM_READ(term_r)
	AM_RANGE( 0xff74, 0xff75 ) AM_READ(term_tx_status_r)
	AM_RANGE( 0xff76, 0xff77 ) AM_WRITE(term_w)

	AM_RANGE( 0xfe78, 0xfe7b ) AM_DEVREADWRITE("rx01", rx01_device, read, write)
ADDRESS_MAP_END

static ADDRESS_MAP_START(pdp11qb_mem, AS_PROGRAM, 16, pdp11_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x0000, 0xe9ff ) AM_RAM  // RAM
	AM_RANGE( 0xea00, 0xefff ) AM_ROM
	AM_RANGE( 0xf000, 0xffff ) AM_RAM
ADDRESS_MAP_END

#define M9312_PORT_CONFSETTING \
PORT_CONFSETTING ( 0x00, "'DL' BOOT prom for RL11 controller") \
PORT_CONFSETTING ( 0x01, "'DM' BOOT prom for RK06/07 controller") \
PORT_CONFSETTING ( 0x02, "'DX' BOOT prom for RX01 compatible controller") \
PORT_CONFSETTING ( 0x03, "'DP/DB' BOOT prom for RP02/03,RP04/5/6 RM02/3 controller") \
PORT_CONFSETTING ( 0x04, "'DK/DT' BOOT prom for RK03/05,TU55/56 controllers") \
PORT_CONFSETTING ( 0x05, "'MM' BOOT prom for TU16/E16 TM02/3 controllers") \
PORT_CONFSETTING ( 0x06, "'MT' BOOT prom for TU10/TS03 controller") \
PORT_CONFSETTING ( 0x07, "'DS' BOOT prom for RS03/RS04 controller") \
PORT_CONFSETTING ( 0x08, "'PR/TT' BOOT prom for PC05,LO SPD RDR controllers") \
PORT_CONFSETTING ( 0x09, "'CT' BOOT prom for TA11/TU60 controller") \
PORT_CONFSETTING ( 0x0a, "'RS' BOOT prom for RS11, RS64 controller") \
PORT_CONFSETTING ( 0x0b, "'CR' BOOT prom for CR11 card reader") \
PORT_CONFSETTING ( 0x0c, "'MS' BOOT prom for TS11/TS04/TU80 compatible controller") \
PORT_CONFSETTING ( 0x0d, "'DD' BOOT prom for TU58 DECtapeII serial tape controller") \
PORT_CONFSETTING ( 0x0e, "'DU' BOOT prom for MSCP compatible controller") \
PORT_CONFSETTING ( 0x0f, "'XX' Unknown 1/3") \
PORT_CONFSETTING ( 0x10, "'XX' Unknown 2/3") \
PORT_CONFSETTING ( 0x11, "'XX' Unknown 3/3") \
PORT_CONFSETTING ( 0x12, "'DY' BOOT prom for RX02 compatible controller") \
PORT_CONFSETTING ( 0x13, "'XM' DECNET 1/3 (DECnet DDCMP DMC11/DMR11)") \
PORT_CONFSETTING ( 0x14, "'XM' DECNET 2/3 (DECnet DDCMP DMC11/DMR11)") \
PORT_CONFSETTING ( 0x15, "'XM' DECNET 3/3 (DECnet DDCMP DMC11/DMR11)") \
PORT_CONFSETTING ( 0x16, "'XU' DECNET 1/3 (DECnet DDCMP DU11)") \
PORT_CONFSETTING ( 0x17, "'XU' DECNET 2/3 (DECnet DDCMP DU11)") \
PORT_CONFSETTING ( 0x18, "'XU' DECNET 3/3 (DECnet DDCMP DU11)") \
PORT_CONFSETTING ( 0x19, "'XW' DECNET 1/3 (DECnet DDCMP DUP11)") \
PORT_CONFSETTING ( 0x1a, "'XW' DECNET 2/3 (DECnet DDCMP DUP11)") \
PORT_CONFSETTING ( 0x1b, "'XW' DECNET 3/3 (DECnet DDCMP DUP11)") \
PORT_CONFSETTING ( 0x1c, "'XL' DECNET 1/3 (DECnet DDCMP DL11-E)") \
PORT_CONFSETTING ( 0x1d, "'XL' DECNET 2/3 (DECnet DDCMP DL11-E)") \
PORT_CONFSETTING ( 0x1e, "'XL' DECNET 3/3 (DECnet DDCMP DL11-E)") \
PORT_CONFSETTING ( 0x1f, "'XE' DEUNA DECnet Ethernet") \
PORT_CONFSETTING ( 0x20, "'MU' TMSCP tapes, including TK50, TU81") \

/* Input ports */
static INPUT_PORTS_START( pdp11 )
	PORT_START("S1")
	PORT_DIPNAME( 0x01, 0x01, "S1-1" )
	PORT_DIPSETTING(    0x00, "Direct boot" )
	PORT_DIPSETTING(    0x01, "Console mode" )
	PORT_DIPNAME( 0x02, 0x02, "S1-2 Boot")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_START("S1_2")
	PORT_DIPNAME( 0x80, 0x00, "S1-3" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, "S1-4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "S1-5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "S1-6" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "S1-7" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "S1-8" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "S1-9" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x00, "S1-10" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_START( "CONSPROM" )
	PORT_CONFNAME ( 0x01, 0, "Console PROM" )
	PORT_CONFSETTING ( 0x00, "11/04/05/34/35/40/45/50/55" )
	PORT_CONFSETTING ( 0x01, "11/60-70" )
	PORT_START( "DEVPROM1" )
	PORT_CONFNAME ( 0x2f, 0x02, "Device 1 PROM" )
	M9312_PORT_CONFSETTING
	PORT_START( "DEVPROM2" )
	PORT_CONFNAME ( 0x2f, 0x00, "Device 2 PROM" )
	M9312_PORT_CONFSETTING
	PORT_START( "DEVPROM3" )
	PORT_CONFNAME ( 0x2f, 0x0d, "Device 3 PROM" )
	M9312_PORT_CONFSETTING
	PORT_START( "DEVPROM4" )
	PORT_CONFNAME ( 0x2f, 0x04, "Device 4 PROM" )
	M9312_PORT_CONFSETTING
INPUT_PORTS_END


void pdp11_state::machine_reset()
{
	// Load M9301-YA
	UINT8* user1 = machine().root_device().memregion("user1")->base();
	UINT8* maincpu = machine().root_device().memregion("maincpu")->base();
	int i;

	for(i=0;i<0x100;i++) {
		UINT8 nib1 = user1[i+0x000] ^ 0x00;
		UINT8 nib2 = user1[i+0x200] ^ 0x01;
		UINT8 nib3 = user1[i+0x400] ^ 0x0f;
		UINT8 nib4 = user1[i+0x600] ^ 0x0e;

		maincpu[0xea00 + i*2 + 1] = (nib1 << 4) + nib2;
		maincpu[0xea00 + i*2 + 0] = (nib3 << 4) + nib4;
	}
	for(i=0x100;i<0x200;i++) {
		UINT8 nib1 = user1[i+0x000] ^ 0x00;
		UINT8 nib2 = user1[i+0x200] ^ 0x01;
		UINT8 nib3 = user1[i+0x400] ^ 0x0f;
		UINT8 nib4 = user1[i+0x600] ^ 0x0e;

		maincpu[0xf600 + (i-0x100)*2 + 1] = (nib1 << 4) + nib2;
		maincpu[0xf600 + (i-0x100)*2 + 0] = (nib3 << 4) + nib4;
	}
}

static void load9312prom(UINT8 *desc, UINT8 *src, int size)
{
	//   3   2   1   8
	//   7   6   5   4
	// ~11 ~10   9   0
	//  15  14  13 ~12
	for(int i=0;i<size;i++) {
		UINT8 nib1 = src[i*4+0];
		UINT8 nib2 = src[i*4+1];
		UINT8 nib3 = src[i*4+2];
		UINT8 nib4 = src[i*4+3];

		desc[i*2 + 0] = (nib2 << 4) + ((nib1 & 0x0e) | (nib3 & 1));
		desc[i*2 + 1] = ((nib4 ^ 0x01)<<4) + ((nib1 & 0x01) | ((nib3 ^ 0x0c) & 0x0e));
	}
}

MACHINE_RESET_MEMBER(pdp11_state,pdp11ub2)
{
	// Load M9312
	UINT8* user1 = machine().root_device().memregion("consproms")->base() + machine().root_device().ioport("CONSPROM")->read() * 0x0400;
	UINT8* maincpu = machine().root_device().memregion("maincpu")->base();

	//0165000
	load9312prom(maincpu + 0165000,user1,0x100);

	UINT8 s1 = machine().root_device().ioport("S1")->read();

	if (s1 & 0x02) { // if boot enabled
		UINT16 addr = 0173000;
		if (s1 & 1) {
			addr = 0165000;
		}
		addr += machine().root_device().ioport("S1_2")->read() * 2;
		machine().device("maincpu")->state().set_state_int(T11_PC, addr);
	}

	//0173000
	load9312prom(maincpu + 0173000,machine().root_device().memregion("devproms")->base() + machine().root_device().ioport("DEVPROM1")->read() * 0x0200,0x080);
	//0173200
	load9312prom(maincpu + 0173200,machine().root_device().memregion("devproms")->base() + machine().root_device().ioport("DEVPROM2")->read() * 0x0200,0x080);
	//0173400
	load9312prom(maincpu + 0173400,machine().root_device().memregion("devproms")->base() + machine().root_device().ioport("DEVPROM3")->read() * 0x0200,0x080);
	//0173600
	load9312prom(maincpu + 0173600,machine().root_device().memregion("devproms")->base() + machine().root_device().ioport("DEVPROM4")->read() * 0x0200,0x080);

}

MACHINE_RESET_MEMBER(pdp11_state,pdp11qb)
{
	machine().device("maincpu")->state().set_state_int(T11_PC, 0xea00);
}

static const struct t11_setup pdp11_data =
{
	6 << 13
};

static const struct t11_setup mxv11_data =
{
	0 << 13
};

WRITE8_MEMBER( pdp11_state::kbd_put )
{
	m_term_data = data;
	m_term_status = 0xffff;
}

static GENERIC_TERMINAL_INTERFACE( terminal_intf )
{
	DEVCB_DRIVER_MEMBER(pdp11_state, kbd_put)
};

static MACHINE_CONFIG_START( pdp11, pdp11_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",T11, XTAL_4MHz) // Need proper CPU here
	MCFG_CPU_CONFIG(pdp11_data)
	MCFG_CPU_PROGRAM_MAP(pdp11_mem)


	/* video hardware */
	MCFG_GENERIC_TERMINAL_ADD(TERMINAL_TAG, terminal_intf)

	MCFG_RX01_ADD("rx01")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pdp11ub2, pdp11 )
	MCFG_MACHINE_RESET_OVERRIDE(pdp11_state,pdp11ub2)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pdp11qb, pdp11 )
	MCFG_MACHINE_RESET_OVERRIDE(pdp11_state,pdp11qb)

	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CONFIG(mxv11_data)
	MCFG_CPU_PROGRAM_MAP(pdp11qb_mem)
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( pdp11ub )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_REGION( 0x1000, "user1", ROMREGION_ERASEFF )
	ROM_LOAD( "23-034a9.bin", 0x0000, 0x0200, CRC(01c5d78d) SHA1(b447c67bfd5134c142240a919f23a949e1953fb2))
	ROM_LOAD( "23-035a9.bin", 0x0200, 0x0200, CRC(c456df6c) SHA1(188c8ece6a2d67911016f55dd22b698a40aff515))
	ROM_LOAD( "23-036a9.bin", 0x0400, 0x0200, CRC(208ff511) SHA1(27198a1110319b70674a72fd03a798dfa2c2109a))
	ROM_LOAD( "23-037a9.bin", 0x0600, 0x0200, CRC(d248b282) SHA1(ea638de6bde8342654d3e62b7810aa041e111913))
ROM_END

ROM_START( pdp11ub2 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_REGION( 0x800, "consproms", ROMREGION_ERASEFF )
	ROM_LOAD( "23-248f1.bin", 0x0000, 0x0400, CRC(ecda1a6d) SHA1(b2bf770dda349fdd469235871564280baf06301d)) // M9312 11/04/05/34/35/40/45/50/55  Console/Diagnostic PROM
	ROM_LOAD( "23-616f1.bin", 0x0400, 0x0400, CRC(a3dfb5aa) SHA1(7f06c624ae3fbb49535258b8722b5a3c548da3ba)) // M9312 11/60-70 Diagnostic/Console ROM
	ROM_REGION( 0x4200, "devproms", ROMREGION_ERASEFF )
	ROM_LOAD( "23-751a9.bin", 0x0000, 0x0200, CRC(15bebc6a) SHA1(a621c5b1cebebbb110ee646a8c36ee4c606e269b)) // M9312 'DL' BOOT prom for RL11 controller
	ROM_LOAD( "23-752a9.bin", 0x0200, 0x0200, CRC(6cf1f859) SHA1(7c876eda2f0d74d6f5d876256c28dbd56c405ca7)) // M9312 'DM' BOOT prom for RK06/07 controller
	ROM_LOAD( "23-753a9.bin", 0x0400, 0x0200, CRC(f4c4b40c) SHA1(a0bdb922c722d439f35ba8149a8f657ffcc8fb54)) // M9312 'DX' BOOT prom for RX01 compatible controller
	ROM_LOAD( "23-755a9.bin", 0x0600, 0x0200, CRC(ed06b35c) SHA1(d972c6a743d73ce9244d2bcfdd40eea2bb22e717)) // M9312 'DP/DB' BOOT prom for RP02/03,RP04/5/6 RM02/3 controller
	ROM_LOAD( "23-756a9.bin", 0x0800, 0x0200, CRC(12271ab2) SHA1(f0ff42a8fd839dd75d6c1a25cc82d0933fd09dbc)) // M9312 'DK/DT' BOOT prom for RK03/05,TU55/56 controllers
	ROM_LOAD( "23-757a9.bin", 0x0a00, 0x0200, CRC(af251aab) SHA1(4d760ec3f6ff5f4e2cafcb44b275183872b69cb6)) // M9312 'MM' BOOT prom for TU16/E16 TM02/3 controllers
	ROM_LOAD( "23-758a9.bin", 0x0c00, 0x0200, CRC(b71e8878) SHA1(f45c47c702c94a70c36732c12173ce60d0be1a11)) // M9312 'MT' BOOT prom for TU10/TS03 controller
	ROM_LOAD( "23-759a9.bin", 0x0e00, 0x0200, CRC(29a93448) SHA1(0b549170c6a3f49c1587adb6cc691786111c0dd3)) // M9312 'DS' BOOT prom for RS03/RS04 controller
	ROM_LOAD( "23-760a9.bin", 0x1000, 0x0200, CRC(ea093648) SHA1(3875a0147c43db1a5a381bbe85937a5628e6220c)) // M9312 'PR/TT' BOOT prom for PC05,LO SPD RDR controllers
	ROM_LOAD( "23-761a9.bin", 0x1200, 0x0200, CRC(4310ebe8) SHA1(a3144f96819ea57acfac5de5e19961294e7d4ad9)) // M9312 'CT' BOOT prom for TA11/TU60 controller
	ROM_LOAD( "23-762a9.bin", 0x1400, 0x0200, NO_DUMP)                                                      // M9312 'RS' BOOT prom for RS11, RS64 controller
	ROM_LOAD( "23-763a9.bin", 0x1600, 0x0200, NO_DUMP)                                                      // M9312 'CR' BOOT prom for CR11 card reader
	ROM_LOAD( "23-764a9.bin", 0x1800, 0x0200, CRC(7c8b7ed4) SHA1(ba0c9f03027eb3dafcc0936e877637d3c9947f94)) // M9312 'MS' BOOT prom for TS11/TS04/TU80 compatible controller
	ROM_LOAD( "23-765a9.bin", 0x1a00, 0x0200, CRC(702dfeb2) SHA1(0d37bdd3846de4b104b8968a0e83ed81abd7f9ae)) // M9312 'DD' BOOT prom for TU58 DECtapeII serial tape controller
	ROM_LOAD( "23-767a9.bin", 0x1c00, 0x0200, CRC(4b94e3fa) SHA1(3cf92c2f64f95e8cc3abb8af2526cc65ce53ca8a)) // M9312 'DU' BOOT prom for MSCP compatible controller
	ROM_LOAD( "23-786a9.bin", 0x1e00, 0x0200, CRC(a5326664) SHA1(238f97fc5b2b540948ea1e27a4cd1dcf18255b21)) // M9312 'XX' Unknown 1/3
	ROM_LOAD( "23-787a9.bin", 0x2000, 0x0200, CRC(025debf9) SHA1(8ea2faf2e2d78be0ad2f77e61bae0dfb9c3b4b01)) // M9312 'XX' Unknown 2/3
	ROM_LOAD( "23-788a9.bin", 0x2200, 0x0200, CRC(3c7ed364) SHA1(519ffac2e4878490128e754a0473502c767a94e2)) // M9312 'XX' Unknown 3/3
	ROM_LOAD( "23-811a9.bin", 0x2400, 0x0200, CRC(9aa8499a) SHA1(11b040e0908d7492dcc450cbb72d76633dd687ca)) // M9312 'DY' BOOT prom for RX02 compatible controller
	ROM_LOAD( "23-862a9.bin", 0x2600, 0x0200, CRC(38dbd994) SHA1(c5db671e6b70f3b4d345a02b46e0ea7566160d04)) // M9312 'XM' DECNET 1/3 (DECnet DDCMP DMC11/DMR11)
	ROM_LOAD( "23-863a9.bin", 0x2800, 0x0200, CRC(bbef2f41) SHA1(f472b7a8bd4c0a49dc3ec38f886755910f73fe66)) // M9312 'XM' DECNET 2/3 (DECnet DDCMP DMC11/DMR11)
	ROM_LOAD( "23-864a9.bin", 0x2a00, 0x0200, CRC(85cc17dc) SHA1(371dbd3c672fe4b1819762c3082c4217a7597547)) // M9312 'XM' DECNET 3/3 (DECnet DDCMP DMC11/DMR11)
	ROM_LOAD( "23-865a9.bin", 0x2c00, 0x0200, NO_DUMP)                                                      // M9312 'XU' DECNET 1/3 (DECnet DDCMP DU11)
	ROM_LOAD( "23-866a9.bin", 0x2e00, 0x0200, NO_DUMP)                                                      // M9312 'XU' DECNET 2/3 (DECnet DDCMP DU11)
	ROM_LOAD( "23-867a9.bin", 0x3000, 0x0200, NO_DUMP)                                                      // M9312 'XU' DECNET 3/3 (DECnet DDCMP DU11)
	ROM_LOAD( "23-868a9.bin", 0x3200, 0x0200, NO_DUMP)                                                      // M9312 'XW' DECNET 1/3 (DECnet DDCMP DUP11)
	ROM_LOAD( "23-869a9.bin", 0x3400, 0x0200, NO_DUMP)                                                      // M9312 'XW' DECNET 2/3 (DECnet DDCMP DUP11)
	ROM_LOAD( "23-870a9.bin", 0x3600, 0x0200, NO_DUMP)                                                      // M9312 'XW' DECNET 3/3 (DECnet DDCMP DUP11)
	ROM_LOAD( "23-926a9.bin", 0x3800, 0x0200, NO_DUMP)                                                      // M9312 'XL' DECNET 1/3 (DECnet DDCMP DL11-E)
	ROM_LOAD( "23-927a9.bin", 0x3a00, 0x0200, NO_DUMP)                                                      // M9312 'XL' DECNET 2/3 (DECnet DDCMP DL11-E)
	ROM_LOAD( "23-928a9.bin", 0x3c00, 0x0200, NO_DUMP)                                                      // M9312 'XL' DECNET 3/3 (DECnet DDCMP DL11-E)
	ROM_LOAD( "23-e22a9.bin", 0x3e00, 0x0200, NO_DUMP)                                                      // M9312 'XE' DEUNA DECnet Ethernet
	ROM_LOAD( "23-e39a9.bin", 0x4000, 0x0200, CRC(4b94e3fa) SHA1(3cf92c2f64f95e8cc3abb8af2526cc65ce53ca8a)) // M9312 'MU' TMSCP tapes, including TK50, TU81

ROM_END

ROM_START( pdp11qb )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "m7195fa.1", 0xc000, 0x2000, CRC(0fa58752) SHA1(4bcd006790a60f2998ee8377ac5e2c18ef330930))
	ROM_LOAD16_BYTE( "m7195fa.2", 0xc001, 0x2000, CRC(15b6f60c) SHA1(80dd4f8ca3c27babb5e75111b04241596a07c53a))
ROM_END
/* Driver */

/*    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT    INIT    COMPANY   FULLNAME       FLAGS */
COMP( ????, pdp11ub,  0,       0,   pdp11,    pdp11, driver_device,  0,   "Digital Equipment Corporation",   "PDP-11 [Unibus](M9301-YA)",       GAME_NOT_WORKING | GAME_NO_SOUND)
COMP( ????, pdp11ub2, pdp11ub, 0,   pdp11ub2, pdp11, driver_device,  0,   "Digital Equipment Corporation",   "PDP-11 [Unibus](M9312)",      GAME_NOT_WORKING | GAME_NO_SOUND)
COMP( ????, pdp11qb,  pdp11ub, 0,   pdp11qb,  pdp11, driver_device,  0,   "Digital Equipment Corporation",   "PDP-11 [Q-BUS] (M7195 - MXV11)",      GAME_NOT_WORKING | GAME_NO_SOUND)
