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

#ifndef _SI_ATSC_DESCRIPTOR_H
#define _SI_ATSC_DESCRIPTOR_H 1

#include <si/common.h>

enum atsc_descriptor_tag {

/* A 52/A describes a
 * dtag_atsc_ac3_registration			= 0x05, */

/* A90 describes a
 * dtag_atsc_association_tag			= 0x14, */

	dtag_atsc_stuffing			= 0x80,
	dtag_atsc_ac3_audio			= 0x81,
	dtag_atsc_caption_service		= 0x86,
	dtag_atsc_content_advisory		= 0x87,
	dtag_atsc_ca				= 0x88,
	dtag_atsc_extended_channel_name		= 0xa0,
	dtag_atsc_service_location		= 0xa1,
	dtag_atsc_time_shifted_service		= 0xa2,
	dtag_atsc_component_name		= 0xa3,
	dtag_atsc_data_service			= 0xa4,
	dtag_atsc_pid_count			= 0xa5,
	dtag_atsc_download			= 0xa6,
	dtag_atsc_MPE				= 0xa7,
	dtag_atsc_dcc_departing_request		= 0xa8,
	dtag_atsc_dcc_arriving_request		= 0xa9,
	dtag_atsc_redistribution_control	= 0xaa,
	dtag_atsc_dcc_location_code		= 0xab,
	dtag_atsc_private_information		= 0xad,
	dtag_atsc_module_link			= 0xb4,
	dtag_atsc_crc32				= 0xb5,
	dtag_atsc_group_link			= 0xb8,
};

#endif
