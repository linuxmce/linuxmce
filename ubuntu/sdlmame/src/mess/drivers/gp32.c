/**************************************************************************
 *
 * gp32.c - Game Park GP32
 * Skeleton by R. Belmont
 *
 * CPU: Samsung S3C2400X01 SoC
 * S3C2400X01 consists of:
 *    ARM920T CPU core + MMU
 *    LCD controller
 *    DMA controller
 *    Interrupt controller
 *    USB controller
 *    and more.
 *
 **************************************************************************/

#include "emu.h"
#include "cpu/arm7/arm7.h"
#include "cpu/arm7/arm7core.h"
#include "machine/smartmed.h"
#include "includes/gp32.h"
#include "sound/dac.h"
#include "rendlay.h"
#include "machine/nvram.h"

#define VERBOSE_LEVEL ( 0 )

INLINE void ATTR_PRINTF(3,4) verboselog( running_machine &machine, int n_level, const char *s_fmt, ...)
{
	if (VERBOSE_LEVEL >= n_level)
	{
		va_list v;
		char buf[32768];
		va_start( v, s_fmt);
		vsprintf( buf, s_fmt, v);
		va_end( v);
		logerror( "%s: %s", machine.describe_context( ), buf);
	}
}

#define CLOCK_MULTIPLIER 1

#define MPLLCON  1
#define UPLLCON  2

#define BIT(x,n) (((x)>>(n))&1)
#define BITS(x,m,n) (((x)>>(n))&((1<<((m)-(n)+1))-1))


static UINT32 s3c240x_get_fclk(gp32_state *state, int reg);
static UINT32 s3c240x_get_hclk(gp32_state *state, int reg);
static UINT32 s3c240x_get_pclk(gp32_state *state, int reg);

static void s3c240x_dma_request_iis( running_machine &machine);
static void s3c240x_dma_request_pwm( running_machine &machine);

// LCD CONTROLLER


#define BPPMODE_TFT_01  0x08
#define BPPMODE_TFT_02  0x09
#define BPPMODE_TFT_04  0x0A
#define BPPMODE_TFT_08  0x0B
#define BPPMODE_TFT_16  0x0C

static rgb_t s3c240x_get_color_5551( UINT16 data)
{
	UINT8 r, g, b, i;
	r = BITS( data, 15, 11) << 3;
	g = BITS( data, 10, 6) << 3;
	b = BITS( data, 5, 1) << 3;
	i = BIT( data, 1) << 2;
	return MAKE_RGB( r | i, g | i, b | i);
}

static void s3c240x_lcd_dma_reload( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	state->m_s3c240x_lcd.vramaddr_cur = state->m_s3c240x_lcd_regs[5] << 1;
	state->m_s3c240x_lcd.vramaddr_max = ((state->m_s3c240x_lcd_regs[5] & 0xFFE00000) | state->m_s3c240x_lcd_regs[6]) << 1;
	state->m_s3c240x_lcd.offsize = BITS( state->m_s3c240x_lcd_regs[7], 21, 11);
	state->m_s3c240x_lcd.pagewidth_cur = 0;
	state->m_s3c240x_lcd.pagewidth_max = BITS( state->m_s3c240x_lcd_regs[7], 10, 0);
	verboselog( machine, 3, "LCD - vramaddr %08X %08X offsize %08X pagewidth %08X\n", state->m_s3c240x_lcd.vramaddr_cur, state->m_s3c240x_lcd.vramaddr_max, state->m_s3c240x_lcd.offsize, state->m_s3c240x_lcd.pagewidth_max);
}

static void s3c240x_lcd_dma_init( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	s3c240x_lcd_dma_reload( machine);
	state->m_s3c240x_lcd.bppmode = BITS( state->m_s3c240x_lcd_regs[0], 4, 1);
	state->m_s3c240x_lcd.bswp = BIT( state->m_s3c240x_lcd_regs[4], 1);
	state->m_s3c240x_lcd.hwswp = BIT( state->m_s3c240x_lcd_regs[4], 0);
	state->m_s3c240x_lcd.lineval = BITS( state->m_s3c240x_lcd_regs[1], 23, 14);
	state->m_s3c240x_lcd.hozval = BITS( state->m_s3c240x_lcd_regs[2], 18, 8);
}

static UINT32 s3c240x_lcd_dma_read( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	UINT8 *vram, data[4];
	int i;
	for (i = 0; i < 2; i++)
	{
		vram = (UINT8 *)state->m_s3c240x_ram.target() + state->m_s3c240x_lcd.vramaddr_cur - 0x0C000000;
		data[i*2+0] = vram[0];
		data[i*2+1] = vram[1];
		state->m_s3c240x_lcd.vramaddr_cur += 2;
		state->m_s3c240x_lcd.pagewidth_cur++;
		if (state->m_s3c240x_lcd.pagewidth_cur >= state->m_s3c240x_lcd.pagewidth_max)
		{
			state->m_s3c240x_lcd.vramaddr_cur += state->m_s3c240x_lcd.offsize << 1;
			state->m_s3c240x_lcd.pagewidth_cur = 0;
		}
	}
	if (state->m_s3c240x_lcd.hwswp == 0)
	{
		if (state->m_s3c240x_lcd.bswp == 0)
		{
			return (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | (data[0] << 0);
		}
		else
		{
			return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3] << 0);
		}
	}
	else
	{
		if (state->m_s3c240x_lcd.bswp == 0)
		{
			return (data[1] << 24) | (data[0] << 16) | (data[3] << 8) | (data[2] << 0);
		}
		else
		{
			return (data[2] << 24) | (data[3] << 16) | (data[0] << 8) | (data[1] << 0);
		}
	}
}

static void s3c240x_lcd_render_01( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	bitmap_rgb32 &bitmap = state->m_bitmap;
	UINT32 *scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
	int i, j;
	for (i = 0; i < 4; i++)
	{
		UINT32 data = s3c240x_lcd_dma_read( machine);
		for (j = 0; j < 32; j++)
		{
			*scanline++ = palette_get_color( machine, (data >> 31) & 0x01);
			data = data << 1;
			state->m_s3c240x_lcd.hpos++;
			if (state->m_s3c240x_lcd.hpos >= (state->m_s3c240x_lcd.pagewidth_max << 4))
			{
				state->m_s3c240x_lcd.vpos = (state->m_s3c240x_lcd.vpos + 1) % (state->m_s3c240x_lcd.lineval + 1);
				state->m_s3c240x_lcd.hpos = 0;
				scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
			}
		}
	}
}

static void s3c240x_lcd_render_02( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	bitmap_rgb32 &bitmap = state->m_bitmap;
	UINT32 *scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
	int i, j;
	for (i = 0; i < 4; i++)
	{
		UINT32 data = s3c240x_lcd_dma_read( machine);
		for (j = 0; j < 16; j++)
		{
			*scanline++ = palette_get_color( machine, (data >> 30) & 0x03);
			data = data << 2;
			state->m_s3c240x_lcd.hpos++;
			if (state->m_s3c240x_lcd.hpos >= (state->m_s3c240x_lcd.pagewidth_max << 3))
			{
				state->m_s3c240x_lcd.vpos = (state->m_s3c240x_lcd.vpos + 1) % (state->m_s3c240x_lcd.lineval + 1);
				state->m_s3c240x_lcd.hpos = 0;
				scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
			}
		}
	}
}

static void s3c240x_lcd_render_04( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	bitmap_rgb32 &bitmap = state->m_bitmap;
	UINT32 *scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
	int i, j;
	for (i = 0; i < 4; i++)
	{
		UINT32 data = s3c240x_lcd_dma_read( machine);
		for (j = 0; j < 8; j++)
		{
			*scanline++ = palette_get_color( machine, (data >> 28) & 0x0F);
			data = data << 4;
			state->m_s3c240x_lcd.hpos++;
			if (state->m_s3c240x_lcd.hpos >= (state->m_s3c240x_lcd.pagewidth_max << 2))
			{
				state->m_s3c240x_lcd.vpos = (state->m_s3c240x_lcd.vpos + 1) % (state->m_s3c240x_lcd.lineval + 1);
				state->m_s3c240x_lcd.hpos = 0;
				scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
			}
		}
	}
}

