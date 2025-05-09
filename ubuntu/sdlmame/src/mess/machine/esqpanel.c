/*
    Ensoniq panel/display device
*/
#include "emu.h"
#include "esqpanel.h"

//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type ESQPANEL1x22 = &device_creator<esqpanel1x22_device>;
const device_type ESQPANEL2x40 = &device_creator<esqpanel2x40_device>;
const device_type ESQPANEL2x40_SQ1 = &device_creator<esqpanel2x40_sq1_device>;

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  esqpanel_device - constructor
//-------------------------------------------------

esqpanel_device::esqpanel_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, type, name, tag, owner, clock),
	device_serial_interface(mconfig, *this)
{
}

void esqpanel_device::device_config_complete()
{
	m_shortname = "esqpanel";

	// inherit a copy of the static data
	const esqpanel_interface *intf = reinterpret_cast<const esqpanel_interface *>(static_config());
	if (intf != NULL)
	{
		*static_cast<esqpanel_interface *>(this) = *intf;
	}
	// or initialize to defaults if none provided
	else
	{
		memset(&m_out_tx_cb, 0, sizeof(m_out_tx_cb));
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void esqpanel_device::device_start()
{
	m_out_tx_func.resolve(m_out_tx_cb, *this);
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void esqpanel_device::device_reset()
{
	// panel comms is at 62500 baud (double the MIDI rate), 8N2
	set_rcv_rate(62500);
	set_tra_rate(62500);
	set_data_frame(8, 2, SERIAL_PARITY_NONE);

	m_tx_busy = false;
	m_xmit_read = m_xmit_write = 0;
	m_bCalibSecondByte = false;
}

void esqpanel_device::rcv_complete()    // Rx completed receiving byte
{
	receive_register_extract();
	UINT8 data = get_received_char();

//  if (data >= 0xe0) printf("Got %02x from motherboard (second %s)\n", data, m_bCalibSecondByte ? "yes" : "no");

	send_to_display(data);

	if (m_bCalibSecondByte)
	{
//      printf("second byte is %02x\n", data);
		if (data == 0xfd)   // calibration request
		{
//          printf("let's send reply!\n");
			xmit_char(0xff);   // this is the correct response for "calibration OK"
		}
		m_bCalibSecondByte = false;
	}
	else if (data == 0xfb)   // request calibration
	{
		m_bCalibSecondByte = true;
	}
	else
	{
		// EPS wants a throwaway reply byte for each byte sent to the KPC
		// VFX-SD and SD-1 definitely don't :)
		if (m_eps_mode)
		{
			if (data == 0xe7)
			{
				xmit_char(0x00);   // actual value of response is never checked
			}
			else if (data == 0x71)
			{
				xmit_char(0x00);   // actual value of response is never checked
			}
			else
			{
				xmit_char(data);   // actual value of response is never checked
			}
		}
	}
}

void esqpanel_device::tra_complete()    // Tx completed sending byte
{
//  printf("panel Tx complete\n");
	// is there more waiting to send?
	if (m_xmit_read != m_xmit_write)
	{
		transmit_register_setup(m_xmitring[m_xmit_read++]);
		if (m_xmit_read >= XMIT_RING_SIZE)
		{
			m_xmit_read = 0;
		}
	}
	else
	{
		m_tx_busy = false;
	}
}

void esqpanel_device::tra_callback()    // Tx send bit
{
	int bit = transmit_register_get_data_bit();
	m_out_tx_func(bit);
}

void esqpanel_device::input_callback(UINT8 state)
{
}

void esqpanel_device::xmit_char(UINT8 data)
{
//  printf("Panel: xmit %02x\n", data);

	// if tx is busy it'll pick this up automatically when it completes
	if (!m_tx_busy)
	{
		m_tx_busy = true;
		transmit_register_setup(data);
	}
	else
	{
		// tx is busy, it'll pick this up next time
		m_xmitring[m_xmit_write++] = data;
		if (m_xmit_write >= XMIT_RING_SIZE)
		{
			m_xmit_write = 0;
		}
	}
}

/* panel with 1x22 VFD display used in the EPS-16 and EPS-16 Plus */

static MACHINE_CONFIG_FRAGMENT(esqpanel1x22)
	MCFG_ESQ1x22_ADD("vfd")
MACHINE_CONFIG_END

machine_config_constructor esqpanel1x22_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( esqpanel1x22 );
}

esqpanel1x22_device::esqpanel1x22_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	esqpanel_device(mconfig, ESQPANEL1x22, "Ensoniq front panel with 1x22 VFD", tag, owner, clock),
	m_vfd(*this, "vfd")
{
	m_eps_mode = true;
}

/* panel with 2x40 VFD display used in the ESQ-1, VFX-SD, SD-1, and others */

static MACHINE_CONFIG_FRAGMENT(esqpanel2x40)
	MCFG_ESQ2x40_ADD("vfd")
MACHINE_CONFIG_END

machine_config_constructor esqpanel2x40_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( esqpanel2x40 );
}

esqpanel2x40_device::esqpanel2x40_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	esqpanel_device(mconfig, ESQPANEL2x40, "Ensoniq front panel with 2x40 VFD", tag, owner, clock),
	m_vfd(*this, "vfd")
{
	m_eps_mode = false;
}

/* panel with 2x16? LCD display used in the SQ and MR series, plus probably more */

static MACHINE_CONFIG_FRAGMENT(esqpanel2x40_sq1)
	MCFG_ESQ2x40_SQ1_ADD("vfd")
MACHINE_CONFIG_END

machine_config_constructor esqpanel2x40_sq1_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( esqpanel2x40_sq1 );
}

esqpanel2x40_sq1_device::esqpanel2x40_sq1_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	esqpanel_device(mconfig, ESQPANEL2x40, "Ensoniq front panel with 2x16 LCD", tag, owner, clock),
	m_vfd(*this, "vfd")
{
	m_eps_mode = false;
}
