/*******************************************************************************/
/* Copyright (C) 2004-2005  Chuck Cannon                                       */
/*                                                                             */
/* This program is free software; you can redistribute it and/or               */
/* modify it under the terms of the GNU General Public License                 */
/* as published by the Free Software Foundation; either version 2              */
/* of the License, or (at your option) any later version.                      */
/*                                                                             */
/* This program is distributed in the hope that it will be useful,             */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of              */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               */
/* GNU General Public License for more details.                                */
/*                                                                             */
/* You should have received a copy of the GNU General Public License           */
/* along with this program; if not, write to the Free Software                 */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */
/*******************************************************************************/

#ifndef _HELP_H_
#define _HELP_H_

/* Includes */

#include "hai_net.h"

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/* Typedefs */

typedef void (*help_func)(const char *cmd_text);

/* Function prototypes */

int do_help(hai_comm_id *id, int argc, char *argv[]);
void help_help(const char *cmd_text);
void help_encrypt(const char *cmd_text);
void help_csv(const char *cmd_text);
void help_sys_info(const char *cmd_text);
void help_sys_stat(const char *cmd_text);
void help_zone_stat(const char *cmd_text);
void help_usersetting_stat(const char *cmd_text);
void help_unit_stat(const char *cmd_text);
void help_sensor_stat(const char *cmd_text);
void help_thermo_stat(const char *cmd_text);
void help_mesg_stat(const char *cmd_text);
void help_events(const char *cmd_text);
void help_unit_cmd(const char *cmd_text);
void help_all_cmd(const char *cmd_text);
void help_counter_cmd(const char *cmd_text);
void help_alc_cmd(const char *cmd_text);
void help_compose_cmd(const char *cmd_text);
void help_upb_cmd(const char *cmd_text);
void help_radiora_cmd(const char *cmd_text);
void help_scene_cmd(const char *cmd_text);
void help_arm_cmd(const char *cmd_text);
void help_button_cmd(const char *cmd_text);
void help_cost_cmd(const char *cmd_text);
void help_saver_cmd(const char *cmd_text);
void help_sensor_cmd(const char *cmd_text);
void help_thermo_cmd(const char *cmd_text);
void help_mesg_cmd(const char *cmd_text);
void help_audio_cmd(const char *cmd_text);
void help_user_cmd(const char *cmd_text);
void help_getnames(const char *cmd_text);
void help_putnames(const char *cmd_text);
void help_names(const char *cmd_text);
void help_buttons(const char *cmd_text);
void help_log(const char *cmd_text);
void help_sec_code_valid(const char *cmd_text);
void help_set_time(const char *cmd_text);
void help_getsetup(const char *cmd_text);
void help_putsetup(const char *cmd_text);
void help_getprogram(const char *cmd_text);
void help_putprogram(const char *cmd_text);
void help_getvoices(const char *cmd_text);
void help_putvoices(const char *cmd_text);
void help_stat_summary(const char *cmd_text);
void help_temp_stat(const char *cmd_text);
void help_zone_ready(const char *cmd_text);
void help_emergency(const char *cmd_text);
void help_memo(const char *cmd_text);
int strcmp_no_case (const char *s, const char *t);
int strncmp_no_case (const char *s, const char *t, size_t n);

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif /* _HELP_H_ */

