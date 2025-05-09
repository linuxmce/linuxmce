/*************************************************************************

    D-Day

*************************************************************************/


class dday_state : public driver_device
{
public:
	dday_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_textvideoram(*this, "textvideoram"),
		m_fgvideoram(*this, "fgvideoram"),
		m_bgvideoram(*this, "bgvideoram"),
		m_colorram(*this, "colorram"){ }

	/* memory pointers */
	required_shared_ptr<UINT8> m_textvideoram;
	required_shared_ptr<UINT8> m_fgvideoram;
	required_shared_ptr<UINT8> m_bgvideoram;
	required_shared_ptr<UINT8> m_colorram;

	/* video-related */
	tilemap_t        *m_fg_tilemap;
	tilemap_t        *m_bg_tilemap;
	tilemap_t        *m_text_tilemap;
	tilemap_t        *m_sl_tilemap;
	bitmap_ind16 m_main_bitmap;
	int            m_control;
	int            m_sl_image;
	int            m_sl_enable;
	int            m_timer_value;

	/* devices */
	device_t *m_ay1;
	DECLARE_READ8_MEMBER(dday_countdown_timer_r);
	DECLARE_WRITE8_MEMBER(dday_bgvideoram_w);
	DECLARE_WRITE8_MEMBER(dday_fgvideoram_w);
	DECLARE_WRITE8_MEMBER(dday_textvideoram_w);
	DECLARE_WRITE8_MEMBER(dday_colorram_w);
	DECLARE_READ8_MEMBER(dday_colorram_r);
	DECLARE_WRITE8_MEMBER(dday_sl_control_w);
	DECLARE_WRITE8_MEMBER(dday_control_w);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_text_tile_info);
	TILE_GET_INFO_MEMBER(get_sl_tile_info);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	virtual void palette_init();
	UINT32 screen_update_dday(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(countdown_timer_callback);
};
