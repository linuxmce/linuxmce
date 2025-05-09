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

#include <ucsi/mpeg/tsdt_section.h>

struct mpeg_tsdt_section * mpeg_tsdt_section_codec(struct section_ext * ext)
{
	uint8_t * buf = (uint8_t *)ext;
	int pos = sizeof(struct section_ext);

	if (verify_descriptors(buf + pos,
	    section_ext_length(ext) - sizeof(struct mpeg_tsdt_section)))
		return NULL;

	return (struct mpeg_tsdt_section *)ext;
}

