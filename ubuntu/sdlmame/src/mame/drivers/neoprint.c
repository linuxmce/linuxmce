/*******************************************************************************************

    Neo Print (c) 1996 SNK

    preliminary driver by David Haywood & Angelo Salese

    npcartv1 bp 1260 pc += 2
    98best44 bp 1312 pc += 2

    TODO:
    - implement remaining video features;
    - sound interface, needs full Neo-Geo conversion;
    - inputs are bare bones and needs extra work;
    - printer/camera devices;
    - lamps;
    - upd4990a returns 4 years less than expected?
    - nprsp: paletteram has 0x40000 palette entries, kludged to work.

*******************************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "machine/nvram.h"
#include "sound/2610intf.h"
#include "machine/pd4990a.h"


class neoprint_state : public driver_device
{
public:
	neoprint_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_npvidram(*this, "npvidram"),
		m_npvidregs(*this, "npvidregs"){ }

	required_shared_ptr<UINT16> m_npvidram;
	required_shared_ptr<UINT16> m_npvidregs;
	UINT8 m_audio_result;
	UINT8 m_bank_val;
	UINT8 m_vblank;
	DECLARE_READ16_MEMBER(neoprint_calendar_r);
	DECLARE_WRITE16_MEMBER(neoprint_calendar_w);
	DECLARE_READ8_MEMBER(neoprint_unk_r);
	DECLARE_READ16_MEMBER(neoprint_audio_result_r);
	DECLARE_WRITE8_MEMBER(audio_cpu_clear_nmi_w);
	DECLARE_WRITE16_MEMBER(audio_command_w);
	DECLARE_READ8_MEMBER(audio_command_r);
	DECLARE_WRITE8_MEMBER(audio_result_w);
	DECLARE_WRITE16_MEMBER(nprsp_palette_w);
	DECLARE_WRITE8_MEMBER(nprsp_bank_w);
	DECLARE_READ16_MEMBER(rom_window_r);
	DECLARE_DRIVER_INIT(98best44);
	DECLARE_DRIVER_INIT(npcartv1);
	DECLARE_DRIVER_INIT(nprsp);
	virtual void video_start();
	DECLARE_MACHINE_RESET(nprsp);
	UINT32 screen_update_neoprint(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_nprsp(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};


void neoprint_state::video_start()
{
}

/*
video registers:
xxxx xxxx xxxx xxxx [0] scroll X, signed
xxxx xxxx xxxx xxxx [2] scroll Y, signed
---- ---x ---- ---- [6] enabled on layer 2 only, priority?
---- ---- -x-- ---- [6] layer enable?
---- ---- --?? ??xx [6] map register
*/

static void draw_layer(running_machine &machine, bitmap_ind16 &bitmap,const rectangle &cliprect,int layer,int data_shift)
{
	neoprint_state *state = machine.driver_data<neoprint_state>();
	int i, y, x;
	gfx_element *gfx = machine.gfx[0];
	INT16 scrollx, scrolly;

	i = (state->m_npvidregs[((layer*8)+0x06)/2] & 7) * 0x1000/4;
	scrollx = ((state->m_npvidregs[((layer*8)+0x00)/2] - (0xd8 + layer*4)) & 0x03ff);
	scrolly = ((state->m_npvidregs[((layer*8)+0x02)/2] - 0xffeb) & 0x03ff);

	scrollx/=2;
	scrolly/=2;

	for (y=0;y<32;y++)
	{
		for (x=0;x<32;x++)
		{
			UINT16 dat = state->m_npvidram[i*2] >> data_shift; // a video register?
			UINT16 color;
			if(state->m_npvidram[i*2+1] & 0x0020) // TODO: 8bpp switch?
				color = ((state->m_npvidram[i*2+1] & 0x8000) << 1) | 0x200 | ((state->m_npvidram[i*2+1] & 0xff00) >> 7);
			else
				color = ((state->m_npvidram[i*2+1] & 0xff00) >> 8) | ((state->m_npvidram[i*2+1] & 0x0010) << 4);
			UINT8 fx = (state->m_npvidram[i*2+1] & 0x0040);
			UINT8 fy = (state->m_npvidram[i*2+1] & 0x0080);

			drawgfx_transpen(bitmap,cliprect,gfx,dat,color,fx,fy,x*16+scrollx,y*16-scrolly,0);
			drawgfx_transpen(bitmap,cliprect,gfx,dat,color,fx,fy,x*16+scrollx-512,y*16-scrolly,0);
			drawgfx_transpen(bitmap,cliprect,gfx,dat,color,fx,fy,x*16+scrollx,y*16-scrolly-512,0);
			drawgfx_transpen(bitmap,cliprect,gfx,dat,color,fx,fy,x*16+scrollx-512,y*16-scrolly-512,0);

			i++;
			//i&=0x3ff;
		}
	}
}

