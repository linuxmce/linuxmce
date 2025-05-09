/******************************************************************************

    Machine Hardware for Nichibutsu Mahjong series.

    Driver by Takahiro Nogi <nogi@kt.rim.or.jp> 1999/11/05 -

******************************************************************************/
/******************************************************************************
Memo:

******************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "includes/nb1413m3.h"


#define NB1413M3_DEBUG  0


int nb1413m3_type;
const char * nb1413m3_sndromrgntag;
int nb1413m3_sndrombank1;
int nb1413m3_sndrombank2;
int nb1413m3_busyctr;
int nb1413m3_busyflag;
int nb1413m3_inputport;

static int nb1413m3_74ls193_counter;
static int nb1413m3_nmi_count;          // for debug
static int nb1413m3_nmi_clock;
static int nb1413m3_nmi_enable;
static int nb1413m3_counter;
static int nb1413m3_gfxradr_l;
static int nb1413m3_gfxradr_h;
static int nb1413m3_gfxrombank;
static int nb1413m3_outcoin_enable;
static int nb1413m3_outcoin_flag;


#define NB1413M3_TIMER_BASE 20000000

/* TODO: is all of this actually programmable? */
static TIMER_CALLBACK( nb1413m3_timer_callback )
{
	machine.scheduler().timer_set(attotime::from_hz(NB1413M3_TIMER_BASE) * 256, FUNC(nb1413m3_timer_callback));

	nb1413m3_74ls193_counter++;
	nb1413m3_74ls193_counter &= 0x0f;

	if (nb1413m3_74ls193_counter == 0x0f)
	{

		if (nb1413m3_nmi_enable)
		{
			machine.device("maincpu")->execute().set_input_line(INPUT_LINE_NMI, PULSE_LINE);
			nb1413m3_nmi_count++;
		}

		switch (nb1413m3_type)
		{
			case NB1413M3_TAIWANMB:
				nb1413m3_74ls193_counter = 0x05;
				break;
			case NB1413M3_OMOTESND:
				nb1413m3_74ls193_counter = 0x05;
				break;
			case NB1413M3_PASTELG:
				nb1413m3_74ls193_counter = 0x02;
				break;
			case NB1413M3_HYHOO:
			case NB1413M3_HYHOO2:
				nb1413m3_74ls193_counter = 0x05;
				break;
		}
	}
}

MACHINE_START( nb1413m3 )
{
	machine.scheduler().synchronize(FUNC(nb1413m3_timer_callback));
}

MACHINE_RESET( nb1413m3 )
{
	nb1413m3_nmi_clock = 0;
	nb1413m3_nmi_enable = 0;
	nb1413m3_nmi_count = 0;
	nb1413m3_74ls193_counter = 0;
	nb1413m3_counter = 0;
	nb1413m3_sndromrgntag = "voice";
	nb1413m3_sndrombank1 = 0;
	nb1413m3_sndrombank2 = 0;
	nb1413m3_busyctr = 0;
	nb1413m3_busyflag = 1;
	nb1413m3_gfxradr_l = 0;
	nb1413m3_gfxradr_h = 0;
	nb1413m3_gfxrombank = 0;
	nb1413m3_inputport = 0xff;
	nb1413m3_outcoin_flag = 1;
}


WRITE8_HANDLER( nb1413m3_nmi_clock_w )
{
	nb1413m3_nmi_clock = data;

	switch (nb1413m3_type)
	{
		case NB1413M3_APPAREL:
		case NB1413M3_CITYLOVE:
		case NB1413M3_MCITYLOV:
		case NB1413M3_SECOLOVE:
		case NB1413M3_SEIHA:
		case NB1413M3_SEIHAM:
		case NB1413M3_IEMOTO:
		case NB1413M3_IEMOTOM:
		case NB1413M3_BIJOKKOY:
		case NB1413M3_BIJOKKOG:
		case NB1413M3_RYUUHA:
		case NB1413M3_OJOUSAN:
		case NB1413M3_OJOUSANM:
		case NB1413M3_KORINAI:
		case NB1413M3_KORINAIM:
		case NB1413M3_HOUSEMNQ:
		case NB1413M3_HOUSEMN2:
		case NB1413M3_LIVEGAL:
		case NB1413M3_ORANGEC:
		case NB1413M3_ORANGECI:
		case NB1413M3_VIPCLUB:
		case NB1413M3_MMSIKAKU:
		case NB1413M3_KANATUEN:
		case NB1413M3_KYUHITO:
			nb1413m3_nmi_clock -= 1;
			break;
#if 1
		case NB1413M3_NIGHTLOV:
			nb1413m3_nmi_enable = ((data & 0x08) >> 3);
			nb1413m3_nmi_enable |= ((data & 0x01) ^ 0x01);
			nb1413m3_nmi_clock -= 1;

			nb1413m3_sndrombank1 = 1;
			break;
#endif
	}

	nb1413m3_74ls193_counter = ((nb1413m3_nmi_clock & 0xf0) >> 4);

}

