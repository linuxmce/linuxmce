// Preliminary X68000 driver for MESS
// Started 18/11/2006
// Written by Barry Rodewald

/*
    *** Basic memory map

    0x000000 - 0xbfffff     RAM (Max 12MB), vector table from ROM at 0xff0000 maps to 0x000000 at reset only
    0xc00000 - 0xdfffff     Graphic VRAM
    0xe00000 - 0xe1ffff     Text VRAM Plane 1
    0xe20000 - 0xe3ffff     Text VRAM Plane 2
    0xe40000 - 0xe5ffff     Text VRAM Plane 3
    0xe60000 - 0xe7ffff     Text VRAM Plane 4
    0xe80000                CRTC
    0xe82000                Video Controller
    0xe84000                DMA Controller
    0xe86000                Supervisor Area set
    0xe88000                MFP
    0xe8a000                RTC
    0xe8c000                Printer
    0xe8e000                System Port (?)
    0xe90000                FM Sound source
    0xe92000                ADPCM
    0xe94000                FDC
    0xe96000                HDC
    0xe96021                SCSI (internal model)
    0xe98000                SCC
    0xe9a000                Serial I/O (PPI)
    0xe9c000                I/O controller

    [Expansions]
    0xe9c000 / 0xe9e000     FPU (Optional, X68000 only)
    0xea0000                SCSI
    0xeaf900                FAX
    0xeafa00 / 0xeafa10     MIDI (1st/2nd)
    0xeafb00                Serial
    0xeafc00/10/20/30       EIA232E
    0xeafd00                EIA232E
    0xeafe00                GPIB (?)
    0xec0000 - 0xecffff     User I/O Expansion

    0xeb0000 - 0xeb7fff     Sprite registers
    0xeb8000 - 0xebffff     Sprite VRAM
    0xed0000 - 0xed3fff     SRAM
    0xf00000 - 0xfb0000     ROM  (CGROM.DAT)
    0xfe0000 - 0xffffff     ROM  (IPLROM.DAT)


    *** System hardware

    CPU : X68000: 68000 at 10MHz
          X68000 XVI: 68000 at 16MHz
          X68030: 68EC030 at 25MHz

    RAM : between 1MB and 4MB stock, expandable to 12MB

    FDD : 2x 5.25", Compact models use 2x 3.5" drives.
    FDC : NEC uPD72065

    HDD : HD models have up to an 81MB HDD.
    HDC : Fujitsu MB89352A (SCSI)

    SCC : Serial controller - Zilog z85C30  (Dual channel, 1 for RS232, 1 for mouse)
    PPI : Parallel controller  - NEC 8255   (Printer, Joystick)

    Sound : FM    - YM2151, with YM3012 DAC
            ADPCM - Okidata MSM6258

    DMA : Hitachi HD63450, DMA I/O for FDD, HDD, Expansion slots, and ADPCM

    MFP : Motorola MC68901 - monitor sync, serial port, RTC, soft power, FM synth, IRQs, keyboard

    RTC : Ricoh RP5C15

    ...plus a number of custom chips for video and other stuff...


    *** Current status (28/12/08)
    MFP : Largely works, as far as the X68000 goes.

    PPI : Joystick controls work okay.

    HDC/HDD : SCSI is not implemented, not a requirement at this point.

    RTC : Seems to work. (Tested using SX-Window's Timer application)

    DMA : Works fine.

    Sound : FM works, ADPCM mostly works (timing(?) issues in a few games).

    SCC : Works enough to get the mouse running, although only with the IPL v1.0 BIOS

    Video : Text mode works, but is rather slow, especially scrolling up (uses multple "raster copy" commands).
            Graphic layers work.
            BG tiles and sprites work, but many games have the sprites offset by a small amount (some by a lot :))
            Still a few minor priority issues around.

    Other issues:
      Bus error exceptions are a bit late at times.  Currently using a fake bus error for MIDI expansion checks.  These
      are used determine if a piece of expansion hardware is present.
      Keyboard doesn't work properly (MFP USART).
      Supervisor area set isn't implemented.

    Some minor game-specific issues (at 28/12/08):
      Pacmania:      Black squares on the maze (transparency?).
      Salamander:    System error when using keys in-game.  No error if a joystick is used.
                     Some text is drawn incorrectly.
      Dragon Buster: Text is black and unreadable. (Text layer actually covers it)
      Tetris:        Black dots over screen (text layer).
      Parodius Da!:  Black squares in areas.


    More detailed documentation at http://x68kdev.emuvibes.com/iomap.html - if you can stand broken english :)

*/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "machine/i8255.h"
#include "machine/mc68901.h"
#include "machine/upd765.h"
#include "sound/2151intf.h"
#include "sound/okim6258.h"
#include "machine/8530scc.h"
#include "machine/hd63450.h"
#include "machine/rp5c15.h"
#include "machine/mb89352.h"
#include "formats/xdf_dsk.h"
#include "machine/x68k_hdc.h"
#include "includes/x68k.h"
#include "machine/ram.h"
#include "machine/nvram.h"
#include "machine/x68kexp.h"
#include "machine/x68k_neptunex.h"
#include "machine/x68k_scsiext.h"
#include "machine/scsibus.h"
#include "machine/scsihd.h"
#include "x68000.lh"



//emu_timer* mfp_timer[4];
//emu_timer* mfp_irq;

// MFP is clocked at 4MHz, so at /4 prescaler the timer is triggered after 1us (4 cycles)
// No longer necessary with the new MFP core
#ifdef UNUSED_FUNCTION
static attotime prescale(int val)
{
	switch(val)
	{
		case 0: return attotime::from_nsec(0);
		case 1: return attotime::from_nsec(1000);
		case 2: return attotime::from_nsec(2500);
		case 3: return attotime::from_nsec(4000);
		case 4: return attotime::from_nsec(12500);
		case 5: return attotime::from_nsec(16000);
		case 6: return attotime::from_nsec(25000);
		case 7: return attotime::from_nsec(50000);
		default:
			fatalerror("out of range\n");
	}
}
#endif

void x68k_state::mfp_init()
{
	m_mfp.tadr = m_mfp.tbdr = m_mfp.tcdr = m_mfp.tddr = 0xff;

	m_mfp.irqline = 6;  // MFP is connected to 68000 IRQ line 6
	m_mfp.current_irq = -1;  // No current interrupt

#if 0
	mfp_timer[0] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::mfp_timer_a_callback),this));
	mfp_timer[1] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::mfp_timer_b_callback),this));
	mfp_timer[2] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::mfp_timer_c_callback),this));
	mfp_timer[3] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::mfp_timer_d_callback),this));
	mfp_irq = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::mfp_update_irq),this));
	mfp_irq->adjust(attotime::zero, 0, attotime::from_usec(32));
#endif
}

#ifdef UNUSED_FUNCTION
TIMER_CALLBACK_MEMBER(x68k_state::mfp_update_irq)
{
	int x;

	if((m_ioc.irqstatus & 0xc0) != 0)
		return;

	// check for pending IRQs, in priority order
	if(m_mfp.ipra != 0)
	{
		for(x=7;x>=0;x--)
		{
			if((m_mfp.ipra & (1 << x)) && (m_mfp.imra & (1 << x)))
			{
				m_current_irq_line = m_mfp.irqline;
				m_mfp.current_irq = x + 8;
				// assert IRQ line
//              if(m_mfp.iera & (1 << x))
				{
					m_current_vector[6] = (m_mfp.vr & 0xf0) | (x+8);
					machine().device("maincpu")->execute().set_input_line_and_vector(m_mfp.irqline,ASSERT_LINE,(m_mfp.vr & 0xf0) | (x + 8));
//                  logerror("MFP: Sent IRQ vector 0x%02x (IRQ line %i)\n",(m_mfp.vr & 0xf0) | (x+8),m_mfp.irqline);
					return;  // one at a time only
				}
			}
		}
	}
	if(m_mfp.iprb != 0)
	{
		for(x=7;x>=0;x--)
		{
			if((m_mfp.iprb & (1 << x)) && (m_mfp.imrb & (1 << x)))
			{
				m_current_irq_line = m_mfp.irqline;
				m_mfp.current_irq = x;
				// assert IRQ line
//              if(m_mfp.ierb & (1 << x))
				{
					m_current_vector[6] = (m_mfp.vr & 0xf0) | x;
					machine().device("maincpu")->execute().set_input_line_and_vector(m_mfp.irqline,ASSERT_LINE,(m_mfp.vr & 0xf0) | x);
//                  logerror("MFP: Sent IRQ vector 0x%02x (IRQ line %i)\n",(m_mfp.vr & 0xf0) | x,m_mfp.irqline);
					return;  // one at a time only
				}
			}
		}
	}
}

void mfp_trigger_irq(running_machine &machine, int irq)
{
	x68k_state *state = machine.driver_data<x68k_state>();
	// check if interrupt is enabled
	if(irq > 7)
	{
		if(!(state->m_mfp.iera & (1 << (irq-8))))
			return;  // not enabled, no action taken
	}
	else
	{
		if(!(state->m_mfp.ierb & (1 << irq)))
			return;  // not enabled, no action taken
	}

	// set requested IRQ as pending
	if(irq > 7)
		state->m_mfp.ipra |= (1 << (irq-8));
	else
		state->m_mfp.iprb |= (1 << irq);

	// check for IRQs to be called
//  mfp_update_irq(0);

}

TIMER_CALLBACK_MEMBER(x68k_state::mfp_timer_a_callback)
{
	m_mfp.timer[0].counter--;
	if(m_mfp.timer[0].counter == 0)
	{
		m_mfp.timer[0].counter = m_mfp.tadr;
		mfp_trigger_irq(MFP_IRQ_TIMERA);
	}
}

TIMER_CALLBACK_MEMBER(x68k_state::mfp_timer_b_callback)
{
	m_mfp.timer[1].counter--;
	if(m_mfp.timer[1].counter == 0)
	{
		m_mfp.timer[1].counter = m_mfp.tbdr;
			mfp_trigger_irq(MFP_IRQ_TIMERB);
	}
}

TIMER_CALLBACK_MEMBER(x68k_state::mfp_timer_c_callback)
{
	m_mfp.timer[2].counter--;
	if(m_mfp.timer[2].counter == 0)
	{
		m_mfp.timer[2].counter = m_mfp.tcdr;
			mfp_trigger_irq(MFP_IRQ_TIMERC);
	}
}

TIMER_CALLBACK_MEMBER(x68k_state::mfp_timer_d_callback)
{
	m_mfp.timer[3].counter--;
	if(m_mfp.timer[3].counter == 0)
	{
		m_mfp.timer[3].counter = m_mfp.tddr;
			mfp_trigger_irq(MFP_IRQ_TIMERD);
	}
}

void mfp_set_timer(int timer, unsigned char data)
{
	if((data & 0x07) == 0x0000)
	{  // Timer stop
		mfp_timer[timer]->adjust(attotime::zero);
		logerror("MFP: Timer #%i stopped. \n",timer);
		return;
	}

	mfp_timer[timer]->adjust(attotime::zero, 0, prescale(data & 0x07));
	logerror("MFP: Timer #%i set to %2.1fus\n",timer, prescale(data & 0x07).as_double() * 1000000);

}
#endif

// LED timer callback
TIMER_CALLBACK_MEMBER(x68k_state::x68k_led_callback)
{
	int drive;
	if(m_led_state == 0)
		m_led_state = 1;
	else
		m_led_state = 0;
	if(m_led_state == 1)
	{
		for(drive=0;drive<4;drive++)
			output_set_indexed_value("ctrl_drv",drive,m_fdc.led_ctrl[drive] ? 0 : 1);
	}
	else
	{
		for(drive=0;drive<4;drive++)
			output_set_indexed_value("ctrl_drv",drive,1);
	}

}

// 4 channel DMA controller (Hitachi HD63450)
WRITE16_MEMBER(x68k_state::x68k_dmac_w)
{
	device_t* device = machine().device("hd63450");
	hd63450_w(device, space, offset, data, mem_mask);
}

READ16_MEMBER(x68k_state::x68k_dmac_r)
{
	device_t* device = machine().device("hd63450");
	return hd63450_r(device, space, offset, mem_mask);
}

void x68k_state::x68k_keyboard_ctrl_w(int data)
{
	/* Keyboard control commands:
	   00xxxxxx - TV Control
	              Not of much use as yet

	   01000xxy - y = Mouse control signal

	   01001xxy - y = Keyboard enable

	   010100xy - y = Sharp X1 display compatibility mode

	   010101xx - xx = LED brightness (00 = bright, 11 = dark)

	   010110xy - y = Display control enable

	   010111xy - y = Display control via the Opt. 2 key enable

	   0110xxxx - xxxx = Key delay (default 500ms)
	                     100 * (delay time) + 200ms

	   0111xxxx - xxxx = Key repeat rate  (default 110ms)
	                     (repeat rate)^2*5 + 30ms

	   1xxxxxxx - xxxxxxx = keyboard LED status
	              b6 = "full size"
	              b5 = hiragana
	              b4 = insert
	              b3 = caps
	              b2 = code input
	              b1 = romaji input
	              b0 = kana
	*/

	if(data & 0x80)  // LED status
	{
		output_set_value("key_led_kana",(data & 0x01) ? 0 : 1);
		output_set_value("key_led_romaji",(data & 0x02) ? 0 : 1);
		output_set_value("key_led_code",(data & 0x04) ? 0 : 1);
		output_set_value("key_led_caps",(data & 0x08) ? 0 : 1);
		output_set_value("key_led_insert",(data & 0x10) ? 0 : 1);
		output_set_value("key_led_hiragana",(data & 0x20) ? 0 : 1);
		output_set_value("key_led_fullsize",(data & 0x40) ? 0 : 1);
		logerror("KB: LED status set to %02x\n",data & 0x7f);
	}

	if((data & 0xc0) == 0)  // TV control
	{
		// nothing for now
	}

	if((data & 0xf8) == 0x48)  // Keyboard enable
	{
		m_keyboard.enabled = data & 0x01;
		logerror("KB: Keyboard enable bit = %i\n",m_keyboard.enabled);
	}

	if((data & 0xf0) == 0x60)  // Key delay time
	{
		m_keyboard.delay = data & 0x0f;
		logerror("KB: Keypress delay time is now %ims\n",(data & 0x0f)*100+200);
	}

	if((data & 0xf0) == 0x70)  // Key repeat rate
	{
		m_keyboard.repeat = data & 0x0f;
		logerror("KB: Keypress repeat rate is now %ims\n",((data & 0x0f)^2)*5+30);
	}

}

int x68k_state::x68k_keyboard_pop_scancode()
{
	int ret;
	if(m_keyboard.keynum == 0)  // no scancodes in USART buffer
		return 0x00;

	m_keyboard.keynum--;
	ret = m_keyboard.buffer[m_keyboard.tailpos++];
	if(m_keyboard.tailpos > 15)
		m_keyboard.tailpos = 0;

	logerror("MFP: Keyboard buffer pop 0x%02x\n",ret);
	return ret;
}

void x68k_state::x68k_keyboard_push_scancode(unsigned char code)
{
	m_keyboard.keynum++;
	if(m_keyboard.keynum >= 1)
	{ // keyboard buffer full
		if(m_keyboard.enabled != 0)
		{
			m_mfp.rsr |= 0x80;  // Buffer full
//          mfp_trigger_irq(MFP_IRQ_RX_FULL);
			if(machine().root_device().ioport("options")->read() & 0x01)
			{
				m_current_vector[6] = 0x4c;
				machine().device("maincpu")->execute().set_input_line_and_vector(6,ASSERT_LINE,0x4c);
				logerror("MFP: Receive buffer full IRQ sent\n");
			}
		}
	}
	m_keyboard.buffer[m_keyboard.headpos++] = code;
	if(m_keyboard.headpos > 15)
	{
		m_keyboard.headpos = 0;
//      mfp_trigger_irq(MFP_IRQ_RX_ERROR);
		m_current_vector[6] = 0x4b;
//      machine.device("maincpu")->execute().set_input_line_and_vector(6,ASSERT_LINE,0x4b);
	}
}