UINT32 neoprint_state::screen_update_neoprint(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);

	draw_layer(machine(),bitmap,cliprect,1,2);
	draw_layer(machine(),bitmap,cliprect,0,2);

	return 0;
}

UINT32 neoprint_state::screen_update_nprsp(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);

	draw_layer(machine(),bitmap,cliprect,1,0);
	draw_layer(machine(),bitmap,cliprect,2,0);
	draw_layer(machine(),bitmap,cliprect,0,0);

	return 0;
}


READ16_MEMBER(neoprint_state::neoprint_calendar_r)
{
	//if(space.device().safe_pc() != 0x4b38 )//&& space.device().safe_pc() != 0x5f86 && space.device().safe_pc() != 0x5f90)
	//  printf("%08x\n",space.device().safe_pc());

	return (upd4990a_databit_r(machine().device("upd4990a"), space, 0) << 15);
}

WRITE16_MEMBER(neoprint_state::neoprint_calendar_w)
{
		upd4990a_control_16_w(machine().device("upd4990a"), space, 0, ((data >> 8) & 7), mem_mask);
}

READ8_MEMBER(neoprint_state::neoprint_unk_r)
{

	/* ---x ---- tested in irq routine, odd/even field number? */
	/* ---- xx-- one of these two must be high */
	/* ---- --xx checked right before entering into attract mode, presumably printer/camera related */

	m_vblank = (machine().primary_screen->frame_number() & 0x1) ? 0x10 : 0x00;

	//if(space.device().safe_pc() != 0x1504 && space.device().safe_pc() != 0x5f86 && space.device().safe_pc() != 0x5f90)
	//  printf("%08x\n",space.device().safe_pc());

	return m_vblank| 4 | 3;
}

READ16_MEMBER(neoprint_state::neoprint_audio_result_r)
{
	return (m_audio_result << 8) | 0x00;
}

static void audio_cpu_assert_nmi(running_machine &machine)
{
	machine.device("audiocpu")->execute().set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}


WRITE8_MEMBER(neoprint_state::audio_cpu_clear_nmi_w)
{
	machine().device("audiocpu")->execute().set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}

WRITE16_MEMBER(neoprint_state::audio_command_w)
{
	/* accessing the LSB only is not mapped */
	if (mem_mask != 0x00ff)
	{
		soundlatch_byte_w(space, 0, data >> 8);

		audio_cpu_assert_nmi(machine());

		/* boost the interleave to let the audio CPU read the command */
		machine().scheduler().boost_interleave(attotime::zero, attotime::from_usec(50));

		//if (LOG_CPU_COMM) logerror("MAIN CPU PC %06x: audio_command_w %04x - %04x\n", space.device().safe_pc(), data, mem_mask);
	}
}


READ8_MEMBER(neoprint_state::audio_command_r)
{
	UINT8 ret = soundlatch_byte_r(space, 0);

	//if (LOG_CPU_COMM) logerror(" AUD CPU PC   %04x: audio_command_r %02x\n", space.device().safe_pc(), ret);

	/* this is a guess */
	audio_cpu_clear_nmi_w(space, 0, 0);

	return ret;
}



WRITE8_MEMBER(neoprint_state::audio_result_w)
{


	//if (LOG_CPU_COMM && (m_audio_result != data)) logerror(" AUD CPU PC   %04x: audio_result_w %02x\n", space.device().safe_pc(), data);

	m_audio_result = data;
}