INTERRUPT_GEN( nb1413m3_interrupt )
{
	device->execute().set_input_line(0, HOLD_LINE);

#if NB1413M3_DEBUG
	popmessage("NMI SW:%01X CLOCK:%02X COUNT:%02X", nb1413m3_nmi_enable, nb1413m3_nmi_clock, nb1413m3_nmi_count);
	nb1413m3_nmi_count = 0;
#endif
}

READ8_HANDLER( nb1413m3_sndrom_r )
{
	int rombank;

	/* get top 8 bits of the I/O port address */
	offset = (offset << 8) | (space.device().state().state_int(Z80_BC) >> 8);

	switch (nb1413m3_type)
	{
		case NB1413M3_IEMOTO:
		case NB1413M3_IEMOTOM:
		case NB1413M3_SEIHA:
		case NB1413M3_SEIHAM:
		case NB1413M3_RYUUHA:
		case NB1413M3_OJOUSAN:
		case NB1413M3_OJOUSANM:
		case NB1413M3_MJSIKAKU:
		case NB1413M3_MMSIKAKU:
		case NB1413M3_KORINAI:
		case NB1413M3_KORINAIM:
			rombank = (nb1413m3_sndrombank2 << 1) + (nb1413m3_sndrombank1 & 0x01);
			break;
		case NB1413M3_HYHOO:
		case NB1413M3_HYHOO2:
			rombank = (nb1413m3_sndrombank1 & 0x01);
			break;
		case NB1413M3_APPAREL:      // no samples
		case NB1413M3_NIGHTLOV:     // 0-1
		case NB1413M3_SECOLOVE:     // 0-1
		case NB1413M3_CITYLOVE:     // 0-1
		case NB1413M3_MCITYLOV:     // 0-1
		case NB1413M3_HOUSEMNQ:     // 0-1
		case NB1413M3_HOUSEMN2:     // 0-1
		case NB1413M3_LIVEGAL:      // 0-1
		case NB1413M3_ORANGEC:      // 0-1
		case NB1413M3_ORANGECI:     // 0-1
		case NB1413M3_VIPCLUB:      // 0-1
		case NB1413M3_KAGUYA:       // 0-3
		case NB1413M3_KAGUYA2:      // 0-3 + 4-5 for protection
		case NB1413M3_BIJOKKOY:     // 0-7
		case NB1413M3_BIJOKKOG:     // 0-7
		case NB1413M3_OTONANO:      // 0-7
		case NB1413M3_MJCAMERA:     // 0 + 4-5 for protection
		case NB1413M3_IDHIMITU:     // 0 + 4-5 for protection
		case NB1413M3_KANATUEN:     // 0 + 6 for protection
			rombank = nb1413m3_sndrombank1;
			break;
		case NB1413M3_TAIWANMB:
		case NB1413M3_OMOTESND:
		case NB1413M3_SCANDAL:
		case NB1413M3_SCANDALM:
		case NB1413M3_MJFOCUSM:
		case NB1413M3_BANANADR:
			offset = (((offset & 0x7f00) >> 8) | ((offset & 0x0080) >> 0) | ((offset & 0x007f) << 8));
			rombank = (nb1413m3_sndrombank1 >> 1);
			break;
		case NB1413M3_MMCAMERA:
		case NB1413M3_MSJIKEN:
		case NB1413M3_HANAMOMO:
		case NB1413M3_TELMAHJN:
		case NB1413M3_GIONBANA:
		case NB1413M3_MGION:
		case NB1413M3_MGMEN89:
		case NB1413M3_MJFOCUS:
		case NB1413M3_GALKOKU:
		case NB1413M3_HYOUBAN:
		case NB1413M3_MJNANPAS:
		case NB1413M3_MLADYHTR:
		case NB1413M3_CLUB90S:
		case NB1413M3_OHPAIPEE:
		case NB1413M3_TOGENKYO:
		case NB1413M3_LOVEHOUS:
		case NB1413M3_CHINMOKU:
		case NB1413M3_GALKAIKA:
		case NB1413M3_MCONTEST:
		case NB1413M3_UCHUUAI:
		case NB1413M3_TOKIMBSJ:
		case NB1413M3_TOKYOGAL:
		case NB1413M3_MAIKO:
		case NB1413M3_MMAIKO:
		case NB1413M3_HANAOJI:
		case NB1413M3_PAIRSNB:
		case NB1413M3_PAIRSTEN:
		default:
			rombank = (nb1413m3_sndrombank1 >> 1);
			break;
	}

	offset += 0x08000 * rombank;

#if NB1413M3_DEBUG
	popmessage("Sound ROM %02X:%05X [B1:%02X B2:%02X]", rombank, offset, nb1413m3_sndrombank1, nb1413m3_sndrombank2);
#endif

	if (offset < space.machine().root_device().memregion(nb1413m3_sndromrgntag)->bytes())
		return space.machine().root_device().memregion(nb1413m3_sndromrgntag)->base()[offset];
	else
	{
		popmessage("read past sound ROM length (%05x[%02X])",offset, rombank);
		return 0;
	}
}