static void s3c240x_lcd_render_08( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	bitmap_rgb32 &bitmap = state->m_bitmap;
	UINT32 *scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
	int i, j;
	for (i = 0; i < 4; i++)
	{
		UINT32 data = s3c240x_lcd_dma_read( machine);
		for (j = 0; j < 4; j++)
		{
			*scanline++ = palette_get_color( machine, (data >> 24) & 0xFF);
			data = data << 8;
			state->m_s3c240x_lcd.hpos++;
			if (state->m_s3c240x_lcd.hpos >= (state->m_s3c240x_lcd.pagewidth_max << 1))
			{
				state->m_s3c240x_lcd.vpos = (state->m_s3c240x_lcd.vpos + 1) % (state->m_s3c240x_lcd.lineval + 1);
				state->m_s3c240x_lcd.hpos = 0;
				scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
			}
		}
	}
}

static void s3c240x_lcd_render_16( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	bitmap_rgb32 &bitmap = state->m_bitmap;
	UINT32 *scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
	int i, j;
	for (i = 0; i < 4; i++)
	{
		UINT32 data = s3c240x_lcd_dma_read( machine);
		for (j = 0; j < 2; j++)
		{
			*scanline++ = s3c240x_get_color_5551( (data >> 16) & 0xFFFF);
			data = data << 16;
			state->m_s3c240x_lcd.hpos++;
			if (state->m_s3c240x_lcd.hpos >= (state->m_s3c240x_lcd.pagewidth_max << 0))
			{
				state->m_s3c240x_lcd.vpos = (state->m_s3c240x_lcd.vpos + 1) % (state->m_s3c240x_lcd.lineval + 1);
				state->m_s3c240x_lcd.hpos = 0;
				scanline = &bitmap.pix32(state->m_s3c240x_lcd.vpos, state->m_s3c240x_lcd.hpos);
			}
		}
	}
}

TIMER_CALLBACK_MEMBER(gp32_state::s3c240x_lcd_timer_exp)
{
	screen_device *screen = machine().primary_screen;
	verboselog( machine(), 2, "LCD timer callback\n");
	m_s3c240x_lcd.vpos = screen->vpos();
	m_s3c240x_lcd.hpos = screen->hpos();
	verboselog( machine(), 3, "LCD - vpos %d hpos %d\n", m_s3c240x_lcd.vpos, m_s3c240x_lcd.hpos);
	if (m_s3c240x_lcd.vramaddr_cur >= m_s3c240x_lcd.vramaddr_max)
	{
		s3c240x_lcd_dma_reload( machine());
	}
	verboselog( machine(), 3, "LCD - vramaddr %08X\n", m_s3c240x_lcd.vramaddr_cur);
	while (m_s3c240x_lcd.vramaddr_cur < m_s3c240x_lcd.vramaddr_max)
	{
		switch (m_s3c240x_lcd.bppmode)
		{
			case BPPMODE_TFT_01 : s3c240x_lcd_render_01( machine()); break;
			case BPPMODE_TFT_02 : s3c240x_lcd_render_02( machine()); break;
			case BPPMODE_TFT_04 : s3c240x_lcd_render_04( machine()); break;
			case BPPMODE_TFT_08 : s3c240x_lcd_render_08( machine()); break;
			case BPPMODE_TFT_16 : s3c240x_lcd_render_16( machine()); break;
			default : verboselog( machine(), 0, "s3c240x_lcd_timer_exp: bppmode %d not supported\n", m_s3c240x_lcd.bppmode); break;
		}
		if ((m_s3c240x_lcd.vpos == 0) && (m_s3c240x_lcd.hpos == 0)) break;
	}
	m_s3c240x_lcd_timer->adjust( screen->time_until_pos(m_s3c240x_lcd.vpos, m_s3c240x_lcd.hpos));
}

void gp32_state::video_start()
{
	machine().primary_screen->register_screen_bitmap(m_bitmap);
}

UINT32 gp32_state::screen_update_gp32(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);
	s3c240x_lcd_dma_init( machine());
	return 0;
}