static ADDRESS_MAP_START( neoprint_map, AS_PROGRAM, 16, neoprint_state )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
/*  AM_RANGE(0x100000, 0x17ffff) multi-cart or banking, some writes points here if anything lies there too */
	AM_RANGE(0x200000, 0x20ffff) AM_RAM
	AM_RANGE(0x300000, 0x30ffff) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0x400000, 0x43ffff) AM_RAM AM_SHARE("npvidram")
	AM_RANGE(0x500000, 0x51ffff) AM_RAM_WRITE(paletteram_xBBBBBGGGGGRRRRR_word_w) AM_SHARE("paletteram")
	AM_RANGE(0x600000, 0x600001) AM_READWRITE(neoprint_audio_result_r,audio_command_w)
	AM_RANGE(0x600002, 0x600003) AM_READWRITE(neoprint_calendar_r,neoprint_calendar_w)
	AM_RANGE(0x600004, 0x600005) AM_READ_PORT("SYSTEM") AM_WRITENOP
	AM_RANGE(0x600006, 0x600007) AM_READ_PORT("IN") AM_WRITENOP
	AM_RANGE(0x600008, 0x600009) AM_READ_PORT("DSW1")
	AM_RANGE(0x60000a, 0x60000b) AM_READ8(neoprint_unk_r,0xff00)
	AM_RANGE(0x60000c, 0x60000d) AM_READ_PORT("DSW2")
	AM_RANGE(0x60000e, 0x60000f) AM_WRITENOP

	AM_RANGE(0x700000, 0x70001b) AM_RAM AM_SHARE("npvidregs")

	AM_RANGE(0x70001e, 0x70001f) AM_WRITENOP //watchdog
ADDRESS_MAP_END

WRITE16_MEMBER(neoprint_state::nprsp_palette_w)
{
	UINT8 r,g,b,i;

	COMBINE_DATA(&m_generic_paletteram_16[offset]);

	g = (m_generic_paletteram_16[offset & ~1] & 0xf800) >> 8;
	r = (m_generic_paletteram_16[offset & ~1] & 0x00f8) >> 0;
	i = (m_generic_paletteram_16[offset | 1] & 0x1c00) >> 10;
	b = (m_generic_paletteram_16[offset | 1] & 0x00f8) >> 0;
	r |= i;
	g |= i;
	b |= i;

	/* hack: bypass MAME 65536 palette entries limit */
	if(offset & 0x10000)
		return;

	{
		UINT32 pal_entry;

		pal_entry = ((offset & 0xfffe) >> 1) + ((offset & 0x20000) ? 0x8000 : 0);

		palette_set_color(machine(), pal_entry, MAKE_RGB(r,g,b));
	}
}

WRITE8_MEMBER(neoprint_state::nprsp_bank_w)
{
	/* this register seems flip-flop based ... */

	if((data & 0xf0) == 0x20)
	{
		if((data & 0xf) == 0x1)
			m_bank_val = 1;
		if((data & 0xf) == 0x2)
			m_bank_val = 0;
	}
}

READ16_MEMBER(neoprint_state::rom_window_r)
{
	UINT16 *rom = (UINT16 *)machine().root_device().memregion("maincpu")->base();

	return rom[offset | 0x80000/2 | m_bank_val*0x40000/2];
}

static ADDRESS_MAP_START( nprsp_map, AS_PROGRAM, 16, neoprint_state )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x080000, 0x0fffff) AM_READ(rom_window_r)
	AM_RANGE(0x200000, 0x200001) AM_READWRITE(neoprint_audio_result_r,audio_command_w)
	AM_RANGE(0x200002, 0x200003) AM_READWRITE(neoprint_calendar_r,neoprint_calendar_w)
	AM_RANGE(0x200004, 0x200005) AM_READ_PORT("SYSTEM") AM_WRITENOP
	AM_RANGE(0x200006, 0x200007) AM_READ_PORT("IN") AM_WRITENOP
	AM_RANGE(0x200008, 0x200009) AM_READ_PORT("DSW1") AM_WRITE8(nprsp_bank_w,0xff00)
	AM_RANGE(0x20000a, 0x20000b) AM_READ8(neoprint_unk_r,0xff00)
	AM_RANGE(0x20000c, 0x20000d) AM_READ_PORT("DSW2")
	AM_RANGE(0x20000e, 0x20000f) AM_WRITENOP

	AM_RANGE(0x240000, 0x24001b) AM_RAM AM_SHARE("npvidregs")
	AM_RANGE(0x24001e, 0x24001f) AM_WRITENOP //watchdog

	AM_RANGE(0x300000, 0x33ffff) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0x380000, 0x38ffff) AM_RAM
	AM_RANGE(0x400000, 0x43ffff) AM_RAM AM_SHARE("npvidram")
	AM_RANGE(0x500000, 0x57ffff) AM_RAM_WRITE(nprsp_palette_w) AM_SHARE("paletteram")
ADDRESS_MAP_END

