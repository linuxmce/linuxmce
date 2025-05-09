/*****************************************************************************
 *
 * includes/coupe.h
 *
 * SAM Coupe
 *
 * Driver by Lee Hammerton
 *
 ****************************************************************************/

#ifndef SAMCOUPE_H_
#define SAMCOUPE_H_

#include "machine/wd_fdc.h"

/* screen dimensions */
#define SAM_BLOCK           8

#define SAM_TOTAL_WIDTH     SAM_BLOCK*96
#define SAM_TOTAL_HEIGHT    312
#define SAM_SCREEN_WIDTH    SAM_BLOCK*64
#define SAM_SCREEN_HEIGHT   192
#define SAM_BORDER_LEFT     SAM_BLOCK*4
#define SAM_BORDER_RIGHT    SAM_BLOCK*4
#define SAM_BORDER_TOP      37
#define SAM_BORDER_BOTTOM   46

/* interrupt sources */
#define SAM_LINE_INT     0x01
#define SAM_MOUSE_INT    0x02
#define SAM_MIDIIN_INT   0x04
#define SAM_FRAME_INT    0x08
#define SAM_MIDIOUT_INT  0x10


class samcoupe_state :  public driver_device
{
public:
	samcoupe_state(const machine_config &mconfig, device_type type, const char *tag)
			: driver_device(mconfig, type, tag)
	{
		sam_bank_read_ptr[0] = NULL;
		sam_bank_write_ptr[0] = NULL;
		sam_bank_read_ptr[1] = NULL;
		sam_bank_write_ptr[1] = NULL;
		sam_bank_read_ptr[2] = NULL;
		sam_bank_write_ptr[2] = NULL;
		sam_bank_read_ptr[3] = NULL;
		sam_bank_write_ptr[3] = NULL;
	}

	virtual void video_start();

	UINT32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	bitmap_ind16 m_bitmap;

	emu_timer *m_video_update_timer;

	UINT8 m_lmpr, m_hmpr, m_vmpr; /* memory pages */
	UINT8 m_lext, m_hext;       /* extended memory page */
	UINT8 m_border;           /* border */
	UINT8 m_clut[16];         /* color lookup table, 16 entries */
	UINT8 m_line_int;         /* line interrupt */
	UINT8 m_status;           /* status register */

	/* attribute */
	UINT8 m_attribute;

	/* mouse */
	int m_mouse_index;
	emu_timer *m_mouse_reset;
	UINT8 m_mouse_data[9];
	int m_mouse_x, m_mouse_y;
	UINT8 *m_videoram;
	DECLARE_WRITE8_MEMBER(samcoupe_ext_mem_w);
	DECLARE_READ8_MEMBER(samcoupe_disk_r);
	DECLARE_WRITE8_MEMBER(samcoupe_disk_w);
	DECLARE_READ8_MEMBER(samcoupe_pen_r);
	DECLARE_WRITE8_MEMBER(samcoupe_clut_w);
	DECLARE_READ8_MEMBER(samcoupe_status_r);
	DECLARE_WRITE8_MEMBER(samcoupe_line_int_w);
	DECLARE_READ8_MEMBER(samcoupe_lmpr_r);
	DECLARE_WRITE8_MEMBER(samcoupe_lmpr_w);
	DECLARE_READ8_MEMBER(samcoupe_hmpr_r);
	DECLARE_WRITE8_MEMBER(samcoupe_hmpr_w);
	DECLARE_READ8_MEMBER(samcoupe_vmpr_r);
	DECLARE_WRITE8_MEMBER(samcoupe_vmpr_w);
	DECLARE_READ8_MEMBER(samcoupe_midi_r);
	DECLARE_WRITE8_MEMBER(samcoupe_midi_w);
	DECLARE_READ8_MEMBER(samcoupe_keyboard_r);
	DECLARE_WRITE8_MEMBER(samcoupe_border_w);
	DECLARE_READ8_MEMBER(samcoupe_attributes_r);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void palette_init();
	INTERRUPT_GEN_MEMBER(samcoupe_frame_interrupt);
	TIMER_CALLBACK_MEMBER(irq_off);
	TIMER_CALLBACK_MEMBER(samcoupe_mouse_reset);
	TIMER_CALLBACK_MEMBER(sam_video_update_callback);
	DECLARE_READ8_MEMBER(samcoupe_lpt1_busy_r);
	DECLARE_WRITE8_MEMBER(samcoupe_lpt1_strobe_w);
	DECLARE_READ8_MEMBER(samcoupe_lpt2_busy_r);
	DECLARE_WRITE8_MEMBER(samcoupe_lpt2_strobe_w);
	DECLARE_READ8_MEMBER(samcoupe_rtc_r);
	DECLARE_WRITE8_MEMBER(samcoupe_rtc_w);

	DECLARE_READ8_MEMBER(sam_bank1_r);
	DECLARE_WRITE8_MEMBER(sam_bank1_w);
	DECLARE_READ8_MEMBER(sam_bank2_r);
	DECLARE_WRITE8_MEMBER(sam_bank2_w);
	DECLARE_READ8_MEMBER(sam_bank3_r);
	DECLARE_WRITE8_MEMBER(sam_bank3_w);
	DECLARE_READ8_MEMBER(sam_bank4_r);
	DECLARE_WRITE8_MEMBER(sam_bank4_w);

	UINT8* sam_bank_read_ptr[4];
	UINT8* sam_bank_write_ptr[4];
	DECLARE_FLOPPY_FORMATS( floppy_formats );
};


/*----------- defined in drivers/samcoupe.c -----------*/

void samcoupe_irq(device_t *device, UINT8 src);


/*----------- defined in machine/samcoupe.c -----------*/

void samcoupe_update_memory(address_space &space);
UINT8 samcoupe_mouse_r(running_machine &machine);

#endif /* SAMCOUPE_H_ */
