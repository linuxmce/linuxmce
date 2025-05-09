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

#include <si/mpeg/pmt_section.h>

struct mpeg_pmt_section * mpeg_pmt_section_parse(struct section_ext * ext)
{
	uint8_t * buf = (uint8_t *) ext;
	struct mpeg_pmt_section * pmt = (struct mpeg_pmt_section *) ext;
	unsigned int pos = sizeof(struct section_ext);
	unsigned int len = section_ext_length(ext);

	if (len < sizeof(struct mpeg_pmt_section))
		return NULL;

	bswap16(buf + pos);
	pos += 2;
	bswap16(buf + pos);
	pos += 2;

	if ((pos + pmt->program_info_length) > len)
		return NULL;

	if (verify_descriptors(buf + pos, pmt->program_info_length))
		return NULL;

	pos += pmt->program_info_length;

	while (pos < len) {
		struct mpeg_pmt_stream * stream =
			(struct mpeg_pmt_stream *) (buf + pos);

		if ((pos + sizeof(struct mpeg_pmt_stream)) > len)
			return NULL;

		bswap16(buf + pos + 1);
		bswap16(buf + pos + 3);
		pos += sizeof(struct mpeg_pmt_stream);

		if ((pos + stream->es_info_length) > len)
			return NULL;

		if (verify_descriptors(buf + pos, stream->es_info_length))
			return NULL;

		pos += stream->es_info_length;
	}

	if (pos != len)
		return NULL;

	return (struct mpeg_pmt_section *) ext;
}