READ32_MEMBER(gp32_state::s3c240x_lcd_r)
{
	UINT32 data = m_s3c240x_lcd_regs[offset];
	switch (offset)
	{
		// LCDCON1
		case 0x00 / 4 :
		{
			// make sure line counter is going
			UINT32 lineval = BITS( m_s3c240x_lcd_regs[1], 23, 14);
			data = (data & ~0xFFFC0000) | ((lineval - machine().primary_screen->vpos()) << 18);
		}
		break;
	}
	verboselog( machine(), 9, "(LCD) %08X -> %08X (PC %08X)\n", 0x14A00000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

static void s3c240x_lcd_configure( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	screen_device *screen = machine.primary_screen;
	UINT32 vspw, vbpd, lineval, vfpd, hspw, hbpd, hfpd, hozval, clkval, hclk;
	double framerate, vclk;
	rectangle visarea;
	vspw = BITS( state->m_s3c240x_lcd_regs[1], 5, 0);
	vbpd = BITS( state->m_s3c240x_lcd_regs[1], 31, 24);
	lineval = BITS( state->m_s3c240x_lcd_regs[1], 23, 14);
	vfpd = BITS( state->m_s3c240x_lcd_regs[1], 13, 6);
	hspw = BITS( state->m_s3c240x_lcd_regs[3], 7, 0);
	hbpd = BITS( state->m_s3c240x_lcd_regs[2], 25, 19);
	hfpd = BITS( state->m_s3c240x_lcd_regs[2], 7, 0);
	hozval = BITS( state->m_s3c240x_lcd_regs[2], 18, 8);
	clkval = BITS( state->m_s3c240x_lcd_regs[0], 17, 8);
	hclk = s3c240x_get_hclk(state, MPLLCON);
	verboselog( machine, 3, "LCD - vspw %d vbpd %d lineval %d vfpd %d hspw %d hbpd %d hfpd %d hozval %d clkval %d hclk %d\n", vspw, vbpd, lineval, vfpd, hspw, hbpd, hfpd, hozval, clkval, hclk);
	vclk = (double)(hclk / ((clkval + 1) * 2));
	verboselog( machine, 3, "LCD - vclk %f\n", vclk);
	framerate = vclk / (((vspw + 1) + (vbpd + 1) + (lineval + 1) + (vfpd + 1)) * ((hspw + 1) + (hbpd + 1) + (hfpd + 1) + (hozval + 1)));
	verboselog( machine, 3, "LCD - framerate %f\n", framerate);
	visarea.set(0, hozval, 0, lineval);
	verboselog( machine, 3, "LCD - visarea min_x %d min_y %d max_x %d max_y %d\n", visarea.min_x, visarea.min_y, visarea.max_x, visarea.max_y);
	screen->configure(hozval + 1, lineval + 1, visarea, HZ_TO_ATTOSECONDS( framerate));
}

static void s3c240x_lcd_start( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	screen_device *screen = machine.primary_screen;
	verboselog( machine, 1, "LCD start\n");
	s3c240x_lcd_configure( machine);
	s3c240x_lcd_dma_init( machine);
	state->m_s3c240x_lcd_timer->adjust( screen->time_until_pos(0, 0));
}

static void s3c240x_lcd_stop( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 1, "LCD stop\n");
	state->m_s3c240x_lcd_timer->adjust( attotime::never);
}

static void s3c240x_lcd_recalc( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	if (state->m_s3c240x_lcd_regs[0] & 1)
	{
		s3c240x_lcd_start( machine);
	}
	else
	{
		s3c240x_lcd_stop( machine);
	}
}

WRITE32_MEMBER(gp32_state::s3c240x_lcd_w)
{
	UINT32 old_value = m_s3c240x_lcd_regs[offset];
	verboselog( machine(), 9, "(LCD) %08X <- %08X (PC %08X)\n", 0x14A00000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_lcd_regs[offset]);
	switch (offset)
	{
		// LCDCON1
		case 0x00 / 4 :
		{
			if ((old_value & 1) != (data & 1))
			{
				s3c240x_lcd_recalc( machine());
			}
		}
		break;
	}
}

// LCD PALETTE


READ32_MEMBER(gp32_state::s3c240x_lcd_palette_r)
{
	UINT32 data = m_s3c240x_lcd_palette[offset];
	verboselog( machine(), 9, "(LCD) %08X -> %08X (PC %08X)\n", 0x14A00400 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_lcd_palette_w)
{
	verboselog( machine(), 9, "(LCD) %08X <- %08X (PC %08X)\n", 0x14A00400 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_lcd_palette[offset]);
	if (mem_mask != 0xffffffff)
	{
		verboselog( machine(), 0, "s3c240x_lcd_palette_w: unknown mask %08x\n", mem_mask);
	}
	palette_set_color( machine(), offset, s3c240x_get_color_5551( data & 0xFFFF));
}

// CLOCK & POWER MANAGEMENT


static UINT32 s3c240x_get_fclk(gp32_state *state, int reg)
{
	UINT32 data, mdiv, pdiv, sdiv;
	data = state->m_s3c240x_clkpow_regs[reg]; // MPLLCON or UPLLCON
	mdiv = BITS( data, 19, 12);
	pdiv = BITS( data, 9, 4);
	sdiv = BITS( data, 1, 0);
	return (UINT32)((double)((mdiv + 8) * 12000000) / (double)((pdiv + 2) * (1 << sdiv)));
}

static UINT32 s3c240x_get_hclk(gp32_state *state, int reg)
{
	switch (state->m_s3c240x_clkpow_regs[5] & 0x3) // CLKDIVN
	{
		case 0 : return s3c240x_get_fclk(state, reg) / 1;
		case 1 : return s3c240x_get_fclk(state, reg) / 1;
		case 2 : return s3c240x_get_fclk(state, reg) / 2;
		case 3 : return s3c240x_get_fclk(state, reg) / 2;
	}
	return 0;
}

static UINT32 s3c240x_get_pclk(gp32_state *state, int reg)
{
	switch (state->m_s3c240x_clkpow_regs[5] & 0x3) // CLKDIVN
	{
		case 0 : return s3c240x_get_fclk(state, reg) / 1;
		case 1 : return s3c240x_get_fclk(state, reg) / 2;
		case 2 : return s3c240x_get_fclk(state, reg) / 2;
		case 3 : return s3c240x_get_fclk(state, reg) / 4;
	}
	return 0;
}

READ32_MEMBER(gp32_state::s3c240x_clkpow_r)
{
	UINT32 data = m_s3c240x_clkpow_regs[offset];
	verboselog( machine(), 9, "(CLKPOW) %08X -> %08X (PC %08X)\n", 0x14800000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_clkpow_w)
{
	verboselog( machine(), 9, "(CLKPOW) %08X <- %08X (PC %08X)\n", 0x14800000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_clkpow_regs[offset]);
	switch (offset)
	{
		// MPLLCON
		case 0x04 / 4 :
		{
			gp32_state *state = machine().driver_data<gp32_state>();
			machine().device("maincpu")->set_unscaled_clock(s3c240x_get_fclk(state, MPLLCON) * CLOCK_MULTIPLIER);
		}
		break;
	}
}

// INTERRUPT CONTROLLER


static void s3c240x_check_pending_irq( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	if (state->m_s3c240x_irq_regs[0] != 0)
	{
		UINT32 int_type = 0, temp;
		temp = state->m_s3c240x_irq_regs[0];
		while (!(temp & 1))
		{
			int_type++;
			temp = temp >> 1;
		}
		state->m_s3c240x_irq_regs[4] |= (1 << int_type); // INTPND
		state->m_s3c240x_irq_regs[5] = int_type; // INTOFFSET
		machine.device( "maincpu")->execute().set_input_line(ARM7_IRQ_LINE, ASSERT_LINE);
	}
	else
	{
		machine.device( "maincpu")->execute().set_input_line(ARM7_IRQ_LINE, CLEAR_LINE);
	}
}

static void s3c240x_request_irq( running_machine &machine, UINT32 int_type)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 5, "request irq %d\n", int_type);
	if (state->m_s3c240x_irq_regs[0] == 0)
	{
		state->m_s3c240x_irq_regs[0] |= (1 << int_type); // SRCPND
		state->m_s3c240x_irq_regs[4] |= (1 << int_type); // INTPND
		state->m_s3c240x_irq_regs[5] = int_type; // INTOFFSET
		machine.device( "maincpu")->execute().set_input_line(ARM7_IRQ_LINE, ASSERT_LINE);
	}
	else
	{
		state->m_s3c240x_irq_regs[0] |= (1 << int_type); // SRCPND
		s3c240x_check_pending_irq( machine);
	}
}


READ32_MEMBER(gp32_state::s3c240x_irq_r)
{
	UINT32 data = m_s3c240x_irq_regs[offset];
	verboselog( machine(), 9, "(IRQ) %08X -> %08X (PC %08X)\n", 0x14400000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_irq_w)
{
	UINT32 old_value = m_s3c240x_irq_regs[offset];
	verboselog( machine(), 9, "(IRQ) %08X <- %08X (PC %08X)\n", 0x14400000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_irq_regs[offset]);
	switch (offset)
	{
		// SRCPND
		case 0x00 / 4 :
		{
			m_s3c240x_irq_regs[0] = (old_value & ~data); // clear only the bit positions of SRCPND corresponding to those set to one in the data
			s3c240x_check_pending_irq( machine());
		}
		break;
		// INTPND
		case 0x10 / 4 :
		{
			m_s3c240x_irq_regs[4] = (old_value & ~data); // clear only the bit positions of INTPND corresponding to those set to one in the data
		}
		break;
	}
}

// PWM TIMER

#if 0
static const char *const timer_reg_names[] =
{
	"Timer config 0",
	"Timer config 1",
	"Timer control",
	"Timer count buffer 0",
	"Timer compare buffer 0",
	"Timer count observation 0",
	"Timer count buffer 1",
	"Timer compare buffer 1",
	"Timer count observation 1",
	"Timer count buffer 2",
	"Timer compare buffer 2",
	"Timer count observation 2",
	"Timer count buffer 3",
	"Timer compare buffer 3",
	"Timer count observation 3",
	"Timer count buffer 4",
	"Timer compare buffer 4",
	"Timer count observation 4",
};
#endif


READ32_MEMBER(gp32_state::s3c240x_pwm_r)
{
	UINT32 data = m_s3c240x_pwm_regs[offset];
	verboselog( machine(), 9, "(PWM) %08X -> %08X (PC %08X)\n", 0x15100000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

static void s3c240x_pwm_start( running_machine &machine, int timer)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	static const int mux_table[] = { 2, 4, 8, 16 };
	static const int prescaler_shift[] = { 0, 0, 8, 8, 8 };
	static const int mux_shift[] = { 0, 4, 8, 12, 16 };
	static const int tcon_shift[] = { 0, 8, 12, 16, 20 };
	const UINT32 *regs = &state->m_s3c240x_pwm_regs[3+timer*3];
	UINT32 prescaler, mux, cnt, cmp, auto_reload;
	double freq, hz;
	verboselog( machine, 1, "PWM %d start\n", timer);
	prescaler = (state->m_s3c240x_pwm_regs[0] >> prescaler_shift[timer]) & 0xFF;
	mux = (state->m_s3c240x_pwm_regs[1] >> mux_shift[timer]) & 0x0F;
	freq = s3c240x_get_pclk(state, MPLLCON) / (prescaler + 1) / mux_table[mux];
	cnt = BITS( regs[0], 15, 0);
	if (timer != 4)
	{
		cmp = BITS( regs[1], 15, 0);
		auto_reload = BIT( state->m_s3c240x_pwm_regs[2], tcon_shift[timer] + 3);
	}
	else
	{
		cmp = 0;
		auto_reload = BIT( state->m_s3c240x_pwm_regs[2], tcon_shift[timer] + 2);
	}
	hz = freq / (cnt - cmp + 1);
	verboselog( machine, 5, "PWM %d - FCLK=%d HCLK=%d PCLK=%d prescaler=%d div=%d freq=%f cnt=%d cmp=%d auto_reload=%d hz=%f\n", timer, s3c240x_get_fclk(state, MPLLCON), s3c240x_get_hclk(state, MPLLCON), s3c240x_get_pclk(state, MPLLCON), prescaler, mux_table[mux], freq, cnt, cmp, auto_reload, hz);
	if (auto_reload)
	{
		state->m_s3c240x_pwm_timer[timer]->adjust( attotime::from_hz( hz), timer, attotime::from_hz( hz));
	}
	else
	{
		state->m_s3c240x_pwm_timer[timer]->adjust( attotime::from_hz( hz), timer);
	}
}

static void s3c240x_pwm_stop( running_machine &machine, int timer)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 1, "PWM %d stop\n", timer);
	state->m_s3c240x_pwm_timer[timer]->adjust( attotime::never);
}

static void s3c240x_pwm_recalc( running_machine &machine, int timer)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	static const int tcon_shift[] = { 0, 8, 12, 16, 20 };
	if (state->m_s3c240x_pwm_regs[2] & (1 << tcon_shift[timer]))
	{
		s3c240x_pwm_start( machine, timer);
	}
	else
	{
		s3c240x_pwm_stop( machine, timer);
	}
}