WRITE8_HANDLER( nb1413m3_sndrombank1_w )
{
	// if (data & 0x02) coin counter ?
	nb1413m3_outcoin_w(space, 0, data);             // (data & 0x04) >> 2;
	nb1413m3_nmi_enable = ((data & 0x20) >> 5);
	nb1413m3_sndrombank1 = (((data & 0xc0) >> 5) | ((data & 0x10) >> 4));
}

WRITE8_HANDLER( nb1413m3_sndrombank2_w )
{
	nb1413m3_sndrombank2 = (data & 0x03);
}

READ8_HANDLER( nb1413m3_gfxrom_r )
{
	UINT8 *GFXROM = space.machine().root_device().memregion("gfx1")->base();

	return GFXROM[(0x20000 * (nb1413m3_gfxrombank | ((nb1413m3_sndrombank1 & 0x02) << 3))) + ((0x0200 * nb1413m3_gfxradr_h) + (0x0002 * nb1413m3_gfxradr_l)) + (offset & 0x01)];
}

WRITE8_HANDLER( nb1413m3_gfxrombank_w )
{
	nb1413m3_gfxrombank = (((data & 0xc0) >> 4) + (data & 0x03));
}

WRITE8_HANDLER( nb1413m3_gfxradr_l_w )
{
	nb1413m3_gfxradr_l = data;
}

WRITE8_HANDLER( nb1413m3_gfxradr_h_w )
{
	nb1413m3_gfxradr_h = data;
}

WRITE8_HANDLER( nb1413m3_inputportsel_w )
{
	nb1413m3_inputport = data;
}

CUSTOM_INPUT( nb1413m3_busyflag_r )
{
	return nb1413m3_busyflag & 0x01;
}


/* 2008-08 FP:
 * In ALL games (but pastelg, hyhoo & hyhoo2) nb1413m3_outcoin_flag is read at inputport0.
 * However, a few games (lovehous, maiko, mmaiko, hanaoji and the ones using inputport3_r below)
 * read nb1413m3_outcoin_flag also at inputport3! Is this the correct behaviour for these games
 * or should they only check the flag at inputport3? */
CUSTOM_INPUT( nb1413m3_outcoin_flag_r )
{
	return nb1413m3_outcoin_flag & 0x01;
}

READ8_HANDLER( nb1413m3_inputport0_r )
{
	return ((space.machine().root_device().ioport("SYSTEM")->read() & 0xfd) | ((nb1413m3_outcoin_flag & 0x01) << 1));
}