TIMER_CALLBACK_MEMBER(x68k_state::x68k_keyboard_poll)
{
	int x;
	static const char *const keynames[] = { "key1", "key2", "key3", "key4" };

	for(x=0;x<0x80;x++)
	{
		// adjust delay/repeat timers
		if(m_keyboard.keytime[x] > 0)
		{
			m_keyboard.keytime[x] -= 5;
		}
		if(!(machine().root_device().ioport(keynames[x / 32])->read() & (1 << (x % 32))))
		{
			if(m_keyboard.keyon[x] != 0)
			{
				x68k_keyboard_push_scancode(0x80 + x);
				m_keyboard.keytime[x] = 0;
				m_keyboard.keyon[x] = 0;
				m_keyboard.last_pressed = 0;
				logerror("KB: Released key 0x%02x\n",x);
			}
		}
		// check to see if a key is being held
		if(m_keyboard.keyon[x] != 0 && m_keyboard.keytime[x] == 0 && m_keyboard.last_pressed == x)
		{
			if(machine().root_device().ioport(keynames[m_keyboard.last_pressed / 32])->read() & (1 << (m_keyboard.last_pressed % 32)))
			{
				x68k_keyboard_push_scancode(m_keyboard.last_pressed);
				m_keyboard.keytime[m_keyboard.last_pressed] = (m_keyboard.repeat^2)*5+30;
				logerror("KB: Holding key 0x%02x\n",m_keyboard.last_pressed);
			}
		}
		if((machine().root_device().ioport(keynames[x / 32])->read() & (1 << (x % 32))))
		{
			if(m_keyboard.keyon[x] == 0)
			{
				x68k_keyboard_push_scancode(x);
				m_keyboard.keytime[x] = m_keyboard.delay * 100 + 200;
				m_keyboard.keyon[x] = 1;
				m_keyboard.last_pressed = x;
				logerror("KB: Pushed key 0x%02x\n",x);
			}
		}
	}
}


#ifdef UNUSED_FUNCTION
void mfp_recv_data(int data)
{
	x68k_state *state = machine.driver_data<x68k_state>();
	state->m_mfp.rsr |= 0x80;  // Buffer full
	state->m_mfp.tsr |= 0x80;
	state->m_mfp.usart.recv_buffer = 0x00;   // TODO: set up keyboard data
	state->m_mfp.vector = state->m_current_vector[6] = (state->m_mfp.vr & 0xf0) | 0x0c;
//  mfp_trigger_irq(MFP_IRQ_RX_FULL);
//  logerror("MFP: Receive buffer full IRQ sent\n");
}
#endif

// mouse input
// port B of the Z8530 SCC
// typically read from the SCC data port on receive buffer full interrupt per byte
int x68k_state::x68k_read_mouse()
{
	scc8530_t *scc = machine().device<scc8530_t>("scc");
	char val = 0;
	char ipt = 0;

	if(!(scc->get_reg_b(5) & 0x02))
		return 0xff;

	switch(m_mouse.inputtype)
	{
	case 0:
		ipt = machine().root_device().ioport("mouse1")->read();
		break;
	case 1:
		val = machine().root_device().ioport("mouse2")->read();
		ipt = val - m_mouse.last_mouse_x;
		m_mouse.last_mouse_x = val;
		break;
	case 2:
		val = machine().root_device().ioport("mouse3")->read();
		ipt = val - m_mouse.last_mouse_y;
		m_mouse.last_mouse_y = val;
		break;
	}
	m_mouse.inputtype++;
	if(m_mouse.inputtype > 2)
	{
		int i_val = scc->get_reg_b(0);
		m_mouse.inputtype = 0;
		m_mouse.bufferempty = 1;
		i_val &= ~0x01;
		scc->set_reg_b(0, i_val);
		logerror("SCC: mouse buffer empty\n");
	}

	return ipt;
}

/*
    0xe98001 - Z8530 command port B
    0xe98003 - Z8530 data port B  (mouse input)
    0xe98005 - Z8530 command port A
    0xe98007 - Z8530 data port A  (RS232)
*/
READ16_MEMBER(x68k_state::x68k_scc_r )
{
	scc8530_t *scc = machine().device<scc8530_t>("scc");
	offset %= 4;
	switch(offset)
	{
	case 0:
		return scc->reg_r(space, 0);
	case 1:
		return x68k_read_mouse();
	case 2:
		return scc->reg_r(space, 1);
	case 3:
		return scc->reg_r(space, 3);
	default:
		return 0xff;
	}
}

WRITE16_MEMBER(x68k_state::x68k_scc_w )
{
	scc8530_t *scc = machine().device<scc8530_t>("scc");
	offset %= 4;

	switch(offset)
	{
	case 0:
		scc->reg_w(space, 0,(UINT8)data);
		if((scc->get_reg_b(5) & 0x02) != m_scc_prev)
		{
			if(scc->get_reg_b(5) & 0x02)  // Request to Send
			{
				int val = scc->get_reg_b(0);
				m_mouse.bufferempty = 0;
				val |= 0x01;
				scc->set_reg_b(0,val);
			}
		}
		break;
	case 1:
		scc->reg_w(space, 2,(UINT8)data);
		break;
	case 2:
		scc->reg_w(space, 1,(UINT8)data);
		break;
	case 3:
		scc->reg_w(space, 3,(UINT8)data);
		break;
	}
	m_scc_prev = scc->get_reg_b(5) & 0x02;
}

TIMER_CALLBACK_MEMBER(x68k_state::x68k_scc_ack)
{
	scc8530_t *scc = machine().device<scc8530_t>("scc");
	if(m_mouse.bufferempty != 0)  // nothing to do if the mouse data buffer is empty
		return;

//  if((m_ioc.irqstatus & 0xc0) != 0)
//      return;

	// hard-code the IRQ vector for now, until the SCC code is more complete
	if((scc->get_reg_a(9) & 0x08) || (scc->get_reg_b(9) & 0x08))  // SCC reg WR9 is the same for both channels
	{
		if((scc->get_reg_b(1) & 0x18) != 0)  // if bits 3 and 4 of WR1 are 0, then Rx IRQs are disabled on this channel
		{
			if(scc->get_reg_b(5) & 0x02)  // RTS signal
			{
				m_mouse.irqactive = 1;
				m_current_vector[5] = 0x54;
				m_current_irq_line = 5;
				machine().device("maincpu")->execute().set_input_line_and_vector(5,ASSERT_LINE,0x54);
			}
		}
	}
}

void x68k_state::x68k_set_adpcm()
{
	device_t *dev = machine().device("hd63450");
	UINT32 rate = 0;

	switch(m_adpcm.rate & 0x0c)
	{
		case 0x00:
			rate = 7812/2;
			break;
		case 0x04:
			rate = 10417/2;
			break;
		case 0x08:
			rate = 15625/2;
			break;
		default:
			logerror("PPI: Invalid ADPCM sample rate set.\n");
			rate = 15625/2;
	}
	if(m_adpcm.clock != 0)
		rate = rate/2;
	hd63450_set_timer(dev,3,attotime::from_hz(rate));
}

// Megadrive 3 button gamepad
// According to XM6, bits 4 and 7 are always 1 (is this correct?)
// Bits 4 and 5 of PPI port C control each controller's multiplexer
// Button inputs (Start, A, B and C) are read in bits 5 and 6 (rather than 4
// and 5 like on a Megadrive)

UINT8 x68k_state::md_3button_r(int port)
{
	if(port == 1)
	{
		UINT8 porta = machine().root_device().ioport("md3b")->read() & 0xff;
		UINT8 portb = (ioport("md3b")->read() >> 8) & 0xff;
		if(m_mdctrl.mux1 & 0x10)
		{
			return porta | 0x90;
		}
		else
		{
			return (portb & 0x60) | (porta & 0x03) | 0x90;
		}
	}
	if(port == 2)
	{
		UINT8 porta = (machine().root_device().ioport("md3b")->read() >> 16) & 0xff;
		UINT8 portb = (machine().root_device().ioport("md3b")->read() >> 24) & 0xff;
		if(m_mdctrl.mux2 & 0x20)
		{
			return porta | 0x90;
		}
		else
		{
			return (portb & 0x60) | (porta & 0x03) | 0x90;
		}
	}
	return 0xff;
}

// Megadrive 6 button gamepad
TIMER_CALLBACK_MEMBER(x68k_state::md_6button_port1_timeout)
{
	m_mdctrl.seq1 = 0;
}

TIMER_CALLBACK_MEMBER(x68k_state::md_6button_port2_timeout)
{
	m_mdctrl.seq2 = 0;
}

void x68k_state::md_6button_init()
{
	m_mdctrl.io_timeout1 = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::md_6button_port1_timeout),this));
	m_mdctrl.io_timeout2 = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::md_6button_port2_timeout),this));
}

UINT8 x68k_state::md_6button_r(int port)
{
	if(port == 1)
	{
		UINT8 porta = machine().root_device().ioport("md6b")->read() & 0xff;
		UINT8 portb = (machine().root_device().ioport("md6b")->read() >> 8) & 0xff;
		UINT8 extra = ioport("md6b_extra")->read() & 0x0f;

		switch(m_mdctrl.seq1)
		{
			case 1:
			default:
				if(m_mdctrl.mux1 & 0x10)
				{
					return porta | 0x90;
				}
				else
				{
					return (portb & 0x60) | (porta & 0x03) | 0x90;
				}
			case 2:
				if(m_mdctrl.mux1 & 0x10)
				{
					return porta | 0x90;
				}
				else
				{
					return (portb & 0x60) | 0x90;
				}
			case 3:
				if(m_mdctrl.mux1 & 0x10)
				{
					return (porta & 0x60) | (extra & 0x0f) | 0x90;
				}
				else
				{
					return (portb & 0x60) | 0x9f;
				}
		}
	}
	if(port == 2)
	{
		UINT8 porta = (machine().root_device().ioport("md6b")->read() >> 16) & 0xff;
		UINT8 portb = (machine().root_device().ioport("md6b")->read() >> 24) & 0xff;
		UINT8 extra = (machine().root_device().ioport("md6b_extra")->read() >> 4) & 0x0f;

		switch(m_mdctrl.seq2)
		{
			case 1:
			default:
				if(m_mdctrl.mux2 & 0x20)
				{
					return porta | 0x90;
				}
				else
				{
					return (portb & 0x60) | (porta & 0x03) | 0x90;
				}
			case 2:
				if(m_mdctrl.mux2 & 0x20)
				{
					return porta | 0x90;
				}
				else
				{
					return (portb & 0x60) | 0x90;
				}
			case 3:
				if(m_mdctrl.mux2 & 0x20)
				{
					return (porta & 0x60) | (extra & 0x0f) | 0x90;
				}
				else
				{
					return (portb & 0x60) | 0x9f;
				}
		}
	}
	return 0xff;
}

// XPD-1LR dual D-pad controller.
// Sold with Video Game Anthology Vol 4: Libble Rabble.
// Also compatible with Video Game Anthology Vol 5: Crazy Climber 1 & 2
// Uses the same input multiplexer hardware as Megadrive controllers
// Output is the same as for standard controllers, but when ctl is high,
// the directions refer to the right D-pad, and when low, the left D-pad
// The buttons are read the same as normal, regardless of ctl.
UINT8 x68k_state::xpd1lr_r(int port)
{
	if(port == 1)
	{
		UINT8 porta = machine().root_device().ioport("xpd1lr")->read() & 0xff;
		UINT8 portb = (ioport("xpd1lr")->read() >> 8) & 0xff;
		if(m_mdctrl.mux1 & 0x10)
		{
			return porta;
		}
		else
		{
			return portb | (porta & 0x60);
		}
	}
	if(port == 2)
	{
		UINT8 porta = (machine().root_device().ioport("xpd1lr")->read() >> 16) & 0xff;
		UINT8 portb = (machine().root_device().ioport("xpd1lr")->read() >> 24) & 0xff;
		if(m_mdctrl.mux2 & 0x20)
		{
			return porta;
		}
		else
		{
			return portb | (porta & 0x60);
		}
	}
	return 0xff;
}

// Judging from the XM6 source code, PPI ports A and B are joystick inputs
READ8_MEMBER(x68k_state::ppi_port_a_r)
{
	int ctrl = machine().root_device().ioport("ctrltype")->read() & 0x0f;

	switch(ctrl)
	{
		case 0x00:  // standard MSX/FM-Towns joystick
			if(m_joy.joy1_enable == 0)
				return ioport("joy1")->read();
			else
				return 0xff;
		case 0x01:  // 3-button Megadrive gamepad
			return md_3button_r(1);
		case 0x02:  // 6-button Megadrive gamepad
			return md_6button_r(1);
		case 0x03:  // XPD-1LR
			return xpd1lr_r(1);
	}

	return 0xff;
}

READ8_MEMBER(x68k_state::ppi_port_b_r)
{
	int ctrl = machine().root_device().ioport("ctrltype")->read() & 0xf0;

	switch(ctrl)
	{
		case 0x00:  // standard MSX/FM-Towns joystick
			if(m_joy.joy2_enable == 0)
				return ioport("joy2")->read();
			else
				return 0xff;
		case 0x10:  // 3-button Megadrive gamepad
			return md_3button_r(2);
		case 0x20:  // 6-button Megadrive gamepad
			return md_6button_r(2);
		case 0x30:  // XPD-1LR
			return xpd1lr_r(2);
	}

	return 0xff;
}

READ8_MEMBER(x68k_state::ppi_port_c_r)
{
	return m_ppi_port[2];
}

/* PPI port C (Joystick control, R/W)
   bit 7    - IOC7 - Function B operation of joystick 1 (?)
   bit 6    - IOC6 - Function A operation of joystick 1 (?)
   bit 5    - IOC5 - Enable Joystick 2
   bit 4    - IOC4 - Enable Joystick 1
   bits 3,2 - ADPCM Sample rate
   bits 1,0 - ADPCM Pan
*/
WRITE8_MEMBER(x68k_state::ppi_port_c_w)
{
	// ADPCM / Joystick control
	device_t *oki = machine().device("okim6258");

	m_ppi_port[2] = data;
	if((data & 0x0f) != (m_ppi_prev & 0x0f))
	{
		m_adpcm.pan = data & 0x03;
		m_adpcm.rate = data & 0x0c;
		x68k_set_adpcm();
		okim6258_set_divider(oki, (data >> 2) & 3);
	}

	// The joystick enable bits also handle the multiplexer for various controllers
	m_joy.joy1_enable = data & 0x10;
	m_mdctrl.mux1 = data & 0x10;
	if((m_ppi_prev & 0x10) == 0x00 && (data & 0x10) == 0x10)
	{
		m_mdctrl.seq1++;
		m_mdctrl.io_timeout1->adjust(machine().device<cpu_device>("maincpu")->cycles_to_attotime(8192));
	}

	m_joy.joy2_enable = data & 0x20;
	m_mdctrl.mux2 = data & 0x20;
	if((m_ppi_prev & 0x20) == 0x00 && (data & 0x20) == 0x20)
	{
		m_mdctrl.seq2++;
		m_mdctrl.io_timeout2->adjust(machine().device<cpu_device>("maincpu")->cycles_to_attotime(8192));
	}
	m_ppi_prev = data;

	m_joy.ioc6 = data & 0x40;
	m_joy.ioc7 = data & 0x80;
}


// NEC uPD72065 at 0xe94000
WRITE16_MEMBER(x68k_state::x68k_fdc_w)
{
	unsigned int drive, x;
	switch(offset)
	{
	case 0x00:  // drive option signal control
		x = data & 0x0f;
		for(drive=0;drive<4;drive++)
		{
			if(m_fdc.selected_drive & (1 << drive))
			{
				if(!(x & (1 << drive)))  // functions take place on 1->0 transitions of drive bits only
				{
					m_fdc.led_ctrl[drive] = data & 0x80;  // blinking drive LED if no disk inserted
					m_fdc.led_eject[drive] = data & 0x40;  // eject button LED (on when set to 0)
					output_set_indexed_value("eject_drv",drive,(data & 0x40) ? 1 : 0);
					if(data & 0x20)  // ejects disk
					{
						m_fdc.floppy[drive]->mon_w(false);
						m_fdc.floppy[drive]->unload();
					}
				}
			}
		}
		m_fdc.selected_drive = data & 0x0f;
		logerror("FDC: signal control set to %02x\n",data);
		break;
	case 0x01: {
		static const int rates[4] = { 500000, 300000, 250000, 125000 };
		m_fdc.fdc->set_rate(rates[(data >> 4) & 3]);
		m_fdc.motor[data & 0x03] = data & 0x80;
		m_fdc.floppy[data & 0x03]->mon_w(!BIT(data, 7));
		if(data & 0x80)
		{
			for(drive=0;drive<4;drive++) // enable motor for this drive
			{
				if(drive == (data & 0x03))
				{
					m_fdc.floppy[drive]->mon_w(false);
					output_set_indexed_value("access_drv",drive,0);
				}
				else
					output_set_indexed_value("access_drv",drive,1);
			}
		}
		else    // BIOS code suggests that setting bit 7 of this port to 0 disables the motor of all floppy drives
		{
			for(drive=0;drive<4;drive++)
			{
				m_fdc.floppy[drive]->mon_w(true);
				output_set_indexed_value("access_drv",drive,1);
			}
		}
		logerror("FDC: Drive #%i: Drive selection set to %02x\n",data & 0x03,data);
		break;
	}
	}
}

READ16_MEMBER(x68k_state::x68k_fdc_r)
{
	unsigned int ret;
	int x;

	switch(offset)
	{
	case 0x00:
		ret = 0x00;
		for(x=0;x<4;x++)
		{
			if(m_fdc.selected_drive & (1 << x))
			{
				ret = 0x00;
				if(m_fdc.floppy[x]->exists())
				{
					ret |= 0x80;
				}
				// bit 7 = disk inserted
				// bit 6 = disk error (in insertion, presumably)
				logerror("FDC: Drive #%i Disk check - returning %02x\n",x,ret);
			}
		}
		return ret;
	case 0x01:
		logerror("FDC: IOC selection is write-only\n");
		return 0xff;
	}
	return 0xff;
}

