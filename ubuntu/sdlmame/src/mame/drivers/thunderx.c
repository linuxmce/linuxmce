/***************************************************************************

    Super Contra / Thunder Cross

    driver by Bryan McPhail, Manuel Abadia

    K052591 emulation by Eddie Edwards

- There was a set in MAME at one time that was given the setname (thndrxja)
  which is supposedly a later revision of the japanese set currently in MAME.
  No roms were ever sourced for this set, so the GAME macro no longer exists.

***************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "cpu/konami/konami.h" /* for the callback and the firq irq definition */
#include "video/konicdev.h"
#include "sound/2151intf.h"
#include "sound/k007232.h"
#include "includes/konamipt.h"
#include "includes/thunderx.h"

static KONAMI_SETLINES_CALLBACK( thunderx_banking );

/***************************************************************************/

INTERRUPT_GEN_MEMBER(thunderx_state::scontra_interrupt)
{

	if (k052109_is_irq_enabled(m_k052109))
		device.execute().set_input_line(KONAMI_IRQ_LINE, HOLD_LINE);
}

TIMER_CALLBACK_MEMBER(thunderx_state::thunderx_firq_callback)
{
	m_maincpu->set_input_line(KONAMI_FIRQ_LINE, HOLD_LINE);
}

READ8_MEMBER(thunderx_state::scontra_bankedram_r)
{

	if (m_palette_selected)
		return m_generic_paletteram_8[offset];
	else
		return m_ram[offset];
}

WRITE8_MEMBER(thunderx_state::scontra_bankedram_w)
{

	if (m_palette_selected)
		paletteram_xBBBBBGGGGGRRRRR_byte_be_w(space, offset, data);
	else
		m_ram[offset] = data;
}

READ8_MEMBER(thunderx_state::thunderx_bankedram_r)
{

	if (m_rambank & 0x01)
		return m_ram[offset];
	else if (m_rambank & 0x10)
	{
		if (m_pmcbank)
		{
//          logerror("%04x read pmcram %04x\n",space.device().safe_pc(),offset);
			return m_pmcram[offset];
		}
		else
		{
			logerror("%04x read pmc internal ram %04x\n",space.device().safe_pc(),offset);
			return 0;
		}
	}
	else
		return m_generic_paletteram_8[offset];
}

WRITE8_MEMBER(thunderx_state::thunderx_bankedram_w)
{

	if (m_rambank & 0x01)
		m_ram[offset] = data;
	else if (m_rambank & 0x10)
	{
		if (m_pmcbank)
		{
			logerror("%04x pmcram %04x = %02x\n",space.device().safe_pc(),offset,data);
			m_pmcram[offset] = data;
		}
		else
			logerror("%04x pmc internal ram %04x = %02x\n",space.device().safe_pc(),offset,data);
	}
	else
		paletteram_xBBBBBGGGGGRRRRR_byte_be_w(space, offset, data);
}

/*
this is the data written to internal ram on startup:

    Japan version   US version
00: e7 00 00 ad 08  e7 00 00 ad 08
01: 5f 80 05 a0 0c  1f 80 05 a0 0c                                                      LDW ACC,RAM+05
02:                 42 7e 00 8b 04                                                      regE
03: df 00 e2 8b 08  df 8e 00 cb 04                                                      regE
04: 5f 80 06 a0 0c  5f 80 07 a0 0c                                                      LDB ACC,RAM+07
05: df 7e 00 cb 08  df 7e 00 cb 08                                                      LDB R7,[Rx]
06: 1b 80 00 a0 0c  1b 80 00 a0 0c  LDPTR #0                                             PTR2,RAM+00
07: df 10 00 cb 08  df 10 00 cb 08  LDB R1,[PTR] (fl)                                   LDB R1,[Rx] (flags)
08: 5f 80 03 a0 0c  5f 80 03 a0 0c  LDB R0,[3] (cm)                                     LDB ACC,RAM+03    load collide mask
09: 1f 20 00 cb 08  1f 20 00 cb 08  LD CMP2,R0                                          test (AND) R1 vs ACC
0a: c4 00 00 ab 0c  c4 00 00 ab 0c  INC PTR                                             LEA Rx,[++PTR2]
0b: df 20 00 cb 08  df 20 00 cb 08  LDB R2,[PTR] (w)                                    LDB R2,[Rx] (width)
0c: c4 00 00 ab 0c  c4 00 00 ab 0c  INC PTR                                             LEA Rx,[++PTR2]
0d: df 30 00 cb 08  df 30 00 cb 08  LDB R3,[PTR] (h)                                    LDB R3,[Rx] (height)
0e: c4 00 00 ab 0c  c4 00 00 ab 0c  INC PTR                                             LEA Rx,[++PTR2]
0f: df 40 00 cb 08  df 40 00 cb 08  LDB R4,[PTR] (x)                                    LDB R4,[Rx] (x)
10: c4 00 00 ab 0c  c4 00 00 ab 0c  INC PTR                                             LEA Rx,[++PTR2]
11: df 50 00 cb 08  df 50 00 cb 08  LDB R5,[PTR] (y)                                    LDB R5,[Rx] (y)
12: 60 22 35 e9 08  60 22 36 e9 08  BANDZ CMP2,R1,36                                    R2/R1, BEQ 36
13: 44 0e 00 ab 08  44 0e 00 ab 08  MOVE PTR,INNER                                      LEA Rx,[PTR,0]    load flags
14: df 60 00 cb 08  df 60 00 cb 08  LDB R6,[PTR] (fl)                                   LDB R6,[Rx]
15: 5f 80 04 a0 0c  5f 80 04 a0 0c  LDB R0,[4] (hm)                                     LDB ACC,RAM+04    load hit mask
16: 1f 60 00 cb 08  1f 60 00 cb 08  LD CMP6,R0                                          test R6 and ACC (AND)
17: 60 6c 31 e9 08  60 6c 32 e9 08  BANDZ CMP6,R6,32                                    R6, BEQ 32
18: 45 8e 01 a0 0c  45 8e 01 a0 0c  LDB R0,[INNER+1]                                    LDB Ry,[PTR,1] (width)
19: c5 64 00 cb 08  c5 64 00 cb 08  ADD ACC,R0,R2                                       R6 = ADD Ry,R2
1a: 45 8e 03 a0 0c  45 8e 03 a0 0c  LDB R0,[INNER+3]                                    LDB Ry,[PTR,3] (x)
1b: 67 00 00 cb 0c  67 00 00 cb 0c  MOV CMP,R0                                          ??? DEC Ry
1c: 15 48 5d c9 0c  15 48 5e c9 0c  SUB CMP,R4 ; BGE 1E                                 SUB R4,Ry; Bcc 1E
1d: 12 00 00 eb 0c  12 00 00 eb 0c  NEG CMP                                             ??? NEG Ry
1e: 48 6c 71 e9 0c  48 6c 72 e9 0c  B (CMP > ACC) 32                                    R6, BLO 32
1f: 45 8e 02 a0 0c  45 8e 02 a0 0c  LDB R0,[INNER+2]                                    LDB Ry,[PTR,2] (height)
20: c5 66 00 cb 08  c5 66 00 cb 08  ADD ACC,R0,R3                                       R6 = ADD Ry,R3
21: 45 8e 04 a0 0c  45 8e 04 a0 0c  LDB R0,[INNER+4]                                    LDB Ry,[PTR,4] (y)
22: 67 00 00 cb 0c  67 00 00 cb 0c  MOV CMP,R0                                          ??? DEC Ry
23: 15 5a 64 c9 0c  15 5a 65 c9 0c  SUB CMP,R5 ; BGE 25                                 SUB R5,Ry; Bcc 25
24: 12 00 00 eb 0c  12 00 00 eb 0c  NEG CMP                                             ??? NEG Ry
25: 48 6c 71 e9 0c  48 6c 72 e9 0c  B (CMP > ACC) 32                                    R6, BLO 32
26: e5 92 9b e0 0c  e5 92 9b e0 0c                                                      AND R1,#$9B
27: dd 92 10 e0 0c  dd 92 10 e0 0c                                                      OR R1,#$10
28: 5c fe 00 a0 0c  5c fe 00 a0 0c                                                      ??? STB [PTR,0]
29: df 60 00 d3 08  df 60 00 d3 08                                                      LDB R6,
2a: e5 ec 9f e0 0c  e5 ec 9f e0 0c                                                      AND R6,#$9F
2b: dd ec 10 00 0c  dd ec 10 00 0c                                                      OR R6,#$10
2c: 25 ec 04 c0 0c  25 ec 04 c0 0c                                                      STB R6,[PTR2,-4]
2d: 18 82 00 00 0c  18 82 00 00 0c
2e: 4d 80 03 a0 0c  4d 80 03 a0 0c                                                      RAM+03
2f: df e0 e6 e0 0c  df e0 36 e1 0c
30: 49 60 75 f1 08  49 60 76 f1 08                                                      Jcc 36
31: 67 00 35 cd 08  67 00 36 cd 08                                                      Jcc 36
32: c5 fe 05 e0 0c  c5 fe 05 e0 0c  ADD R7,R7,5                                         ADD regE,#5
33: 5f 80 02 a0 0c  5f 80 02 a0 0c  LDB R0, [2]                                         LDB ACC,RAM+02
34: 1f 00 00 cb 08  1f 00 00 cb 08  LCMP CMP0,R0
35: 48 6e 52 c9 0c  48 6e 53 c9 0c  BNEQ CMP0,R7, 33                                    R6/R7, BLO 13
36: c4 00 00 ab 0c  c4 00 00 ab 0c  INC PTR                                             LEA Rx,[++PTR2]
37: 27 00 00 ab 0c  27 00 00 ab 0c
38: 42 00 00 8b 04  42 00 00 8b 04  MOVE PTR, OUTER
39: 1f 00 00 cb 00  1f 00 00 cb 00   LCMP CMP0 ??                                       test PTR2 vs ACC
3a: 48 00 43 c9 00  48 00 44 c9 00  BLT 4                                               BLT 04      next in set 0
3b: 5f fe 00 e0 08  5f fe 00 e0 08
3c: 5f 7e 00 ed 08  5f 7e 00 ed 08
3d: ff 04 00 ff 06  ff 04 00 ff 06  STOP                                                STOP
3e: 05 07 ff 02 03  05 07 ff 02 03
3f: 01 01 e0 02 6c  01 00 60 00 a0
    03 6c 04 40 04
*/