READ8_HANDLER( nb1413m3_inputport1_r )
{
	device_t &root = space.machine().root_device();
	switch (nb1413m3_type)
	{
		case NB1413M3_HYHOO:
		case NB1413M3_HYHOO2:
			switch ((nb1413m3_inputport ^ 0xff) & 0x07)
			{
				case 0x01:  return root.ioport("IN0")->read();
				case 0x02:  return root.ioport("IN1")->read();
				case 0x04:  return 0xff;
				default:    return 0xff;
			}
			break;
		case NB1413M3_MSJIKEN:
		case NB1413M3_TELMAHJN:
			if (root.ioport("DSWA")->read() & 0x80)
			{
				switch ((nb1413m3_inputport ^ 0xff) & 0x1f)
				{
					case 0x01:  return root.ioport("KEY0")->read();
					case 0x02:  return root.ioport("KEY1")->read();
					case 0x04:  return root.ioport("KEY2")->read();
					case 0x08:  return root.ioport("KEY3")->read();
					case 0x10:  return root.ioport("KEY4")->read();
					default:    return (root.ioport("KEY0")->read() & root.ioport("KEY1")->read() & root.ioport("KEY2")->read()
										& root.ioport("KEY3")->read() & root.ioport("KEY4")->read());
				}
			}
			else return root.ioport("JAMMA2")->read();
			break;
		case NB1413M3_PAIRSNB:
		case NB1413M3_PAIRSTEN:
		case NB1413M3_OHPAIPEE:
		case NB1413M3_TOGENKYO:
			return root.ioport("P1")->read();
		default:
			switch ((nb1413m3_inputport ^ 0xff) & 0x1f)
			{
				case 0x01:  return root.ioport("KEY0")->read();
				case 0x02:  return root.ioport("KEY1")->read();
				case 0x04:  return root.ioport("KEY2")->read();
				case 0x08:  return root.ioport("KEY3")->read();
				case 0x10:  return root.ioport("KEY4")->read();
				default:    return (root.ioport("KEY0")->read() & root.ioport("KEY1")->read() & root.ioport("KEY2")->read()
									& root.ioport("KEY3")->read() & root.ioport("KEY4")->read());
			}
			break;
	}
}

READ8_HANDLER( nb1413m3_inputport2_r )
{
	device_t &root = space.machine().root_device();
	switch (nb1413m3_type)
	{
		case NB1413M3_HYHOO:
		case NB1413M3_HYHOO2:
			switch ((nb1413m3_inputport ^ 0xff) & 0x07)
			{
				case 0x01:  return 0xff;
				case 0x02:  return 0xff;
				case 0x04:  return root.ioport("IN2")->read();
				default:    return 0xff;
			}
			break;
		case NB1413M3_MSJIKEN:
		case NB1413M3_TELMAHJN:
			if (root.ioport("DSWA")->read() & 0x80)
			{
				switch ((nb1413m3_inputport ^ 0xff) & 0x1f)
				{
					case 0x01:  return root.ioport("KEY5")->read();
					case 0x02:  return root.ioport("KEY6")->read();
					case 0x04:  return root.ioport("KEY7")->read();
					case 0x08:  return root.ioport("KEY8")->read();
					case 0x10:  return root.ioport("KEY9")->read();
					default:    return (root.ioport("KEY5")->read() & root.ioport("KEY6")->read() & root.ioport("KEY7")->read()
										& root.ioport("KEY8")->read() & root.ioport("KEY9")->read());
				}
			}
			else return root.ioport("JAMMA1")->read();
			break;
		case NB1413M3_PAIRSNB:
		case NB1413M3_PAIRSTEN:
		case NB1413M3_OHPAIPEE:
		case NB1413M3_TOGENKYO:
			return root.ioport("P2")->read();
		default:
			switch ((nb1413m3_inputport ^ 0xff) & 0x1f)
			{
				case 0x01:  return root.ioport("KEY5")->read();
				case 0x02:  return root.ioport("KEY6")->read();
				case 0x04:  return root.ioport("KEY7")->read();
				case 0x08:  return root.ioport("KEY8")->read();
				case 0x10:  return root.ioport("KEY9")->read();
				default:    return (root.ioport("KEY5")->read() & root.ioport("KEY6")->read() & root.ioport("KEY7")->read()
									& root.ioport("KEY8")->read() & root.ioport("KEY9")->read());
			}
			break;
	}
}

READ8_HANDLER( nb1413m3_inputport3_r )
{
	switch (nb1413m3_type)
	{
		case NB1413M3_TAIWANMB:
		case NB1413M3_IEMOTOM:
		case NB1413M3_OJOUSANM:
		case NB1413M3_SEIHAM:
		case NB1413M3_RYUUHA:
		case NB1413M3_KORINAIM:
		case NB1413M3_HYOUBAN:
		case NB1413M3_TOKIMBSJ:
		case NB1413M3_MJFOCUSM:
		case NB1413M3_SCANDALM:
		case NB1413M3_BANANADR:
		case NB1413M3_FINALBNY:
		case NB1413M3_MMSIKAKU:
			return ((nb1413m3_outcoin_flag & 0x01) << 1);
		default:
			return 0xff;
	}
}