WRITE32_MEMBER(gp32_state::s3c240x_pwm_w)
{
	UINT32 old_value = m_s3c240x_pwm_regs[offset];
	verboselog( machine(), 9, "(PWM) %08X <- %08X (PC %08X)\n", 0x15100000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_pwm_regs[offset]);
	switch (offset)
	{
		// TCON
		case 0x08 / 4 :
		{
			if ((data & 1) != (old_value & 1))
			{
				s3c240x_pwm_recalc( machine(), 0);
			}
			if ((data & 0x100) != (old_value & 0x100))
			{
				s3c240x_pwm_recalc( machine(), 1);
			}
			if ((data & 0x1000) != (old_value & 0x1000))
			{
				s3c240x_pwm_recalc( machine(), 2);
			}
			if ((data & 0x10000) != (old_value & 0x10000))
			{
				s3c240x_pwm_recalc( machine(), 3);
			}
			if ((data & 0x100000) != (old_value & 0x100000))
			{
				s3c240x_pwm_recalc( machine(), 4);
			}
		}
	}
}

TIMER_CALLBACK_MEMBER(gp32_state::s3c240x_pwm_timer_exp)
{
	int ch = param;
	static const int ch_int[] = { INT_TIMER0, INT_TIMER1, INT_TIMER2, INT_TIMER3, INT_TIMER4 };
	verboselog( machine(), 2, "PWM %d timer callback\n", ch);
	if (BITS( m_s3c240x_pwm_regs[1], 23, 20) == (ch + 1))
	{
		s3c240x_dma_request_pwm( machine());
	}
	else
	{
		s3c240x_request_irq( machine(), ch_int[ch]);
	}
}

// DMA


static void s3c240x_dma_reload( running_machine &machine, int dma)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	UINT32 *regs = &state->m_s3c240x_dma_regs[dma<<3];
	regs[3] = (regs[3] & ~0x000FFFFF) | BITS( regs[2], 19, 0);
	regs[4] = (regs[4] & ~0x1FFFFFFF) | BITS( regs[0], 28, 0);
	regs[5] = (regs[5] & ~0x1FFFFFFF) | BITS( regs[1], 28, 0);
}

static void s3c240x_dma_trigger( running_machine &machine, int dma)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	UINT32 *regs = &state->m_s3c240x_dma_regs[dma<<3];
	UINT32 curr_tc, curr_src, curr_dst;
	address_space &space = machine.device( "maincpu")->memory().space( AS_PROGRAM);
	int dsz, inc_src, inc_dst, servmode;
	static const UINT32 ch_int[] = { INT_DMA0, INT_DMA1, INT_DMA2, INT_DMA3 };
	verboselog( machine, 5, "DMA %d trigger\n", dma);
	curr_tc = BITS( regs[3], 19, 0);
	curr_src = BITS( regs[4], 28, 0);
	curr_dst = BITS( regs[5], 28, 0);
	dsz = BITS( regs[2], 21, 20);
	servmode = BIT( regs[2], 26);
	inc_src = BIT( regs[0], 29);
	inc_dst = BIT( regs[1], 29);
	verboselog( machine, 5, "DMA %d - curr_src %08X curr_dst %08X curr_tc %d dsz %d\n", dma, curr_src, curr_dst, curr_tc, dsz);
	while (curr_tc > 0)
	{
		curr_tc--;
		switch (dsz)
		{
			case 0 : space.write_byte( curr_dst, space.read_byte( curr_src)); break;
			case 1 : space.write_word( curr_dst, space.read_word( curr_src)); break;
			case 2 : space.write_dword( curr_dst, space.read_dword( curr_src)); break;
		}
		if (inc_src == 0) curr_src += (1 << dsz);
		if (inc_dst == 0) curr_dst += (1 << dsz);
		if (servmode == 0) break;
	}
	// update curr_src
	regs[4] = (regs[4] & ~0x1FFFFFFF) | curr_src;
	// update curr_dst
	regs[5] = (regs[5] & ~0x1FFFFFFF) | curr_dst;
	// update curr_tc
	regs[3] = (regs[3] & ~0x000FFFFF) | curr_tc;
	// ...
	if (curr_tc == 0)
	{
		int _int, reload;
		reload = BIT( regs[2], 22);
		if (!reload)
		{
			s3c240x_dma_reload( machine, dma);
		}
		else
		{
			regs[6] &= ~(1 << 1); // clear on/off
		}
		_int = BIT( regs[2], 28);
		if (_int)
		{
			s3c240x_request_irq( machine, ch_int[dma]);
		}
	}
}

static void s3c240x_dma_request_iis( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	UINT32 *regs = &state->m_s3c240x_dma_regs[2<<3];
	verboselog( machine, 5, "s3c240x_dma_request_iis\n");
	if ((BIT( regs[6], 1) != 0) && (BIT( regs[2], 23) != 0) && (BITS( regs[2], 25, 24) == 0))
	{
		s3c240x_dma_trigger( machine, 2);
	}
}

static void s3c240x_dma_request_pwm( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	int i;
	verboselog( machine, 5, "s3c240x_dma_request_pwm\n");
	for (i = 0; i < 4; i++)
	{
		if (i != 1)
		{
			UINT32 *regs = &state->m_s3c240x_dma_regs[i<<3];
			if ((BIT( regs[6], 1) != 0) && (BIT( regs[2], 23) != 0) && (BITS( regs[2], 25, 24) == 3))
			{
				s3c240x_dma_trigger( machine, i);
			}
		}
	}
}

static void s3c240x_dma_start( running_machine &machine, int dma)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	UINT32 addr_src, addr_dst, tc;
	UINT32 *regs = &state->m_s3c240x_dma_regs[dma<<3];
	UINT32 dsz, tsz, reload;
	int inc_src, inc_dst, _int, servmode, swhwsel, hwsrcsel;
	verboselog( machine, 1, "DMA %d start\n", dma);
	addr_src = BITS( regs[0], 28, 0);
	addr_dst = BITS( regs[1], 28, 0);
	tc = BITS( regs[2], 19, 0);
	inc_src = BIT( regs[0], 29);
	inc_dst = BIT( regs[1], 29);
	tsz = BIT( regs[2], 27);
	_int = BIT( regs[2], 28);
	servmode = BIT( regs[2], 26);
	hwsrcsel = BITS( regs[2], 25, 24);
	swhwsel = BIT( regs[2], 23);
	reload = BIT( regs[2], 22);
	dsz = BITS( regs[2], 21, 20);
	verboselog( machine, 5, "DMA %d - addr_src %08X inc_src %d addr_dst %08X inc_dst %d int %d tsz %d servmode %d hwsrcsel %d swhwsel %d reload %d dsz %d tc %d\n", dma, addr_src, inc_src, addr_dst, inc_dst, _int, tsz, servmode, hwsrcsel, swhwsel, reload, dsz, tc);
	verboselog( machine, 5, "DMA %d - copy %08X bytes from %08X (%s) to %08X (%s)\n", dma, tc << dsz, addr_src, inc_src ? "fix" : "inc", addr_dst, inc_dst ? "fix" : "inc");
	s3c240x_dma_reload( machine, dma);
	if (swhwsel == 0)
	{
		s3c240x_dma_trigger( machine, dma);
	}
}

static void s3c240x_dma_stop( running_machine &machine, int dma)
{
	verboselog( machine, 1, "DMA %d stop\n", dma);
}

static void s3c240x_dma_recalc( running_machine &machine, int dma)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	if (state->m_s3c240x_dma_regs[(dma<<3)+6] & 2)
	{
		s3c240x_dma_start( machine, dma);
	}
	else
	{
		s3c240x_dma_stop( machine, dma);
	}
}

