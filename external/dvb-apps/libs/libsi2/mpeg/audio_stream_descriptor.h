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

#ifndef _SI_MPEG_AUDIO_STREAM_DESCRIPTOR
#define _SI_MPEG_AUDIO_STREAM_DESCRIPTOR 1

#include <si/descriptor.h>
#include <si/common.h>

struct mpeg_audio_stream_descriptor {
	struct descriptor d;

  EBIT5(uint8_t free_format_flag		: 1; ,
	uint8_t id				: 1; ,
	uint8_t layer				: 2; ,
	uint8_t variable_rate_audio_indicator	: 1; ,
	uint8_t reserved			: 3; );
} packed;

static inline struct mpeg_audio_stream_descriptor*
	mpeg_audio_stream_descriptor_parse(struct descriptor* d)
{
	if (d->len != (sizeof(struct mpeg_audio_stream_descriptor) - 2))
		return NULL;

	return (struct mpeg_audio_stream_descriptor*) d;
}

#endif