READ8_HANDLER( nb1413m3_dipsw1_r )
{
	device_t &root = space.machine().root_device();
	switch (nb1413m3_type)
	{
		case NB1413M3_KANATUEN:
		case NB1413M3_KYUHITO:
			return root.ioport("DSWB")->read();
		case NB1413M3_TAIWANMB:
			return ((root.ioport("DSWA")->read() & 0xf0) | ((root.ioport("DSWB")->read() & 0xf0) >> 4));
		case NB1413M3_OTONANO:
		case NB1413M3_MJCAMERA:
		case NB1413M3_IDHIMITU:
		case NB1413M3_KAGUYA2:
			return (((root.ioport("DSWA")->read() & 0x0f) << 4) | (root.ioport("DSWB")->read() & 0x0f));
		case NB1413M3_SCANDAL:
		case NB1413M3_SCANDALM:
		case NB1413M3_MJFOCUSM:
		case NB1413M3_GALKOKU:
		case NB1413M3_HYOUBAN:
		case NB1413M3_GALKAIKA:
		case NB1413M3_MCONTEST:
		case NB1413M3_UCHUUAI:
		case NB1413M3_TOKIMBSJ:
		case NB1413M3_TOKYOGAL:
			return ((root.ioport("DSWA")->read() & 0x0f) | ((root.ioport("DSWB")->read() & 0x0f) << 4));
		case NB1413M3_TRIPLEW1:
		case NB1413M3_NTOPSTAR:
		case NB1413M3_PSTADIUM:
		case NB1413M3_TRIPLEW2:
		case NB1413M3_VANILLA:
		case NB1413M3_FINALBNY:
		case NB1413M3_MJLSTORY:
		case NB1413M3_QMHAYAKU:
		case NB1413M3_MJGOTTUB:
			return (((root.ioport("DSWB")->read() & 0x01) >> 0) | ((root.ioport("DSWB")->read() & 0x04) >> 1) |
					((root.ioport("DSWB")->read() & 0x10) >> 2) | ((root.ioport("DSWB")->read() & 0x40) >> 3) |
					((root.ioport("DSWA")->read() & 0x01) << 4) | ((root.ioport("DSWA")->read() & 0x04) << 3) |
					((root.ioport("DSWA")->read() & 0x10) << 2) | ((root.ioport("DSWA")->read() & 0x40) << 1));
		default:
			return space.machine().root_device().ioport("DSWA")->read();
	}
}

READ8_HANDLER( nb1413m3_dipsw2_r )
{
	device_t &root = space.machine().root_device();
	switch (nb1413m3_type)
	{
		case NB1413M3_KANATUEN:
		case NB1413M3_KYUHITO:
			return root.ioport("DSWA")->read();
		case NB1413M3_TAIWANMB:
			return (((root.ioport("DSWA")->read() & 0x0f) << 4) | (root.ioport("DSWB")->read() & 0x0f));
		case NB1413M3_OTONANO:
		case NB1413M3_MJCAMERA:
		case NB1413M3_IDHIMITU:
		case NB1413M3_KAGUYA2:
			return ((root.ioport("DSWA")->read() & 0xf0) | ((root.ioport("DSWB")->read() & 0xf0) >> 4));
		case NB1413M3_SCANDAL:
		case NB1413M3_SCANDALM:
		case NB1413M3_MJFOCUSM:
		case NB1413M3_GALKOKU:
		case NB1413M3_HYOUBAN:
		case NB1413M3_GALKAIKA:
		case NB1413M3_MCONTEST:
		case NB1413M3_UCHUUAI:
		case NB1413M3_TOKIMBSJ:
		case NB1413M3_TOKYOGAL:
			return (((root.ioport("DSWA")->read() & 0xf0) >> 4) | (root.ioport("DSWB")->read() & 0xf0));
		case NB1413M3_TRIPLEW1:
		case NB1413M3_NTOPSTAR:
		case NB1413M3_PSTADIUM:
		case NB1413M3_TRIPLEW2:
		case NB1413M3_VANILLA:
		case NB1413M3_FINALBNY:
		case NB1413M3_MJLSTORY:
		case NB1413M3_QMHAYAKU:
		case NB1413M3_MJGOTTUB:
			return (((root.ioport("DSWB")->read() & 0x02) >> 1) | ((root.ioport("DSWB")->read() & 0x08) >> 2) |
					((root.ioport("DSWB")->read() & 0x20) >> 3) | ((root.ioport("DSWB")->read() & 0x80) >> 4) |
					((root.ioport("DSWA")->read() & 0x02) << 3) | ((root.ioport("DSWA")->read() & 0x08) << 2) |
					((root.ioport("DSWA")->read() & 0x20) << 1) | ((root.ioport("DSWA")->read() & 0x80) << 0));
		default:
			return space.machine().root_device().ioport("DSWB")->read();
	}
}