READ32_MEMBER(gp32_state::s3c240x_dma_r)
{
	UINT32 data = m_s3c240x_dma_regs[offset];
	verboselog( machine(), 9, "(DMA) %08X -> %08X (PC %08X)\n", 0x14600000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_dma_w)
{
	UINT32 old_value = m_s3c240x_dma_regs[offset];
	verboselog( machine(), 9, "(DMA) %08X <- %08X (PC %08X)\n", 0x14600000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_dma_regs[offset]);
	switch (offset)
	{
		// DCON0
		case 0x08 / 4 :
		{
			if (((data >> 22) & 1) != 0) // reload
			{
				m_s3c240x_dma_regs[0x18/4] &= ~(1 << 1); // clear on/off
			}
		}
		break;
		// DMASKTRIG0
		case 0x18 / 4 :
		{
			if ((old_value & 2) != (data & 2)) s3c240x_dma_recalc( machine(), 0);
		}
		break;
		// DCON1
		case 0x28 / 4 :
		{
			if (((data >> 22) & 1) != 0) // reload
			{
				m_s3c240x_dma_regs[0x38/4] &= ~(1 << 1); // clear on/off
			}
		}
		break;
		// DMASKTRIG1
		case 0x38 / 4 :
		{
			if ((old_value & 2) != (data & 2)) s3c240x_dma_recalc( machine(), 1);
		}
		break;
		// DCON2
		case 0x48 / 4 :
		{
			if (((data >> 22) & 1) != 0) // reload
			{
				m_s3c240x_dma_regs[0x58/4] &= ~(1 << 1); // clear on/off
			}
		}
		break;
		// DMASKTRIG2
		case 0x58 / 4 :
		{
			if ((old_value & 2) != (data & 2)) s3c240x_dma_recalc( machine(), 2);
		}
		break;
		// DCON3
		case 0x68 / 4 :
		{
			if (((data >> 22) & 1) != 0) // reload
			{
				m_s3c240x_dma_regs[0x78/4] &= ~(1 << 1); // clear on/off
			}
		}
		break;
		// DMASKTRIG3
		case 0x78 / 4 :
		{
			if ((old_value & 2) != (data & 2)) s3c240x_dma_recalc( machine(), 3);
		}
		break;
	}
}

TIMER_CALLBACK_MEMBER(gp32_state::s3c240x_dma_timer_exp)
{
	int ch = param;
	verboselog( machine(), 2, "DMA %d timer callback\n", ch);
}

// SMARTMEDIA

static void smc_reset( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 5, "smc_reset\n");
	state->m_smc.add_latch = 0;
	state->m_smc.chip = 0;
	state->m_smc.cmd_latch = 0;
	state->m_smc.do_read = 0;
	state->m_smc.do_write = 0;
	state->m_smc.read = 0;
	state->m_smc.wp = 0;
	state->m_smc.busy = 0;
}

static void smc_init( running_machine &machine)
{
	verboselog( machine, 5, "smc_init\n");
	smc_reset( machine);
}

static UINT8 smc_read( running_machine &machine)
{
	smartmedia_image_device *smartmedia = machine.device<smartmedia_image_device>( "smartmedia");
	UINT8 data;
	data = smartmedia->data_r();
	verboselog( machine, 5, "smc_read %08X\n", data);
	return data;
}

static void smc_write( running_machine &machine, UINT8 data)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 5, "smc_write %08X\n", data);
	if ((state->m_smc.chip) && (!state->m_smc.read))
	{
		smartmedia_image_device *smartmedia = machine.device<smartmedia_image_device>( "smartmedia");
		if (state->m_smc.cmd_latch)
		{
			verboselog( machine, 5, "smartmedia_command_w %08X\n", data);
			smartmedia->command_w(data);
		}
		else if (state->m_smc.add_latch)
		{
			verboselog( machine, 5, "smartmedia_address_w %08X\n", data);
			smartmedia->address_w(data);
		}
		else
		{
			verboselog( machine, 5, "smartmedia_data_w %08X\n", data);
			smartmedia->data_w(data);
		}
	}
}

static void smc_update( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	if (!state->m_smc.chip)
	{
		smc_reset( machine);
	}
	else
	{
		if ((state->m_smc.do_write) && (!state->m_smc.read))
		{
			smc_write( machine, state->m_smc.datatx);
		}
		else if ((!state->m_smc.do_write) && (state->m_smc.do_read) && (state->m_smc.read) && (!state->m_smc.cmd_latch) && (!state->m_smc.add_latch))
		{
			state->m_smc.datarx = smc_read( machine);
		}
	}
}

// I2S

#define I2S_L3C ( 1 )
#define I2S_L3M ( 2 )
#define I2S_L3D ( 3 )

static void i2s_reset( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 5, "i2s_reset\n");
	state->m_i2s.l3d = 0;
	state->m_i2s.l3m = 0;
	state->m_i2s.l3c = 0;
}

static void i2s_init( running_machine &machine)
{
	verboselog( machine, 5, "i2s_init\n");
	i2s_reset( machine);
}

static void i2s_write( running_machine &machine, int line, int data)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	switch (line)
	{
		case I2S_L3C :
		{
			if (data != state->m_i2s.l3c)
			{
				verboselog( machine, 5, "I2S L3C %d\n", data);
				state->m_i2s.l3c = data;
			}
		}
		break;
		case I2S_L3M :
		{
			if (data != state->m_i2s.l3m)
			{
				verboselog( machine, 5, "I2S L3M %d\n", data);
				state->m_i2s.l3m = data;
			}
		}
		break;
		case I2S_L3D :
		{
			if (data != state->m_i2s.l3d)
			{
				verboselog( machine, 5, "I2S L3D %d\n", data);
				state->m_i2s.l3d = data;
			}
		}
		break;
	}
}

// I/O PORT