// run_collisions
//
// collide objects from s0 to e0 against
// objects from s1 to e1
//
// only compare objects with the specified bits (cm) set in their flags
// only set object 0's hit bit if (hm & 0x40) is true
//
// the data format is:
//
// +0 : flags
// +1 : width (4 pixel units)
// +2 : height (4 pixel units)
// +3 : x (2 pixel units) of center of object
// +4 : y (2 pixel units) of center of object

static void run_collisions( running_machine &machine, int s0, int e0, int s1, int e1, int cm, int hm )
{
	thunderx_state *state = machine.driver_data<thunderx_state>();
	UINT8* p0;
	UINT8* p1;
	int ii, jj;

	p0 = &state->m_pmcram[16 + 5 * s0];
	for (ii = s0; ii < e0; ii++, p0 += 5)
	{
		int l0, r0, b0, t0;

		// check valid
		if (!(p0[0] & cm))          continue;

		// get area
		l0 = p0[3] - p0[1];
		r0 = p0[3] + p0[1];
		t0 = p0[4] - p0[2];
		b0 = p0[4] + p0[2];

		p1 = &state->m_pmcram[16 + 5 * s1];
		for (jj = s1; jj < e1; jj++,p1 += 5)
		{
			int l1,r1,b1,t1;

			// check valid
			if (!(p1[0] & hm))      continue;

			// get area
			l1 = p1[3] - p1[1];
			r1 = p1[3] + p1[1];
			t1 = p1[4] - p1[2];
			b1 = p1[4] + p1[2];

			// overlap check
			if (l1 >= r0)   continue;
			if (l0 >= r1)   continue;
			if (t1 >= b0)   continue;
			if (t0 >= b1)   continue;

			// set flags
			p0[0] = (p0[0] & 0x9f) | (p1[0] & 0x04) | 0x10;
			p1[0] = (p1[0] & 0x9f) | 0x10;
		}
	}
}

// calculate_collisions
//
// emulates K052591 collision detection

static void calculate_collisions( running_machine &machine )
{
	thunderx_state *state = machine.driver_data<thunderx_state>();
	int X0,Y0;
	int X1,Y1;
	int CM,HM;

	// the data at 0x00 to 0x06 defines the operation
	//
	// 0x00 : word : last byte of set 0
	// 0x02 : byte : last byte of set 1
	// 0x03 : byte : collide mask
	// 0x04 : byte : hit mask
	// 0x05 : byte : first byte of set 0
	// 0x06 : byte : first byte of set 1
	//
	// the USA version is slightly different:
	//
	// 0x05 : word : first byte of set 0
	// 0x07 : byte : first byte of set 1
	//
	// the operation is to intersect set 0 with set 1
	// collide mask specifies objects to ignore
	// hit mask is 40 to set bit on object 0 and object 1
	// hit mask is 20 to set bit on object 1 only

	Y0 = state->m_pmcram[0];
	Y0 = (Y0 << 8) + state->m_pmcram[1];
	Y0 = (Y0 - 15) / 5;
	Y1 = (state->m_pmcram[2] - 15) / 5;

	if (state->m_pmcram[5] < 16)
	{
		// US Thunder Cross uses this form
		X0 = state->m_pmcram[5];
		X0 = (X0 << 8) + state->m_pmcram[6];
		X0 = (X0 - 16) / 5;
		X1 = (state->m_pmcram[7] - 16) / 5;
	}
	else
	{
		// Japan Thunder Cross uses this form
		X0 = (state->m_pmcram[5] - 16) / 5;
		X1 = (state->m_pmcram[6] - 16) / 5;
	}

	CM = state->m_pmcram[3];
	HM = state->m_pmcram[4];

	run_collisions(machine, X0, Y0, X1, Y1, CM, HM);
}

READ8_MEMBER(thunderx_state::thunderx_1f98_r)
{
	return m_1f98_data;
}

WRITE8_MEMBER(thunderx_state::thunderx_1f98_w)
{

	// logerror("%04x: 1f98_w %02x\n", space.device().safe_pc(),data);

	/* bit 0 = enable char ROM reading through the video RAM */
	k052109_set_rmrd_line(m_k052109, (data & 0x01) ? ASSERT_LINE : CLEAR_LINE);

	/* bit 1 = PMC-BK */
	m_pmcbank = (data & 0x02) >> 1;

	/* bit 2 = do collision detection when 0->1 */
	if ((data & 4) && !(m_1f98_data & 4))
	{
		calculate_collisions(machine());

		/* 100 cycle delay is arbitrary */
		machine().scheduler().timer_set(downcast<cpu_device *>(&space.device())->cycles_to_attotime(100), timer_expired_delegate(FUNC(thunderx_state::thunderx_firq_callback),this));
	}

	m_1f98_data = data;
}

WRITE8_MEMBER(thunderx_state::scontra_bankswitch_w)
{
	UINT8 *RAM = memregion("maincpu")->base();
	int offs;

//logerror("%04x: bank switch %02x\n",space.device().safe_pc(),data);

	/* bits 0-3 ROM bank */
	offs = 0x10000 + (data & 0x0f)*0x2000;
	membank("bank1")->set_base(&RAM[offs] );

	/* bit 4 select work RAM or palette RAM at 5800-5fff */
	m_palette_selected = ~data & 0x10;

	/* bits 5/6 coin counters */
	coin_counter_w(machine(), 0, data & 0x20);
	coin_counter_w(machine(), 1, data & 0x40);

	/* bit 7 controls layer priority */
	m_priority = data & 0x80;
}

WRITE8_MEMBER(thunderx_state::thunderx_videobank_w)
{
	//logerror("%04x: select video ram bank %02x\n",space.device().safe_pc(),data);
	/* 0x01 = work RAM at 4000-5fff */
	/* 0x00 = palette at 5800-5fff */
	/* 0x10 = unknown RAM at 5800-5fff */
	m_rambank = data;

	/* bits 1/2 coin counters */
	coin_counter_w(machine(), 0, data & 0x02);
	coin_counter_w(machine(), 1, data & 0x04);

	/* bit 3 controls layer priority (seems to be always 1) */
	m_priority = data & 0x08;
}

WRITE8_MEMBER(thunderx_state::thunderx_sh_irqtrigger_w)
{
	m_audiocpu->set_input_line_and_vector(0, HOLD_LINE, 0xff);
}

