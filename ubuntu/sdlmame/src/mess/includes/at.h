/*****************************************************************************
 *
 * includes/at.h
 *
 * IBM AT Compatibles
 *
 ****************************************************************************/

#ifndef AT_H_
#define AT_H_

#include "emu.h"
#include "cpu/i86/i86.h"
#include "cpu/i86/i286.h"
#include "cpu/i386/i386.h"

#include "machine/ins8250.h"
#include "machine/mc146818.h"
#include "machine/pic8259.h"
#include "machine/i82371ab.h"
#include "machine/i82371sb.h"
#include "machine/i82439tx.h"
#include "machine/cs4031.h"
#include "machine/cs8221.h"
#include "machine/pit8253.h"
#include "video/pc_cga.h"
#include "video/isa_cga.h"
#include "video/isa_ega.h"
#include "video/isa_vga.h"
#include "video/isa_vga_ati.h"
#include "video/isa_svga_cirrus.h"
#include "video/isa_svga_s3.h"
#include "video/isa_svga_tseng.h"

#include "machine/idectrl.h"
#include "machine/isa_aha1542.h"
#include "machine/at_keybc.h"
#include "includes/ps2.h"

#include "imagedev/harddriv.h"
#include "machine/am9517a.h"
#include "machine/pci.h"
#include "machine/kb_keytro.h"

#include "sound/dac.h"
#include "sound/speaker.h"
#include "machine/ram.h"
#include "machine/nvram.h"
#include "machine/isa.h"

#include "machine/isa_adlib.h"
#include "machine/isa_com.h"
#include "machine/isa_fdc.h"
#include "machine/isa_gblaster.h"
#include "machine/isa_hdc.h"
#include "machine/isa_sblaster.h"
#include "machine/isa_stereo_fx.h"
#include "machine/isa_gus.h"
#include "machine/isa_ssi2001.h"
#include "machine/3c503.h"
#include "machine/ne1000.h"
#include "machine/ne2000.h"
#include "video/isa_mda.h"
#include "machine/isa_mpu401.h"
#include "machine/isa_ibm_mfc.h"

#include "machine/isa_ide.h"
#include "machine/isa_ide_cd.h"

#include "machine/pc_lpt.h"
#include "machine/pc_kbdc.h"


class at586_state : public driver_device
{
public:
	at586_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }
};

class at_state : public driver_device
{
public:
	at_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
	m_maincpu(*this, "maincpu"),
	m_pic8259_master(*this, "pic8259_master"),
	m_pic8259_slave(*this, "pic8259_slave"),
	m_dma8237_1(*this, "dma8237_1"),
	m_dma8237_2(*this, "dma8237_2"),
	m_pit8254(*this, "pit8254"),
	m_cs8221(*this, "cs8221"),
	m_cs4031(*this, "cs4031"),
	m_ide(*this, "ide"),
	m_keybc(*this, "keybc"),
	m_isabus(*this, "isabus"),
	m_speaker(*this, SPEAKER_TAG),
	m_ram(*this, RAM_TAG),
	m_mc146818(*this, "rtc"),
	m_pc_kbdc(*this, "pc_kbdc")
	{ }