READ32_MEMBER(gp32_state::s3c240x_gpio_r)
{
	UINT32 data = m_s3c240x_gpio[offset];
	switch (offset)
	{
		// PBCON
		case 0x08 / 4 :
		{
			// smartmedia
			data = (data & ~0x00000001);
			if (!m_smc.read) data = data | 0x00000001;
		}
		break;
		// PBDAT
		case 0x0C / 4 :
		{
			// smartmedia
			data = (data & ~0x000000FF) | (m_smc.datarx & 0xFF);
			// buttons
			data = (data & ~0x0000FF00) | (ioport( "IN0")->read() & 0x0000FF00);
		}
		break;
		// PDDAT
		case 0x24 / 4 :
		{
			smartmedia_image_device *smartmedia = machine().device<smartmedia_image_device>( "smartmedia");
			// smartmedia
			data = (data & ~0x000003C0);
			if (!m_smc.busy) data = data | 0x00000200;
			if (!m_smc.do_read) data = data | 0x00000100;
			if (!m_smc.chip) data = data | 0x00000080;
			if (!smartmedia->is_protected()) data = data | 0x00000040;
		}
		break;
		// PEDAT
		case 0x30 / 4 :
		{
			smartmedia_image_device *smartmedia = machine().device<smartmedia_image_device>( "smartmedia");
			// smartmedia
			data = (data & ~0x0000003C);
			if (m_smc.cmd_latch) data = data | 0x00000020;
			if (m_smc.add_latch) data = data | 0x00000010;
			if (!m_smc.do_write) data = data | 0x00000008;
			if (!smartmedia->is_present()) data = data | 0x00000004;
			// buttons
			data = (data & ~0x000000C0) | (ioport( "IN1")->read() & 0x000000C0);
		}
		break;
	}
	verboselog( machine(), 9, "(GPIO) %08X -> %08X (PC %08X)\n", 0x15600000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_gpio_w)
{
	COMBINE_DATA(&m_s3c240x_gpio[offset]);
	verboselog( machine(), 9, "(GPIO) %08X <- %08X (PC %08X)\n", 0x15600000 + (offset << 2), data, space.device().safe_pc( ));
	switch (offset)
	{
		// PBCON
		case 0x08 / 4 :
		{
			// smartmedia
			m_smc.read = ((data & 0x00000001) == 0);
			smc_update( machine());
		}
		break;
		// PBDAT
		case 0x0C / 4 :
		{
			// smartmedia
			m_smc.datatx = data & 0xFF;
		}
		break;
		// PDDAT
		case 0x24 / 4 :
		{
			// smartmedia
			m_smc.do_read = ((data & 0x00000100) == 0);
			m_smc.chip = ((data & 0x00000080) == 0);
			m_smc.wp = ((data & 0x00000040) == 0);
			smc_update( machine());
		}
		break;
		// PEDAT
		case 0x30 / 4 :
		{
			// smartmedia
			m_smc.cmd_latch = ((data & 0x00000020) != 0);
			m_smc.add_latch = ((data & 0x00000010) != 0);
			m_smc.do_write = ((data & 0x00000008) == 0);
			smc_update( machine());
			// sound
			i2s_write( machine(), I2S_L3D, (data & 0x00000800) ? 1 : 0);
			i2s_write( machine(), I2S_L3M, (data & 0x00000400) ? 1 : 0);
			i2s_write( machine(), I2S_L3C, (data & 0x00000200) ? 1 : 0);
		}
		break;
#if 0
		// PGDAT
		case 0x48 / 4 :
		{
			int i2ssdo;
			i2ssdo = BIT( data, 3);
		}
		break;
#endif
	}
}

// MEMORY CONTROLLER


READ32_MEMBER(gp32_state::s3c240x_memcon_r)
{
	UINT32 data = m_s3c240x_memcon_regs[offset];
	verboselog( machine(), 9, "(MEMCON) %08X -> %08X (PC %08X)\n", 0x14000000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_memcon_w)
{
	verboselog( machine(), 9, "(MEMCON) %08X <- %08X (PC %08X)\n", 0x14000000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_memcon_regs[offset]);
}

// USB HOST CONTROLLER


READ32_MEMBER(gp32_state::s3c240x_usb_host_r)
{
	UINT32 data = m_s3c240x_usb_host_regs[offset];
	verboselog( machine(), 9, "(USB H) %08X -> %08X (PC %08X)\n", 0x14200000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_usb_host_w)
{
	verboselog( machine(), 9, "(USB H) %08X <- %08X (PC %08X)\n", 0x14200000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_usb_host_regs[offset]);
}

// UART 0


READ32_MEMBER(gp32_state::s3c240x_uart_0_r)
{
	UINT32 data = m_s3c240x_uart_0_regs[offset];
	switch (offset)
	{
		// UTRSTAT0
		case 0x10 / 4 :
		{
			data = (data & ~0x00000006) | 0x00000004 | 0x00000002; // [bit 2] Transmitter empty / [bit 1] Transmit buffer empty
		}
		break;
	}
	verboselog( machine(), 9, "(UART 0) %08X -> %08X (PC %08X)\n", 0x15000000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_uart_0_w)
{
	verboselog( machine(), 9, "(UART 0) %08X <- %08X (PC %08X)\n", 0x15000000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_uart_0_regs[offset]);
}

// UART 1


READ32_MEMBER(gp32_state::s3c240x_uart_1_r)
{
	UINT32 data = m_s3c240x_uart_1_regs[offset];
	switch (offset)
	{
		// UTRSTAT1
		case 0x10 / 4 :
		{
			data = (data & ~0x00000006) | 0x00000004 | 0x00000002; // [bit 2] Transmitter empty / [bit 1] Transmit buffer empty
		}
		break;
	}
	verboselog( machine(), 9, "(UART 1) %08X -> %08X (PC %08X)\n", 0x15004000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_uart_1_w)
{
	verboselog( machine(), 9, "(UART 1) %08X <- %08X (PC %08X)\n", 0x15004000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_uart_1_regs[offset]);
}

// USB DEVICE


READ32_MEMBER(gp32_state::s3c240x_usb_device_r)
{
	UINT32 data = m_s3c240x_usb_device_regs[offset];
	verboselog( machine(), 9, "(USB D) %08X -> %08X (PC %08X)\n", 0x15200140 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_usb_device_w)
{
	verboselog( machine(), 9, "(USB D) %08X <- %08X (PC %08X)\n", 0x15200140 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_usb_device_regs[offset]);
}

// WATCHDOG TIMER


READ32_MEMBER(gp32_state::s3c240x_watchdog_r)
{
	UINT32 data = m_s3c240x_watchdog_regs[offset];
	verboselog( machine(), 9, "(WDOG) %08X -> %08X (PC %08X)\n", 0x15300000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_watchdog_w)
{
	verboselog( machine(), 9, "(WDOG) %08X <- %08X (PC %08X)\n", 0x15300000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_watchdog_regs[offset]);
}

// EEPROM

static UINT8 eeprom_read( running_machine &machine, UINT16 address)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	UINT8 data;
	data = state->m_eeprom_data[address];
	verboselog( machine, 5, "EEPROM %04X -> %02X\n", address, data);
	return data;
}

static void eeprom_write( running_machine &machine, UINT16 address, UINT8 data)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 5, "EEPROM %04X <- %02X\n", address, data);
	state->m_eeprom_data[address] = data;
}

// IIC

#if 0
static UINT8 i2cmem_read_byte( running_machine &machine, int last)
{
	UINT8 data = 0;
	int i;
	i2cmem_write( machine, 0, I2CMEM_SDA, 1);
	for (i = 0; i < 8; i++)
	{
		i2cmem_write( machine, 0, I2CMEM_SCL, 1);
		data = (data << 1) + (i2cmem_read( machine, 0, I2CMEM_SDA) ? 1 : 0);
		i2cmem_write( machine, 0, I2CMEM_SCL, 0);
	}
	i2cmem_write( machine, 0, I2CMEM_SDA, last);
	i2cmem_write( machine, 0, I2CMEM_SCL, 1);
	i2cmem_write( machine, 0, I2CMEM_SCL, 0);
	return data;
}
#endif

#if 0
static void i2cmem_write_byte( running_machine &machine, UINT8 data)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		i2cmem_write( machine, 0, I2CMEM_SDA, (data & 0x80) ? 1 : 0);
		data = data << 1;
		i2cmem_write( machine, 0, I2CMEM_SCL, 1);
		i2cmem_write( machine, 0, I2CMEM_SCL, 0);
	}
	i2cmem_write( machine, 0, I2CMEM_SDA, 1); // ack bit
	i2cmem_write( machine, 0, I2CMEM_SCL, 1);
	i2cmem_write( machine, 0, I2CMEM_SCL, 0);
}
#endif

#if 0
static void i2cmem_start( running_machine &machine)
{
	i2cmem_write( machine, 0, I2CMEM_SDA, 1);
	i2cmem_write( machine, 0, I2CMEM_SCL, 1);
	i2cmem_write( machine, 0, I2CMEM_SDA, 0);
	i2cmem_write( machine, 0, I2CMEM_SCL, 0);
}
#endif

#if 0
static void i2cmem_stop( running_machine &machine)
{
	i2cmem_write( machine, 0, I2CMEM_SDA, 0);
	i2cmem_write( machine, 0, I2CMEM_SCL, 1);
	i2cmem_write( machine, 0, I2CMEM_SDA, 1);
	i2cmem_write( machine, 0, I2CMEM_SCL, 0);
}
#endif

static void iic_start( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 1, "IIC start\n");
	state->m_s3c240x_iic.data_index = 0;
	state->m_s3c240x_iic_timer->adjust( attotime::from_msec( 1));
}

static void iic_stop( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 1, "IIC stop\n");
	state->m_s3c240x_iic_timer->adjust( attotime::never);
}

static void iic_resume( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 1, "IIC resume\n");
	state->m_s3c240x_iic_timer->adjust( attotime::from_msec( 1));
}

