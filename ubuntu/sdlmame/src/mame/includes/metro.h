/*************************************************************************

    Metro Games

*************************************************************************/

#include "sound/okim6295.h"
#include "sound/2151intf.h"
#include "video/konicdev.h"

class metro_state : public driver_device
{
public:
	metro_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_oki(*this, "oki"),
		m_ymsnd(*this, "ymsnd"),
		m_k053936(*this, "k053936") ,
		m_vram_0(*this, "vram_0"),
		m_vram_1(*this, "vram_1"),
		m_vram_2(*this, "vram_2"),
		m_spriteram(*this, "spriteram"),
		m_tiletable(*this, "tiletable"),
		m_blitter_regs(*this, "blitter_regs"),
		m_scroll(*this, "scroll"),
		m_window(*this, "window"),
		m_irq_enable(*this, "irq_enable"),
		m_irq_levels(*this, "irq_levels"),
		m_irq_vectors(*this, "irq_vectors"),
		m_rombank(*this, "rombank"),
		m_videoregs(*this, "videoregs"),
		m_screenctrl(*this, "screenctrl"),
		m_input_sel(*this, "input_sel"),
		m_k053936_ram(*this, "k053936_ram")
	{ }

	/* devices */
	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_audiocpu;
	optional_device<okim6295_device> m_oki;
	optional_device<device_t> m_ymsnd; // TODO set correct type
	optional_device<k053936_device> m_k053936;
	/* memory pointers */
	optional_shared_ptr<UINT16> m_vram_0;
	optional_shared_ptr<UINT16> m_vram_1;
	optional_shared_ptr<UINT16> m_vram_2;
	required_shared_ptr<UINT16> m_spriteram;
	optional_shared_ptr<UINT16> m_tiletable;
	UINT16 *    m_tiletable_old;
	optional_shared_ptr<UINT16> m_blitter_regs;
	optional_shared_ptr<UINT16> m_scroll;
	optional_shared_ptr<UINT16> m_window;
	optional_shared_ptr<UINT16> m_irq_enable;
	optional_shared_ptr<UINT16> m_irq_levels;
	optional_shared_ptr<UINT16> m_irq_vectors;
	optional_shared_ptr<UINT16> m_rombank;
	required_shared_ptr<UINT16> m_videoregs;
	optional_shared_ptr<UINT16> m_screenctrl;
	optional_shared_ptr<UINT16> m_input_sel;
	optional_shared_ptr<UINT16> m_k053936_ram;


	int         m_flip_screen;

	/* video-related */
	tilemap_t   *m_k053936_tilemap;
	int         m_tilemap_scrolldx[3];

	int         m_support_8bpp;
	int         m_support_16x16;
	int         m_has_zoom;
	int         m_sprite_xoffs;
	int         m_sprite_yoffs;
	int         m_sprite_xoffs_dx;

	UINT8       *m_expanded_gfx1;

	/* irq_related */
	int         m_vblank_bit;
	int         m_blitter_bit;
	int         m_irq_line;
	UINT8       m_requested_int[8];
	emu_timer   *m_mouja_irq_timer;

	/* sound related */
	UINT16      m_soundstatus;
	int         m_porta;
	int         m_portb;
	int         m_busy_sndcpu;

	/* misc */
	int         m_gakusai_oki_bank_lo;
	int         m_gakusai_oki_bank_hi;


