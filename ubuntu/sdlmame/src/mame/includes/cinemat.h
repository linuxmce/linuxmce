/*************************************************************************

    Cinematronics vector hardware

*************************************************************************/


class cinemat_state : public driver_device
{
public:
	cinemat_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_rambase(*this, "rambase"){ }

	UINT8 m_sound_control;
	void (*m_sound_handler)(running_machine &,UINT8 sound_val, UINT8 bits_changed);
	UINT32 m_current_shift;
	UINT32 m_last_shift;
	UINT32 m_last_shift2;
	UINT32 m_current_pitch;
	UINT32 m_last_frame;
	UINT8 m_sound_fifo[16];
	UINT8 m_sound_fifo_in;
	UINT8 m_sound_fifo_out;
	UINT8 m_last_portb_write;
	float m_target_volume;
	float m_current_volume;
	optional_shared_ptr<UINT16> m_rambase;
	UINT8 m_coin_detected;
	UINT8 m_coin_last_reset;
	UINT8 m_mux_select;
	int m_gear;
	int m_color_mode;
	rgb_t m_vector_color;
	INT16 m_lastx;
	INT16 m_lasty;
	UINT8 m_last_control;
	int m_qb3_lastx;
	int m_qb3_lasty;
	DECLARE_READ8_MEMBER(inputs_r);
	DECLARE_READ8_MEMBER(switches_r);
	DECLARE_READ8_MEMBER(coin_input_r);
	DECLARE_WRITE8_MEMBER(coin_reset_w);
	DECLARE_WRITE8_MEMBER(mux_select_w);
	DECLARE_READ8_MEMBER(speedfrk_wheel_r);
	DECLARE_READ8_MEMBER(speedfrk_gear_r);
	DECLARE_READ8_MEMBER(sundance_inputs_r);
	DECLARE_READ8_MEMBER(boxingb_dial_r);
	DECLARE_READ8_MEMBER(qb3_frame_r);
	DECLARE_WRITE8_MEMBER(qb3_ram_bank_w);
	DECLARE_WRITE8_MEMBER(cinemat_vector_control_w);
	DECLARE_WRITE8_MEMBER(cinemat_sound_control_w);
	DECLARE_WRITE8_MEMBER(qb3_sound_w);
	DECLARE_INPUT_CHANGED_MEMBER(coin_inserted);
	DECLARE_DRIVER_INIT(speedfrk);
	DECLARE_DRIVER_INIT(boxingb);
	DECLARE_DRIVER_INIT(tailg);
	DECLARE_DRIVER_INIT(sundance);
	DECLARE_DRIVER_INIT(qb3);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	DECLARE_VIDEO_START(cinemat_16level);
	DECLARE_VIDEO_START(cinemat_64level);
	DECLARE_VIDEO_START(cinemat_color);
	DECLARE_VIDEO_START(cinemat_qb3color);
	UINT32 screen_update_cinemat(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_spacewar(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	DECLARE_READ8_MEMBER(sound_porta_r);
	DECLARE_READ8_MEMBER(sound_portb_r);
	DECLARE_WRITE8_MEMBER(sound_portb_w);
	DECLARE_WRITE8_MEMBER(sound_output_w);
};

/*----------- defined in audio/cinemat.c -----------*/
MACHINE_CONFIG_EXTERN( spacewar_sound );
MACHINE_CONFIG_EXTERN( barrier_sound );
MACHINE_CONFIG_EXTERN( speedfrk_sound );
MACHINE_CONFIG_EXTERN( starhawk_sound );
MACHINE_CONFIG_EXTERN( sundance_sound );
MACHINE_CONFIG_EXTERN( tailg_sound );
MACHINE_CONFIG_EXTERN( warrior_sound );
MACHINE_CONFIG_EXTERN( armora_sound );
MACHINE_CONFIG_EXTERN( ripoff_sound );
MACHINE_CONFIG_EXTERN( starcas_sound );
MACHINE_CONFIG_EXTERN( solarq_sound );
MACHINE_CONFIG_EXTERN( boxingb_sound );
MACHINE_CONFIG_EXTERN( wotw_sound );
MACHINE_CONFIG_EXTERN( wotwc_sound );
MACHINE_CONFIG_EXTERN( demon_sound );
MACHINE_CONFIG_EXTERN( qb3_sound );

/*----------- defined in video/cinemat.c -----------*/

void cinemat_vector_callback(device_t *device, INT16 sx, INT16 sy, INT16 ex, INT16 ey, UINT8 shift);