WRITE8_MEMBER(thunderx_state::scontra_snd_bankswitch_w)
{
	device_t *device = machine().device("k007232");
	/* b3-b2: bank for chanel B */
	/* b1-b0: bank for chanel A */

	int bank_A = (data & 0x03);
	int bank_B = ((data >> 2) & 0x03);
	k007232_set_bank(device, bank_A, bank_B);
}

READ8_MEMBER(thunderx_state::k052109_051960_r)
{

	if (k052109_get_rmrd_line(m_k052109) == CLEAR_LINE)
	{
		if (offset >= 0x3800 && offset < 0x3808)
			return k051937_r(m_k051960, space, offset - 0x3800);
		else if (offset < 0x3c00)
			return k052109_r(m_k052109, space, offset);
		else
			return k051960_r(m_k051960, space, offset - 0x3c00);
	}
	else
		return k052109_r(m_k052109, space, offset);
}

WRITE8_MEMBER(thunderx_state::k052109_051960_w)
{

	if (offset >= 0x3800 && offset < 0x3808)
		k051937_w(m_k051960, space, offset - 0x3800, data);
	else if (offset < 0x3c00)
		k052109_w(m_k052109, space, offset, data);
	else
		k051960_w(m_k051960, space, offset - 0x3c00, data);
}

/***************************************************************************/

static ADDRESS_MAP_START( scontra_map, AS_PROGRAM, 8, thunderx_state )
	AM_RANGE(0x1f80, 0x1f80) AM_WRITE(scontra_bankswitch_w) /* bankswitch control + coin counters */
	AM_RANGE(0x1f84, 0x1f84) AM_WRITE(soundlatch_byte_w)
	AM_RANGE(0x1f88, 0x1f88) AM_WRITE(thunderx_sh_irqtrigger_w)     /* cause interrupt on audio CPU */
	AM_RANGE(0x1f8c, 0x1f8c) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x1f90, 0x1f90) AM_READ_PORT("SYSTEM")
	AM_RANGE(0x1f91, 0x1f91) AM_READ_PORT("P1")
	AM_RANGE(0x1f92, 0x1f92) AM_READ_PORT("P2")
	AM_RANGE(0x1f93, 0x1f93) AM_READ_PORT("DSW3")
	AM_RANGE(0x1f94, 0x1f94) AM_READ_PORT("DSW1")
	AM_RANGE(0x1f95, 0x1f95) AM_READ_PORT("DSW2")
	AM_RANGE(0x1f98, 0x1f98) AM_WRITE(thunderx_1f98_w)
	AM_RANGE(0x0000, 0x3fff) AM_READWRITE(k052109_051960_r, k052109_051960_w)       /* video RAM + sprite RAM */

	AM_RANGE(0x4000, 0x57ff) AM_RAM
	AM_RANGE(0x5800, 0x5fff) AM_READWRITE(scontra_bankedram_r, scontra_bankedram_w) AM_SHARE("ram")         /* palette + work RAM */
	AM_RANGE(0x6000, 0x7fff) AM_ROMBANK("bank1")
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( thunderx_map, AS_PROGRAM, 8, thunderx_state )
	AM_RANGE(0x1f80, 0x1f80) AM_WRITE(thunderx_videobank_w)
	AM_RANGE(0x1f84, 0x1f84) AM_WRITE(soundlatch_byte_w)
	AM_RANGE(0x1f88, 0x1f88) AM_WRITE(thunderx_sh_irqtrigger_w)     /* cause interrupt on audio CPU */
	AM_RANGE(0x1f8c, 0x1f8c) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x1f90, 0x1f90) AM_READ_PORT("SYSTEM")
	AM_RANGE(0x1f91, 0x1f91) AM_READ_PORT("P1")
	AM_RANGE(0x1f92, 0x1f92) AM_READ_PORT("P2")
	AM_RANGE(0x1f93, 0x1f93) AM_READ_PORT("DSW3")
	AM_RANGE(0x1f94, 0x1f94) AM_READ_PORT("DSW1")
	AM_RANGE(0x1f95, 0x1f95) AM_READ_PORT("DSW2")
	AM_RANGE(0x1f98, 0x1f98) AM_READWRITE(thunderx_1f98_r, thunderx_1f98_w) /* registers */
	AM_RANGE(0x0000, 0x3fff) AM_READWRITE(k052109_051960_r, k052109_051960_w)

	AM_RANGE(0x4000, 0x57ff) AM_RAM
	AM_RANGE(0x5800, 0x5fff) AM_READWRITE(thunderx_bankedram_r, thunderx_bankedram_w) AM_SHARE("ram")           /* palette + work RAM + unknown RAM */
	AM_RANGE(0x6000, 0x7fff) AM_ROMBANK("bank1")
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( scontra_sound_map, AS_PROGRAM, 8, thunderx_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM                 /* ROM */
	AM_RANGE(0x8000, 0x87ff) AM_RAM                 /* RAM */
	AM_RANGE(0xa000, 0xa000) AM_READ(soundlatch_byte_r)         /* soundlatch_byte_r */
	AM_RANGE(0xb000, 0xb00d) AM_DEVREADWRITE_LEGACY("k007232", k007232_r, k007232_w)        /* 007232 registers */
	AM_RANGE(0xc000, 0xc001) AM_DEVREADWRITE("ymsnd", ym2151_device, read, write)       /* YM2151 */
	AM_RANGE(0xf000, 0xf000) AM_WRITE(scontra_snd_bankswitch_w) /* 007232 bank select */
ADDRESS_MAP_END

static ADDRESS_MAP_START( thunderx_sound_map, AS_PROGRAM, 8, thunderx_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0xa000, 0xa000) AM_READ(soundlatch_byte_r)
	AM_RANGE(0xc000, 0xc001) AM_DEVREADWRITE("ymsnd", ym2151_device, read, write)
ADDRESS_MAP_END

/***************************************************************************

    Input Ports

***************************************************************************/

static INPUT_PORTS_START( scontra )
	PORT_START("SYSTEM")
	KONAMI8_SYSTEM_UNK

	PORT_START("P1")
	KONAMI8_B12_UNK(1)

	PORT_START("P2")
	KONAMI8_B12_UNK(2)

	PORT_START("DSW1")
	KONAMI_COINAGE_LOC(DEF_STR( Free_Play ), "No Coin B", SW1)
	/* "No Coin B" = coins produce sound, but no effect on coin counter */

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )        PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x00, "SW2:3" ) // test mode calls it cabinet type, but this is a 2 players game
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )   PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(    0x18, "30000 200000" )
	PORT_DIPSETTING(    0x10, "50000 300000" )
	PORT_DIPSETTING(    0x08, "30000" )
	PORT_DIPSETTING(    0x00, "50000" )
	PORT_DIPNAME( 0x60, 0x40, DEF_STR( Difficulty ) )   PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(    0x60, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Difficult ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Very_Difficult ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW3")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )  PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x02, "SW3:2" )
	PORT_SERVICE_DIPLOC( 0x04, IP_ACTIVE_LOW, "SW3:3" )
	PORT_DIPNAME( 0x08, 0x00, "Continue Limit (1Player/2Players)" ) PORT_DIPLOCATION("SW3:4")
	PORT_DIPSETTING(    0x08, "3times / Twice altogether" )
	PORT_DIPSETTING(    0x00, "5times / 4times altogether" )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( thunderx )
	PORT_START("SYSTEM")
	KONAMI8_SYSTEM_10
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("P1")
	KONAMI8_B12_UNK(1)

	PORT_START("P2")
	KONAMI8_B12_UNK(2)

	PORT_START("DSW1")
	KONAMI_COINAGE_LOC(DEF_STR( Free_Play ), "No Coin B", SW1)
	/* "No Coin B" = coins produce sound, but no effect on coin counter */

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )        PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x00, "Award Bonus Life" )      PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x04, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )   PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(    0x18, "30000 200000" ) // Japanese default
	PORT_DIPSETTING(    0x10, "50000 300000" ) // US default
	PORT_DIPSETTING(    0x08, "30000" )
	PORT_DIPSETTING(    0x00, "50000" )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Difficulty ) )   PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(    0x60, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Difficult ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Very_Difficult ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW3")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )  PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x02, IP_ACTIVE_LOW, "SW3:2" )
	PORT_SERVICE_DIPLOC(   0x04, IP_ACTIVE_LOW, "SW3:3" )
	PORT_DIPUNUSED_DIPLOC( 0x08, IP_ACTIVE_LOW, "SW3:4" )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( thnderxj )
	PORT_INCLUDE( thunderx )

	PORT_MODIFY("DSW2")
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "SW2:3" ) // manual says "OFF=Table On=Upright", but not work?
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )   PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(    0x18, "30000 200000" ) // Japanese default
	PORT_DIPSETTING(    0x10, "50000 300000" ) // US default
	PORT_DIPSETTING(    0x08, "30000" )
	PORT_DIPSETTING(    0x00, "50000" )