/*************************************
 *
 *  Audio CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( neoprint_audio_map, AS_PROGRAM, 8, neoprint_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM//AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_MAIN_BANK)
//  AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK + 3)
//  AM_RANGE(0xc000, 0xdfff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK + 2)
//  AM_RANGE(0xe000, 0xefff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK + 1)
//  AM_RANGE(0xf000, 0xf7ff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK + 0)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END



/*************************************
 *
 *  Audio CPU port handlers
 *
 *************************************/

static ADDRESS_MAP_START( neoprint_audio_io_map, AS_IO, 8, neoprint_state )
	/*AM_RANGE(0x00, 0x00) AM_MIRROR(0xff00) AM_READWRITE(audio_command_r, audio_cpu_clear_nmi_w);*/  /* may not and NMI clear */
	AM_RANGE(0x00, 0x00) AM_MIRROR(0xff00) AM_READ(audio_command_r) AM_WRITENOP
	AM_RANGE(0x04, 0x07) AM_MIRROR(0xff00) AM_DEVREADWRITE_LEGACY("ymsnd", ym2610_r, ym2610_w)
//  AM_RANGE(0x08, 0x08) AM_MIRROR(0xff00) /* write - NMI enable / acknowledge? (the data written doesn't matter) */
//  AM_RANGE(0x08, 0x08) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ_LEGACY(audio_cpu_bank_select_f000_f7ff_r)
//  AM_RANGE(0x09, 0x09) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ_LEGACY(audio_cpu_bank_select_e000_efff_r)
//  AM_RANGE(0x0a, 0x0a) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ_LEGACY(audio_cpu_bank_select_c000_dfff_r)
//  AM_RANGE(0x0b, 0x0b) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ_LEGACY(audio_cpu_bank_select_8000_bfff_r)
	AM_RANGE(0x0c, 0x0c) AM_MIRROR(0xff00) AM_WRITE(audio_result_w)
//  AM_RANGE(0x18, 0x18) AM_MIRROR(0xff00) /* write - NMI disable? (the data written doesn't matter) */
ADDRESS_MAP_END

