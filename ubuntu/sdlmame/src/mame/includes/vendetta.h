/*************************************************************************

    Vendetta

*************************************************************************/

class vendetta_state : public driver_device
{
public:
	vendetta_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	/* memory pointers */
	UINT8 *    m_ram;
//  UINT8 *    m_paletteram;    // currently this uses generic palette handling

	/* video-related */
	int        m_layer_colorbase[3];
	int        m_sprite_colorbase;
	int        m_layerpri[3];

	/* misc */
	int        m_irq_enabled;
	offs_t     m_video_banking_base;

	/* devices */
	cpu_device *m_maincpu;
	cpu_device *m_audiocpu;
	device_t *m_k053260;
	device_t *m_k052109;
	device_t *m_k053246;
	device_t *m_k053251;
	device_t *m_k054000;
	DECLARE_WRITE8_MEMBER(vendetta_eeprom_w);
	DECLARE_READ8_MEMBER(vendetta_K052109_r);
	DECLARE_WRITE8_MEMBER(vendetta_K052109_w);
	DECLARE_WRITE8_MEMBER(vendetta_5fe0_w);
	DECLARE_WRITE8_MEMBER(z80_arm_nmi_w);
	DECLARE_WRITE8_MEMBER(z80_irq_w);
	DECLARE_READ8_MEMBER(vendetta_sound_interrupt_r);
	DECLARE_READ8_MEMBER(vendetta_sound_r);
	DECLARE_DRIVER_INIT(vendetta);
	DECLARE_DRIVER_INIT(esckids);
	virtual void machine_start();
	virtual void machine_reset();
	UINT32 screen_update_vendetta(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(vendetta_irq);
	TIMER_CALLBACK_MEMBER(z80_nmi_callback);
};

/*----------- defined in video/vendetta.c -----------*/

extern void vendetta_tile_callback(running_machine &machine, int layer,int bank,int *code,int *color,int *flags,int *priority);
extern void esckids_tile_callback(running_machine &machine, int layer,int bank,int *code,int *color,int *flags,int *priority);
extern void vendetta_sprite_callback(running_machine &machine, int *code,int *color,int *priority_mask);