INPUT_PORTS_END


/***************************************************************************

    Machine Driver

***************************************************************************/

static void volume_callback(device_t *device, int v)
{
	k007232_set_volume(device, 0, (v >> 4) * 0x11, 0);
	k007232_set_volume(device, 1, 0, (v & 0x0f) * 0x11);
}

static const k007232_interface k007232_config =
{
	volume_callback /* external port callback */
};



static const k052109_interface thunderx_k052109_intf =
{
	"gfx1", 0,
	NORMAL_PLANE_ORDER,
	KONAMI_ROM_DEINTERLEAVE_2,
	thunderx_tile_callback
};

static const k051960_interface thunderx_k051960_intf =
{
	"gfx2", 1,
	NORMAL_PLANE_ORDER,
	KONAMI_ROM_DEINTERLEAVE_2,
	thunderx_sprite_callback
};

MACHINE_START_MEMBER(thunderx_state,scontra)
{

	m_generic_paletteram_8.allocate(0x800);

	m_maincpu = machine().device<cpu_device>("maincpu");
	m_audiocpu = machine().device<cpu_device>("audiocpu");
	m_k007232 = machine().device("k007232");
	m_k052109 = machine().device("k052109");
	m_k051960 = machine().device("k051960");

	save_item(NAME(m_priority));
	save_item(NAME(m_1f98_data));
	save_item(NAME(m_palette_selected));
	save_item(NAME(m_rambank));
	save_item(NAME(m_pmcbank));
}

MACHINE_START_MEMBER(thunderx_state,thunderx)
{
	UINT8 *ROM = memregion("maincpu")->base();

	membank("bank1")->configure_entries(0, 12, &ROM[0x10000], 0x2000);
	membank("bank1")->configure_entries(12, 4, &ROM[0x08000], 0x2000);
	membank("bank1")->set_entry(0);

	memset(m_pmcram, 0, sizeof(m_pmcram));

	MACHINE_START_CALL_MEMBER(scontra);

	save_item(NAME(m_pmcram));
}

MACHINE_RESET_MEMBER(thunderx_state,scontra)
{

	m_priority = 0;
	m_1f98_data = 0;
	m_palette_selected = 0;
	m_rambank = 0;
	m_pmcbank = 0;
}

MACHINE_RESET_MEMBER(thunderx_state,thunderx)
{
	konami_configure_set_lines(machine().device("maincpu"), thunderx_banking);

	MACHINE_RESET_CALL_MEMBER(scontra);
}

