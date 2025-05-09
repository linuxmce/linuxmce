/*
 * section and descriptor parser
 *
 * Copyright (C) 2005 Kenneth Aafloy (kenneth@linuxtv.org)
 * Copyright (C) 2005 Andrew de Quincey (adq_dvb@lidskialf.net)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef _UCSI_DVB_DIT_SECTION_H
#define _UCSI_DVB_DIT_SECTION_H 1

#ifdef __cplusplus
extern "C"
{
#endif

#include <ucsi/section.h>

/**
 * dvb_dit_section structure.
 */
struct dvb_dit_section {
	struct section head;

  EBIT2(uint8_t transition_flag : 1; ,
	uint8_t reserved 	: 7; );
};

/**
 * Process a dvb_dit_section.
 *
 * @param section Pointer to a generic section header.
 * @return Pointer to a dvb_dit_section, or NULL on error.
 */
struct dvb_dit_section * dvb_dit_section_codec(struct section *section);

#ifdef __cplusplus
}
#endif

#endif