void x68k_state::fdc_irq(bool state)
{
	if((m_ioc.irqstatus & 0x04) && state)
	{
		m_current_vector[1] = m_ioc.fdcvector;
		m_ioc.irqstatus |= 0x80;
		m_current_irq_line = 1;
		logerror("FDC: IRQ triggered\n");
		machine().device("maincpu")->execute().set_input_line_and_vector(1, ASSERT_LINE, m_current_vector[1]);
	}
}

static int x68k_fdc_read_byte(running_machine &machine,int addr)
{
	x68k_state *state = machine.driver_data<x68k_state>();
	return state->m_fdc.fdc->dma_r();
}

static void x68k_fdc_write_byte(running_machine &machine,int addr, int data)
{
	x68k_state *state = machine.driver_data<x68k_state>();
	return state->m_fdc.fdc->dma_w(data);
}

void x68k_state::fdc_drq(bool state)
{
	m_fdc.drq_state = state;
}

WRITE16_MEMBER(x68k_state::x68k_fm_w)
{
	switch(offset)
	{
	case 0x00:
	case 0x01:
		machine().device<ym2151_device>("ym2151")->write(space, offset, data);
		break;
	}
}

READ16_MEMBER(x68k_state::x68k_fm_r)
{
	if(offset == 0x01)
		return machine().device<ym2151_device>("ym2151")->read(space, 1);

	return 0xffff;
}

WRITE8_MEMBER(x68k_state::x68k_ct_w)
{
	device_t *okim = space.machine().device("okim6258");

	// CT1 and CT2 bits from YM2151 port 0x1b
	// CT1 - ADPCM clock - 0 = 8MHz, 1 = 4MHz
	// CT2 - 1 = Set ready state of FDC
	m_fdc.fdc->ready_w(data & 0x01);
	m_adpcm.clock = data & 0x02;
	x68k_set_adpcm();
	okim6258_set_clock(okim, data & 0x02 ? 4000000 : 8000000);
}

/*
 Custom I/O controller at 0xe9c000
 0xe9c001 (R) - Interrupt status
 0xe9c001 (W) - Interrupt mask (low nibble only)
                - bit 7 = FDC interrupt
                - bit 6 = FDD interrupt
                - bit 5 = Printer Busy signal
                - bit 4 = HDD interrupt
                - bit 3 = HDD interrupts enabled
                - bit 2 = FDC interrupts enabled
                - bit 1 = FDD interrupts enabled
                - bit 0 = Printer interrupts enabled
 0xe9c003 (W) - Interrupt vector
                - bits 7-2 = vector
                - bits 1,0 = device (00 = FDC, 01 = FDD, 10 = HDD, 11 = Printer)
*/
WRITE16_MEMBER(x68k_state::x68k_ioc_w)
{
	switch(offset)
	{
	case 0x00:
		m_ioc.irqstatus = data & 0x0f;
		logerror("I/O: Status register write %02x\n",data);
		break;
	case 0x01:
		switch(data & 0x03)
		{
		case 0x00:
			m_ioc.fdcvector = data & 0xfc;
			logerror("IOC: FDC IRQ vector = 0x%02x\n",data & 0xfc);
			break;
		case 0x01:
			m_ioc.fddvector = data & 0xfc;
			logerror("IOC: FDD IRQ vector = 0x%02x\n",data & 0xfc);
			break;
		case 0x02:
			m_ioc.hdcvector = data & 0xfc;
			logerror("IOC: HDD IRQ vector = 0x%02x\n",data & 0xfc);
			break;
		case 0x03:
			m_ioc.prnvector = data & 0xfc;
			logerror("IOC: Printer IRQ vector = 0x%02x\n",data & 0xfc);
			break;
		}
		break;
	}
}

READ16_MEMBER(x68k_state::x68k_ioc_r)
{
	switch(offset)
	{
	case 0x00:
		logerror("I/O: Status register read\n");
		return (m_ioc.irqstatus & 0xdf) | 0x20;
	default:
		return 0x00;
	}
}

/*
 System ports at 0xe8e000
 Port 1 (0xe8e001) - Monitor contrast (bits 3-0)
 Port 2 (0xe8e003) - Display / 3D scope control
                     - bit 3 - Display control signal (0 = on)
                     - bit 1 - 3D scope left shutter (0 = closed)
                     - bit 0 - 3D scope right shutter
 Port 3 (0xe8e005) - Colour image unit control (bits 3-0)
 Port 4 (0xe8e007) - Keyboard/NMI/Dot clock
                     - bit 3 - (R) 1 = Keyboard connected, (W) 1 = Key data can be transmitted
                     - bit 1 - NMI Reset
                     - bit 0 - HRL - high resolution dot clock - 1 = 1/2, 1/4, 1/8, 0 = 1/2, 1/3, 1/6 (normal)
 Port 5 (0xe8e009) - ROM (bits 7-4)/DRAM (bits 3-0) wait, X68030 only
 Port 6 (0xe8e00b) - CPU type and clock speed (XVI or later only, X68000 returns 0xFF)
                     - bits 7-4 - CPU Type (1100 = 68040, 1101 = 68030, 1110 = 68020, 1111 = 68000)
                     - bits 3-0 - clock speed (1001 = 50MHz, 40, 33, 25, 20, 16, 1111 = 10MHz)
 Port 7 (0xe8e00d) - SRAM write enable - if 0x31 is written to this port, writing to SRAM is allowed.
                                         Any other value, then SRAM is read only.
 Port 8 (0xe8e00f) - Power off control - write 0x00, 0x0f, 0x0f sequentially to switch power off.
*/
WRITE16_MEMBER(x68k_state::x68k_sysport_w)
{
	switch(offset)
	{
	case 0x00:
		m_sysport.contrast = data & 0x0f;  // often used for screen fades / blanking
		// TODO: implement a decent, not slow, brightness control
		break;
	case 0x01:
		m_sysport.monitor = data & 0x08;
		break;
	case 0x03:
		m_sysport.keyctrl = data & 0x08;  // bit 3 = enable keyboard data transmission
		break;
	case 0x06:
		m_sysport.sram_writeprotect = data;
		break;
	default:
//      logerror("SYS: [%08x] Wrote %04x to invalid or unimplemented system port %04x\n",space.device().safe_pc(),data,offset);
		break;
	}
}

READ16_MEMBER(x68k_state::x68k_sysport_r)
{
	int ret = 0;
	switch(offset)
	{
	case 0x00:  // monitor contrast setting (bits3-0)
		return m_sysport.contrast;
	case 0x01:  // monitor control (bit3) / 3D Scope (bits1,0)
		ret |= m_sysport.monitor;
		return ret;
	case 0x03:  // bit 3 = key control (is 1 if keyboard is connected)
		return 0x08;
	case 0x05:  // CPU type and speed
		return m_sysport.cputype;
	default:
		logerror("Read from invalid or unimplemented system port %04x\n",offset);
		return 0xff;
	}
}

#ifdef UNUSED_FUNCTION
READ16_MEMBER(x68k_state::x68k_mfp_r)
{
	device_t *x68k_mfp = machine().device(MC68901_TAG);

	return mc68901_register_r(x68k_mfp, offset);
}
#endif

READ16_MEMBER(x68k_state::x68k_mfp_r)
{

	// Initial settings indicate that IRQs are generated for FM (YM2151), Receive buffer error or full,
	// MFP Timer C, and the power switch
//  logerror("MFP: [%08x] Reading offset %i\n",space.device().safe_pc(),offset);
	switch(offset)
	{
#if 0
	case 0x00:  // GPIP - General purpose I/O register (read-only)
		ret = 0x23;
		if(machine.primary_screen->vpos() == m_crtc.reg[9])
			ret |= 0x40;
		if(m_crtc.vblank == 0)
			ret |= 0x10;  // Vsync signal (low if in vertical retrace)
//      if(m_mfp.isrb & 0x08)
//          ret |= 0x08;  // FM IRQ signal
		if(machine.primary_screen->hpos() > m_crtc.width - 32)
			ret |= 0x80;  // Hsync signal
//      logerror("MFP: [%08x] Reading offset %i (ret=%02x)\n",space.device().safe_pc(),offset,ret);
		return ret;  // bit 5 is always 1
	case 3:
		return m_mfp.iera;
	case 4:
		return m_mfp.ierb;
	case 5:
		return m_mfp.ipra;
	case 6:
		return m_mfp.iprb;
	case 7:
		if(m_mfp.eoi_mode == 0)  // forced low in auto EOI mode
			return 0;
		else
			return m_mfp.isra;
	case 8:
		if(m_mfp.eoi_mode == 0)  // forced low in auto EOI mode
			return 0;
		else
			return m_mfp.isrb;
	case 9:
		return m_mfp.imra;
	case 10:
		return m_mfp.imrb;
	case 15:  // TADR
		return m_mfp.timer[0].counter;  // Timer data registers return their main counter values
	case 16:  // TBDR
		return m_mfp.timer[1].counter;
	case 17:  // TCDR
		return m_mfp.timer[2].counter;
	case 18:  // TDDR
		return m_mfp.timer[3].counter;
#endif
	case 21:  // RSR
		return m_mfp.rsr;
	case 22:  // TSR
		return m_mfp.tsr | 0x80;  // buffer is typically empty?
	case 23:
		return x68k_keyboard_pop_scancode();
	default:
		if (ACCESSING_BITS_0_7) return m_mfpdev->read(space, offset);
	}
	return 0xffff;
}

WRITE16_MEMBER(x68k_state::x68k_mfp_w)
{

	/* For the Interrupt registers, the bits are set out as such:
	   Reg A - bit 7: GPIP7 (HSync)
	           bit 6: GPIP6 (CRTC CIRQ)
	           bit 5: Timer A
	           bit 4: Receive buffer full
	           bit 3: Receive error
	           bit 2: Transmit buffer empty
	           bit 1: Transmit error
	           bit 0: Timer B
	   Reg B - bit 7: GPIP5 (Unused, always 1)
	           bit 6: GPIP4 (VSync)
	           bit 5: Timer C
	           bit 4: Timer D
	           bit 3: GPIP3 (FM IRQ)
	           bit 2: GPIP2 (Power switch)
	           bit 1: GPIP1 (EXPON)
	           bit 0: GPIP0 (Alarm)
	*/
	switch(offset)
	{
#if 0
	case 0:  // GPDR
		// All bits are inputs generally, so no action taken.
		break;
	case 1:  // AER
		m_mfp.aer = data;
		break;
	case 2:  // DDR
		m_mfp.ddr = data;  // usually all bits are 0 (input)
		break;
	case 3:  // IERA
		m_mfp.iera = data;
		break;
	case 4:  // IERB
		m_mfp.ierb = data;
		break;
	case 5:  // IPRA
		m_mfp.ipra = data;
		break;
	case 6:  // IPRB
		m_mfp.iprb = data;
		break;
	case 7:
		m_mfp.isra = data;
		break;
	case 8:
		m_mfp.isrb = data;
		break;
	case 9:
		m_mfp.imra = data;
//      mfp_update_irq(0);
//      logerror("MFP: IRQ Mask A write: %02x\n",data);
		break;
	case 10:
		m_mfp.imrb = data;
//      mfp_update_irq(0);
//      logerror("MFP: IRQ Mask B write: %02x\n",data);
		break;
	case 11:  // VR
		m_mfp.vr = 0x40;//data;  // High 4 bits = high 4 bits of IRQ vector
		m_mfp.eoi_mode = data & 0x08;  // 0 = Auto, 1 = Software End-of-interrupt
		if(m_mfp.eoi_mode == 0)  // In-service registers are cleared if this bit is cleared.
		{
			m_mfp.isra = 0;
			m_mfp.isrb = 0;
		}
		break;
	case 12:  // TACR
		m_mfp.tacr = data;
		mfp_set_timer(0,data & 0x0f);
		break;
	case 13:  // TBCR
		m_mfp.tbcr = data;
		mfp_set_timer(1,data & 0x0f);
		break;
	case 14:  // TCDCR
		m_mfp.tcdcr = data;
		mfp_set_timer(2,(data & 0x70)>>4);
		mfp_set_timer(3,data & 0x07);
		break;
	case 15:  // TADR
		m_mfp.tadr = data;
		m_mfp.timer[0].counter = data;
		break;
	case 16:  // TBDR
		m_mfp.tbdr = data;
		m_mfp.timer[1].counter = data;
		break;
	case 17:  // TCDR
		m_mfp.tcdr = data;
		m_mfp.timer[2].counter = data;
		break;
	case 18:  // TDDR
		m_mfp.tddr = data;
		m_mfp.timer[3].counter = data;
		break;
	case 20:
		m_mfp.ucr = data;
		break;
#endif
	case 21:
		if(data & 0x01)
			m_mfp.usart.recv_enable = 1;
		else
			m_mfp.usart.recv_enable = 0;
		break;
	case 22:
		if(data & 0x01)
			m_mfp.usart.send_enable = 1;
		else
			m_mfp.usart.send_enable = 0;
		break;
	case 23:
		if(m_mfp.usart.send_enable != 0)
		{
			// Keyboard control command.
			m_mfp.usart.send_buffer = data;
			x68k_keyboard_ctrl_w(data);
//          logerror("MFP: [%08x] USART Sent data %04x\n",space.device().safe_pc(),data);
		}
		break;
	default:
		if (ACCESSING_BITS_0_7) m_mfpdev->write(space, offset, data & 0xff);
		return;
	}
}


WRITE16_MEMBER(x68k_state::x68k_ppi_w)
{
	i8255_device *ppi = machine().device<i8255_device>("ppi8255");
	ppi->write(space,offset & 0x03,data);
}

READ16_MEMBER(x68k_state::x68k_ppi_r)
{
	i8255_device *ppi = machine().device<i8255_device>("ppi8255");
	return ppi->read(space,offset & 0x03);
}

READ16_MEMBER(x68k_state::x68k_rtc_r)
{
	return m_rtc->read(space, offset);
}

WRITE16_MEMBER(x68k_state::x68k_rtc_w)
{
	m_rtc->write(space, offset, data);
}

WRITE_LINE_MEMBER(x68k_state::x68k_rtc_alarm_irq)
{
	if(m_mfp.aer & 0x01)
	{
		if(state == 1)
		{
			m_mfp.gpio |= 0x01;
			m_mfpdev->i0_w(1);
			//mfp_trigger_irq(MFP_IRQ_GPIP0);  // RTC ALARM
		}
	}
	else
	{
		if(state == 0)
		{
			m_mfp.gpio &= ~0x01;
			m_mfpdev->i0_w(0);
			//mfp_trigger_irq(MFP_IRQ_GPIP0);  // RTC ALARM
		}
	}
}


WRITE16_MEMBER(x68k_state::x68k_sram_w)
{
	if(m_sysport.sram_writeprotect == 0x31)
	{
		COMBINE_DATA(m_nvram16 + offset);
	}
}

READ16_MEMBER(x68k_state::x68k_sram_r)
{
	// HACKS!
//  if(offset == 0x5a/2)  // 0x5a should be 0 if no SASI HDs are present.
//      return 0x0000;
	if(offset == 0x08/2)
		return machine().device<ram_device>(RAM_TAG)->size() >> 16;  // RAM size
#if 0
	if(offset == 0x46/2)
		return 0x0024;
	if(offset == 0x6e/2)
		return 0xff00;
	if(offset == 0x70/2)
		return 0x0700;
#endif
	return m_nvram16[offset];
}

READ32_MEMBER(x68k_state::x68k_sram32_r)
{
	if(offset == 0x08/4)
		return (machine().device<ram_device>(RAM_TAG)->size() & 0xffff0000);  // RAM size
#if 0
	if(offset == 0x46/2)
		return 0x0024;
	if(offset == 0x6e/2)
		return 0xff00;
	if(offset == 0x70/2)
		return 0x0700;
#endif
	return m_nvram32[offset];
}

WRITE32_MEMBER(x68k_state::x68k_sram32_w)
{
	if(m_sysport.sram_writeprotect == 0x31)
	{
		COMBINE_DATA(m_nvram32 + offset);
	}
}