static MACHINE_CONFIG_START( scontra, thunderx_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", KONAMI, XTAL_24MHz/8)       /* Verified on pcb, CPU is 052001 */
	MCFG_CPU_PROGRAM_MAP(scontra_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", thunderx_state,  scontra_interrupt)

	MCFG_CPU_ADD("audiocpu", Z80, XTAL_3_579545MHz)     /* verified on pcb */
	MCFG_CPU_PROGRAM_MAP(scontra_sound_map)

	MCFG_MACHINE_START_OVERRIDE(thunderx_state,scontra)
	MCFG_MACHINE_RESET_OVERRIDE(thunderx_state,scontra)

	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_HAS_SHADOWS)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(59.17)             /* verified on pcb */
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(14*8, (64-14)*8-1, 2*8, 30*8-1 )
	MCFG_SCREEN_UPDATE_DRIVER(thunderx_state, screen_update_scontra)

	MCFG_PALETTE_LENGTH(1024)


	MCFG_K052109_ADD("k052109", thunderx_k052109_intf)
	MCFG_K051960_ADD("k051960", thunderx_k051960_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_YM2151_ADD("ymsnd", XTAL_3_579545MHz)  /* verified on pcb */
	MCFG_SOUND_ROUTE(0, "mono", 1.0)
	MCFG_SOUND_ROUTE(1, "mono", 1.0)

	MCFG_SOUND_ADD("k007232", K007232, XTAL_3_579545MHz)    /* verified on pcb */
	MCFG_SOUND_CONFIG(k007232_config)
	MCFG_SOUND_ROUTE(0, "mono", 0.20)
	MCFG_SOUND_ROUTE(1, "mono", 0.20)
MACHINE_CONFIG_END


static MACHINE_CONFIG_START( thunderx, thunderx_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", KONAMI, 3000000)        /* ? */
	MCFG_CPU_PROGRAM_MAP(thunderx_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", thunderx_state,  scontra_interrupt)

	MCFG_CPU_ADD("audiocpu", Z80, 3579545)      /* ? */
	MCFG_CPU_PROGRAM_MAP(thunderx_sound_map)

	MCFG_MACHINE_START_OVERRIDE(thunderx_state,thunderx)
	MCFG_MACHINE_RESET_OVERRIDE(thunderx_state,thunderx)

	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_HAS_SHADOWS)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(14*8, (64-14)*8-1, 2*8, 30*8-1 )
	MCFG_SCREEN_UPDATE_DRIVER(thunderx_state, screen_update_scontra)

	MCFG_PALETTE_LENGTH(1024)


	MCFG_K052109_ADD("k052109", thunderx_k052109_intf)
	MCFG_K051960_ADD("k051960", thunderx_k051960_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_YM2151_ADD("ymsnd", 3579545)
	MCFG_SOUND_ROUTE(0, "mono", 1.0)
	MCFG_SOUND_ROUTE(1, "mono", 1.0)
MACHINE_CONFIG_END


/***************************************************************************

  Game ROMs

***************************************************************************/

ROM_START( scontra )
	ROM_REGION( 0x30000, "maincpu", 0 ) /* ROMs + banked RAM */
	ROM_LOAD( "775-e02.k11",     0x10000, 0x08000, CRC(a61c0ead) SHA1(9a0aadc8d3538fc1d88b761753fffcac8923a218) )   /* banked ROM */
	ROM_CONTINUE(            0x08000, 0x08000 )             /* fixed ROM */
	ROM_LOAD( "775-e03.k13",     0x20000, 0x10000, CRC(00b02622) SHA1(caf1da53815e437e3fb952d29e71f2c314684cd9) )   /* banked ROM */

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for the SOUND CPU */
	ROM_LOAD( "775-c01.bin", 0x00000, 0x08000, CRC(0ced785a) SHA1(1eebe005a968fbaac595c168499107e34763976c) )

	ROM_REGION( 0x100000, "gfx1", 0 ) /* tiles */
	ROM_LOAD16_BYTE( "775-a07a.bin", 0x00000, 0x20000, CRC(e716bdf3) SHA1(82e10132f248aed8cc1aea6bb7afe9a1479c8b59) )   /* tiles */
	ROM_LOAD16_BYTE( "775-a07e.bin", 0x00001, 0x20000, CRC(0986e3a5) SHA1(61c33a3f2e4fde7d23d440b5c3151fe38e25716b) )
	ROM_LOAD16_BYTE( "775-f07c.bin", 0x40000, 0x10000, CRC(b0b30915) SHA1(0abd858f93f7cc5383a805a5ae06c086c120f208) )
	ROM_LOAD16_BYTE( "775-f07g.bin", 0x40001, 0x10000, CRC(fbed827d) SHA1(7fcc6cc03ab6238b05799dd50f38c29eb9f98b5a) )
	ROM_LOAD16_BYTE( "775-f07d.bin", 0x60000, 0x10000, CRC(f184be8e) SHA1(c266be12762f7e81edbe4b36f3c96b03f6ec552b) )
	ROM_LOAD16_BYTE( "775-f07h.bin", 0x60001, 0x10000, CRC(7b56c348) SHA1(f75c1c0962389f204c8cf1a0bc2da01a922cd742) )
	ROM_LOAD16_BYTE( "775-a08a.bin", 0x80000, 0x20000, CRC(3ddd11a4) SHA1(4831a891d6cb4507053d576eddd658c338318176) )
	ROM_LOAD16_BYTE( "775-a08e.bin", 0x80001, 0x20000, CRC(1007d963) SHA1(cba4ca058dee1c8cdeb019e1cc50cae76bf419a1) )
	ROM_LOAD16_BYTE( "775-f08c.bin", 0xc0000, 0x10000, CRC(53abdaec) SHA1(0e0f7fe4bb9139a1ae94506a832153b711961564) )
	ROM_LOAD16_BYTE( "775-f08g.bin", 0xc0001, 0x10000, CRC(3df85a6e) SHA1(25a49abbf6e9fe63d4ff6bfff9219c98aa1b5e7b) )
	ROM_LOAD16_BYTE( "775-f08d.bin", 0xe0000, 0x10000, CRC(102dcace) SHA1(03036b6d9d66a12cb3e97980f149c09d1efbd6d8) )
	ROM_LOAD16_BYTE( "775-f08h.bin", 0xe0001, 0x10000, CRC(ad9d7016) SHA1(91e9f279b781eefcafffc70afe207f35cc6f4d9d) )

	ROM_REGION( 0x100000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "775-a05a.bin", 0x00000, 0x10000, CRC(a0767045) SHA1(e6df0731a9fb3b3d918607de81844e1f9353aac7) )   /* sprites */
	ROM_LOAD16_BYTE( "775-a05e.bin", 0x00001, 0x10000, CRC(2f656f08) SHA1(140e7948c45d27c6705622d588a65b59ebcc624c) )
	ROM_LOAD16_BYTE( "775-a05b.bin", 0x20000, 0x10000, CRC(ab8ad4fd) SHA1(c9ae537fa1607fbd11403390d1da923955f0d1ab) )
	ROM_LOAD16_BYTE( "775-a05f.bin", 0x20001, 0x10000, CRC(1c0eb1b6) SHA1(420eb26acd54ff484301aa2dad587f1b6b437363) )
	ROM_LOAD16_BYTE( "775-f05c.bin", 0x40000, 0x10000, CRC(5647761e) SHA1(ff7983cb0c2f84f7be9d44e20b01266db4b2836a) )
	ROM_LOAD16_BYTE( "775-f05g.bin", 0x40001, 0x10000, CRC(a1692cca) SHA1(2cefc4b7532a9d29361843419ee427fb9421b79b) )
	ROM_LOAD16_BYTE( "775-f05d.bin", 0x60000, 0x10000, CRC(ad676a6f) SHA1(f2ca759c8c8a8007aa022d6c058d0431057a639a) )
	ROM_LOAD16_BYTE( "775-f05h.bin", 0x60001, 0x10000, CRC(3f925bcf) SHA1(434dd442c0cb5c5c039a69683a3a5f226e49261c) )
	ROM_LOAD16_BYTE( "775-a06a.bin", 0x80000, 0x10000, CRC(77a34ad0) SHA1(3653fb8458c1e7eb7d83b5cd63f02343c0f2d93e) )
	ROM_LOAD16_BYTE( "775-a06e.bin", 0x80001, 0x10000, CRC(8a910c94) SHA1(0387a7f412a977fa7a5ca685653ac1bb3dfdbbcb) )
	ROM_LOAD16_BYTE( "775-a06b.bin", 0xa0000, 0x10000, CRC(563fb565) SHA1(96a2a95ab02456e53651718a7080f18c252451c8) )
	ROM_LOAD16_BYTE( "775-a06f.bin", 0xa0001, 0x10000, CRC(e14995c0) SHA1(1d7fdfb8f9eacb005b0897b2b62b85ce334cd4d6) )
	ROM_LOAD16_BYTE( "775-f06c.bin", 0xc0000, 0x10000, CRC(5ee6f3c1) SHA1(9138ea3588b63862849f6e783725a711e7e50669) )
	ROM_LOAD16_BYTE( "775-f06g.bin", 0xc0001, 0x10000, CRC(2645274d) SHA1(2fd04b0adbcf53562669946259b59f1ec9c52bda) )
	ROM_LOAD16_BYTE( "775-f06d.bin", 0xe0000, 0x10000, CRC(c8b764fa) SHA1(62f7f59ed36dca7346ec9eb019a4e435e8476dc6) )
	ROM_LOAD16_BYTE( "775-f06h.bin", 0xe0001, 0x10000, CRC(d6595f59) SHA1(777ea6da2026c90e7fbbc598275c8f95f2eb99c2) )

	ROM_REGION( 0x80000, "k007232", 0 ) /* k007232 data */
	ROM_LOAD( "775-a04a.bin", 0x00000, 0x10000, CRC(7efb2e0f) SHA1(fb350a056b547fe4f981bc211e2f9518ae5a3499) )
	ROM_LOAD( "775-a04b.bin", 0x10000, 0x10000, CRC(f41a2b33) SHA1(dffa06360b6032f7370fe72698aacad4d8779472) )
	ROM_LOAD( "775-a04c.bin", 0x20000, 0x10000, CRC(e4e58f14) SHA1(23dcb4dfa9a44115d1b730d9efcc314801b811c7) )
	ROM_LOAD( "775-a04d.bin", 0x30000, 0x10000, CRC(d46736f6) SHA1(586e914a35d3d7a71cccec66ca45a5bbbb9e504b) )
	ROM_LOAD( "775-f04e.bin", 0x40000, 0x10000, CRC(fbf7e363) SHA1(53578eb7dab8f723439dc12eefade3edb027c148) )
	ROM_LOAD( "775-f04f.bin", 0x50000, 0x10000, CRC(b031ef2d) SHA1(0124fe15871c3972ef1e2dbaf53d17668c1dccfd) )
	ROM_LOAD( "775-f04g.bin", 0x60000, 0x10000, CRC(ee107bbb) SHA1(e21de761a0dfd3811ddcbc33d8868479010e86d0) )
	ROM_LOAD( "775-f04h.bin", 0x70000, 0x10000, CRC(fb0fab46) SHA1(fcbf904f7cf4d265352dc73ed228390b29784aad) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "775a09.b19",   0x0000, 0x0100, CRC(46d1e0df) SHA1(65dad04a124cc49cbc9bb271f865d77efbc4d57c) )    /* priority encoder (not used) */
ROM_END

ROM_START( scontraj )
	ROM_REGION( 0x30000, "maincpu", 0 ) /* ROMs + banked RAM */
	ROM_LOAD( "775-f02.bin", 0x10000, 0x08000, CRC(8d5933a7) SHA1(e13ec62a4209b790b609429d98620ec0d07bd0ee) )   /* banked ROM */
	ROM_CONTINUE(            0x08000, 0x08000 )             /* fixed ROM */
	ROM_LOAD( "775-f03.bin", 0x20000, 0x10000, CRC(1ef63d80) SHA1(8fa41038ec2928f9572d0d4511a4bb3a3d8de06d) )   /* banked ROM */

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for the SOUND CPU */
	ROM_LOAD( "775-c01.bin", 0x00000, 0x08000, CRC(0ced785a) SHA1(1eebe005a968fbaac595c168499107e34763976c) )

	ROM_REGION( 0x100000, "gfx1", 0 ) /* tiles */
	ROM_LOAD16_BYTE( "775-a07a.bin", 0x00000, 0x20000, CRC(e716bdf3) SHA1(82e10132f248aed8cc1aea6bb7afe9a1479c8b59) )   /* tiles */
	ROM_LOAD16_BYTE( "775-a07e.bin", 0x00001, 0x20000, CRC(0986e3a5) SHA1(61c33a3f2e4fde7d23d440b5c3151fe38e25716b) )
	ROM_LOAD16_BYTE( "775-f07c.bin", 0x40000, 0x10000, CRC(b0b30915) SHA1(0abd858f93f7cc5383a805a5ae06c086c120f208) )
	ROM_LOAD16_BYTE( "775-f07g.bin", 0x40001, 0x10000, CRC(fbed827d) SHA1(7fcc6cc03ab6238b05799dd50f38c29eb9f98b5a) )
	ROM_LOAD16_BYTE( "775-f07d.bin", 0x60000, 0x10000, CRC(f184be8e) SHA1(c266be12762f7e81edbe4b36f3c96b03f6ec552b) )
	ROM_LOAD16_BYTE( "775-f07h.bin", 0x60001, 0x10000, CRC(7b56c348) SHA1(f75c1c0962389f204c8cf1a0bc2da01a922cd742) )
	ROM_LOAD16_BYTE( "775-a08a.bin", 0x80000, 0x20000, CRC(3ddd11a4) SHA1(4831a891d6cb4507053d576eddd658c338318176) )
	ROM_LOAD16_BYTE( "775-a08e.bin", 0x80001, 0x20000, CRC(1007d963) SHA1(cba4ca058dee1c8cdeb019e1cc50cae76bf419a1) )
	ROM_LOAD16_BYTE( "775-f08c.bin", 0xc0000, 0x10000, CRC(53abdaec) SHA1(0e0f7fe4bb9139a1ae94506a832153b711961564) )
	ROM_LOAD16_BYTE( "775-f08g.bin", 0xc0001, 0x10000, CRC(3df85a6e) SHA1(25a49abbf6e9fe63d4ff6bfff9219c98aa1b5e7b) )
	ROM_LOAD16_BYTE( "775-f08d.bin", 0xe0000, 0x10000, CRC(102dcace) SHA1(03036b6d9d66a12cb3e97980f149c09d1efbd6d8) )
	ROM_LOAD16_BYTE( "775-f08h.bin", 0xe0001, 0x10000, CRC(ad9d7016) SHA1(91e9f279b781eefcafffc70afe207f35cc6f4d9d) )

	ROM_REGION( 0x100000, "gfx2", 0 ) /* sprites */
	ROM_LOAD16_BYTE( "775-a05a.bin", 0x00000, 0x10000, CRC(a0767045) SHA1(e6df0731a9fb3b3d918607de81844e1f9353aac7) )   /* sprites */
	ROM_LOAD16_BYTE( "775-a05e.bin", 0x00001, 0x10000, CRC(2f656f08) SHA1(140e7948c45d27c6705622d588a65b59ebcc624c) )
	ROM_LOAD16_BYTE( "775-a05b.bin", 0x20000, 0x10000, CRC(ab8ad4fd) SHA1(c9ae537fa1607fbd11403390d1da923955f0d1ab) )
	ROM_LOAD16_BYTE( "775-a05f.bin", 0x20001, 0x10000, CRC(1c0eb1b6) SHA1(420eb26acd54ff484301aa2dad587f1b6b437363) )
	ROM_LOAD16_BYTE( "775-f05c.bin", 0x40000, 0x10000, CRC(5647761e) SHA1(ff7983cb0c2f84f7be9d44e20b01266db4b2836a) )
	ROM_LOAD16_BYTE( "775-f05g.bin", 0x40001, 0x10000, CRC(a1692cca) SHA1(2cefc4b7532a9d29361843419ee427fb9421b79b) )
	ROM_LOAD16_BYTE( "775-f05d.bin", 0x60000, 0x10000, CRC(ad676a6f) SHA1(f2ca759c8c8a8007aa022d6c058d0431057a639a) )
	ROM_LOAD16_BYTE( "775-f05h.bin", 0x60001, 0x10000, CRC(3f925bcf) SHA1(434dd442c0cb5c5c039a69683a3a5f226e49261c) )
	ROM_LOAD16_BYTE( "775-a06a.bin", 0x80000, 0x10000, CRC(77a34ad0) SHA1(3653fb8458c1e7eb7d83b5cd63f02343c0f2d93e) )
	ROM_LOAD16_BYTE( "775-a06e.bin", 0x80001, 0x10000, CRC(8a910c94) SHA1(0387a7f412a977fa7a5ca685653ac1bb3dfdbbcb) )
	ROM_LOAD16_BYTE( "775-a06b.bin", 0xa0000, 0x10000, CRC(563fb565) SHA1(96a2a95ab02456e53651718a7080f18c252451c8) )
	ROM_LOAD16_BYTE( "775-a06f.bin", 0xa0001, 0x10000, CRC(e14995c0) SHA1(1d7fdfb8f9eacb005b0897b2b62b85ce334cd4d6) )
	ROM_LOAD16_BYTE( "775-f06c.bin", 0xc0000, 0x10000, CRC(5ee6f3c1) SHA1(9138ea3588b63862849f6e783725a711e7e50669) )
	ROM_LOAD16_BYTE( "775-f06g.bin", 0xc0001, 0x10000, CRC(2645274d) SHA1(2fd04b0adbcf53562669946259b59f1ec9c52bda) )
	ROM_LOAD16_BYTE( "775-f06d.bin", 0xe0000, 0x10000, CRC(c8b764fa) SHA1(62f7f59ed36dca7346ec9eb019a4e435e8476dc6) )
	ROM_LOAD16_BYTE( "775-f06h.bin", 0xe0001, 0x10000, CRC(d6595f59) SHA1(777ea6da2026c90e7fbbc598275c8f95f2eb99c2) )

	ROM_REGION( 0x80000, "k007232", 0 ) /* k007232 data */
	ROM_LOAD( "775-a04a.bin", 0x00000, 0x10000, CRC(7efb2e0f) SHA1(fb350a056b547fe4f981bc211e2f9518ae5a3499) )
	ROM_LOAD( "775-a04b.bin", 0x10000, 0x10000, CRC(f41a2b33) SHA1(dffa06360b6032f7370fe72698aacad4d8779472) )
	ROM_LOAD( "775-a04c.bin", 0x20000, 0x10000, CRC(e4e58f14) SHA1(23dcb4dfa9a44115d1b730d9efcc314801b811c7) )
	ROM_LOAD( "775-a04d.bin", 0x30000, 0x10000, CRC(d46736f6) SHA1(586e914a35d3d7a71cccec66ca45a5bbbb9e504b) )
	ROM_LOAD( "775-f04e.bin", 0x40000, 0x10000, CRC(fbf7e363) SHA1(53578eb7dab8f723439dc12eefade3edb027c148) )
	ROM_LOAD( "775-f04f.bin", 0x50000, 0x10000, CRC(b031ef2d) SHA1(0124fe15871c3972ef1e2dbaf53d17668c1dccfd) )
	ROM_LOAD( "775-f04g.bin", 0x60000, 0x10000, CRC(ee107bbb) SHA1(e21de761a0dfd3811ddcbc33d8868479010e86d0) )
	ROM_LOAD( "775-f04h.bin", 0x70000, 0x10000, CRC(fb0fab46) SHA1(fcbf904f7cf4d265352dc73ed228390b29784aad) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "775a09.b19",   0x0000, 0x0100, CRC(46d1e0df) SHA1(65dad04a124cc49cbc9bb271f865d77efbc4d57c) )    /* priority encoder (not used) */
ROM_END

ROM_START( thunderx )
	ROM_REGION( 0x28000, "maincpu", 0 ) /* ROMs + banked RAM */
	ROM_LOAD( "873-s03.k15", 0x10000, 0x10000, CRC(2aec2699) SHA1(8f52703a6a1ba6417c484925192ce697af9c73f1) )
	ROM_LOAD( "873-s02.k13", 0x20000, 0x08000, CRC(6619333a) SHA1(1961658d528b0870c57f1cb78e016fb881f50392) )
	ROM_CONTINUE(            0x08000, 0x08000 )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "873-f01.f8",   0x0000, 0x8000, CRC(ea35ffa3) SHA1(91e82b77d4f3af8238fb198db26182bebc5026e4) )

	ROM_REGION( 0x80000, "gfx1", 0 )    /* temporary space for graphics (disposed after conversion) */
	ROM_LOAD16_BYTE( "873c06a.f6",   0x00000, 0x10000, CRC(0e340b67) SHA1(a76b1ee4bd4c99826a02b63a705447d0ba4e7b01) ) /* Chars */
	ROM_LOAD16_BYTE( "873c06c.f5",   0x00001, 0x10000, CRC(ef0e72cd) SHA1(85b77a303378386f2d395da8707f4b638d37833e) )
	ROM_LOAD16_BYTE( "873c06b.e6",   0x20000, 0x10000, CRC(97ad202e) SHA1(fd155aeb691814950711ead3bc2c93c67b7b0434) )
	ROM_LOAD16_BYTE( "873c06d.e5",   0x20001, 0x10000, CRC(8393d42e) SHA1(ffcb5eca3f58994e05c49d803fa4831c0213e2e2) )
	ROM_LOAD16_BYTE( "873c07a.f4",   0x40000, 0x10000, CRC(a8aab84f) SHA1(a68521a9abf45c3292b3090a2483edbf31356c7d) )
	ROM_LOAD16_BYTE( "873c07c.f3",   0x40001, 0x10000, CRC(2521009a) SHA1(6546b88943615389c81b753ff5bb6aa9378c3266) )
	ROM_LOAD16_BYTE( "873c07b.e4",   0x60000, 0x10000, CRC(12a2b8ba) SHA1(ffa32ca116e0b6ca65bb9ce83dd28f5c027956a5) )
	ROM_LOAD16_BYTE( "873c07d.e3",   0x60001, 0x10000, CRC(fae9f965) SHA1(780c234507835c37bde445ab34f069714cc7a506) )

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "873c04a.f11",  0x00000, 0x10000, CRC(f7740bf3) SHA1(f64b7e807f19a9523a517024a9eb56736cdda6bb) ) /* Sprites */
	ROM_LOAD16_BYTE( "873c04c.f10",  0x00001, 0x10000, CRC(5dacbd2b) SHA1(deb943b99fd296d20be9c4250b2348549f65ba37) )
	ROM_LOAD16_BYTE( "873c04b.e11",  0x20000, 0x10000, CRC(9ac581da) SHA1(fd0a603de8586621444055bbff8bb83349b8a0d8) )
	ROM_LOAD16_BYTE( "873c04d.e10",  0x20001, 0x10000, CRC(44a4668c) SHA1(6d1526ed3408ddc763a071604e7b1e0773c87b99) )
	ROM_LOAD16_BYTE( "873c05a.f9",   0x40000, 0x10000, CRC(d73e107d) SHA1(ba63b195e20a98c476e7d0f8d0187bc3327a8822) )
	ROM_LOAD16_BYTE( "873c05c.f8",   0x40001, 0x10000, CRC(59903200) SHA1(d076802c53aa604df8c5fdd33cb41876ba2a3385) )
	ROM_LOAD16_BYTE( "873c05b.e9",   0x60000, 0x10000, CRC(81059b99) SHA1(1e1a22ca45599abe0dce32fc0b188281deb3b8ac) )
	ROM_LOAD16_BYTE( "873c05d.e8",   0x60001, 0x10000, CRC(7fa3d7df) SHA1(c78b9a949abdf44366d872daa1f2041158fae790) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "873a08.f20",   0x0000, 0x0100, CRC(e2d09a1b) SHA1(a9651e137486b2df367c39eb43f52d0833589e87) )    /* priority encoder (not used) */
ROM_END

ROM_START( thunderxa ) /* Alternate Starting stage then the other 2 sets, Perhaps a US set? */
	ROM_REGION( 0x28000, "maincpu", 0 ) /* ROMs + banked RAM */
	ROM_LOAD( "873-k03.k15", 0x10000, 0x10000, CRC(276817ad) SHA1(34b1beecf2a4c54dd7cd150c5d83b44f67be288a) )
	ROM_LOAD( "873-k02.k13", 0x20000, 0x08000, CRC(80cc1c45) SHA1(881bc6eea94671e8c3fdb7a10b0e742b18cb7212) )
	ROM_CONTINUE(           0x08000, 0x08000 )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "873-h01.f8",    0x0000, 0x8000, CRC(990b7a7c) SHA1(0965e7350c6006a9652cea0f24d836b4979910fd) )

	ROM_REGION( 0x80000, "gfx1", 0 )    /* temporary space for graphics (disposed after conversion) */
	ROM_LOAD16_BYTE( "873c06a.f6",   0x00000, 0x10000, CRC(0e340b67) SHA1(a76b1ee4bd4c99826a02b63a705447d0ba4e7b01) ) /* Chars */
	ROM_LOAD16_BYTE( "873c06c.f5",   0x00001, 0x10000, CRC(ef0e72cd) SHA1(85b77a303378386f2d395da8707f4b638d37833e) )
	ROM_LOAD16_BYTE( "873c06b.e6",   0x20000, 0x10000, CRC(97ad202e) SHA1(fd155aeb691814950711ead3bc2c93c67b7b0434) )
	ROM_LOAD16_BYTE( "873c06d.e5",   0x20001, 0x10000, CRC(8393d42e) SHA1(ffcb5eca3f58994e05c49d803fa4831c0213e2e2) )
	ROM_LOAD16_BYTE( "873c07a.f4",   0x40000, 0x10000, CRC(a8aab84f) SHA1(a68521a9abf45c3292b3090a2483edbf31356c7d) )
	ROM_LOAD16_BYTE( "873c07c.f3",   0x40001, 0x10000, CRC(2521009a) SHA1(6546b88943615389c81b753ff5bb6aa9378c3266) )
	ROM_LOAD16_BYTE( "873c07b.e4",   0x60000, 0x10000, CRC(12a2b8ba) SHA1(ffa32ca116e0b6ca65bb9ce83dd28f5c027956a5) )
	ROM_LOAD16_BYTE( "873c07d.e3",   0x60001, 0x10000, CRC(fae9f965) SHA1(780c234507835c37bde445ab34f069714cc7a506) )

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "873c04a.f11",  0x00000, 0x10000, CRC(f7740bf3) SHA1(f64b7e807f19a9523a517024a9eb56736cdda6bb) ) /* Sprites */
	ROM_LOAD16_BYTE( "873c04c.f10",  0x00001, 0x10000, CRC(5dacbd2b) SHA1(deb943b99fd296d20be9c4250b2348549f65ba37) )
	ROM_LOAD16_BYTE( "873c04b.e11",  0x20000, 0x10000, CRC(9ac581da) SHA1(fd0a603de8586621444055bbff8bb83349b8a0d8) )
	ROM_LOAD16_BYTE( "873c04d.e10",  0x20001, 0x10000, CRC(44a4668c) SHA1(6d1526ed3408ddc763a071604e7b1e0773c87b99) )
	ROM_LOAD16_BYTE( "873c05a.f9",   0x40000, 0x10000, CRC(d73e107d) SHA1(ba63b195e20a98c476e7d0f8d0187bc3327a8822) )
	ROM_LOAD16_BYTE( "873c05c.f8",   0x40001, 0x10000, CRC(59903200) SHA1(d076802c53aa604df8c5fdd33cb41876ba2a3385) )
	ROM_LOAD16_BYTE( "873c05b.e9",   0x60000, 0x10000, CRC(81059b99) SHA1(1e1a22ca45599abe0dce32fc0b188281deb3b8ac) )
	ROM_LOAD16_BYTE( "873c05d.e8",   0x60001, 0x10000, CRC(7fa3d7df) SHA1(c78b9a949abdf44366d872daa1f2041158fae790) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "873a08.f20",   0x0000, 0x0100, CRC(e2d09a1b) SHA1(a9651e137486b2df367c39eb43f52d0833589e87) )    /* priority encoder (not used) */
