/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef Tribunexml_h
#define RFID_Reader_h

//#include <cstdlib>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <string.h>
#define MaxBuf 1024
using namespace std;

const char *mappedDevice[30]={
	"Cable A","Cable B","-","Rebuild","-","Device D","Non-addressable","Hamlin converter","Jerrold converter","Jerrold converter","-","Pioneer converter","Oak converter","-","-","Cable Ready","-","Tocom converter","Cable Ready Cable A","Cable Ready Cable B","Scientific-Atlanta","Digital","-","Zenith converter"};

typedef struct {
	string tf_station_num;
	string tf_station_time_zone;
	string tf_station_name;
	string tf_station_call_sign;
	string tf_station_affil;
	string tf_station_city;
	string tf_station_state;
	string tf_station_zip_code;
	string tf_station_country;
	string tf_dma_name;
	string tf_dma_num;
	string tf_fcc_channel_num;
	string tf_user_data;
} Station;

typedef struct {
	string tf_program;
	string tf_cast_first_name;
	string tf_cast_last_name;
	string tf_cast_role_desc;
} Crew;

typedef struct {
	string tf_program;
	string tf_genre_desc;
} Genre;

typedef struct {
	string tf_database_key;
	string tf_title;
	string tf_reduced_title[4];
	string tf_alt_title;
	string tf_reduced_desc[4];
	string tf_advisory_desc[6];
	string tf_desc[2];
	string tf_year;
	string tf_mpaa_rating;
	string tf_star_rating;
	string tf_run_time;
	string tf_color_code;
	string tf_program_language;
	string tf_org_country;
	string tf_made_for_tv;
	string tf_source_type;
	string tf_show_type;
	string tf_holiday;
	string tf_syn_epi_num;
	string tf_alt_syn_epi_num;
	string tf_epi_title;
	string tf_net_syn_source;
	string tf_net_syn_type;
	string tf_org_studio;
	string tf_game_date;
	string tf_game_time;
	string tf_game_time_xone;
	string tf_org_air_date;
	string tf_unique_id;
	string tf_user_data;
	string tf_;
} Program;

typedef struct {
	string tf_station_num;
	string tf_database_key;
	string tf_air_date;
	string tf_air_time;
	string tf_duration;
	string tf_part_num;
	string tf_num_of_parts;
	string tf_cc;
	string tf_stereo;
	string tf_repeat;
	string tf_live_tape_delay;
	string tf_subtitled;
	string tf_premiere_finale;
	string tf_joined_in_progress;
	string tf_cable_in_the_classroom;
	string tf_tv_raiting;
	string tf_sap;
	string tf_blackout;
	string tf_sex_raiting;
	string tf_violence_raiting;
	string tf_language_raiting;
	string tf_dialog_raiting;
	string tf_fv_raiting;
	string tf_enhanced;
	string tf_three_d;
	string tf_letterbox;
	string tf_hdtv;
	string tf_dolby;
	string tf_dvs;
	string tf_user_data;
} Schedule;

typedef struct {
	string cl_row_id;
	string cl_headend_id;
	string cl_device;
	string cl_station_num;
	string cl_tms_chan;
	string cl_service_tier;
	string cl_effective_date;
	string cl_expiration_date;
} Lineup;

typedef struct {
	string he_headend_id;
	string he_he_communiti_name;
	string he_county_name;
	string he_county_size;
	string he_st_county_code;
	string he_state_served;
	string he_zip_code;
	string he_dma_code;
	string he_dma_name;
	string he_mso_code;
	string he_dma_rank;
	string he_headend_name;
	string he_headend_location;
	string he_mso_name;
	string he_time_zone_code;
} Headend;

typedef struct {
	string name;
	string postalCode;
	string station_num;
	string device;
	string type;
	string channel;
	string effective_date;
	string expiration_date;
} ChannelLineup;

#endif