static INPUT_PORTS_START( neoprint )
	PORT_START("SYSTEM")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_SERVICE_NO_TOGGLE( 0x0800, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_START("IN")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0100, 0x0100, "IN0" )
	PORT_DIPSETTING(    0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON4) PORT_NAME( UTF8_RIGHT " Button") PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3) PORT_NAME( UTF8_LEFT " Button") PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1) PORT_NAME("Green Button") PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON2) PORT_NAME("Red Button") PORT_CODE(KEYCODE_Z)
	PORT_START("DSW1")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0100, 0x0100, "DSW1" )
	PORT_DIPSETTING(    0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_START("DSW2")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0100, 0x0100, "DSW2" )
	PORT_DIPSETTING(    0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
INPUT_PORTS_END


static const gfx_layout neoprint_layout =
{
	16,16,
	RGN_FRAC(1,3),
	6,
	{ RGN_FRAC(2,3)+8, RGN_FRAC(2,3)+0, RGN_FRAC(1,3)+8, RGN_FRAC(1,3)+0, RGN_FRAC(0,3)+8, RGN_FRAC(0,3)+0  },
	{ 0,1,2,3,4,5,6,7, 256,257,258,259,260,261,262,263 },
	{ 0*16,1*16,2*16,3*16,4*16,5*16,6*16,7*16,8*16,9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	32*16,
};

static GFXDECODE_START( neoprint )
	GFXDECODE_ENTRY( "gfx1", 0, neoprint_layout,   0x0, 0x1000 )
GFXDECODE_END

static void audio_cpu_irq(device_t *device, int assert)
{
	device->machine().device("audiocpu")->execute().set_input_line(0, assert ? ASSERT_LINE : CLEAR_LINE);
}

static const ym2610_interface ym2610_config =
{
	audio_cpu_irq
};


static MACHINE_CONFIG_START( neoprint, neoprint_state )
	MCFG_CPU_ADD("maincpu", M68000, 12000000)
	MCFG_CPU_PROGRAM_MAP(neoprint_map)
	MCFG_CPU_PERIODIC_INT_DRIVER(neoprint_state, irq3_line_hold, 45) /* camera / printer irq, unknown timing */
	MCFG_CPU_VBLANK_INT_DRIVER("screen", neoprint_state,  irq2_line_hold) // lv1,2,3 valid?

	MCFG_CPU_ADD("audiocpu", Z80, 4000000)
	MCFG_CPU_PROGRAM_MAP(neoprint_audio_map)
	MCFG_CPU_IO_MAP(neoprint_audio_io_map)

	MCFG_UPD4990A_OLD_ADD("upd4990a")
	MCFG_NVRAM_ADD_0FILL("nvram")

	MCFG_GFXDECODE(neoprint)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 48*8-1, 0*8, 30*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(neoprint_state, screen_update_neoprint)

	MCFG_PALETTE_LENGTH(0x10000)


	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("ymsnd", YM2610, 24000000 / 3)
	MCFG_SOUND_CONFIG(ym2610_config)
	MCFG_SOUND_ROUTE(0, "lspeaker",  0.60)
	MCFG_SOUND_ROUTE(0, "rspeaker", 0.60)
	MCFG_SOUND_ROUTE(1, "lspeaker",  1.0)
	MCFG_SOUND_ROUTE(2, "rspeaker", 1.0)
MACHINE_CONFIG_END

MACHINE_RESET_MEMBER(neoprint_state,nprsp)
{
	m_bank_val = 0;
}

static MACHINE_CONFIG_START( nprsp, neoprint_state )
	MCFG_CPU_ADD("maincpu", M68000, 12000000)
	MCFG_CPU_PROGRAM_MAP(nprsp_map)
	MCFG_CPU_PERIODIC_INT_DRIVER(neoprint_state, irq3_line_hold, 45) /* camera / printer irq, unknown timing */
	MCFG_CPU_VBLANK_INT_DRIVER("screen", neoprint_state,  irq2_line_hold) // lv1,2,3 valid?

	MCFG_CPU_ADD("audiocpu", Z80, 4000000)
	MCFG_CPU_PROGRAM_MAP(neoprint_audio_map)
	MCFG_CPU_IO_MAP(neoprint_audio_io_map)

	MCFG_UPD4990A_OLD_ADD("upd4990a")
	MCFG_NVRAM_ADD_0FILL("nvram")

	MCFG_GFXDECODE(neoprint)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 48*8-1, 0*8, 30*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(neoprint_state, screen_update_nprsp)

	MCFG_MACHINE_RESET_OVERRIDE(neoprint_state,nprsp)

	MCFG_PALETTE_LENGTH(0x10000)


	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("ymsnd", YM2610, 24000000 / 3)
	MCFG_SOUND_CONFIG(ym2610_config)
	MCFG_SOUND_ROUTE(0, "lspeaker",  0.60)
	MCFG_SOUND_ROUTE(0, "rspeaker", 0.60)
	MCFG_SOUND_ROUTE(1, "lspeaker",  1.0)
	MCFG_SOUND_ROUTE(2, "rspeaker", 1.0)
MACHINE_CONFIG_END


ROM_START( npcartv1 )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD16_WORD_SWAP( "ep1.bin", 0x000000, 0x80000, CRC(18606198) SHA1(d968e09131c22769e22c7310aca1f02e739f38f1) )
//  ROM_RELOAD(                      0x100000, 0x80000 ) /* checks the same string from above to be present there? Why? */

	ROM_REGION( 0x20000, "audiocpu", 0 ) /* Z80 program */
	ROM_LOAD( "m1.bin", 0x00000, 0x20000, CRC(b2d38e12) SHA1(ab96c5d3d22eb71ed6e0a03f3ff5d4b23e72fad8) )

	ROM_REGION( 0x080000, "ymsnd", 0 ) /* Samples */
	ROM_LOAD( "v1.bin", 0x00000, 0x80000, CRC(2d6608f9) SHA1(7dbde1c305ab3438b7fe7417816427c682371bd4) )

	ROM_REGION( 0x180000, "gfx1", ROMREGION_ERASE00 )
	ROM_LOAD( "c1.bin", 0x00000, 0x80000, CRC(b89f1fb6) SHA1(e711f91a7872b2e0edc3f42a726d969096d684f2) )
	ROM_LOAD( "c2.bin", 0x80000, 0x80000, CRC(7ce39dc2) SHA1(c5be90657350258b670b55dd9c77f7899133ced3) )
ROM_END

	/* logo: Neo Print
	small text: Cassette supporting Neo Print and Neo Print Multi
	(cassette=cartridge)
	title: '98 NeoPri Best 44 version */

ROM_START( 98best44 )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD16_WORD_SWAP( "p060-ep1", 0x000000, 0x080000, CRC(d42e505d) SHA1(0ad6b0288f36c339832730a03e53cbc07dab4f82))
//  ROM_RELOAD(                      0x100000, 0x80000 ) /* checks the same string from above to be present there? Why? */

	ROM_REGION( 0x20000, "audiocpu", 0 ) /* Z80 program */
	ROM_LOAD( "pt004-m1", 0x00000, 0x20000, CRC(6d77cdaa) SHA1(f88a93b3085b18b6663b4e51fccaa41958aafae1) )

	ROM_REGION( 0x200000, "ymsnd", 0 ) /* Samples */
	ROM_LOAD( "pt004-v1", 0x000000, 0x200000, CRC(118a84fd) SHA1(9059297a42a329eca47a82327c301853219013bd) )

	ROM_REGION( 0x300000, "gfx1", ROMREGION_ERASE00 )
	ROM_LOAD( "pt060-c1", 0x000000, 0x100000, CRC(22a23090) SHA1(0e219fcfea6ca2ddf4b7b4197aac8bc55a29d5cf) )
	ROM_LOAD( "pt060-c2", 0x100000, 0x100000, CRC(66a8e56a) SHA1(adfd1e52d52806a785f1e9b1ae2ac969b6ed60af) )
ROM_END

ROM_START( nprsp )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD16_WORD_SWAP( "s038a-ep1.bin", 0x000000, 0x080000, CRC(529fb4fa) SHA1(f31ba8998bb01458f43df1934222995f22d590a1) ) // program ROM
	ROM_LOAD16_WORD_SWAP( "s046-ep2.bin",  0x080000, 0x080000, CRC(846ae929) SHA1(e5544cde32794865e17d7dffd4e603ad5418d91e) ) // data ROM

	ROM_REGION( 0x20000, "audiocpu", 0 ) /* Z80 program */
	ROM_LOAD( "s001-m1.bin", 0x00000, 0x20000, CRC(ea8111c1) SHA1(69e6bb7ad9a8d61db4513a762c0ce9e9da2a1785) )

	ROM_REGION( 0x200000, "ymsnd", 0 ) /* Samples */
	ROM_LOAD( "s001-v1.bin", 0x000000, 0x100000, CRC(13d63625) SHA1(4a9e3b1192a4a7e405becfd5d2a95ffc14ae6e79)  )

	ROM_REGION( 0x600000, "gfx1", 0 )
	ROM_LOAD( "s046-c1.bin", 0x000000, 0x200000, CRC(06fffce0) SHA1(0d9bb9d3107b1efb66ee82341c3f80ec093d5987) )
	ROM_LOAD( "s046-c2.bin", 0x200000, 0x200000, CRC(7cc353e7) SHA1(5c4fa9fdf90bd0d03608becaa174d68735f28bbb) )
	ROM_LOAD( "s046-c3.bin", 0x400000, 0x200000, CRC(f68f0f6f) SHA1(2fc105953a17259353f74376661c442658f9a644) )
ROM_END

/* FIXME: get rid of these two, probably something to do with irq3 and camera / printer devices */
DRIVER_INIT_MEMBER(neoprint_state,npcartv1)
{
	UINT16 *ROM = (UINT16 *)machine().root_device().memregion( "maincpu" )->base();

	ROM[0x1260/2] = 0x4e71;

	ROM[0x43c8/2] = 0x4e71; //ROM checksum
}


DRIVER_INIT_MEMBER(neoprint_state,98best44)
{
	UINT16 *ROM = (UINT16 *)machine().root_device().memregion( "maincpu" )->base();

	ROM[0x1312/2] = 0x4e71;
}

DRIVER_INIT_MEMBER(neoprint_state,nprsp)
{
	UINT16 *ROM = (UINT16 *)machine().root_device().memregion( "maincpu" )->base();

	ROM[0x13a4/2] = 0x4e71;
	ROM[0x13bc/2] = 0x4e71;
	ROM[0x140c/2] = 0x4e71;

	ROM[0x4832/2] = 0x4e71; //ROM checksum
	ROM[0x4834/2] = 0x4e71;
}

GAME( 1996, npcartv1,    0,        neoprint,    neoprint, neoprint_state,   npcartv1, ROT0, "SNK", "Neo Print V1 (World)", GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS | GAME_NOT_WORKING )
GAME( 1998, 98best44,    0,        neoprint,    neoprint, neoprint_state,   98best44, ROT0, "SNK", "Neo Print - '98 NeoPri Best 44 (Japan)", GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS |  GAME_NOT_WORKING )
GAME( 1996, nprsp,       0,        nprsp,       neoprint, neoprint_state,   nprsp,    ROT0, "SNK", "NeopriSP Retro Collection (Japan)", GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS | GAME_NOT_WORKING )