ROM_END

ROM_START( thunderxb ) /* Set had no labels, same starting stage as parent set */
	ROM_REGION( 0x28000, "maincpu", 0 ) /* ROMs + banked RAM */
	ROM_LOAD( "873-03.k15", 0x10000, 0x10000, CRC(36680a4e) SHA1(9b3b6bf75a9c04e764448cd958277bd081cc4a53) )
	ROM_LOAD( "873-02.k13", 0x20000, 0x08000, CRC(c58b2c34) SHA1(4050d2edc579ffedba3d40782a08e43ac89b1b86) )
	ROM_CONTINUE(           0x08000, 0x08000 )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "873-f01.f8",   0x0000, 0x8000, CRC(ea35ffa3) SHA1(91e82b77d4f3af8238fb198db26182bebc5026e4) )

	ROM_REGION( 0x80000, "gfx1", 0 )    /* temporary space for graphics (disposed after conversion) */
	ROM_LOAD16_BYTE( "873c06a.f6",   0x00000, 0x10000, CRC(0e340b67) SHA1(a76b1ee4bd4c99826a02b63a705447d0ba4e7b01) ) /* Chars */
	ROM_LOAD16_BYTE( "873c06c.f5",   0x00001, 0x10000, CRC(ef0e72cd) SHA1(85b77a303378386f2d395da8707f4b638d37833e) )
	ROM_LOAD16_BYTE( "873c06b.e6",   0x20000, 0x10000, CRC(97ad202e) SHA1(fd155aeb691814950711ead3bc2c93c67b7b0434) )
	ROM_LOAD16_BYTE( "873c06d.e5",   0x20001, 0x10000, CRC(8393d42e) SHA1(ffcb5eca3f58994e05c49d803fa4831c0213e2e2) )
	ROM_LOAD16_BYTE( "873c07a.f4",   0x40000, 0x10000, CRC(a8aab84f) SHA1(a68521a9abf45c3292b3090a2483edbf31356c7d) )
	ROM_LOAD16_BYTE( "873c07c.f3",   0x40001, 0x10000, CRC(2521009a) SHA1(6546b88943615389c81b753ff5bb6aa9378c3266) )
	ROM_LOAD16_BYTE( "873c07b.e4",   0x60000, 0x10000, CRC(12a2b8ba) SHA1(ffa32ca116e0b6ca65bb9ce83dd28f5c027956a5) )
	ROM_LOAD16_BYTE( "873c07d.e3",   0x60001, 0x10000, CRC(fae9f965) SHA1(780c234507835c37bde445ab34f069714cc7a506) )

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "873c04a.f11",  0x00000, 0x10000, CRC(f7740bf3) SHA1(f64b7e807f19a9523a517024a9eb56736cdda6bb) ) /* Sprites */
	ROM_LOAD16_BYTE( "873c04c.f10",  0x00001, 0x10000, CRC(5dacbd2b) SHA1(deb943b99fd296d20be9c4250b2348549f65ba37) )
	ROM_LOAD16_BYTE( "873c04b.e11",  0x20000, 0x10000, CRC(9ac581da) SHA1(fd0a603de8586621444055bbff8bb83349b8a0d8) )
	ROM_LOAD16_BYTE( "873c04d.e10",  0x20001, 0x10000, CRC(44a4668c) SHA1(6d1526ed3408ddc763a071604e7b1e0773c87b99) )
	ROM_LOAD16_BYTE( "873c05a.f9",   0x40000, 0x10000, CRC(d73e107d) SHA1(ba63b195e20a98c476e7d0f8d0187bc3327a8822) )
	ROM_LOAD16_BYTE( "873c05c.f8",   0x40001, 0x10000, CRC(59903200) SHA1(d076802c53aa604df8c5fdd33cb41876ba2a3385) )
	ROM_LOAD16_BYTE( "873c05b.e9",   0x60000, 0x10000, CRC(81059b99) SHA1(1e1a22ca45599abe0dce32fc0b188281deb3b8ac) )
	ROM_LOAD16_BYTE( "873c05d.e8",   0x60001, 0x10000, CRC(7fa3d7df) SHA1(c78b9a949abdf44366d872daa1f2041158fae790) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "873a08.f20",   0x0000, 0x0100, CRC(e2d09a1b) SHA1(a9651e137486b2df367c39eb43f52d0833589e87) )    /* priority encoder (not used) */
