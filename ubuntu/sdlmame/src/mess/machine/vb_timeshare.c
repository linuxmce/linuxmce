/**********************************************************************

    VideoBrain Timeshare cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#include "vb_timeshare.h"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type VB_TIMESHARE = &device_creator<videobrain_timeshare_cartridge_device>;



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  videobrain_timeshare_cartridge_device - constructor
//-------------------------------------------------

videobrain_timeshare_cartridge_device::videobrain_timeshare_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, VB_TIMESHARE, "VideoBrain Timeshare cartridge", tag, owner, clock),
	device_videobrain_expansion_card_interface(mconfig, *this)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void videobrain_timeshare_cartridge_device::device_start()
{
}


//-------------------------------------------------
//  videobrain_bo_r - cartridge data read
//-------------------------------------------------

UINT8 videobrain_timeshare_cartridge_device::videobrain_bo_r(address_space &space, offs_t offset, int cs1, int cs2)
{
	UINT8 data = 0;

	if (!cs1)
	{
		data = m_rom[offset & m_rom_mask];
	}
	else if (!cs2)
	{
		data = m_ram[offset & m_ram_mask];
	}
	return data;
}


//-------------------------------------------------
//  videobrain_bo_w - cartridge data write
//-------------------------------------------------

void videobrain_timeshare_cartridge_device::videobrain_bo_w(address_space &space, offs_t offset, UINT8 data, int cs1, int cs2)
{
	if (!cs2)
	{
		m_ram[offset & m_ram_mask] = data;
	}
}