WRITE16_MEMBER(x68k_state::x68k_vid_w)
{
	int val;
	if(offset < 0x100)  // Graphic layer palette
	{
		COMBINE_DATA(m_video.gfx_pal+offset);
		val = m_video.gfx_pal[offset];
		palette_set_color_rgb(machine(),offset,(val & 0x07c0) >> 3,(val & 0xf800) >> 8,(val & 0x003e) << 2);
		return;
	}

	if(offset >= 0x100 && offset < 0x200)  // Text / Sprites / Tilemap palette
	{
		COMBINE_DATA(m_video.text_pal+(offset-0x100));
		val = m_video.text_pal[offset-0x100];
		palette_set_color_rgb(machine(),offset,(val & 0x07c0) >> 3,(val & 0xf800) >> 8,(val & 0x003e) << 2);
		return;
	}

	switch(offset)
	{
	case 0x200:
		COMBINE_DATA(m_video.reg);
		break;
	case 0x280:  // priority levels
		COMBINE_DATA(m_video.reg+1);
		if(ACCESSING_BITS_0_7)
		{
			m_video.gfxlayer_pri[0] = data & 0x0003;
			m_video.gfxlayer_pri[1] = (data & 0x000c) >> 2;
			m_video.gfxlayer_pri[2] = (data & 0x0030) >> 4;
			m_video.gfxlayer_pri[3] = (data & 0x00c0) >> 6;
		}
		if(ACCESSING_BITS_8_15)
		{
			m_video.gfx_pri = (data & 0x0300) >> 8;
			m_video.text_pri = (data & 0x0c00) >> 10;
			m_video.sprite_pri = (data & 0x3000) >> 12;
			if(m_video.gfx_pri == 3)
				m_video.gfx_pri--;
			if(m_video.text_pri == 3)
				m_video.text_pri--;
			if(m_video.sprite_pri == 3)
				m_video.sprite_pri--;
		}
		break;
	case 0x300:
		COMBINE_DATA(m_video.reg+2);
		break;
	default:
		logerror("VC: Invalid video controller write (offset = 0x%04x, data = %04x)\n",offset,data);
	}
}

READ16_MEMBER(x68k_state::x68k_vid_r)
{
	if(offset < 0x100)
		return m_video.gfx_pal[offset];

	if(offset >= 0x100 && offset < 0x200)
		return m_video.text_pal[offset-0x100];

	switch(offset)
	{
	case 0x200:
		return m_video.reg[0];
	case 0x280:
		return m_video.reg[1];
	case 0x300:
		return m_video.reg[2];
	default:
		logerror("VC: Invalid video controller read (offset = 0x%04x)\n",offset);
	}

	return 0xff;
}

READ16_MEMBER(x68k_state::x68k_areaset_r)
{
	// register is write-only
	return 0xffff;
}

WRITE16_MEMBER(x68k_state::x68k_areaset_w)
{
	// TODO
	logerror("SYS: Supervisor area set: 0x%02x\n",data & 0xff);
}

WRITE16_MEMBER(x68k_state::x68k_enh_areaset_w )
{
	// TODO
	logerror("SYS: Enhanced Supervisor area set (from %iMB): 0x%02x\n",(offset + 1) * 2,data & 0xff);
}

TIMER_CALLBACK_MEMBER(x68k_state::x68k_bus_error)
{
	int val = param;
	int v;
	UINT8 *ram = machine().device<ram_device>(RAM_TAG)->pointer();

	if(strcmp(machine().system().name,"x68030") == 0)
		v = 0x0b;
	else
		v = 0x09;
	if(ram[v] != 0x02)  // normal vector for bus errors points to 02FF0540
	{
		machine().device("maincpu")->execute().set_input_line(M68K_LINE_BUSERROR, ASSERT_LINE);
		machine().device("maincpu")->execute().set_input_line(M68K_LINE_BUSERROR, CLEAR_LINE);
		popmessage("Bus error: Unused RAM access [%08x]", val);
	}
}

READ16_MEMBER(x68k_state::x68k_rom0_r)
{
	/* this location contains the address of some expansion device ROM, if no ROM exists,
	   then access causes a bus error */
	m_current_vector[2] = 0x02;  // bus error
	m_current_irq_line = 2;
//  machine().device("maincpu")->execute().set_input_line_and_vector(2,ASSERT_LINE,m_current_vector[2]);
	if(ioport("options")->read() & 0x02)
	{
		offset *= 2;
		if(ACCESSING_BITS_0_7)
			offset++;
		machine().scheduler().timer_set(machine().device<cpu_device>("maincpu")->cycles_to_attotime(4), timer_expired_delegate(FUNC(x68k_state::x68k_bus_error),this), 0xbffffc+offset);
	}
	return 0xff;
}

WRITE16_MEMBER(x68k_state::x68k_rom0_w)
{
	/* this location contains the address of some expansion device ROM, if no ROM exists,
	   then access causes a bus error */
	m_current_vector[2] = 0x02;  // bus error
	m_current_irq_line = 2;
//  machine().device("maincpu")->execute().set_input_line_and_vector(2,ASSERT_LINE,m_current_vector[2]);
	if(ioport("options")->read() & 0x02)
	{
		offset *= 2;
		if(ACCESSING_BITS_0_7)
			offset++;
		machine().scheduler().timer_set(machine().device<cpu_device>("maincpu")->cycles_to_attotime(4), timer_expired_delegate(FUNC(x68k_state::x68k_bus_error),this), 0xbffffc+offset);
	}
}

READ16_MEMBER(x68k_state::x68k_emptyram_r)
{
	/* this location is unused RAM, access here causes a bus error
	   Often a method for detecting amount of installed RAM, is to read or write at 1MB intervals, until a bus error occurs */
	m_current_vector[2] = 0x02;  // bus error
	m_current_irq_line = 2;
//  machine().device("maincpu")->execute().set_input_line_and_vector(2,ASSERT_LINE,m_current_vector[2]);
	if(ioport("options")->read() & 0x02)
	{
		offset *= 2;
		if(ACCESSING_BITS_0_7)
			offset++;
		machine().scheduler().timer_set(machine().device<cpu_device>("maincpu")->cycles_to_attotime(4), timer_expired_delegate(FUNC(x68k_state::x68k_bus_error),this), offset);
	}
	return 0xff;
}

WRITE16_MEMBER(x68k_state::x68k_emptyram_w)
{
	/* this location is unused RAM, access here causes a bus error
	   Often a method for detecting amount of installed RAM, is to read or write at 1MB intervals, until a bus error occurs */
	m_current_vector[2] = 0x02;  // bus error
	m_current_irq_line = 2;
//  machine().device("maincpu")->execute().set_input_line_and_vector(2,ASSERT_LINE,m_current_vector[2]);
	if(ioport("options")->read() & 0x02)
	{
		offset *= 2;
		if(ACCESSING_BITS_0_7)
			offset++;
		machine().scheduler().timer_set(machine().device<cpu_device>("maincpu")->cycles_to_attotime(4), timer_expired_delegate(FUNC(x68k_state::x68k_bus_error),this), offset);
	}
}

READ16_MEMBER(x68k_state::x68k_exp_r)
{
	/* These are expansion devices, if not present, they cause a bus error */
	if(ioport("options")->read() & 0x02)
	{
		m_current_vector[2] = 0x02;  // bus error
		m_current_irq_line = 2;
		offset *= 2;
		if(ACCESSING_BITS_0_7)
			offset++;
		machine().scheduler().timer_set(machine().device<cpu_device>("maincpu")->cycles_to_attotime(16), timer_expired_delegate(FUNC(x68k_state::x68k_bus_error),this), 0xeafa00+offset);
//      machine.device("maincpu")->execute().set_input_line_and_vector(2,ASSERT_LINE,state->m_current_vector[2]);
	}
	return 0xffff;
}

WRITE16_MEMBER(x68k_state::x68k_exp_w)
{
	/* These are expansion devices, if not present, they cause a bus error */
	if(ioport("options")->read() & 0x02)
	{
		m_current_vector[2] = 0x02;  // bus error
		m_current_irq_line = 2;
		offset *= 2;
		if(ACCESSING_BITS_0_7)
			offset++;
		machine().scheduler().timer_set(machine().device<cpu_device>("maincpu")->cycles_to_attotime(16), timer_expired_delegate(FUNC(x68k_state::x68k_bus_error),this), 0xeafa00+offset);
//      machine.device("maincpu")->execute().set_input_line_and_vector(2,ASSERT_LINE,state->m_current_vector[2]);
	}
}

static void x68k_dma_irq(running_machine &machine, int channel)
{
	x68k_state *state = machine.driver_data<x68k_state>();
	device_t *device = machine.device("hd63450");
	state->m_current_vector[3] = hd63450_get_vector(device, channel);
	state->m_current_irq_line = 3;
	logerror("DMA#%i: DMA End (vector 0x%02x)\n",channel,state->m_current_vector[3]);
	machine.device("maincpu")->execute().set_input_line_and_vector(3,ASSERT_LINE,state->m_current_vector[3]);
}

static void x68k_dma_end(running_machine &machine, int channel,int irq)
{
	if(irq != 0)
	{
		x68k_dma_irq(machine, channel);
	}
}

static void x68k_dma_error(running_machine &machine, int channel, int irq)
{
	x68k_state *state = machine.driver_data<x68k_state>();
	device_t *device = machine.device("hd63450");
	if(irq != 0)
	{
		state->m_current_vector[3] = hd63450_get_error_vector(device,channel);
		state->m_current_irq_line = 3;
		machine.device("maincpu")->execute().set_input_line_and_vector(3,ASSERT_LINE,state->m_current_vector[3]);
	}
}

WRITE_LINE_MEMBER(x68k_state::x68k_fm_irq)
{
	if(state == CLEAR_LINE)
	{
		m_mfp.gpio |= 0x08;
		m_mfpdev->i3_w(1);
	}
	else
	{
		m_mfp.gpio &= ~0x08;
		m_mfpdev->i3_w(0);
	}
}

READ8_MEMBER( x68k_state::mfp_gpio_r )
{
	UINT8 data = m_mfp.gpio;

	data &= ~(m_crtc.hblank << 7);
	data &= ~(m_crtc.vblank << 4);
	data |= 0x23;  // GPIP5 is unused, always 1

//  m_mfpdev->tai_w(state->m_crtc.vblank);

	return data;
}

WRITE8_MEMBER(x68k_state::x68030_adpcm_w)
{
	device_t *device = machine().device("okim6258");
	switch(offset)
	{
		case 0x00:
			okim6258_ctrl_w(device,space,0,data);
			break;
		case 0x01:
			okim6258_data_w(device,space,0,data);
			break;
	}
}

WRITE_LINE_MEMBER(x68k_state::mfp_irq_callback)
{
	if(m_mfp_prev == CLEAR_LINE && state == CLEAR_LINE)  // eliminate unnecessary calls to set the IRQ line for speed reasons
		return;
	if(state != CLEAR_LINE)
		state = HOLD_LINE;  // to get around erroneous spurious interrupt
//  if((m_ioc.irqstatus & 0xc0) != 0)  // if the FDC is busy, then we don't want to miss that IRQ
//      return;
	machine().device("maincpu")->execute().set_input_line(6, state);
	m_current_vector[6] = 0;
	m_mfp_prev = state;
}

INTERRUPT_GEN_MEMBER(x68k_state::x68k_vsync_irq)
{
#if 0
	x68k_state *state = machine.driver_data<x68k_state>();
	if(m_mfp.ierb & 0x40)
	{
		m_mfp.isrb |= 0x40;
		m_current_vector[6] = (m_mfp.vr & 0xf0) | 0x06;  // GPIP4 (V-DISP)
		m_current_irq_line = 6;
		mfp_timer_a_callback(0);  // Timer A is usually always in event count mode, and is tied to V-DISP
		mfp_trigger_irq(MFP_IRQ_GPIP4);
	}
	if(m_crtc.height == 256)
		machine.primary_screen->update_partial(256);//m_crtc.reg[4]/2);
	else
		machine.primary_screen->update_partial(512);//m_crtc.reg[4]);
#endif
}

static IRQ_CALLBACK(x68k_int_ack)
{
	x68k_state *state = device->machine().driver_data<x68k_state>();

	if(irqline == 6)  // MFP
	{
		state->m_mfp.current_irq = -1;
		if(state->m_current_vector[6] != 0x4b && state->m_current_vector[6] != 0x4c)
			state->m_current_vector[6] = state->m_mfpdev->get_vector();
		else
			device->machine().device("maincpu")->execute().set_input_line_and_vector(irqline,CLEAR_LINE,state->m_current_vector[irqline]);
		logerror("SYS: IRQ acknowledged (vector=0x%02x, line = %i)\n",state->m_current_vector[6],irqline);
		return state->m_current_vector[6];
	}

	device->machine().device("maincpu")->execute().set_input_line_and_vector(irqline,CLEAR_LINE,state->m_current_vector[irqline]);
	if(irqline == 1)  // IOSC
	{
		state->m_ioc.irqstatus &= ~0xf0;
	}
	if(irqline == 5)  // SCC
	{
		state->m_mouse.irqactive = 0;
	}

	logerror("SYS: IRQ acknowledged (vector=0x%02x, line = %i)\n",state->m_current_vector[irqline],irqline);
	return state->m_current_vector[irqline];
}

WRITE_LINE_MEMBER(x68k_state::x68k_scsi_irq)
{
	// TODO : Internal SCSI IRQ vector 0x6c, External SCSI IRQ vector 0xf6, IRQs go through the IOSC (IRQ line 1)
	if(state != 0)
	{
		m_current_vector[1] = 0x6c;
		m_current_irq_line = 1;
		machine().device("maincpu")->execute().set_input_line_and_vector(1,ASSERT_LINE,m_current_vector[1]);
	}
}

WRITE_LINE_MEMBER(x68k_state::x68k_scsi_drq)
{
	// TODO
}

static ADDRESS_MAP_START(x68k_map, AS_PROGRAM, 16, x68k_state )
//  AM_RANGE(0x000000, 0xbfffff) AM_RAMBANK(1)
	AM_RANGE(0xbffffc, 0xbfffff) AM_READWRITE(x68k_rom0_r, x68k_rom0_w)
//  AM_RANGE(0xc00000, 0xdfffff) AM_READWRITE(x68k_gvram_r, x68k_gvram_w) AM_SHARE("gvram")
//  AM_RANGE(0xe00000, 0xe7ffff) AM_READWRITE(x68k_tvram_r, x68k_tvram_w) AM_SHARE("tvram")
	AM_RANGE(0xc00000, 0xdfffff) AM_RAMBANK("bank2") AM_SHARE("gvram16")
	AM_RANGE(0xe00000, 0xe7ffff) AM_RAMBANK("bank3") AM_SHARE("tvram16")
	AM_RANGE(0xe80000, 0xe81fff) AM_READWRITE(x68k_crtc_r, x68k_crtc_w)
	AM_RANGE(0xe82000, 0xe83fff) AM_READWRITE(x68k_vid_r, x68k_vid_w)
	AM_RANGE(0xe84000, 0xe85fff) AM_READWRITE(x68k_dmac_r, x68k_dmac_w)
	AM_RANGE(0xe86000, 0xe87fff) AM_READWRITE(x68k_areaset_r, x68k_areaset_w)
	AM_RANGE(0xe88000, 0xe89fff) AM_READWRITE(x68k_mfp_r, x68k_mfp_w)
	AM_RANGE(0xe8a000, 0xe8bfff) AM_READWRITE(x68k_rtc_r, x68k_rtc_w)
//  AM_RANGE(0xe8c000, 0xe8dfff) AM_READWRITE(x68k_printer_r, x68k_printer_w)
	AM_RANGE(0xe8e000, 0xe8ffff) AM_READWRITE(x68k_sysport_r, x68k_sysport_w)
	AM_RANGE(0xe90000, 0xe91fff) AM_READWRITE(x68k_fm_r, x68k_fm_w)
	AM_RANGE(0xe92000, 0xe92001) AM_DEVREADWRITE8_LEGACY("okim6258", okim6258_status_r, okim6258_ctrl_w, 0x00ff)
	AM_RANGE(0xe92002, 0xe92003) AM_DEVREADWRITE8_LEGACY("okim6258", okim6258_status_r, okim6258_data_w, 0x00ff)
	AM_RANGE(0xe94000, 0xe94003) AM_DEVICE8("upd72065", upd72065_device, map, 0x00ff)
	AM_RANGE(0xe94004, 0xe94007) AM_READWRITE(x68k_fdc_r, x68k_fdc_w)
	AM_RANGE(0xe96000, 0xe9601f) AM_DEVREADWRITE("x68k_hdc", x68k_hdc_image_device, hdc_r, hdc_w)
	AM_RANGE(0xe98000, 0xe99fff) AM_READWRITE(x68k_scc_r, x68k_scc_w)
	AM_RANGE(0xe9a000, 0xe9bfff) AM_READWRITE(x68k_ppi_r, x68k_ppi_w)
	AM_RANGE(0xe9c000, 0xe9dfff) AM_READWRITE(x68k_ioc_r, x68k_ioc_w)
	AM_RANGE(0xea0000, 0xea1fff) AM_READWRITE(x68k_exp_r, x68k_exp_w)  // external SCSI ROM and controller
	AM_RANGE(0xeafa00, 0xeafa1f) AM_READWRITE(x68k_exp_r, x68k_exp_w)
	AM_RANGE(0xeafa80, 0xeafa89) AM_READWRITE(x68k_areaset_r, x68k_enh_areaset_w)
	AM_RANGE(0xeb0000, 0xeb7fff) AM_READWRITE(x68k_spritereg_r, x68k_spritereg_w)
	AM_RANGE(0xeb8000, 0xebffff) AM_READWRITE(x68k_spriteram_r, x68k_spriteram_w)
	AM_RANGE(0xece000, 0xece3ff) AM_READWRITE(x68k_exp_r, x68k_exp_w)  // User I/O