ROM_END

ROM_START( thunderxj )
	ROM_REGION( 0x28000, "maincpu", 0 ) /* ROMs + banked RAM */
	ROM_LOAD( "873-n03.k15", 0x10000, 0x10000, CRC(a01e2e3e) SHA1(eba0d95dc0c5eed18743a96e4bbda5e60d5d9c97) )
	ROM_LOAD( "873-n02.k13", 0x20000, 0x08000, CRC(55afa2cc) SHA1(5fb9df0c7c7c0c2029dbe0f3c1e0340234a03e8a) )
	ROM_CONTINUE(            0x08000, 0x08000 )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "873-f01.f8",   0x0000, 0x8000, CRC(ea35ffa3) SHA1(91e82b77d4f3af8238fb198db26182bebc5026e4) )

	ROM_REGION( 0x80000, "gfx1", 0 )    /* temporary space for graphics (disposed after conversion) */
	ROM_LOAD16_BYTE( "873c06a.f6",   0x00000, 0x10000, CRC(0e340b67) SHA1(a76b1ee4bd4c99826a02b63a705447d0ba4e7b01) ) /* Chars */
	ROM_LOAD16_BYTE( "873c06c.f5",   0x00001, 0x10000, CRC(ef0e72cd) SHA1(85b77a303378386f2d395da8707f4b638d37833e) )
	ROM_LOAD16_BYTE( "873c06b.e6",   0x20000, 0x10000, CRC(97ad202e) SHA1(fd155aeb691814950711ead3bc2c93c67b7b0434) )
	ROM_LOAD16_BYTE( "873c06d.e5",   0x20001, 0x10000, CRC(8393d42e) SHA1(ffcb5eca3f58994e05c49d803fa4831c0213e2e2) )
	ROM_LOAD16_BYTE( "873c07a.f4",   0x40000, 0x10000, CRC(a8aab84f) SHA1(a68521a9abf45c3292b3090a2483edbf31356c7d) )
	ROM_LOAD16_BYTE( "873c07c.f3",   0x40001, 0x10000, CRC(2521009a) SHA1(6546b88943615389c81b753ff5bb6aa9378c3266) )
	ROM_LOAD16_BYTE( "873c07b.e4",   0x60000, 0x10000, CRC(12a2b8ba) SHA1(ffa32ca116e0b6ca65bb9ce83dd28f5c027956a5) )
	ROM_LOAD16_BYTE( "873c07d.e3",   0x60001, 0x10000, CRC(fae9f965) SHA1(780c234507835c37bde445ab34f069714cc7a506) )

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "873c04a.f11",  0x00000, 0x10000, CRC(f7740bf3) SHA1(f64b7e807f19a9523a517024a9eb56736cdda6bb) ) /* Sprites */
	ROM_LOAD16_BYTE( "873c04c.f10",  0x00001, 0x10000, CRC(5dacbd2b) SHA1(deb943b99fd296d20be9c4250b2348549f65ba37) )
	ROM_LOAD16_BYTE( "873c04b.e11",  0x20000, 0x10000, CRC(9ac581da) SHA1(fd0a603de8586621444055bbff8bb83349b8a0d8) )
	ROM_LOAD16_BYTE( "873c04d.e10",  0x20001, 0x10000, CRC(44a4668c) SHA1(6d1526ed3408ddc763a071604e7b1e0773c87b99) )
	ROM_LOAD16_BYTE( "873c05a.f9",   0x40000, 0x10000, CRC(d73e107d) SHA1(ba63b195e20a98c476e7d0f8d0187bc3327a8822) )
	ROM_LOAD16_BYTE( "873c05c.f8",   0x40001, 0x10000, CRC(59903200) SHA1(d076802c53aa604df8c5fdd33cb41876ba2a3385) )
	ROM_LOAD16_BYTE( "873c05b.e9",   0x60000, 0x10000, CRC(81059b99) SHA1(1e1a22ca45599abe0dce32fc0b188281deb3b8ac) )
	ROM_LOAD16_BYTE( "873c05d.e8",   0x60001, 0x10000, CRC(7fa3d7df) SHA1(c78b9a949abdf44366d872daa1f2041158fae790) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "873a08.f20",   0x0000, 0x0100, CRC(e2d09a1b) SHA1(a9651e137486b2df367c39eb43f52d0833589e87) )    /* priority encoder (not used) */