	DECLARE_READ16_MEMBER(metro_irq_cause_r);
	DECLARE_WRITE16_MEMBER(metro_irq_cause_w);
	DECLARE_WRITE16_MEMBER(mouja_irq_timer_ctrl_w);
	DECLARE_WRITE16_MEMBER(metro_soundlatch_w);
	DECLARE_READ16_MEMBER(metro_soundstatus_r);
	DECLARE_WRITE16_MEMBER(metro_soundstatus_w);
	DECLARE_WRITE8_MEMBER(metro_sound_rombank_w);
	DECLARE_WRITE8_MEMBER(daitorid_sound_rombank_w);
	DECLARE_READ8_MEMBER(metro_porta_r);
	DECLARE_WRITE8_MEMBER(metro_porta_w);
	DECLARE_WRITE8_MEMBER(metro_portb_w);
	DECLARE_WRITE8_MEMBER(daitorid_portb_w);
	DECLARE_WRITE16_MEMBER(metro_coin_lockout_1word_w);
	DECLARE_WRITE16_MEMBER(metro_coin_lockout_4words_w);
	DECLARE_READ16_MEMBER(metro_bankedrom_r);
	DECLARE_WRITE16_MEMBER(metro_blitter_w);
	DECLARE_READ16_MEMBER(balcube_dsw_r);
	DECLARE_READ16_MEMBER(karatour_vram_0_r);
	DECLARE_READ16_MEMBER(karatour_vram_1_r);
	DECLARE_READ16_MEMBER(karatour_vram_2_r);
	DECLARE_WRITE16_MEMBER(karatour_vram_0_w);
	DECLARE_WRITE16_MEMBER(karatour_vram_1_w);
	DECLARE_WRITE16_MEMBER(karatour_vram_2_w);
	DECLARE_READ16_MEMBER(gakusai_input_r);
	DECLARE_WRITE16_MEMBER(blzntrnd_sound_w);
	DECLARE_WRITE8_MEMBER(blzntrnd_sh_bankswitch_w);
	DECLARE_WRITE16_MEMBER(puzzlet_irq_enable_w);
	DECLARE_WRITE16_MEMBER(vram_0_clr_w);
	DECLARE_WRITE16_MEMBER(vram_1_clr_w);
	DECLARE_WRITE16_MEMBER(vram_2_clr_w);
	DECLARE_WRITE8_MEMBER(puzzlet_portb_w);
	DECLARE_WRITE16_MEMBER(metro_k053936_w);
	DECLARE_WRITE16_MEMBER(metro_vram_0_w);
	DECLARE_WRITE16_MEMBER(metro_vram_1_w);
	DECLARE_WRITE16_MEMBER(metro_vram_2_w);
	DECLARE_WRITE16_MEMBER(metro_window_w);
	void blt_write( address_space &space, const int tmap, const offs_t offs, const UINT16 data, const UINT16 mask );
	DECLARE_CUSTOM_INPUT_MEMBER(custom_soundstatus_r);
	DECLARE_WRITE16_MEMBER(gakusai_oki_bank_hi_w);
	DECLARE_WRITE16_MEMBER(gakusai_oki_bank_lo_w);
	DECLARE_READ16_MEMBER(gakusai_eeprom_r);
	DECLARE_WRITE16_MEMBER(gakusai_eeprom_w);
	DECLARE_READ16_MEMBER(dokyusp_eeprom_r);
	DECLARE_WRITE16_MEMBER(dokyusp_eeprom_bit_w);
	DECLARE_WRITE16_MEMBER(dokyusp_eeprom_reset_w);
	DECLARE_WRITE16_MEMBER(mouja_sound_rombank_w);

	// vmetal
	DECLARE_WRITE8_MEMBER(vmetal_control_w);
	DECLARE_WRITE8_MEMBER(vmetal_es8712_w);

	DECLARE_DRIVER_INIT(karatour);
	DECLARE_DRIVER_INIT(daitorid);
	DECLARE_DRIVER_INIT(blzntrnd);
	DECLARE_DRIVER_INIT(mouja);
	DECLARE_DRIVER_INIT(balcube);
	DECLARE_DRIVER_INIT(gakusai);
	DECLARE_DRIVER_INIT(dharmak);
	DECLARE_DRIVER_INIT(puzzlet);
	DECLARE_DRIVER_INIT(metro);
	TILE_GET_INFO_MEMBER(metro_k053936_get_tile_info);
	TILE_GET_INFO_MEMBER(metro_k053936_gstrik2_get_tile_info);
	TILEMAP_MAPPER_MEMBER(tilemap_scan_gstrik2);
	DECLARE_MACHINE_START(metro);
	DECLARE_MACHINE_RESET(metro);
	void expand_gfx1();
	DECLARE_VIDEO_START(metro_i4100);
	DECLARE_VIDEO_START(metro_i4220);
	DECLARE_VIDEO_START(metro_i4220_dx_tmap);
	DECLARE_VIDEO_START(metro_i4220_dx_sprite);
	DECLARE_VIDEO_START(metro_i4300);
	DECLARE_VIDEO_START(blzntrnd);
	DECLARE_VIDEO_START(gstrik2);
	UINT32 screen_update_metro(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(metro_vblank_interrupt);
	INTERRUPT_GEN_MEMBER(metro_periodic_interrupt);
	INTERRUPT_GEN_MEMBER(karatour_interrupt);
	INTERRUPT_GEN_MEMBER(puzzlet_interrupt);
	TIMER_CALLBACK_MEMBER(karatour_irq_callback);
	TIMER_CALLBACK_MEMBER(mouja_irq_callback);
	TIMER_CALLBACK_MEMBER(metro_blit_done);
};


/*----------- defined in video/metro.c -----------*/
void metro_draw_sprites(running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect);