	required_device<cpu_device> m_maincpu;
	optional_device<pic8259_device> m_pic8259_master;
	optional_device<pic8259_device> m_pic8259_slave;
	optional_device<am9517a_device> m_dma8237_1;
	optional_device<am9517a_device> m_dma8237_2;
	optional_device<pit8254_device> m_pit8254;
	optional_device<cs8221_device> m_cs8221;
	optional_device<cs4031_device> m_cs4031;
	optional_device<ide_controller_device> m_ide;
	optional_device<at_keyboard_controller_device> m_keybc;
	optional_device<isa16_device> m_isabus;
	required_device<speaker_sound_device> m_speaker;
	optional_device<ram_device> m_ram;
	optional_device<mc146818_device> m_mc146818;
	optional_device<pc_kbdc_device> m_pc_kbdc;
	DECLARE_READ8_MEMBER(at_page8_r);
	DECLARE_WRITE8_MEMBER(at_page8_w);
	DECLARE_READ8_MEMBER(at_portb_r);
	DECLARE_WRITE8_MEMBER(at_portb_w);
	DECLARE_READ8_MEMBER(get_slave_ack);
	DECLARE_WRITE_LINE_MEMBER(at_pit8254_out0_changed);
	DECLARE_WRITE_LINE_MEMBER(at_pit8254_out2_changed);
	DECLARE_WRITE_LINE_MEMBER(pc_dma_hrq_changed);
	DECLARE_READ8_MEMBER(pc_dma8237_0_dack_r);
	DECLARE_READ8_MEMBER(pc_dma8237_1_dack_r);
	DECLARE_READ8_MEMBER(pc_dma8237_2_dack_r);
	DECLARE_READ8_MEMBER(pc_dma8237_3_dack_r);
	DECLARE_READ8_MEMBER(pc_dma8237_5_dack_r);
	DECLARE_READ8_MEMBER(pc_dma8237_6_dack_r);
	DECLARE_READ8_MEMBER(pc_dma8237_7_dack_r);
	DECLARE_WRITE8_MEMBER(pc_dma8237_0_dack_w);
	DECLARE_WRITE8_MEMBER(pc_dma8237_1_dack_w);
	DECLARE_WRITE8_MEMBER(pc_dma8237_2_dack_w);
	DECLARE_WRITE8_MEMBER(pc_dma8237_3_dack_w);
	DECLARE_WRITE8_MEMBER(pc_dma8237_5_dack_w);
	DECLARE_WRITE8_MEMBER(pc_dma8237_6_dack_w);
	DECLARE_WRITE8_MEMBER(pc_dma8237_7_dack_w);
	DECLARE_WRITE_LINE_MEMBER(at_dma8237_out_eop);
	DECLARE_WRITE_LINE_MEMBER(pc_dack0_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack1_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack2_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack3_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack4_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack5_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack6_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack7_w);
	DECLARE_READ32_MEMBER(ide_r);
	DECLARE_WRITE32_MEMBER(ide_w);
	DECLARE_READ8_MEMBER(at_dma8237_2_r);
	DECLARE_WRITE8_MEMBER(at_dma8237_2_w);
	DECLARE_READ8_MEMBER(at_keybc_r);
	DECLARE_WRITE8_MEMBER(at_keybc_w);
	DECLARE_READ16_MEMBER(neat_chipset_r);
	DECLARE_WRITE16_MEMBER(neat_chipset_w);
	DECLARE_READ32_MEMBER(ct486_chipset_r);
	DECLARE_WRITE32_MEMBER(ct486_chipset_w);
	DECLARE_WRITE_LINE_MEMBER(at_mc146818_irq);
	DECLARE_WRITE8_MEMBER(write_rtc);
	int m_poll_delay;
	UINT8 m_at_spkrdata;
	UINT8 m_at_speaker_input;
	int m_dma_channel;
	bool m_cur_eop;
	UINT8 m_dma_offset[2][4];
	UINT8 m_at_pages[0x10];
	UINT16 m_dma_high_byte;
	UINT8 m_at_speaker;
	UINT8 m_at_offset1;
	void at_speaker_set_spkrdata(UINT8 data);
	void at_speaker_set_input(UINT8 data);

	UINT8 m_channel_check;
	UINT8 m_nmi_enabled;
	DECLARE_READ8_MEMBER(pc_dma_read_byte);
	DECLARE_WRITE8_MEMBER(pc_dma_write_byte);
	DECLARE_READ8_MEMBER(pc_dma_read_word);
	DECLARE_WRITE8_MEMBER(pc_dma_write_word);

	DECLARE_DRIVER_INIT(atcga);
	DECLARE_DRIVER_INIT(atvga);
	DECLARE_MACHINE_START(at);
	DECLARE_MACHINE_RESET(at);
	void pc_set_dma_channel(int channel, int state);
};


/*----------- defined in machine/at.c -----------*/

extern const struct pic8259_interface at_pic8259_master_config;
extern const struct pic8259_interface at_pic8259_slave_config;
extern const struct pit8253_config at_pit8254_config;
extern const am9517a_interface at_dma8237_1_config;
extern const am9517a_interface at_dma8237_2_config;

#endif /* AT_H_ */