READ32_MEMBER(gp32_state::s3c240x_iic_r)
{
	UINT32 data = m_s3c240x_iic_regs[offset];
	switch (offset)
	{
		// IICSTAT
		case 0x04 / 4 :
		{
			data = data & ~0x0000000F;
		}
		break;
	}
	verboselog( machine(), 9, "(IIC) %08X -> %08X (PC %08X)\n", 0x15400000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_iic_w)
{
	verboselog( machine(), 9, "(IIC) %08X <- %08X (PC %08X)\n", 0x15400000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_iic_regs[offset]);
	switch (offset)
	{
		// ADDR_IICCON
		case 0x00 / 4 :
		{
			int interrupt_pending_flag;
#if 0
			static const int div_table[] = { 16, 512 };
			int enable_interrupt, transmit_clock_value, tx_clock_source_selection
			double clock;
			transmit_clock_value = (data >> 0) & 0xF;
			tx_clock_source_selection = (data >> 6) & 1;
			enable_interrupt = (data >> 5) & 1;
			clock = (double)(s3c240x_get_pclk(state, MPLLCON) / div_table[tx_clock_source_selection] / (transmit_clock_value + 1));
#endif
			interrupt_pending_flag = BIT( data, 4);
			if (interrupt_pending_flag == 0)
			{
				int start_stop_condition;
				start_stop_condition = BIT( m_s3c240x_iic_regs[1], 5);
				if (start_stop_condition != 0)
				{
					iic_resume( machine());
				}
			}
		}
		break;
		// IICSTAT
		case 0x04 / 4 :
		{
			int start_stop_condition;
			start_stop_condition = BIT( data, 5);
			if (start_stop_condition != 0)
			{
				iic_start( machine());
			}
			else
			{
				iic_stop( machine());
			}
		}
		break;
	}
}

TIMER_CALLBACK_MEMBER(gp32_state::s3c240x_iic_timer_exp)
{
	int enable_interrupt, mode_selection;
	verboselog( machine(), 2, "IIC timer callback\n");
	mode_selection = BITS( m_s3c240x_iic_regs[1], 7, 6);
	switch (mode_selection)
	{
		// master receive mode
		case 2 :
		{
			if (m_s3c240x_iic.data_index == 0)
			{
				UINT8 data_shift = m_s3c240x_iic_regs[3] & 0xFF;
				verboselog( machine(), 5, "IIC write %02X\n", data_shift);
			}
			else
			{
				UINT8 data_shift = eeprom_read( machine(), m_s3c240x_iic.address);
				verboselog( machine(), 5, "IIC read %02X\n", data_shift);
				m_s3c240x_iic_regs[3] = (m_s3c240x_iic_regs[3] & ~0xFF) | data_shift;
			}
			m_s3c240x_iic.data_index++;
		}
		break;
		// master transmit mode
		case 3 :
		{
			UINT8 data_shift = m_s3c240x_iic_regs[3] & 0xFF;
			verboselog( machine(), 5, "IIC write %02X\n", data_shift);
			m_s3c240x_iic.data[m_s3c240x_iic.data_index++] = data_shift;
			if (m_s3c240x_iic.data_index == 3)
			{
				m_s3c240x_iic.address = (m_s3c240x_iic.data[1] << 8) | m_s3c240x_iic.data[2];
			}
			if ((m_s3c240x_iic.data_index == 4) && (m_s3c240x_iic.data[0] == 0xA0))
			{
				eeprom_write( machine(), m_s3c240x_iic.address, data_shift);
			}
		}
		break;
	}
	enable_interrupt = BIT( m_s3c240x_iic_regs[0], 5);
	if (enable_interrupt)
	{
		s3c240x_request_irq( machine(), INT_IIC);
	}
}

// IIS

static void s3c240x_iis_start( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	static const UINT32 codeclk_table[] = { 256, 384 };
	double freq;
	int prescaler_enable, prescaler_control_a, prescaler_control_b, codeclk;
	verboselog( machine, 1, "IIS start\n");
	prescaler_enable = BIT( state->m_s3c240x_iis_regs[0], 1);
	prescaler_control_a = BITS( state->m_s3c240x_iis_regs[2], 9, 5);
	prescaler_control_b = BITS( state->m_s3c240x_iis_regs[2], 4, 0);
	codeclk = BIT( state->m_s3c240x_iis_regs[1], 2);
	freq = (double)(s3c240x_get_pclk(state, MPLLCON) / (prescaler_control_a + 1) / codeclk_table[codeclk]) * 2; // why do I have to multiply by two?
	verboselog( machine, 5, "IIS - pclk %d psc_enable %d psc_a %d psc_b %d codeclk %d freq %f\n", s3c240x_get_pclk(state, MPLLCON), prescaler_enable, prescaler_control_a, prescaler_control_b, codeclk_table[codeclk], freq);
	state->m_s3c240x_iis_timer->adjust( attotime::from_hz( freq), 0, attotime::from_hz( freq));
}

static void s3c240x_iis_stop( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	verboselog( machine, 1, "IIS stop\n");
	state->m_s3c240x_iis_timer->adjust( attotime::never);
}

static void s3c240x_iis_recalc( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	if (state->m_s3c240x_iis_regs[0] & 1)
	{
		s3c240x_iis_start( machine);
	}
	else
	{
		s3c240x_iis_stop( machine);
	}
}

READ32_MEMBER(gp32_state::s3c240x_iis_r)
{
	UINT32 data = m_s3c240x_iis_regs[offset];
#if 0
	switch (offset)
	{
		// IISCON
		case 0x00 / 4 :
		{
			data = data & ~1; // for mp3 player
		}
		break;
	}
#endif
	verboselog( machine(), 9, "(IIS) %08X -> %08X (PC %08X)\n", 0x15508000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_iis_w)
{
	UINT32 old_value = m_s3c240x_iis_regs[offset];
	verboselog( machine(), 9, "(IIS) %08X <- %08X (PC %08X)\n", 0x15508000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_iis_regs[offset]);
	switch (offset)
	{
		// IISCON
		case 0x00 / 4 :
		{
			if ((old_value & 1) != (data & 1)) s3c240x_iis_recalc( machine());
		}
		break;
		// IISFIF
		case 0x10 / 4 :
		{
			if (ACCESSING_BITS_16_31)
			{
				m_s3c240x_iis.fifo[m_s3c240x_iis.fifo_index++] = BITS( data, 31, 16);
			}
			if (ACCESSING_BITS_0_15)
			{
				m_s3c240x_iis.fifo[m_s3c240x_iis.fifo_index++] = BITS( data, 15, 0);
			}
			if (m_s3c240x_iis.fifo_index == 2)
			{
				dac_device *dac[2];
				dac[0] = machine().device<dac_device>("dac1");
				dac[1] = machine().device<dac_device>("dac2");
				m_s3c240x_iis.fifo_index = 0;
				dac[0]->write_signed16(m_s3c240x_iis.fifo[0] + 0x8000);
				dac[1]->write_signed16(m_s3c240x_iis.fifo[1] + 0x8000);
			}
		}
		break;
	}
}

TIMER_CALLBACK_MEMBER(gp32_state::s3c240x_iis_timer_exp)
{
	verboselog( machine(), 2, "IIS timer callback\n");
	s3c240x_dma_request_iis( machine());
}

// RTC


READ32_MEMBER(gp32_state::s3c240x_rtc_r)
{
	UINT32 data = m_s3c240x_rtc_regs[offset];
	verboselog( machine(), 9, "(RTC) %08X -> %08X (PC %08X)\n", 0x15700040 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_rtc_w)
{
	verboselog( machine(), 9, "(RTC) %08X <- %08X (PC %08X)\n", 0x15700040 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_rtc_regs[offset]);
}

// A/D CONVERTER


READ32_MEMBER(gp32_state::s3c240x_adc_r)
{
	UINT32 data = m_s3c240x_adc_regs[offset];
	verboselog( machine(), 9, "(ADC) %08X -> %08X (PC %08X)\n", 0x15800000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_adc_w)
{
	verboselog( machine(), 9, "(ADC) %08X <- %08X (PC %08X)\n", 0x15800000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_adc_regs[offset]);
}

// SPI


READ32_MEMBER(gp32_state::s3c240x_spi_r)
{
	UINT32 data = m_s3c240x_spi_regs[offset];
	verboselog( machine(), 9, "(SPI) %08X -> %08X (PC %08X)\n", 0x15900000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_spi_w)
{
	verboselog( machine(), 9, "(SPI) %08X <- %08X (PC %08X)\n", 0x15900000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_spi_regs[offset]);
}

// MMC INTERFACE


READ32_MEMBER(gp32_state::s3c240x_mmc_r)
{
	UINT32 data = m_s3c240x_mmc_regs[offset];
	verboselog( machine(), 9, "(MMC) %08X -> %08X (PC %08X)\n", 0x15A00000 + (offset << 2), data, space.device().safe_pc( ));
	return data;
}

WRITE32_MEMBER(gp32_state::s3c240x_mmc_w)
{
	verboselog( machine(), 9, "(MMC) %08X <- %08X (PC %08X)\n", 0x15A00000 + (offset << 2), data, space.device().safe_pc( ));
	COMBINE_DATA(&m_s3c240x_mmc_regs[offset]);
}

// ...

static void s3c240x_machine_start( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	state->m_s3c240x_pwm_timer[0] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_pwm_timer_exp),state), (void *)(FPTR)0);
	state->m_s3c240x_pwm_timer[1] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_pwm_timer_exp),state), (void *)(FPTR)1);
	state->m_s3c240x_pwm_timer[2] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_pwm_timer_exp),state), (void *)(FPTR)2);
	state->m_s3c240x_pwm_timer[3] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_pwm_timer_exp),state), (void *)(FPTR)3);
	state->m_s3c240x_pwm_timer[4] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_pwm_timer_exp),state), (void *)(FPTR)4);
	state->m_s3c240x_dma_timer[0] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_dma_timer_exp),state), (void *)(FPTR)0);
	state->m_s3c240x_dma_timer[1] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_dma_timer_exp),state), (void *)(FPTR)1);
	state->m_s3c240x_dma_timer[2] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_dma_timer_exp),state), (void *)(FPTR)2);
	state->m_s3c240x_dma_timer[3] = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_dma_timer_exp),state), (void *)(FPTR)3);
	state->m_s3c240x_iic_timer = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_iic_timer_exp),state), (void *)(FPTR)0);
	state->m_s3c240x_iis_timer = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_iis_timer_exp),state), (void *)(FPTR)0);
	state->m_s3c240x_lcd_timer = machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(gp32_state::s3c240x_lcd_timer_exp),state), (void *)(FPTR)0);
	state->m_eeprom_data = auto_alloc_array( machine, UINT8, 0x2000);
	machine.device<nvram_device>("nvram")->set_base(state->m_eeprom_data, 0x2000);
	smc_init( machine);
	i2s_init( machine);
}

