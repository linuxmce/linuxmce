/*****************************************************************************
 *
 * includes/p2000t.h
 *
 ****************************************************************************/

#ifndef P2000T_H_
#define P2000T_H_

#include "emu.h"
#include "cpu/z80/z80.h"
#include "sound/speaker.h"
#include "video/saa5050.h"


class p2000t_state : public driver_device
{
public:
	p2000t_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_speaker(*this, SPEAKER_TAG),
			m_videoram(*this, "videoram")
	{ }

	required_device<cpu_device> m_maincpu;
	required_device<speaker_sound_device> m_speaker;
	DECLARE_READ8_MEMBER(p2000t_port_000f_r);
	DECLARE_READ8_MEMBER(p2000t_port_202f_r);
	DECLARE_WRITE8_MEMBER(p2000t_port_101f_w);
	DECLARE_WRITE8_MEMBER(p2000t_port_303f_w);
	DECLARE_WRITE8_MEMBER(p2000t_port_505f_w);
	DECLARE_WRITE8_MEMBER(p2000t_port_707f_w);
	DECLARE_WRITE8_MEMBER(p2000t_port_888b_w);
	DECLARE_WRITE8_MEMBER(p2000t_port_8c90_w);
	DECLARE_WRITE8_MEMBER(p2000t_port_9494_w);
	DECLARE_READ8_MEMBER(videoram_r);
	required_shared_ptr<UINT8> m_videoram;
	UINT8 m_port_101f;
	UINT8 m_port_202f;
	UINT8 m_port_303f;
	UINT8 m_port_707f;
	INT8 m_frame_count;
	DECLARE_VIDEO_START(p2000m);
	DECLARE_PALETTE_INIT(p2000m);
	UINT32 screen_update_p2000m(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(p2000_interrupt);
};

#endif /* P2000T_H_ */