ROM_END

/***************************************************************************/

static KONAMI_SETLINES_CALLBACK( thunderx_banking )
{
	//logerror("thunderx %04x: bank select %02x\n", device->cpu->safe_pc(), lines);
	device->machine().root_device().membank("bank1")->set_entry(((lines & 0x0f) ^ 0x08));
}

GAME( 1988, scontra,   0,        scontra,  scontra, driver_device,  0, ROT90, "Konami", "Super Contra", GAME_SUPPORTS_SAVE )
GAME( 1988, scontraj,  scontra,  scontra,  scontra, driver_device,  0, ROT90, "Konami", "Super Contra (Japan)", GAME_SUPPORTS_SAVE )
GAME( 1988, thunderx,  0,        thunderx, thunderx, driver_device, 0, ROT0,  "Konami", "Thunder Cross (set 1)", GAME_SUPPORTS_SAVE )
GAME( 1988, thunderxa, thunderx, thunderx, thunderx, driver_device, 0, ROT0,  "Konami", "Thunder Cross (set 2)", GAME_SUPPORTS_SAVE )
GAME( 1988, thunderxb, thunderx, thunderx, thunderx, driver_device, 0, ROT0,  "Konami", "Thunder Cross (set 3)", GAME_SUPPORTS_SAVE )
GAME( 1988, thunderxj, thunderx, thunderx, thnderxj, driver_device, 0, ROT0,  "Konami", "Thunder Cross (Japan)", GAME_SUPPORTS_SAVE )