static void s3c240x_machine_reset( running_machine &machine)
{
	gp32_state *state = machine.driver_data<gp32_state>();
	smc_reset( machine);
	i2s_reset( machine);
	state->m_s3c240x_iis.fifo_index = 0;
	state->m_s3c240x_iic.data_index = 0;
}

static ADDRESS_MAP_START( gp32_map, AS_PROGRAM, 32, gp32_state )
	AM_RANGE(0x00000000, 0x0007ffff) AM_ROM
	AM_RANGE(0x0c000000, 0x0c7fffff) AM_RAM AM_SHARE("s3c240x_ram")
	AM_RANGE(0x14000000, 0x1400003b) AM_READWRITE(s3c240x_memcon_r, s3c240x_memcon_w)
	AM_RANGE(0x14200000, 0x1420005b) AM_READWRITE(s3c240x_usb_host_r, s3c240x_usb_host_w)
	AM_RANGE(0x14400000, 0x14400017) AM_READWRITE(s3c240x_irq_r, s3c240x_irq_w)
	AM_RANGE(0x14600000, 0x1460007b) AM_READWRITE(s3c240x_dma_r, s3c240x_dma_w)
	AM_RANGE(0x14800000, 0x14800017) AM_READWRITE(s3c240x_clkpow_r, s3c240x_clkpow_w)
	AM_RANGE(0x14a00000, 0x14a003ff) AM_READWRITE(s3c240x_lcd_r, s3c240x_lcd_w)
	AM_RANGE(0x14a00400, 0x14a007ff) AM_READWRITE(s3c240x_lcd_palette_r, s3c240x_lcd_palette_w)
	AM_RANGE(0x15000000, 0x1500002b) AM_READWRITE(s3c240x_uart_0_r, s3c240x_uart_0_w)
	AM_RANGE(0x15004000, 0x1500402b) AM_READWRITE(s3c240x_uart_1_r, s3c240x_uart_1_w)
	AM_RANGE(0x15100000, 0x15100043) AM_READWRITE(s3c240x_pwm_r, s3c240x_pwm_w)
	AM_RANGE(0x15200140, 0x152001fb) AM_READWRITE(s3c240x_usb_device_r, s3c240x_usb_device_w)
	AM_RANGE(0x15300000, 0x1530000b) AM_READWRITE(s3c240x_watchdog_r, s3c240x_watchdog_w)
	AM_RANGE(0x15400000, 0x1540000f) AM_READWRITE(s3c240x_iic_r, s3c240x_iic_w)
	AM_RANGE(0x15508000, 0x15508013) AM_READWRITE(s3c240x_iis_r, s3c240x_iis_w)
	AM_RANGE(0x15600000, 0x1560005b) AM_READWRITE(s3c240x_gpio_r, s3c240x_gpio_w)
	AM_RANGE(0x15700040, 0x1570008b) AM_READWRITE(s3c240x_rtc_r, s3c240x_rtc_w)
	AM_RANGE(0x15800000, 0x15800007) AM_READWRITE(s3c240x_adc_r, s3c240x_adc_w)
	AM_RANGE(0x15900000, 0x15900017) AM_READWRITE(s3c240x_spi_r, s3c240x_spi_w)
	AM_RANGE(0x15a00000, 0x15a0003f) AM_READWRITE(s3c240x_mmc_r, s3c240x_mmc_w)
ADDRESS_MAP_END

static INPUT_PORTS_START( gp32 )
	PORT_START("IN0")
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("R") PORT_PLAYER(1)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("L") PORT_PLAYER(1)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("B") PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("A") PORT_PLAYER(1)
	PORT_START("IN1")
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_SELECT ) PORT_NAME("SELECT") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_START ) PORT_NAME("START") PORT_PLAYER(1)
INPUT_PORTS_END

void gp32_state::machine_start()
{
	s3c240x_machine_start(machine());
}

void gp32_state::machine_reset()
{
	s3c240x_machine_reset(machine());
}

static MACHINE_CONFIG_START( gp32, gp32_state )
	MCFG_CPU_ADD("maincpu", ARM9, 40000000)
	MCFG_CPU_PROGRAM_MAP(gp32_map)

	MCFG_PALETTE_LENGTH(32768)

	MCFG_SCREEN_ADD("screen", LCD)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(240, 320)
	MCFG_SCREEN_VISIBLE_AREA(0, 239, 0, 319)
	MCFG_SCREEN_UPDATE_DRIVER(gp32_state, screen_update_gp32)

	/* 320x240 is 4:3 but ROT270 causes an aspect ratio of 3:4 by default */
	MCFG_DEFAULT_LAYOUT(layout_lcd_rot)


	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SOUND_ADD("dac1", DAC, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 1.0)
	MCFG_SOUND_ADD("dac2", DAC, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 1.0)

	MCFG_NVRAM_ADD_1FILL("nvram")

	MCFG_SMARTMEDIA_ADD("smartmedia")

	MCFG_SOFTWARE_LIST_ADD("memc_list","gp32")
MACHINE_CONFIG_END

ROM_START( gp32 )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_SYSTEM_BIOS( 0, "157e", "Firmware 1.5.7 (English)" )
	ROMX_LOAD( "gp32157e.bin", 0x000000, 0x080000, CRC(b1e35643) SHA1(1566bc2a27980602e9eb501cf8b2d62939bfd1e5), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "100k", "Firmware 1.0.0 (Korean)" )
	ROMX_LOAD( "gp32100k.bin", 0x000000, 0x080000, CRC(d9925ac9) SHA1(3604d0d7210ed72eddd3e3e0c108f1102508423c), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "156k", "Firmware 1.5.6 (Korean)" )
	ROMX_LOAD( "gp32156k.bin", 0x000000, 0x080000, CRC(667fb1c8) SHA1(d179ab8e96411272b6a1d683e59da752067f9da8), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "166m", "Firmware 1.6.6 (European)" )
	ROMX_LOAD( "gp32166m.bin", 0x000000, 0x080000, CRC(4548a840) SHA1(1ad0cab0af28fb45c182e5e8c87ead2aaa4fffe1), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "mfv2", "Mr. Spiv Multi Firmware V2" )
	ROMX_LOAD( "gp32mfv2.bin", 0x000000, 0x080000, CRC(7ddaaaeb) SHA1(5a85278f721beb3b00125db5c912d1dc552c5897), ROM_BIOS(5) )
#if 0
	ROM_SYSTEM_BIOS( 5, "test", "test" )
	ROMX_LOAD( "test.bin", 0x000000, 0x080000, CRC(00000000) SHA1(0000000000000000000000000000000000000000), ROM_BIOS(6) )
#endif
ROM_END

CONS(2001, gp32, 0, 0, gp32, gp32, driver_device, 0, "Game Park Holdings", "GP32", ROT270|GAME_NOT_WORKING|GAME_NO_SOUND)