READ8_HANDLER( nb1413m3_dipsw3_l_r )
{
	return ((space.machine().root_device().ioport("DSWC")->read() & 0xf0) >> 4);
}

READ8_HANDLER( nb1413m3_dipsw3_h_r )
{
	return ((space.machine().root_device().ioport("DSWC")->read() & 0x0f) >> 0);
}

WRITE8_HANDLER( nb1413m3_outcoin_w )
{
	static int counter = 0;

	nb1413m3_outcoin_enable = (data & 0x04) >> 2;

	switch (nb1413m3_type)
	{
		case NB1413M3_TAIWANMB:
		case NB1413M3_IEMOTOM:
		case NB1413M3_OJOUSANM:
		case NB1413M3_SEIHAM:
		case NB1413M3_RYUUHA:
		case NB1413M3_KORINAIM:
		case NB1413M3_MMSIKAKU:
		case NB1413M3_HYOUBAN:
		case NB1413M3_TOKIMBSJ:
		case NB1413M3_MJFOCUSM:
		case NB1413M3_SCANDALM:
		case NB1413M3_BANANADR:
		case NB1413M3_MGION:
		case NB1413M3_HANAOJI:
		case NB1413M3_FINALBNY:
		case NB1413M3_LOVEHOUS:
		case NB1413M3_MMAIKO:
			if (nb1413m3_outcoin_enable)
			{
				if (counter++ == 2)
				{
					nb1413m3_outcoin_flag ^= 1;
					counter = 0;
				}
			}
			break;
		default:
			break;
	}

	set_led_status(space.machine(), 2, nb1413m3_outcoin_flag);      // out coin
}

WRITE8_HANDLER( nb1413m3_vcrctrl_w )
{
	if (data & 0x08)
	{
		popmessage(" ** VCR CONTROL ** ");
		set_led_status(space.machine(), 2, 1);
	}
	else
	{
		set_led_status(space.machine(), 2, 0);
	}
}

/* Nichibutsu Mahjong games share a common control panel */
INPUT_PORTS_START( nbmjcontrols )
	PORT_START("KEY0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_KAN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_M )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_I )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_E )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_A )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MAHJONG_BET )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_REACH )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_N )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_J )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_F )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_B )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_RON )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_CHI )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_K )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_G )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_C )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_PON )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_L )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_H )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_D )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MAHJONG_SMALL )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_BIG )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_FLIP_FLOP )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_DOUBLE_UP )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_SCORE )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_LAST_CHANCE )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_KAN ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_M ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_I ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_E ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_A ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MAHJONG_BET ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_REACH ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_N ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_J ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_F ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_B ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_RON ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_CHI ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_K ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_G ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_C ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_PON ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_L ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_H ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_D ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY9")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MAHJONG_SMALL ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_BIG ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_FLIP_FLOP ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_DOUBLE_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_SCORE ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_LAST_CHANCE ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

/* Hanafuda controls share part of the mahjong panel. Notice that some of the remaining
inputs are detected in Service Mode, even if we label them as IPT_UNKNOWN because they
do not correspond to actual inputs */
INPUT_PORTS_START( nbhf1_ctrl ) // used by gionbana, mgion, abunai
	PORT_START("KEY0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_E )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_A )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MAHJONG_BET )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_HANAFUDA_YES )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_F )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_B )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_HANAFUDA_NO )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_G )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_C )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_H )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_D )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_E ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_A ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MAHJONG_BET ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_HANAFUDA_YES ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_F ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_B ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_HANAFUDA_NO ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_G ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_C ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_H ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_D ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("KEY9")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

INPUT_PORTS_START( nbhf2_ctrl ) // used by maiko, hanaoji, hnxmasev and hnageman
	PORT_INCLUDE( nbhf1_ctrl )

	PORT_MODIFY("KEY0")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_HANAFUDA_YES )

	PORT_MODIFY("KEY1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_HANAFUDA_NO )

	PORT_MODIFY("KEY2")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("KEY5")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_HANAFUDA_YES ) PORT_PLAYER(2)

	PORT_MODIFY("KEY6")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_HANAFUDA_NO ) PORT_PLAYER(2)

	PORT_MODIFY("KEY7")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END
