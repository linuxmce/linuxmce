#include "emu.h"
#include "includes/hexion.h"




/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

INLINE void get_tile_info(running_machine &machine,tile_data &tileinfo,int tile_index,UINT8 *ram)
{
	tile_index *= 4;
	SET_TILE_INFO(
			0,
			ram[tile_index] + ((ram[tile_index+1] & 0x3f) << 8),
			ram[tile_index+2] & 0x0f,
			0);
}

TILE_GET_INFO_MEMBER(hexion_state::get_tile_info0)
{
	get_tile_info(machine(),tileinfo,tile_index,m_vram[0]);
}

TILE_GET_INFO_MEMBER(hexion_state::get_tile_info1)
{
	get_tile_info(machine(),tileinfo,tile_index,m_vram[1]);
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

void hexion_state::video_start()
{
	m_bg_tilemap[0] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(hexion_state::get_tile_info0),this),TILEMAP_SCAN_ROWS,8,8,64,32);
	m_bg_tilemap[1] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(hexion_state::get_tile_info1),this),TILEMAP_SCAN_ROWS,     8,8,64,32);

	m_bg_tilemap[0]->set_transparent_pen(0);
	m_bg_tilemap[1]->set_scrollx(0,-4);
	m_bg_tilemap[1]->set_scrolly(0,4);

	m_vram[0] = memregion("maincpu")->base() + 0x30000;
	m_vram[1] = m_vram[0] + 0x2000;
	m_unkram = m_vram[1] + 0x2000;
}



/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_MEMBER(hexion_state::hexion_bankswitch_w)
{
	UINT8 *rom = memregion("maincpu")->base() + 0x10000;

	/* bits 0-3 select ROM bank */
	membank("bank1")->set_base(rom + 0x2000 * (data & 0x0f));

	/* does bit 6 trigger the 052591? */
	if (data & 0x40)
	{
		int bank = m_unkram[0]&1;
		memset(m_vram[bank],m_unkram[1],0x2000);
		m_bg_tilemap[bank]->mark_all_dirty();
	}
	/* bit 7 = PMC-BK */
	m_pmcbank = (data & 0x80) >> 7;

	/* other bits unknown */
if (data & 0x30)
	popmessage("bankswitch %02x",data&0xf0);

//logerror("%04x: bankswitch_w %02x\n",space.device().safe_pc(),data);
}

READ8_MEMBER(hexion_state::hexion_bankedram_r)
{
	if (m_gfxrom_select && offset < 0x1000)
	{
		return memregion("gfx1")->base()[((m_gfxrom_select & 0x7f) << 12) + offset];
	}
	else if (m_bankctrl == 0)
	{
		return m_vram[m_rambank][offset];
	}
	else if (m_bankctrl == 2 && offset < 0x800)
	{
		return m_unkram[offset];
	}
	else
	{
//logerror("%04x: bankedram_r offset %04x, bankctrl = %02x\n",space.device().safe_pc(),offset,m_bankctrl);
		return 0;
	}
}

WRITE8_MEMBER(hexion_state::hexion_bankedram_w)
{
	if (m_bankctrl == 3 && offset == 0 && (data & 0xfe) == 0)
	{
//logerror("%04x: bankedram_w offset %04x, data %02x, bankctrl = %02x\n",space.device().safe_pc(),offset,data,m_bankctrl);
		m_rambank = data & 1;
	}
	else if (m_bankctrl == 0)
	{
		if (m_pmcbank)
		{
//logerror("%04x: bankedram_w offset %04x, data %02x, bankctrl = %02x\n",space.device().safe_pc(),offset,data,m_bankctrl);
			m_vram[m_rambank][offset] = data;
			m_bg_tilemap[m_rambank]->mark_tile_dirty(offset/4);
		}
		else
			logerror("%04x pmc internal ram %04x = %02x\n",space.device().safe_pc(),offset,data);
	}
	else if (m_bankctrl == 2 && offset < 0x800)
	{
		if (m_pmcbank)
		{
//logerror("%04x: unkram_w offset %04x, data %02x, bankctrl = %02x\n",space.device().safe_pc(),offset,data,m_bankctrl);
			m_unkram[offset] = data;
		}
		else
			logerror("%04x pmc internal ram %04x = %02x\n",space.device().safe_pc(),offset,data);
	}
	else
logerror("%04x: bankedram_w offset %04x, data %02x, bankctrl = %02x\n",space.device().safe_pc(),offset,data,m_bankctrl);
}

WRITE8_MEMBER(hexion_state::hexion_bankctrl_w)
{
//logerror("%04x: bankctrl_w %02x\n",space.device().safe_pc(),data);
	m_bankctrl = data;
}

WRITE8_MEMBER(hexion_state::hexion_gfxrom_select_w)
{
//logerror("%04x: gfxrom_select_w %02x\n",space.device().safe_pc(),data);
	m_gfxrom_select = data;
}



/***************************************************************************

  Display refresh

***************************************************************************/

UINT32 hexion_state::screen_update_hexion(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_bg_tilemap[1]->draw(bitmap, cliprect, 0,0);
	m_bg_tilemap[0]->draw(bitmap, cliprect, 0,0);
	return 0;
}
