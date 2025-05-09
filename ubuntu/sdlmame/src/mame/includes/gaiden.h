/***************************************************************************

    Ninja Gaiden

***************************************************************************/

class gaiden_state : public driver_device
{
public:
	gaiden_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_videoram(*this, "videoram"),
		m_videoram2(*this, "videoram2"),
		m_videoram3(*this, "videoram3"),
		m_spriteram(*this, "spriteram"){ }

	/* memory pointers */
	required_shared_ptr<UINT16> m_videoram;
	required_shared_ptr<UINT16> m_videoram2;
	required_shared_ptr<UINT16> m_videoram3;
	required_shared_ptr<UINT16> m_spriteram;

	/* video-related */
	tilemap_t   *m_text_layer;
	tilemap_t   *m_foreground;
	tilemap_t   *m_background;
	bitmap_ind16 m_sprite_bitmap;
	bitmap_ind16 m_tile_bitmap_bg;
	bitmap_ind16 m_tile_bitmap_fg;
	UINT16      m_tx_scroll_x;
	UINT16      m_tx_scroll_y;
	UINT16      m_bg_scroll_x;
	UINT16      m_bg_scroll_y;
	UINT16      m_fg_scroll_x;
	UINT16      m_fg_scroll_y;
	INT8        m_tx_offset_y;
	INT8        m_bg_offset_y;
	INT8        m_fg_offset_y;
	INT8        m_spr_offset_y;

	/* misc */
	int         m_sprite_sizey;
	int         m_prot;
	int         m_jumpcode;
	const int   *m_raiga_jumppoints;

	/* devices */
	cpu_device *m_audiocpu;
	DECLARE_WRITE16_MEMBER(gaiden_sound_command_w);
	DECLARE_WRITE16_MEMBER(drgnbowl_sound_command_w);
	DECLARE_WRITE16_MEMBER(wildfang_protection_w);
	DECLARE_READ16_MEMBER(wildfang_protection_r);
	DECLARE_WRITE16_MEMBER(raiga_protection_w);
	DECLARE_READ16_MEMBER(raiga_protection_r);
	DECLARE_WRITE16_MEMBER(gaiden_flip_w);
	DECLARE_WRITE16_MEMBER(gaiden_txscrollx_w);
	DECLARE_WRITE16_MEMBER(gaiden_txscrolly_w);
	DECLARE_WRITE16_MEMBER(gaiden_fgscrollx_w);
	DECLARE_WRITE16_MEMBER(gaiden_fgscrolly_w);
	DECLARE_WRITE16_MEMBER(gaiden_bgscrollx_w);
	DECLARE_WRITE16_MEMBER(gaiden_bgscrolly_w);
	DECLARE_WRITE16_MEMBER(gaiden_txoffsety_w);
	DECLARE_WRITE16_MEMBER(gaiden_fgoffsety_w);
	DECLARE_WRITE16_MEMBER(gaiden_bgoffsety_w);
	DECLARE_WRITE16_MEMBER(gaiden_sproffsety_w);
	DECLARE_WRITE16_MEMBER(gaiden_videoram3_w);
	DECLARE_READ16_MEMBER(gaiden_videoram3_r);
	DECLARE_WRITE16_MEMBER(gaiden_videoram2_w);
	DECLARE_READ16_MEMBER(gaiden_videoram2_r);
	DECLARE_WRITE16_MEMBER(gaiden_videoram_w);
	DECLARE_DRIVER_INIT(raiga);
	DECLARE_DRIVER_INIT(drgnbowl);
	DECLARE_DRIVER_INIT(mastninj);
	DECLARE_DRIVER_INIT(shadoww);
	DECLARE_DRIVER_INIT(wildfang);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info_raiga);
	TILE_GET_INFO_MEMBER(get_tx_tile_info);
	DECLARE_MACHINE_START(raiga);
	DECLARE_MACHINE_RESET(raiga);
	DECLARE_VIDEO_START(gaiden);
	DECLARE_VIDEO_START(drgnbowl);
	DECLARE_VIDEO_START(mastninj);
	DECLARE_VIDEO_START(raiga);
	UINT32 screen_update_gaiden(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_drgnbowl(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_raiga(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
};
