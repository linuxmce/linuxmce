/***************************************************************************

    coco_232.c

    Code for emulating the CoCo RS-232 PAK

***************************************************************************/

#include "emu.h"
#include "coco_232.h"


/***************************************************************************
    CONSTANTS
***************************************************************************/

#define UART_TAG        "uart"

/***************************************************************************
    IMPLEMENTATION
***************************************************************************/

static MACHINE_CONFIG_FRAGMENT(coco_rs232)
	MCFG_ACIA6551_ADD(UART_TAG)
MACHINE_CONFIG_END

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type COCO_232 = &device_creator<coco_232_device>;

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  coco_232_device - constructor
//-------------------------------------------------

coco_232_device::coco_232_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
		: device_t(mconfig, COCO_232, "CoCo RS-232 PAK", tag, owner, clock),
		device_cococart_interface( mconfig, *this ),
		m_uart(*this, UART_TAG)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void coco_232_device::device_start()
{
}

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor coco_232_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( coco_rs232 );
}

/*-------------------------------------------------
    read
-------------------------------------------------*/

READ8_MEMBER(coco_232_device::read)
{
	UINT8 result = 0x00;

	if ((offset >= 0x28) && (offset <= 0x2F))
		result = m_uart->read(space, offset - 0x28);

	return result;
}


/*-------------------------------------------------
    write
-------------------------------------------------*/

WRITE8_MEMBER(coco_232_device::write)
{
	if ((offset >= 0x28) && (offset <= 0x2F))
		m_uart->write(space, offset - 0x28, data);
}