//  AM_RANGE(0xed0000, 0xed3fff) AM_READWRITE(sram_r, sram_w)
	AM_RANGE(0xed0000, 0xed3fff) AM_RAMBANK("bank4") AM_SHARE("nvram16")
	AM_RANGE(0xed4000, 0xefffff) AM_NOP
	AM_RANGE(0xf00000, 0xfbffff) AM_ROM
	AM_RANGE(0xfc0000, 0xfdffff) AM_READWRITE(x68k_exp_r, x68k_exp_w)  // internal SCSI ROM
	AM_RANGE(0xfe0000, 0xffffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(x68kxvi_map, AS_PROGRAM, 16, x68k_state )
//  AM_RANGE(0x000000, 0xbfffff) AM_RAMBANK(1)
	AM_RANGE(0xbffffc, 0xbfffff) AM_READWRITE(x68k_rom0_r, x68k_rom0_w)
//  AM_RANGE(0xc00000, 0xdfffff) AM_READWRITE(x68k_gvram_r, x68k_gvram_w) AM_SHARE("gvram")
//  AM_RANGE(0xe00000, 0xe7ffff) AM_READWRITE(x68k_tvram_r, x68k_tvram_w) AM_SHARE("tvram")
	AM_RANGE(0xc00000, 0xdfffff) AM_RAMBANK("bank2") AM_SHARE("gvram16")
	AM_RANGE(0xe00000, 0xe7ffff) AM_RAMBANK("bank3") AM_SHARE("tvram16")
	AM_RANGE(0xe80000, 0xe81fff) AM_READWRITE(x68k_crtc_r, x68k_crtc_w)
	AM_RANGE(0xe82000, 0xe83fff) AM_READWRITE(x68k_vid_r, x68k_vid_w)
	AM_RANGE(0xe84000, 0xe85fff) AM_READWRITE(x68k_dmac_r, x68k_dmac_w)
	AM_RANGE(0xe86000, 0xe87fff) AM_READWRITE(x68k_areaset_r, x68k_areaset_w)
	AM_RANGE(0xe88000, 0xe89fff) AM_READWRITE(x68k_mfp_r, x68k_mfp_w)
	AM_RANGE(0xe8a000, 0xe8bfff) AM_READWRITE(x68k_rtc_r, x68k_rtc_w)
//  AM_RANGE(0xe8c000, 0xe8dfff) AM_READWRITE(x68k_printer_r, x68k_printer_w)
	AM_RANGE(0xe8e000, 0xe8ffff) AM_READWRITE(x68k_sysport_r, x68k_sysport_w)
	AM_RANGE(0xe90000, 0xe91fff) AM_READWRITE(x68k_fm_r, x68k_fm_w)
	AM_RANGE(0xe92000, 0xe92001) AM_DEVREADWRITE8_LEGACY("okim6258", okim6258_status_r, okim6258_ctrl_w, 0x00ff)
	AM_RANGE(0xe92002, 0xe92003) AM_DEVREADWRITE8_LEGACY("okim6258", okim6258_status_r, okim6258_data_w, 0x00ff)
	AM_RANGE(0xe94000, 0xe94003) AM_DEVICE8("upd72065", upd72065_device, map, 0x00ff)
	AM_RANGE(0xe94004, 0xe94007) AM_READWRITE(x68k_fdc_r, x68k_fdc_w)
//  AM_RANGE(0xe96000, 0xe9601f) AM_DEVREADWRITE_LEGACY("x68k_hdc",x68k_hdc_r, x68k_hdc_w)
	AM_RANGE(0xe96020, 0xe9603f) AM_DEVREADWRITE8("scsi:mb89352",mb89352_device,mb89352_r,mb89352_w,0x00ff)
	AM_RANGE(0xe98000, 0xe99fff) AM_READWRITE(x68k_scc_r, x68k_scc_w)
	AM_RANGE(0xe9a000, 0xe9bfff) AM_READWRITE(x68k_ppi_r, x68k_ppi_w)
	AM_RANGE(0xe9c000, 0xe9dfff) AM_READWRITE(x68k_ioc_r, x68k_ioc_w)
	AM_RANGE(0xea0000, 0xea1fff) AM_READWRITE(x68k_exp_r, x68k_exp_w)  // external SCSI ROM and controller
	AM_RANGE(0xeafa00, 0xeafa1f) AM_READWRITE(x68k_exp_r, x68k_exp_w)
	AM_RANGE(0xeafa80, 0xeafa89) AM_READWRITE(x68k_areaset_r, x68k_enh_areaset_w)
	AM_RANGE(0xeb0000, 0xeb7fff) AM_READWRITE(x68k_spritereg_r, x68k_spritereg_w)
	AM_RANGE(0xeb8000, 0xebffff) AM_READWRITE(x68k_spriteram_r, x68k_spriteram_w)
	AM_RANGE(0xece000, 0xece3ff) AM_READWRITE(x68k_exp_r, x68k_exp_w)  // User I/O
//  AM_RANGE(0xed0000, 0xed3fff) AM_READWRITE(sram_r, sram_w)
	AM_RANGE(0xed0000, 0xed3fff) AM_RAMBANK("bank4") AM_SHARE("nvram16")
	AM_RANGE(0xed4000, 0xefffff) AM_NOP
	AM_RANGE(0xf00000, 0xfbffff) AM_ROM
	AM_RANGE(0xfc0000, 0xfdffff) AM_ROM  // internal SCSI ROM
	AM_RANGE(0xfe0000, 0xffffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(x68030_map, AS_PROGRAM, 32, x68k_state )
	ADDRESS_MAP_GLOBAL_MASK(0x00ffffff)  // Still only has 24-bit address space
//  AM_RANGE(0x000000, 0xbfffff) AM_RAMBANK(1)
	AM_RANGE(0xbffffc, 0xbfffff) AM_READWRITE16(x68k_rom0_r, x68k_rom0_w,0xffffffff)
//  AM_RANGE(0xc00000, 0xdfffff) AM_READWRITE(x68k_gvram_r, x68k_gvram_w) AM_SHARE("gvram")
//  AM_RANGE(0xe00000, 0xe7ffff) AM_READWRITE(x68k_tvram_r, x68k_tvram_w) AM_SHARE("tvram")
	AM_RANGE(0xc00000, 0xdfffff) AM_RAMBANK("bank2") AM_SHARE("gvram32")
	AM_RANGE(0xe00000, 0xe7ffff) AM_RAMBANK("bank3") AM_SHARE("tvram32")
	AM_RANGE(0xe80000, 0xe81fff) AM_READWRITE16(x68k_crtc_r, x68k_crtc_w,0xffffffff)
	AM_RANGE(0xe82000, 0xe83fff) AM_READWRITE16(x68k_vid_r, x68k_vid_w,0xffffffff)
	AM_RANGE(0xe84000, 0xe85fff) AM_READWRITE16(x68k_dmac_r, x68k_dmac_w,0xffffffff)
	AM_RANGE(0xe86000, 0xe87fff) AM_READWRITE16(x68k_areaset_r, x68k_areaset_w,0xffffffff)
	AM_RANGE(0xe88000, 0xe89fff) AM_READWRITE16(x68k_mfp_r, x68k_mfp_w,0xffffffff)
	AM_RANGE(0xe8a000, 0xe8bfff) AM_READWRITE16(x68k_rtc_r, x68k_rtc_w,0xffffffff)
//  AM_RANGE(0xe8c000, 0xe8dfff) AM_READWRITE(x68k_printer_r, x68k_printer_w)
	AM_RANGE(0xe8e000, 0xe8ffff) AM_READWRITE16(x68k_sysport_r, x68k_sysport_w,0xffffffff)
	AM_RANGE(0xe90000, 0xe91fff) AM_READWRITE16(x68k_fm_r, x68k_fm_w,0xffffffff)
	AM_RANGE(0xe92000, 0xe92003) AM_DEVREAD8_LEGACY("okim6258", okim6258_status_r, 0x00ff00ff) AM_WRITE8(x68030_adpcm_w, 0x00ff00ff)
	AM_RANGE(0xe94000, 0xe94003) AM_DEVICE8("upd72065", upd72065_device, map, 0x00ff00ff)
	AM_RANGE(0xe94004, 0xe94007) AM_READWRITE16(x68k_fdc_r, x68k_fdc_w,0xffffffff)
//  AM_RANGE(0xe96000, 0xe9601f) AM_DEVREADWRITE16_LEGACY("x68k_hdc",x68k_hdc_r, x68k_hdc_w,0xffffffff)
	AM_RANGE(0xe96020, 0xe9603f) AM_DEVREADWRITE8("scsi:mb89352",mb89352_device,mb89352_r,mb89352_w,0x00ff00ff)
	AM_RANGE(0xe98000, 0xe99fff) AM_READWRITE16(x68k_scc_r, x68k_scc_w,0xffffffff)
	AM_RANGE(0xe9a000, 0xe9bfff) AM_READWRITE16(x68k_ppi_r, x68k_ppi_w,0xffffffff)
	AM_RANGE(0xe9c000, 0xe9dfff) AM_READWRITE16(x68k_ioc_r, x68k_ioc_w,0xffffffff)
	AM_RANGE(0xea0000, 0xea1fff) AM_NOP//AM_READWRITE16_LEGACY(x68k_exp_r, x68k_exp_w,0xffffffff)  // external SCSI ROM and controller
	AM_RANGE(0xeafa00, 0xeafa1f) AM_READWRITE16(x68k_exp_r, x68k_exp_w,0xffffffff)
	AM_RANGE(0xeafa80, 0xeafa8b) AM_READWRITE16(x68k_areaset_r, x68k_enh_areaset_w,0xffffffff)
	AM_RANGE(0xeb0000, 0xeb7fff) AM_READWRITE16(x68k_spritereg_r, x68k_spritereg_w,0xffffffff)
	AM_RANGE(0xeb8000, 0xebffff) AM_READWRITE16(x68k_spriteram_r, x68k_spriteram_w,0xffffffff)
	AM_RANGE(0xece000, 0xece3ff) AM_READWRITE16(x68k_exp_r, x68k_exp_w,0xffffffff)  // User I/O
//  AM_RANGE(0xed0000, 0xed3fff) AM_READWRITE(sram_r, sram_w)
	AM_RANGE(0xed0000, 0xed3fff) AM_RAMBANK("bank4") AM_SHARE("nvram32")
	AM_RANGE(0xed4000, 0xefffff) AM_NOP
	AM_RANGE(0xf00000, 0xfbffff) AM_ROM
	AM_RANGE(0xfc0000, 0xfdffff) AM_ROM  // internal SCSI ROM
	AM_RANGE(0xfe0000, 0xffffff) AM_ROM
ADDRESS_MAP_END

WRITE_LINE_MEMBER( x68k_state::mfp_tdo_w )
{
	m_mfpdev->tc_w(state);
	m_mfpdev->rc_w(state);
}

static MC68901_INTERFACE( mfp_interface )
{
	4000000,                                            /* timer clock */
	0,                                                  /* receive clock */
	0,                                                  /* transmit clock */
	DEVCB_DRIVER_LINE_MEMBER(x68k_state,mfp_irq_callback),                      /* interrupt */
	DEVCB_DRIVER_MEMBER(x68k_state, mfp_gpio_r),        /* GPIO read */
	DEVCB_NULL,                                         /* GPIO write */
	DEVCB_NULL,                                         /* TAO */
	DEVCB_NULL,                                         /* TBO */
	DEVCB_NULL,                                         /* TCO */
	DEVCB_DRIVER_LINE_MEMBER(x68k_state, mfp_tdo_w),    /* TDO */
	DEVCB_NULL,                                         /* serial input */
	DEVCB_NULL,                                         /* serial output */
	DEVCB_NULL,
	DEVCB_NULL
};

static I8255A_INTERFACE( ppi_interface )
{
	DEVCB_DRIVER_MEMBER(x68k_state,ppi_port_a_r),
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(x68k_state,ppi_port_b_r),
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(x68k_state,ppi_port_c_r),
	DEVCB_DRIVER_MEMBER(x68k_state,ppi_port_c_w)
};

static const hd63450_intf dmac_interface =
{
	"maincpu",  // CPU - 68000
	{attotime::from_usec(32),attotime::from_nsec(450),attotime::from_usec(4),attotime::from_hz(15625/2)},  // Cycle steal mode timing (guesstimate)
	{attotime::from_usec(32),attotime::from_nsec(450),attotime::from_nsec(50),attotime::from_nsec(50)}, // Burst mode timing (guesstimate)
	x68k_dma_end,
	x68k_dma_error,
	{ x68k_fdc_read_byte, 0, 0, 0 },
	{ x68k_fdc_write_byte, 0, 0, 0 }
//  { 0, 0, 0, 0 },
//  { 0, 0, 0, 0 }
};

static const okim6258_interface x68k_okim6258_interface =
{
	FOSC_DIV_BY_512,
	TYPE_4BITS,
	OUTPUT_10BITS,
};

static RP5C15_INTERFACE( rtc_intf )
{
	DEVCB_DRIVER_LINE_MEMBER(x68k_state,x68k_rtc_alarm_irq),
	DEVCB_NULL
};

static INPUT_PORTS_START( x68000 )
	PORT_START("ctrltype")
	PORT_CONFNAME(0x0f, 0x00, "Joystick Port 1")
	PORT_CONFSETTING(0x00, "Standard 2-button MSX/FM-Towns joystick")
	PORT_CONFSETTING(0x01, "3-button Megadrive gamepad")
	PORT_CONFSETTING(0x02, "6-button Megadrive gamepad")
	PORT_CONFSETTING(0x03, "XPD-1LR dual D-pad gamepad")
	PORT_CONFNAME(0xf0, 0x00, "Joystick Port 2")
	PORT_CONFSETTING(0x00, "Standard 2-button MSX/FM-Towns joystick")
	PORT_CONFSETTING(0x10, "3-button Megadrive gamepad")
	PORT_CONFSETTING(0x20, "6-button Megadrive gamepad")
	PORT_CONFSETTING(0x30, "XPD-1LR dual D-pad gamepad")
// TODO: Sharp Cyber Stick (CZ-8NJ2) support

	PORT_START( "joy1" )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_CODE(JOYCODE_Y_UP_SWITCH)  PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x00)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_CODE(JOYCODE_Y_DOWN_SWITCH)  PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x00)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_CODE(JOYCODE_X_LEFT_SWITCH)  PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x00)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_CODE(JOYCODE_X_RIGHT_SWITCH)    PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x00)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x00)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(JOYCODE_BUTTON1)  PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x00)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_CODE(JOYCODE_BUTTON2)  PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x00)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x00)

	PORT_START( "joy2" )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_CODE(JOYCODE_Y_UP_SWITCH)  PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x00)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_CODE(JOYCODE_Y_DOWN_SWITCH)  PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x00)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_CODE(JOYCODE_X_LEFT_SWITCH)  PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x00)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_CODE(JOYCODE_X_RIGHT_SWITCH)    PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x00)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x00)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(JOYCODE_BUTTON1)  PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x00)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_CODE(JOYCODE_BUTTON2)  PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x00)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x00)

	PORT_START( "key1" )
	PORT_BIT( 0x00000001, IP_ACTIVE_HIGH, IPT_UNUSED) // unused
	PORT_BIT( 0x00000002, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("ESC") PORT_CODE(KEYCODE_ESC) PORT_CHAR(27)  /* ESC */
	PORT_BIT( 0x00000004, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("1  !  \xE3\x83\x8C") PORT_CODE(KEYCODE_1)  PORT_CHAR('1') PORT_CHAR('!') /* 1 ! */
	PORT_BIT( 0x00000008, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("2  \"  \xE3\x83\x95") PORT_CODE(KEYCODE_2)  PORT_CHAR('2') PORT_CHAR('\"') /* 2 " */
	PORT_BIT( 0x00000010, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("3  #  \xE3\x82\xA2  \xE3\x82\xA1") PORT_CODE(KEYCODE_3)  PORT_CHAR('3') PORT_CHAR('#') /* 3 # */
	PORT_BIT( 0x00000020, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("4  $  \xE3\x82\xA6  \xE3\x82\xA5") PORT_CODE(KEYCODE_4)  PORT_CHAR('4') PORT_CHAR('$') /* 4 $ */
	PORT_BIT( 0x00000040, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("5  %  \xE3\x82\xA8  \xE3\x82\xA7") PORT_CODE(KEYCODE_5)  PORT_CHAR('5') PORT_CHAR('%') /* 5 % */
	PORT_BIT( 0x00000080, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("6  &  \xE3\x82\xAA  \xE3\x82\xA9") PORT_CODE(KEYCODE_6)  PORT_CHAR('6') PORT_CHAR('&') /* 6 & */
	PORT_BIT( 0x00000100, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("7  \'  \xE3\x83\xA4  \xE3\x83\xA3") PORT_CODE(KEYCODE_7)  PORT_CHAR('7') PORT_CHAR('\'') /* 7 ' */
	PORT_BIT( 0x00000200, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("8  (  \xE3\x83\xA6  \xE3\x83\xA5") PORT_CODE(KEYCODE_8)  PORT_CHAR('8') PORT_CHAR('(') /* 8 ( */
	PORT_BIT( 0x00000400, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("9  )  \xE3\x83\xA8  \xE3\x83\xA7") PORT_CODE(KEYCODE_9)  PORT_CHAR('9') PORT_CHAR(')') /* 9 ) */
	PORT_BIT( 0x00000800, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("0  \xE3\x83\xAF  \xE3\x83\xB2") PORT_CODE(KEYCODE_0)  PORT_CHAR('0')                /* 0 */
	PORT_BIT( 0x00001000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("-  =  \xE3\x83\x9B") PORT_CODE(KEYCODE_MINUS)  PORT_CHAR('-') PORT_CHAR('=') /* - = */
	PORT_BIT( 0x00002000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("^  \xE3\x83\x98") PORT_CHAR('^') /* ^ */
	PORT_BIT( 0x00004000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("\xC2\xA5  \xE3\x83\xBC  |") PORT_CODE(KEYCODE_BACKSLASH)  PORT_CHAR('\\') PORT_CHAR('|') /* Yen | */
	PORT_BIT( 0x00008000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(8) /* Backspace */
	PORT_BIT( 0x00010000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_CODE(KEYCODE_TAB)  PORT_CHAR(9)  /* Tab */
	PORT_BIT( 0x00020000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Q  \xE3\x82\xBF") PORT_CODE(KEYCODE_Q)  PORT_CHAR('Q')  /* Q */
	PORT_BIT( 0x00040000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("W  \xE3\x83\x86") PORT_CODE(KEYCODE_W)  PORT_CHAR('W')  /* W */
	PORT_BIT( 0x00080000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("E  \xE3\x82\xA4  \xE3\x82\xA3") PORT_CODE(KEYCODE_E)  PORT_CHAR('E')  /* E */
	PORT_BIT( 0x00100000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("R  \xE3\x82\xB9") PORT_CODE(KEYCODE_R)  PORT_CHAR('R')  /* R */
	PORT_BIT( 0x00200000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("T  \xE3\x82\xAB") PORT_CODE(KEYCODE_T)  PORT_CHAR('T')  /* T */
	PORT_BIT( 0x00400000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Y  \xE3\x83\xB3") PORT_CODE(KEYCODE_Y)  PORT_CHAR('Y')  /* Y */
	PORT_BIT( 0x00800000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("U  \xE3\x83\x8A") PORT_CODE(KEYCODE_U)  PORT_CHAR('U')  /* U */
	PORT_BIT( 0x01000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("I  \xE3\x83\x8B") PORT_CODE(KEYCODE_I)  PORT_CHAR('I')  /* I */
	PORT_BIT( 0x02000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("O  \xE3\x83\xA9") PORT_CODE(KEYCODE_O)  PORT_CHAR('O')  /* O */
	PORT_BIT( 0x04000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("P  \xE3\x82\xBB") PORT_CODE(KEYCODE_P)  PORT_CHAR('P')  /* P */
	PORT_BIT( 0x08000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("@  `  \xE3\x82\x9B") PORT_CHAR('@') PORT_CHAR('`')  /* @ */
	PORT_BIT( 0x10000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("[  {  \xE3\x82\x9C \xE3\x80\x8C") PORT_CODE(KEYCODE_OPENBRACE)  PORT_CHAR('[') PORT_CHAR('{')  /* [ { */
	PORT_BIT( 0x20000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_CODE(KEYCODE_ENTER)  PORT_CHAR(13)  /* Return */
	PORT_BIT( 0x40000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("A  \xE3\x83\x81") PORT_CODE(KEYCODE_A)  PORT_CHAR('A')  /* A */
	PORT_BIT( 0x80000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("S  \xE3\x83\x88") PORT_CODE(KEYCODE_S)  PORT_CHAR('S')  /* S */

	PORT_START( "key2" )
	PORT_BIT( 0x00000001, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("D  \xE3\x82\xB7") PORT_CODE(KEYCODE_D)  PORT_CHAR('D')  /* D */
	PORT_BIT( 0x00000002, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F  \xE3\x83\x8F") PORT_CODE(KEYCODE_F)  PORT_CHAR('F')  /* F */
	PORT_BIT( 0x00000004, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("G  \xE3\x82\xAD") PORT_CODE(KEYCODE_G)  PORT_CHAR('G')  /* G */
	PORT_BIT( 0x00000008, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("H  \xE3\x82\xAF") PORT_CODE(KEYCODE_H)  PORT_CHAR('H')  /* H */
	PORT_BIT( 0x00000010, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("J  \xE3\x83\x9E") PORT_CODE(KEYCODE_J)  PORT_CHAR('J')  /* J */
	PORT_BIT( 0x00000020, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("K  \xE3\x83\x8E") PORT_CODE(KEYCODE_K)  PORT_CHAR('K')  /* K */
	PORT_BIT( 0x00000040, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("L  \xE3\x83\xAA") PORT_CODE(KEYCODE_L)  PORT_CHAR('L')  /* L */
	PORT_BIT( 0x00000080, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME(";  +  \xE3\x83\xAC") PORT_CODE(KEYCODE_COLON)  PORT_CHAR(';')  PORT_CHAR('+')  /* ; + */
	PORT_BIT( 0x00000100, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME(":  *  \xE3\x82\xB1") PORT_CODE(KEYCODE_QUOTE)  PORT_CHAR(':')  PORT_CHAR('*')  /* : * */
	PORT_BIT( 0x00000200, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("]  }  \xE3\x83\xA0  \xE3\x80\x8D") PORT_CODE(KEYCODE_CLOSEBRACE)  PORT_CHAR(']')  PORT_CHAR('}')  /* ] } */
	PORT_BIT( 0x00000400, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Z  \xE3\x83\x84  \xE3\x83\x83") PORT_CODE(KEYCODE_Z)  PORT_CHAR('Z')  /* Z */
	PORT_BIT( 0x00000800, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("X  \xE3\x82\xB5") PORT_CODE(KEYCODE_X)  PORT_CHAR('X')  /* X */
	PORT_BIT( 0x00001000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("C  \xE3\x82\xBD") PORT_CODE(KEYCODE_C)  PORT_CHAR('C')  /* C */
	PORT_BIT( 0x00002000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("V  \xE3\x83\x92") PORT_CODE(KEYCODE_V)  PORT_CHAR('V')  /* V */
	PORT_BIT( 0x00004000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("B  \xE3\x82\xB3") PORT_CODE(KEYCODE_B)  PORT_CHAR('B')  /* B */
	PORT_BIT( 0x00008000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("N  \xE3\x83\x9F") PORT_CODE(KEYCODE_N)  PORT_CHAR('N')  /* N */
	PORT_BIT( 0x00010000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("M  \xE3\x83\xA2") PORT_CODE(KEYCODE_M)  PORT_CHAR('M')  /* M */
	PORT_BIT( 0x00020000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME(",  <  \xE3\x83\x8D  \xE3\x80\x81") PORT_CODE(KEYCODE_COMMA)  PORT_CHAR(',')  PORT_CHAR('<')  /* , < */
	PORT_BIT( 0x00040000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME(".  >  \xE3\x83\xAB  \xE3\x80\x82") PORT_CODE(KEYCODE_STOP)  PORT_CHAR('.')  PORT_CHAR('>')  /* . > */
	PORT_BIT( 0x00080000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("/  ?  \xE3\x83\xA1  \xE3\x83\xBB") PORT_CODE(KEYCODE_SLASH)  PORT_CHAR('/')  PORT_CHAR('?')  /* / ? */
	PORT_BIT( 0x00100000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("_  \xE3\x83\xAD") PORT_CHAR('_')  /* Underscore (shifted only?) */
	PORT_BIT( 0x00200000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Space")  PORT_CODE(KEYCODE_SPACE)  PORT_CHAR(' ')  /* Space */
	PORT_BIT( 0x00400000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Home")  PORT_CODE(KEYCODE_HOME)  /* Home */
	PORT_BIT( 0x00800000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Delete")  PORT_CODE(KEYCODE_DEL)  /* Del */
	PORT_BIT( 0x01000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Roll Up")  PORT_CODE(KEYCODE_PGUP)  /* Roll Up */
	PORT_BIT( 0x02000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Roll Down")  PORT_CODE(KEYCODE_PGDN)  /* Roll Down */
	PORT_BIT( 0x04000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Undo")  PORT_CODE(KEYCODE_END)  /* Undo */
	PORT_BIT( 0x08000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Cursor Left")  PORT_CODE(KEYCODE_LEFT)  /* Left */
	PORT_BIT( 0x10000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Cursor Up")  PORT_CODE(KEYCODE_UP)  /* Up */
	PORT_BIT( 0x20000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Cursor Right")  PORT_CODE(KEYCODE_RIGHT)  /* Right */
	PORT_BIT( 0x40000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Cursor Down")  PORT_CODE(KEYCODE_DOWN)  /* Down */
	PORT_BIT( 0x80000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey CLR")  PORT_CODE(KEYCODE_NUMLOCK)  /* CLR */

	PORT_START( "key3" )
	PORT_BIT( 0x00000001, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey /")  PORT_CODE(KEYCODE_SLASH_PAD)  /* / (numpad) */
	PORT_BIT( 0x00000002, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey *")  PORT_CODE(KEYCODE_ASTERISK)  /* * (numpad) */
	PORT_BIT( 0x00000004, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey -")  PORT_CODE(KEYCODE_MINUS_PAD)  /* - (numpad) */
	PORT_BIT( 0x00000008, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 7")  PORT_CODE(KEYCODE_7_PAD)  /* 7 (numpad) */
	PORT_BIT( 0x00000010, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 8")  PORT_CODE(KEYCODE_8_PAD)  /* 8 (numpad) */
	PORT_BIT( 0x00000020, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 9")  PORT_CODE(KEYCODE_9_PAD)  /* 9 (numpad) */
	PORT_BIT( 0x00000040, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey +")  PORT_CODE(KEYCODE_PLUS_PAD)  /* + (numpad) */
	PORT_BIT( 0x00000080, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 4")  PORT_CODE(KEYCODE_4_PAD)  /* 4 (numpad) */
	PORT_BIT( 0x00000100, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 5")  PORT_CODE(KEYCODE_5_PAD)  /* 5 (numpad) */
	PORT_BIT( 0x00000200, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 6")  PORT_CODE(KEYCODE_6_PAD)  /* 6 (numpad) */
	PORT_BIT( 0x00000400, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey =")  /* = (numpad) */
	PORT_BIT( 0x00000800, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 1")  PORT_CODE(KEYCODE_1_PAD)  /* 1 (numpad) */
	PORT_BIT( 0x00001000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 2")  PORT_CODE(KEYCODE_2_PAD)  /* 2 (numpad) */
	PORT_BIT( 0x00002000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 3")  PORT_CODE(KEYCODE_3_PAD)  /* 3 (numpad) */
	PORT_BIT( 0x00004000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey Enter")  PORT_CODE(KEYCODE_ENTER_PAD)  /* Enter (numpad) */
	PORT_BIT( 0x00008000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey 0")  PORT_CODE(KEYCODE_0_PAD)  /* 0 (numpad) */
	PORT_BIT( 0x00010000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey ,")  /* , (numpad) */
	PORT_BIT( 0x00020000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Tenkey .")  PORT_CODE(KEYCODE_DEL_PAD)  /* 2 (numpad) */
	PORT_BIT( 0x00040000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("\xE8\xA8\x98\xE5\x8F\xB7 (Symbolic input)")  /* Sign / Symbolic input (babelfish translation) */
	PORT_BIT( 0x00080000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("\xE7\x99\xBB\xE9\x8C\xB2 (Register)")  /* Register (babelfish translation) */
	PORT_BIT( 0x00100000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Help")  /* Help */
	PORT_BIT( 0x00200000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("XF1")  PORT_CODE(KEYCODE_F11)  /* XF1 */
	PORT_BIT( 0x00400000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("XF2")  PORT_CODE(KEYCODE_F12)  /* XF2 */
	PORT_BIT( 0x00800000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("XF3")  /* XF3 */
	PORT_BIT( 0x01000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("XF4")  /* XF4 */
	PORT_BIT( 0x02000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("XF5")  /* XF5 */
	PORT_BIT( 0x04000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("\xe3\x81\x8b\xe3\x81\xaa (Kana)")  /* Kana */
	PORT_BIT( 0x08000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("\xe3\x83\xad\xe3\x83\xbc\xe3\x83\x9e\xe5\xad\x97 (Romaji)")  /* Romaji */
	PORT_BIT( 0x10000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("\xE3\x82\xB3\xE3\x83\xBC\xE3\x83\x89 (Code input)")  /* Code input */
	PORT_BIT( 0x20000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Caps")  PORT_CODE(KEYCODE_CAPSLOCK)  /* Caps lock */
	PORT_BIT( 0x40000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Insert")  PORT_CODE(KEYCODE_INSERT)  /* Insert */
	PORT_BIT( 0x80000000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("\xE3\x81\xB2\xE3\x82\x89\xE3\x81\x8C\xE3\x81\xAA (Hiragana)")  /* Hiragana */

	PORT_START( "key4" )
	PORT_BIT( 0x00000001, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("\xE5\x85\xA8\xE8\xA7\x92 (Full size)")  /* Full size (babelfish translation) */
	PORT_BIT( 0x00000002, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Break")  /* Break */
	PORT_BIT( 0x00000004, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Copy")  /* Copy */
	PORT_BIT( 0x00000008, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F1")  PORT_CODE(KEYCODE_F1)  /* F1 */
	PORT_BIT( 0x00000010, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F2")  PORT_CODE(KEYCODE_F2)  /* F2 */
	PORT_BIT( 0x00000020, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F3")  PORT_CODE(KEYCODE_F3)  /* F3 */
	PORT_BIT( 0x00000040, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F4")  PORT_CODE(KEYCODE_F4)  /* F4 */
	PORT_BIT( 0x00000080, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F5")  PORT_CODE(KEYCODE_F5)  /* F5 */
	PORT_BIT( 0x00000100, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F6")  PORT_CODE(KEYCODE_F6)  /* F6 */
	PORT_BIT( 0x00000200, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F7")  PORT_CODE(KEYCODE_F7)  /* F7 */
	PORT_BIT( 0x00000400, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F8")  PORT_CODE(KEYCODE_F8)  /* F8 */
	PORT_BIT( 0x00000800, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F9")  PORT_CODE(KEYCODE_F9)  /* F9 */
	PORT_BIT( 0x00001000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("F10")  PORT_CODE(KEYCODE_F10)  /* F10 */
		// 0x6d reserved
		// 0x6e reserved
		// 0x6f reserved
	PORT_BIT( 0x00010000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Shift")  PORT_CODE(KEYCODE_LSHIFT)  /* Shift */
	PORT_BIT( 0x00020000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Ctrl")  PORT_CODE(KEYCODE_LCONTROL)  /* Ctrl */
	PORT_BIT( 0x00040000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Opt. 1")  PORT_CODE(KEYCODE_PRTSCR) /* Opt1 */
	PORT_BIT( 0x00080000, IP_ACTIVE_HIGH, IPT_KEYBOARD )  PORT_NAME("Opt. 2")  PORT_CODE(KEYCODE_PAUSE)  /* Opt2 */

	PORT_START("options")
	PORT_CONFNAME( 0x01, 0x01, "Enable keyboard hack")
	PORT_CONFSETTING(   0x00, DEF_STR( Off ))
	PORT_CONFSETTING(   0x01, DEF_STR( On ))
	PORT_CONFNAME( 0x02, 0x02, "Enable fake bus errors")
	PORT_CONFSETTING(   0x00, DEF_STR( Off ))
	PORT_CONFSETTING(   0x02, DEF_STR( On ))
	PORT_CONFNAME( 0x04, 0x04, "Enable partial updates on each HSync")
	PORT_CONFSETTING(   0x00, DEF_STR( Off ))
	PORT_CONFSETTING(   0x04, DEF_STR( On ))

	PORT_START("mouse1")  // mouse buttons
	PORT_BIT( 0x00000001, IP_ACTIVE_HIGH, IPT_BUTTON9) PORT_NAME("Left mouse button") PORT_CODE(MOUSECODE_BUTTON1)
	PORT_BIT( 0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_NAME("Right mouse button") PORT_CODE(MOUSECODE_BUTTON2)

	PORT_START("mouse2")  // X-axis
	PORT_BIT( 0xff, 0x00, IPT_MOUSE_X) PORT_SENSITIVITY(100) PORT_KEYDELTA(0) PORT_PLAYER(1)

	PORT_START("mouse3")  // Y-axis
	PORT_BIT( 0xff, 0x00, IPT_MOUSE_Y) PORT_SENSITIVITY(100) PORT_KEYDELTA(0) PORT_PLAYER(1)

	// 3-button Megadrive gamepad
	PORT_START("md3b")
	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("MD Pad 1 Up") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("MD Pad 1 Down") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_NAME("MD Pad 1 Left") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_NAME("MD Pad 1 Right") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 B Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 C Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000080, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)

	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000400, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00000800, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00001000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00002000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 A Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00004000, IP_ACTIVE_LOW, IPT_START ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 Start Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x00008000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x01)

	PORT_BIT( 0x00010000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("MD Pad 2 Up") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x00020000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("MD Pad 2 Down") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x00040000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_NAME("MD Pad 2 Left") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x00080000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_NAME("MD Pad 2 Right") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x00100000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x00200000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 B Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x00400000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 C Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x00800000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)

	PORT_BIT( 0x01000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x02000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x04000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x08000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x10000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x20000000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 A Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x40000000, IP_ACTIVE_LOW, IPT_START ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 Start Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x80000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x10)

	// 6-button Megadrive gamepad
	PORT_START("md6b")
	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("MD Pad 1 Up") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("MD Pad 1 Down") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_NAME("MD Pad 1 Left") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_NAME("MD Pad 1 Right") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 B Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 C Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000080, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)

	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000400, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00000800, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00001000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00002000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 A Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00004000, IP_ACTIVE_LOW, IPT_START ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 Start Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x00008000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)

	PORT_BIT( 0x00010000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("MD Pad 2 Up") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x00020000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("MD Pad 2 Down") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x00040000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_NAME("MD Pad 2 Left") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x00080000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_NAME("MD Pad 2 Right") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x00100000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x00200000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 B Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x00400000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 C Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x00800000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)

	PORT_BIT( 0x01000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x02000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x04000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x08000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x10000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x20000000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 A Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x40000000, IP_ACTIVE_LOW, IPT_START ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 Start Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x80000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)

	// extra inputs
	PORT_START("md6b_extra")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 Z Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 Y Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 X Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON7 ) PORT_PLAYER(1) PORT_NAME("MD Pad 1 Mode Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x02)

	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 Z Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 Y Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 X Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON7 ) PORT_PLAYER(2) PORT_NAME("MD Pad 2 Mode Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x20)

	// Dempa/Micomsoft XPD-1LR (dual D-pad gamepad sold with Libble Rabble)
	PORT_START("xpd1lr")
	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_NAME("XPD Pad 1 Left/Up") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_NAME("XPD Pad 1 Left/Down") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_NAME("XPD Pad 1 Left/Left") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_NAME("XPD Pad 1 Left/Right") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("XPD Pad 1 B Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("XPD Pad 1 A Button") PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000080, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)

	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_NAME("XPD Pad 1 Right/Up") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_NAME("XPD Pad 1 Right/Down") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000400, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_NAME("XPD Pad 1 Right/Left") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00000800, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_NAME("XPD Pad 1 Right/Right") PORT_8WAY PORT_PLAYER(1) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00001000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00002000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00004000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x00008000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0x0f, EQUALS, 0x03)

	PORT_BIT( 0x00010000, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_NAME("XPD Pad 2 Left/Up") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x00020000, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_NAME("XPD Pad 2 Left/Down") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x00040000, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_NAME("XPD Pad 2 Left/Left") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x00080000, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_NAME("XPD Pad 2 Left/Right") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x00100000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x00200000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("XPD Pad 2 B Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x00400000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("XPD Pad 2 A Button") PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x00800000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)

	PORT_BIT( 0x01000000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_NAME("XPD Pad 2 Right/Up") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x02000000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_NAME("XPD Pad 2 Right/Down") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x04000000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_NAME("XPD Pad 2 Right/Left") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x08000000, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_NAME("XPD Pad 2 Right/Right") PORT_8WAY PORT_PLAYER(2) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x10000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x20000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x40000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)
	PORT_BIT( 0x80000000, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_CONDITION("ctrltype", 0xf0, EQUALS, 0x30)

INPUT_PORTS_END

void x68k_state::floppy_load_unload()
{
	if(m_ioc.irqstatus & 0x02)
	{
		m_current_vector[1] = 0x61;
		m_ioc.irqstatus |= 0x40;
		m_current_irq_line = 1;
		machine().device("maincpu")->execute().set_input_line_and_vector(1,ASSERT_LINE,m_current_vector[1]);  // Disk insert/eject interrupt
		logerror("IOC: Disk image inserted\n");
	}
}

int x68k_state::floppy_load(floppy_image_device *dev)
{
	floppy_load_unload();
	return IMAGE_INIT_PASS;
}

void x68k_state::floppy_unload(floppy_image_device *dev)
{
	floppy_load_unload();
}

TIMER_CALLBACK_MEMBER(x68k_state::x68k_net_irq)
{

	m_current_vector[2] = 0xf9;
	m_current_irq_line = 2;
	machine().device("maincpu")->execute().set_input_line_and_vector(2,ASSERT_LINE,m_current_vector[2]);
}

WRITE_LINE_MEMBER(x68k_state::x68k_irq2_line)
{
	if(state==ASSERT_LINE)
	{
		m_net_timer->adjust(attotime::from_usec(16));
	}
	else
		machine().device("maincpu")->execute().set_input_line_and_vector(2,CLEAR_LINE,m_current_vector[2]);
	logerror("EXP: IRQ2 set to %i\n",state);

}

static const mb89352_interface x68k_scsi_intf =
{
	DEVCB_DRIVER_LINE_MEMBER(x68k_state,x68k_scsi_irq),
	DEVCB_DRIVER_LINE_MEMBER(x68k_state,x68k_scsi_drq)
};

static X68K_EXPANSION_INTERFACE(x68k_exp_intf)
{
	DEVCB_DRIVER_LINE_MEMBER(x68k_state,x68k_irq2_line),
	DEVCB_CPU_INPUT_LINE("maincpu", M68K_IRQ_4),
	DEVCB_CPU_INPUT_LINE("maincpu", INPUT_LINE_NMI),
	DEVCB_NULL  // RESET
};

static SLOT_INTERFACE_START(x68000_exp_cards)
	SLOT_INTERFACE("neptunex",X68K_NEPTUNEX) // Neptune-X ethernet adapter (ISA NE2000 bridge)
	SLOT_INTERFACE("cz6bs1",X68K_SCSIEXT)  // Sharp CZ-6BS1 SCSI-1 controller
SLOT_INTERFACE_END

MACHINE_RESET_MEMBER(x68k_state,x68000)
{
	/* The last half of the IPLROM is mapped to 0x000000 on reset only
	   Just copying the inital stack pointer and program counter should
	   more or less do the same job */

	int drive;
	UINT8* romdata = memregion("user2")->base();
	attotime irq_time;

	memset(machine().device<ram_device>(RAM_TAG)->pointer(),0,machine().device<ram_device>(RAM_TAG)->size());
	memcpy(machine().device<ram_device>(RAM_TAG)->pointer(),romdata,8);

	// init keyboard
	m_keyboard.delay = 500;  // 3*100+200
	m_keyboard.repeat = 110;  // 4^2*5+30

	// initialise CRTC, set registers to defaults for the standard text mode (768x512)
	m_crtc.reg[0] = 137;  // Horizontal total  (in characters)
	m_crtc.reg[1] = 14;   // Horizontal sync end
	m_crtc.reg[2] = 28;   // Horizontal start
	m_crtc.reg[3] = 124;  // Horizontal end
	m_crtc.reg[4] = 567;  // Vertical total
	m_crtc.reg[5] = 5;    // Vertical sync end
	m_crtc.reg[6] = 40;   // Vertical start
	m_crtc.reg[7] = 552;  // Vertical end
	m_crtc.reg[8] = 27;   // Horizontal adjust

	mfp_init();

	m_scanline = machine().primary_screen->vpos();// = m_crtc.reg[6];  // Vertical start

	// start VBlank timer
	m_crtc.vblank = 1;
	irq_time = machine().primary_screen->time_until_pos(m_crtc.reg[6],2);
	m_vblank_irq->adjust(irq_time);

	// start HBlank timer
	m_scanline_timer->adjust(machine().primary_screen->scan_period(), 1);

	m_mfp.gpio = 0xfb;

	// reset output values
	output_set_value("key_led_kana",1);
	output_set_value("key_led_romaji",1);
	output_set_value("key_led_code",1);
	output_set_value("key_led_caps",1);
	output_set_value("key_led_insert",1);
	output_set_value("key_led_hiragana",1);
	output_set_value("key_led_fullsize",1);
	for(drive=0;drive<4;drive++)
	{
		output_set_indexed_value("eject_drv",drive,1);
		output_set_indexed_value("ctrl_drv",drive,1);
		output_set_indexed_value("access_drv",drive,1);
	}

	// reset CPU
	machine().device("maincpu")->reset();
}

MACHINE_START_MEMBER(x68k_state,x68000)
{
	address_space &space = machine().device("maincpu")->memory().space(AS_PROGRAM);
	/*  Install RAM handlers  */
	m_spriteram = (UINT16*)(*memregion("user1"));
	space.install_read_handler(0x000000,0xbffffb,0xffffffff,0,read16_delegate(FUNC(x68k_state::x68k_emptyram_r),this));
	space.install_write_handler(0x000000,0xbffffb,0xffffffff,0,write16_delegate(FUNC(x68k_state::x68k_emptyram_w),this));
	space.install_readwrite_bank(0x000000,machine().device<ram_device>(RAM_TAG)->size()-1,0xffffffff,0,"bank1");
	membank("bank1")->set_base(machine().device<ram_device>(RAM_TAG)->pointer());
	space.install_read_handler(0xc00000,0xdfffff,0xffffffff,0,read16_delegate(FUNC(x68k_state::x68k_gvram_r),this));
	space.install_write_handler(0xc00000,0xdfffff,0xffffffff,0,write16_delegate(FUNC(x68k_state::x68k_gvram_w),this));
	membank("bank2")->set_base(m_gvram16);  // so that code in VRAM is executable - needed for Terra Cresta
	space.install_read_handler(0xe00000,0xe7ffff,0xffffffff,0,read16_delegate(FUNC(x68k_state::x68k_tvram_r),this));
	space.install_write_handler(0xe00000,0xe7ffff,0xffffffff,0,write16_delegate(FUNC(x68k_state::x68k_tvram_w),this));
	membank("bank3")->set_base(m_tvram16);  // so that code in VRAM is executable - needed for Terra Cresta
	space.install_read_handler(0xed0000,0xed3fff,0xffffffff,0,read16_delegate(FUNC(x68k_state::x68k_sram_r),this));
	space.install_write_handler(0xed0000,0xed3fff,0xffffffff,0,write16_delegate(FUNC(x68k_state::x68k_sram_w),this));
	membank("bank4")->set_base(m_nvram16);  // so that code in SRAM is executable, there is an option for booting from SRAM

	// start keyboard timer
	m_kb_timer->adjust(attotime::zero, 0, attotime::from_msec(5));  // every 5ms

	// start mouse timer
	m_mouse_timer->adjust(attotime::zero, 0, attotime::from_msec(1));  // a guess for now
	m_mouse.inputtype = 0;

	// start LED timer
	m_led_timer->adjust(attotime::zero, 0, attotime::from_msec(400));

	// check for disks
	m_fdc.fdc = machine().device<upd72065_device>("upd72065");
	m_fdc.fdc->setup_intrq_cb(upd72065_device::line_cb(FUNC(x68k_state::fdc_irq), this));
	m_fdc.fdc->setup_drq_cb(upd72065_device::line_cb(FUNC(x68k_state::fdc_drq), this));

	for(int drive=0;drive<4;drive++)
	{
		char devname[16];
		sprintf(devname, "upd72065:%d", drive);
		floppy_image_device *floppy = machine().device<floppy_connector>(devname)->get_device();
		m_fdc.floppy[drive] = floppy;
		if(floppy) {
			floppy->setup_load_cb(floppy_image_device::load_cb(FUNC(x68k_state::floppy_load), this));
			floppy->setup_unload_cb(floppy_image_device::unload_cb(FUNC(x68k_state::floppy_unload), this));
		}
	}
}

MACHINE_START_MEMBER(x68k_state,x68030)
{
	address_space &space = machine().device("maincpu")->memory().space(AS_PROGRAM);
	/*  Install RAM handlers  */
	m_spriteram = (UINT16*)(*memregion("user1"));
	space.install_read_handler(0x000000,0xbffffb,0xffffffff,0,read16_delegate(FUNC(x68k_state::x68k_rom0_r),this),0xffffffff);
	space.install_write_handler(0x000000,0xbffffb,0xffffffff,0,write16_delegate(FUNC(x68k_state::x68k_rom0_w),this),0xffffffff);
	space.install_readwrite_bank(0x000000,machine().device<ram_device>(RAM_TAG)->size()-1,0xffffffff,0,"bank1");
	membank("bank1")->set_base(machine().device<ram_device>(RAM_TAG)->pointer());
	space.install_read_handler(0xc00000,0xdfffff,0xffffffff,0,read32_delegate(FUNC(x68k_state::x68k_gvram32_r),this));
	space.install_write_handler(0xc00000,0xdfffff,0xffffffff,0,write32_delegate(FUNC(x68k_state::x68k_gvram32_w),this));
	membank("bank2")->set_base(m_gvram32);  // so that code in VRAM is executable - needed for Terra Cresta
	space.install_read_handler(0xe00000,0xe7ffff,0xffffffff,0,read32_delegate(FUNC(x68k_state::x68k_tvram32_r),this));
	space.install_write_handler(0xe00000,0xe7ffff,0xffffffff,0,write32_delegate(FUNC(x68k_state::x68k_tvram32_w),this));
	membank("bank3")->set_base(m_tvram32);  // so that code in VRAM is executable - needed for Terra Cresta
	space.install_read_handler(0xed0000,0xed3fff,0xffffffff,0,read32_delegate(FUNC(x68k_state::x68k_sram32_r),this));
	space.install_write_handler(0xed0000,0xed3fff,0xffffffff,0,write32_delegate(FUNC(x68k_state::x68k_sram32_w),this));
	membank("bank4")->set_base(m_nvram32);  // so that code in SRAM is executable, there is an option for booting from SRAM

	// start keyboard timer
	m_kb_timer->adjust(attotime::zero, 0, attotime::from_msec(5));  // every 5ms

	// start mouse timer
	m_mouse_timer->adjust(attotime::zero, 0, attotime::from_msec(1));  // a guess for now
	m_mouse.inputtype = 0;

	// start LED timer
	m_led_timer->adjust(attotime::zero, 0, attotime::from_msec(400));

	// check for disks
	m_fdc.fdc = machine().device<upd72065_device>("upd72065");
	m_fdc.fdc->setup_intrq_cb(upd72065_device::line_cb(FUNC(x68k_state::fdc_irq), this));
	m_fdc.fdc->setup_drq_cb(upd72065_device::line_cb(FUNC(x68k_state::fdc_drq), this));

	for(int drive=0;drive<4;drive++)
	{
		char devname[16];
		sprintf(devname, "upd72065:%d", drive);
		floppy_image_device *floppy = machine().device<floppy_connector>(devname)->get_device();
		m_fdc.floppy[drive] = floppy;
		if(floppy) {
			floppy->setup_load_cb(floppy_image_device::load_cb(FUNC(x68k_state::floppy_load), this));
			floppy->setup_unload_cb(floppy_image_device::unload_cb(FUNC(x68k_state::floppy_unload), this));
		}
	}
}

DRIVER_INIT_MEMBER(x68k_state,x68000)
{
	unsigned char* rom = machine().root_device().memregion("maincpu")->base();
	unsigned char* user2 = machine().root_device().memregion("user2")->base();
	//FIXME
//  m_gvram = auto_alloc_array(machine(), UINT16, 0x080000/sizeof(UINT16));
//  m_tvram = auto_alloc_array(machine(), UINT16, 0x080000/sizeof(UINT16));
	m_sram = auto_alloc_array(machine(), UINT16, 0x4000/sizeof(UINT16));

	m_spritereg = auto_alloc_array_clear(machine(), UINT16, 0x8000/sizeof(UINT16));

#ifdef USE_PREDEFINED_SRAM
	{
		unsigned char* ramptr = memregion("user3")->base();
		memcpy(m_sram,ramptr,0x4000);
	}
#endif

	// copy last half of BIOS to a user region, to use for inital startup
	memcpy(user2,(rom+0xff0000),0x10000);

	mfp_init();

	machine().device("maincpu")->execute().set_irq_acknowledge_callback(x68k_int_ack);

	// init keyboard
	m_keyboard.delay = 500;  // 3*100+200
	m_keyboard.repeat = 110;  // 4^2*5+30
	m_kb_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::x68k_keyboard_poll),this));
	m_scanline_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::x68k_hsync),this));
	m_raster_irq = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::x68k_crtc_raster_irq),this));
	m_vblank_irq = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::x68k_crtc_vblank_irq),this));
	m_mouse_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::x68k_scc_ack),this));
	m_led_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::x68k_led_callback),this));
	m_net_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(x68k_state::x68k_net_irq),this));

	// Initialise timers for 6-button MD controllers
	md_6button_init();

	m_sysport.cputype = 0xff;  // 68000, 10MHz
	m_is_32bit = false;
}

DRIVER_INIT_MEMBER(x68k_state,x68kxvi)
{
	DRIVER_INIT_CALL( x68000 );
	m_sysport.cputype = 0xfe; // 68000, 16MHz
	m_is_32bit = false;
}

DRIVER_INIT_MEMBER(x68k_state,x68030)
{
	DRIVER_INIT_CALL( x68000 );
	m_sysport.cputype = 0xdc; // 68030, 25MHz
	m_is_32bit = true;
}

FLOPPY_FORMATS_MEMBER( x68k_state::floppy_formats )
	FLOPPY_XDF_FORMAT
FLOPPY_FORMATS_END

static SLOT_INTERFACE_START( x68k_floppies )
	SLOT_INTERFACE( "525hd", FLOPPY_525_HD )
SLOT_INTERFACE_END

static MACHINE_CONFIG_FRAGMENT( x68000_base )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, 10000000)  /* 10 MHz */
	MCFG_CPU_PROGRAM_MAP(x68k_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", x68k_state,  x68k_vsync_irq)
	MCFG_QUANTUM_TIME(attotime::from_hz(60))

	MCFG_MACHINE_START_OVERRIDE(x68k_state, x68000 )
	MCFG_MACHINE_RESET_OVERRIDE(x68k_state, x68000 )

	/* device hardware */
	MCFG_MC68901_ADD(MC68901_TAG, 4000000, mfp_interface)

	MCFG_I8255A_ADD( "ppi8255",  ppi_interface )

	MCFG_HD63450_ADD( "hd63450", dmac_interface )

	MCFG_SCC8530_ADD( "scc", 5000000, line_cb_t() )

	MCFG_RP5C15_ADD(RP5C15_TAG, XTAL_32_768kHz, rtc_intf)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(55.45)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
//  MCFG_GFXDECODE(x68k)
	MCFG_SCREEN_SIZE(1096, 568)  // inital setting
	MCFG_SCREEN_VISIBLE_AREA(0, 767, 0, 511)
	MCFG_SCREEN_UPDATE_DRIVER(x68k_state, screen_update_x68000)

	MCFG_PALETTE_LENGTH(65536)
	MCFG_PALETTE_INIT_OVERRIDE(x68k_state, x68000 )

	MCFG_VIDEO_START_OVERRIDE(x68k_state, x68000 )

	MCFG_DEFAULT_LAYOUT( layout_x68000 )

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_YM2151_ADD("ym2151", 4000000)
	MCFG_YM2151_IRQ_HANDLER(WRITELINE(x68k_state,x68k_fm_irq))
	MCFG_YM2151_PORT_WRITE_HANDLER(WRITE8(x68k_state,x68k_ct_w))  // CT1, CT2 from YM2151 port 0x1b
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.50)
	MCFG_SOUND_ADD("okim6258", OKIM6258, 4000000)
	MCFG_SOUND_CONFIG(x68k_okim6258_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.50)

	MCFG_UPD72065_ADD("upd72065", true, true)
	MCFG_FLOPPY_DRIVE_ADD("upd72065:0", x68k_floppies, "525hd", 0, x68k_state::floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD("upd72065:1", x68k_floppies, "525hd", 0, x68k_state::floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD("upd72065:2", x68k_floppies, "525hd", 0, x68k_state::floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD("upd72065:3", x68k_floppies, "525hd", 0, x68k_state::floppy_formats)

	MCFG_SOFTWARE_LIST_ADD("flop_list","x68k_flop")

	MCFG_X68K_EXPANSION_SLOT_ADD("exp",x68k_exp_intf,x68000_exp_cards,NULL,NULL)

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("4M")
	MCFG_RAM_EXTRA_OPTIONS("1M,2M,3M,5M,6M,7M,8M,9M,10M,11M,12M")
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( x68000, x68k_state )
	MCFG_FRAGMENT_ADD(x68000_base)

	MCFG_NVRAM_ADD_0FILL("nvram16")

	MCFG_X68KHDC_ADD( "x68k_hdc" )

MACHINE_CONFIG_END

static MACHINE_CONFIG_START( x68ksupr, x68k_state )
	MCFG_FRAGMENT_ADD(x68000_base)

	MCFG_NVRAM_ADD_0FILL("nvram16")

	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(x68kxvi_map)

	MCFG_SCSIBUS_ADD("scsi")
	MCFG_SCSIDEV_ADD("scsi:harddisk0", SCSIHD, SCSI_ID_0)
	MCFG_SCSIDEV_ADD("scsi:harddisk1", SCSIHD, SCSI_ID_1)
	MCFG_SCSIDEV_ADD("scsi:harddisk2", SCSIHD, SCSI_ID_2)
	MCFG_SCSIDEV_ADD("scsi:harddisk3", SCSIHD, SCSI_ID_3)
	MCFG_SCSIDEV_ADD("scsi:harddisk4", SCSIHD, SCSI_ID_4)
	MCFG_SCSIDEV_ADD("scsi:harddisk5", SCSIHD, SCSI_ID_5)
	MCFG_SCSIDEV_ADD("scsi:harddisk6", SCSIHD, SCSI_ID_6)
	MCFG_MB89352A_ADD("scsi:mb89352",x68k_scsi_intf)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( x68kxvi, x68k_state )

	MCFG_FRAGMENT_ADD(x68000_base)

	MCFG_NVRAM_ADD_0FILL("nvram16")

	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK(16000000)  /* 16 MHz */
	MCFG_CPU_PROGRAM_MAP(x68kxvi_map)

	MCFG_SCSIBUS_ADD("scsi")
	MCFG_SCSIDEV_ADD("scsi:harddisk0", SCSIHD, SCSI_ID_0)
	MCFG_SCSIDEV_ADD("scsi:harddisk1", SCSIHD, SCSI_ID_1)
	MCFG_SCSIDEV_ADD("scsi:harddisk2", SCSIHD, SCSI_ID_2)
	MCFG_SCSIDEV_ADD("scsi:harddisk3", SCSIHD, SCSI_ID_3)
	MCFG_SCSIDEV_ADD("scsi:harddisk4", SCSIHD, SCSI_ID_4)
	MCFG_SCSIDEV_ADD("scsi:harddisk5", SCSIHD, SCSI_ID_5)
	MCFG_SCSIDEV_ADD("scsi:harddisk6", SCSIHD, SCSI_ID_6)
	MCFG_MB89352A_ADD("scsi:mb89352",x68k_scsi_intf)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( x68030, x68k_state )
	MCFG_FRAGMENT_ADD(x68000_base)

	MCFG_CPU_REPLACE("maincpu", M68030, 25000000)  /* 25 MHz 68EC030 */
	MCFG_CPU_PROGRAM_MAP(x68030_map)

	MCFG_MACHINE_START_OVERRIDE(x68k_state, x68030 )
	MCFG_MACHINE_RESET_OVERRIDE(x68k_state, x68000 )

	MCFG_NVRAM_ADD_0FILL("nvram32")

	MCFG_SCSIBUS_ADD("scsi")
	MCFG_SCSIDEV_ADD("scsi:harddisk0", SCSIHD, SCSI_ID_0)
	MCFG_SCSIDEV_ADD("scsi:harddisk1", SCSIHD, SCSI_ID_1)
	MCFG_SCSIDEV_ADD("scsi:harddisk2", SCSIHD, SCSI_ID_2)
	MCFG_SCSIDEV_ADD("scsi:harddisk3", SCSIHD, SCSI_ID_3)
	MCFG_SCSIDEV_ADD("scsi:harddisk4", SCSIHD, SCSI_ID_4)
	MCFG_SCSIDEV_ADD("scsi:harddisk5", SCSIHD, SCSI_ID_5)
	MCFG_SCSIDEV_ADD("scsi:harddisk6", SCSIHD, SCSI_ID_6)
	MCFG_MB89352A_ADD("scsi:mb89352",x68k_scsi_intf)
MACHINE_CONFIG_END

ROM_START( x68000 )
	ROM_REGION16_BE(0x1000000, "maincpu", 0)  // 16MB address space
	ROM_DEFAULT_BIOS("ipl10")
	ROM_LOAD( "cgrom.dat",  0xf00000, 0xc0000, CRC(9f3195f1) SHA1(8d72c5b4d63bb14c5dbdac495244d659aa1498b6) )
	ROM_SYSTEM_BIOS(0, "ipl10",  "IPL-ROM V1.0 (87/05/07)")
	ROMX_LOAD( "iplrom.dat", 0xfe0000, 0x20000, CRC(72bdf532) SHA1(0ed038ed2133b9f78c6e37256807424e0d927560), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS(1, "ipl11",  "IPL-ROM V1.1 (91/01/11)")
	ROMX_LOAD( "iplromxv.dat", 0xfe0000, 0x020000, CRC(00eeb408) SHA1(e33cdcdb69cd257b0b211ef46e7a8b144637db57), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS(2, "ipl12",  "IPL-ROM V1.2 (91/10/24)")
	ROMX_LOAD( "iplromco.dat", 0xfe0000, 0x020000, CRC(6c7ef608) SHA1(77511fc58798404701f66b6bbc9cbde06596eba7), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS(3, "ipl13",  "IPL-ROM V1.3 (92/11/27)")
	ROMX_LOAD( "iplrom30.dat", 0xfe0000, 0x020000, CRC(e8f8fdad) SHA1(239e9124568c862c31d9ec0605e32373ea74b86a), ROM_BIOS(4) )
	ROM_REGION(0x8000, "user1",0)  // For Background/Sprite decoding
	ROM_FILL(0x0000,0x8000,0x00)
	ROM_REGION(0x20000, "user2", 0)
	ROM_FILL(0x000,0x20000,0x00)
ROM_END

ROM_START( x68ksupr )
	ROM_REGION16_BE(0x1000000, "maincpu", 0)  // 16MB address space
	ROM_DEFAULT_BIOS("ipl11")
	ROM_LOAD( "cgrom.dat",  0xf00000, 0xc0000, CRC(9f3195f1) SHA1(8d72c5b4d63bb14c5dbdac495244d659aa1498b6) )
	ROM_SYSTEM_BIOS(0, "ipl10",  "IPL-ROM V1.0 (87/05/07)")
	ROMX_LOAD( "iplrom.dat", 0xfe0000, 0x20000, CRC(72bdf532) SHA1(0ed038ed2133b9f78c6e37256807424e0d927560), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS(1, "ipl11",  "IPL-ROM V1.1 (91/01/11)")
	ROMX_LOAD( "iplromxv.dat", 0xfe0000, 0x020000, CRC(00eeb408) SHA1(e33cdcdb69cd257b0b211ef46e7a8b144637db57), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS(2, "ipl12",  "IPL-ROM V1.2 (91/10/24)")
	ROMX_LOAD( "iplromco.dat", 0xfe0000, 0x020000, CRC(6c7ef608) SHA1(77511fc58798404701f66b6bbc9cbde06596eba7), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS(3, "ipl13",  "IPL-ROM V1.3 (92/11/27)")
	ROMX_LOAD( "iplrom30.dat", 0xfe0000, 0x020000, CRC(e8f8fdad) SHA1(239e9124568c862c31d9ec0605e32373ea74b86a), ROM_BIOS(4) )
	ROM_LOAD("scsiinsu.bin",0xfc0000, 0x002000, CRC(f65a3e24) SHA1(15a17798839a3f7f361119205aebc301c2df5967) )  // Dumped from an X68000 Super HD
//  ROM_LOAD("scsiexrom.dat",0xea0000, 0x002000, NO_DUMP )
	ROM_REGION(0x8000, "user1",0)  // For Background/Sprite decoding
	ROM_FILL(0x0000,0x8000,0x00)
	ROM_REGION(0x20000, "user2", 0)
	ROM_FILL(0x000,0x20000,0x00)
ROM_END

ROM_START( x68kxvi )
	ROM_REGION16_BE(0x1000000, "maincpu", 0)  // 16MB address space
	ROM_DEFAULT_BIOS("ipl11")
	ROM_LOAD( "cgrom.dat",  0xf00000, 0xc0000, CRC(9f3195f1) SHA1(8d72c5b4d63bb14c5dbdac495244d659aa1498b6) )
	ROM_SYSTEM_BIOS(0, "ipl10",  "IPL-ROM V1.0 (87/05/07)")
	ROMX_LOAD( "iplrom.dat", 0xfe0000, 0x20000, CRC(72bdf532) SHA1(0ed038ed2133b9f78c6e37256807424e0d927560), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS(1, "ipl11",  "IPL-ROM V1.1 (91/01/11)")
	ROMX_LOAD( "iplromxv.dat", 0xfe0000, 0x020000, CRC(00eeb408) SHA1(e33cdcdb69cd257b0b211ef46e7a8b144637db57), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS(2, "ipl12",  "IPL-ROM V1.2 (91/10/24)")
	ROMX_LOAD( "iplromco.dat", 0xfe0000, 0x020000, CRC(6c7ef608) SHA1(77511fc58798404701f66b6bbc9cbde06596eba7), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS(3, "ipl13",  "IPL-ROM V1.3 (92/11/27)")
	ROMX_LOAD( "iplrom30.dat", 0xfe0000, 0x020000, CRC(e8f8fdad) SHA1(239e9124568c862c31d9ec0605e32373ea74b86a), ROM_BIOS(4) )
	ROM_LOAD("scsiinco.bin",0xfc0000, 0x002000, CRC(2485e14d) SHA1(101a9bba8ea4bb90965c144bcfd7182f889ab958) )  // Dumped from an X68000 XVI Compact
//  ROM_LOAD("scsiexrom.dat",0xea0000, 0x002000, NO_DUMP )
	ROM_REGION(0x8000, "user1",0)  // For Background/Sprite decoding
	ROM_FILL(0x0000,0x8000,0x00)
	ROM_REGION(0x20000, "user2", 0)
	ROM_FILL(0x000,0x20000,0x00)
ROM_END

ROM_START( x68030 )
	ROM_REGION16_BE(0x1000000, "maincpu", 0)  // 16MB address space
	ROM_DEFAULT_BIOS("ipl13")
	ROM_LOAD( "cgrom.dat",  0xf00000, 0xc0000, CRC(9f3195f1) SHA1(8d72c5b4d63bb14c5dbdac495244d659aa1498b6) )
	ROM_SYSTEM_BIOS(0, "ipl10",  "IPL-ROM V1.0 (87/05/07)")
	ROMX_LOAD( "iplrom.dat", 0xfe0000, 0x20000, CRC(72bdf532) SHA1(0ed038ed2133b9f78c6e37256807424e0d927560), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS(1, "ipl11",  "IPL-ROM V1.1 (91/01/11)")
	ROMX_LOAD( "iplromxv.dat", 0xfe0000, 0x020000, CRC(00eeb408) SHA1(e33cdcdb69cd257b0b211ef46e7a8b144637db57), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS(2, "ipl12",  "IPL-ROM V1.2 (91/10/24)")
	ROMX_LOAD( "iplromco.dat", 0xfe0000, 0x020000, CRC(6c7ef608) SHA1(77511fc58798404701f66b6bbc9cbde06596eba7), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS(3, "ipl13",  "IPL-ROM V1.3 (92/11/27)")
	ROMX_LOAD( "iplrom30.dat", 0xfe0000, 0x020000, CRC(e8f8fdad) SHA1(239e9124568c862c31d9ec0605e32373ea74b86a), ROM_BIOS(4) )
	ROM_LOAD("scsiinrom.dat",0xfc0000, 0x002000, CRC(1c6c889e) SHA1(3f063d4231cdf53da6adc4db96533725e260076a) BAD_DUMP )
//  ROM_LOAD("scsiexrom.dat",0xea0000, 0x002000, NO_DUMP )
	ROM_REGION(0x8000, "user1",0)  // For Background/Sprite decoding
	ROM_FILL(0x0000,0x8000,0x00)
	ROM_REGION(0x20000, "user2", 0)
	ROM_FILL(0x000,0x20000,0x00)
ROM_END


/*    YEAR  NAME    PARENT  COMPAT  MACHINE INPUT   INIT    COMPANY     FULLNAME        FLAGS */
COMP( 1987, x68000, 0,      0,      x68000, x68000, x68k_state, x68000, "Sharp",    "X68000", GAME_IMPERFECT_GRAPHICS )
COMP( 1990, x68ksupr,x68000, 0,     x68ksupr,x68000, x68k_state,x68000, "Sharp",    "X68000 Super", GAME_IMPERFECT_GRAPHICS | GAME_NOT_WORKING )
COMP( 1991, x68kxvi,x68000, 0,      x68kxvi,x68000, x68k_state, x68kxvi,"Sharp",    "X68000 XVI", GAME_IMPERFECT_GRAPHICS | GAME_NOT_WORKING )
COMP( 1993, x68030, x68000, 0,      x68030, x68000, x68k_state, x68030, "Sharp",    "X68030", GAME_IMPERFECT_GRAPHICS | GAME_NOT_WORKING )
