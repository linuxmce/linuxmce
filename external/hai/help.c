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

/* Includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hai.h"
#include "hai_net.h"
#include "help.h"

#define HELP_FORMAT "\nFormat:\t%s %s\n\n\t%s\n\t%s"

/********************************************************************************/

int do_help(hai_comm_id *id, int argc, char *argv[])
{
    if (argc < 2)
    {
        cmd_item *list;
        int len = 0;

        /* Find longest command */
        list = cmd_list;
        while (list->cmd_text != NULL)
        {
            if (((int) strlen(list->cmd_text)) > len)
                len = strlen(list->cmd_text);
            list++;
        }

        /* Display list of commands */
        printf("\n");
        list = cmd_list;
        while (list->cmd_text != NULL)
        {
            int i;

            printf("%s", list->cmd_text);
            for (i = 0; i < (len - ((int) strlen(list->cmd_text)) + 2); i++)
                printf(" ");
            printf("%s\n", list->desc);
            list++;
        }
    }
    else
    {
        cmd_item *cmd = cmd_list;

        while (cmd->cmd_text != NULL)
        {
            if (strcmp_no_case(cmd->cmd_text, argv[1]) == 0)
            {
                if (cmd->help != NULL)
                    (cmd->help)(argv[1]);
                break;
            }
            cmd++;
        }
    }

    return 0;
}

/********************************************************************************/

void help_help(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<command>]",
    "Displays help info on <command>. If no command is specified,",
    "then a list of commands with a short description is shown.\n");
}

/********************************************************************************/

void help_encrypt(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<password>",
    "Displays encrypted versions of the current ip, port, key, an code. These",
    "can be used in the configuration file.");
}

/********************************************************************************/

void help_csv(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<on/off>",
    "Turns CSV mode on or off. CSV mode outputs all info as comma separated ",
    "values for easy parsing by other programs. Command-line option also.");
}

/********************************************************************************/

void help_sys_info(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "",
    "Displays the HAI system information.",
    "");
}

/********************************************************************************/

void help_sys_stat(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "",
    "Displays the HAI system status.",
    "");
}

/********************************************************************************/

void help_zone_stat(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<start zone>] [<end zone>]",
    "Displays the status of a set of zones.",
    "Default is all named. Specify 'all' to include unnamed.");
}

/********************************************************************************/

void help_unit_stat(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<start unit>] [<end unit>]",
    "Displays the status of a set of units.",
    "Default is all named. Specify 'all' to include unnamed.");
}

/********************************************************************************/

void help_sensor_stat(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<start sensor>] [<end sensor>]",
    "Displays the status of a set of temperature sensors.",
    "Default is all named. Specify 'all' to include unnamed.");
}

/********************************************************************************/

void help_thermo_stat(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<start thermostat>] [<end thermostat>]",
    "Displays the status of a set of thermostats.",
    "Default is all named. Specify 'all' to include unnamed.");
}

/********************************************************************************/

void help_mesg_stat(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<start mesg>] [<end mesg>]",
    "Displays the status of a set of messages.",
    "Default is all named. Specify 'all' to include unnamed.");
}

/********************************************************************************/

void help_usersetting_stat(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<start user setting>] [<end user setting>]",
    "Displays the status of a set of user settings.",
    "Default is all named. Specify 'all' to include unnamed.");
}

/********************************************************************************/

void help_events(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "",
    "Displays the system events since last display.",
    "");
}

/********************************************************************************/

void help_unit_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<unit> <command> <parms>",
    "Send command to unit. Command is on/off/dim/brighten/level/ramp\n        unit <unit> <on|off|> [<time>]\n        unit <unit> <dim|brighten> <step> [<time>]\n        unit <unit> level <level> [<time>]\n        unit <unit> ramp <level> [<time>]",
    "Step is 1 to 9. Level is 0 to 100. ALC ramp is to <level> over <time>.\n        Time is #h, #m, or #s. (ex: 20m = 20 minutes)\n        Time can be a duration user-setting (ex: unit 10 on user 7).");
}

/********************************************************************************/

void help_all_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<on/off> [<area>]",
    "Send all on/off command.",
    "");
}

/********************************************************************************/

void help_counter_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<counter> <inc/dec/set> [<value>]",
    "Increment, decrement or set to <value> a counter.",
    "");
}

/********************************************************************************/

void help_alc_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<unit> <level>  <time>",
    "Ramp ALC unit to <level> over <time>. Level is 0-100.",
    "Time is #h, #m, or #s.\n");
}

/********************************************************************************/

void help_compose_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<unit> <off/on/scene> [<scene>]",
    "Send HLC Room or Lightolier Compose command. Optional scene is A-L",
    "");
}

/********************************************************************************/

void help_upb_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<unit> <stat/link/led> [<led num>] [<on/off/set>]",
    "Send UPB command. Valid options for link are on/off/set. Valid",
    "options for led are on/off. Stat will request status message.\n");
}

/********************************************************************************/

void help_radiora_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<unit> <off/on>",
    "Send RadioRA phantom button command.",
    "");
}

/********************************************************************************/

void help_scene_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<scene> <off/on/set>",
    "Send scene command.",
    "");
}

/********************************************************************************/

void help_arm_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<mode> [<area>]",
    "Send arm command. Mode is off/day/night/vac/dayi/nightd.",
    "Default area is 0 (all)\n");
}

/********************************************************************************/

void help_button_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<button>",
    "Execute macro button.",
    "");
}

/********************************************************************************/

void help_saver_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<saver> <command> [<time>]",
    "Send command to energy saver. Command is on/off.",
    "Optional time is #h, #m, or #s.\n");
}

/********************************************************************************/

void help_cost_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<low/mid/high/critical>",
    "Set energy cost.",
    "");
}

/********************************************************************************/

void help_sensor_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<sensor> <high/low> <temp>",
    "Set temp sensor setpoint. Temp is F (default) or C (ex: 20C).",
    "");
}

/********************************************************************************/

void help_thermo_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<thermostat> <mode/fan/hold/heat/cool> <parm>",
    "Send thermostat command. mode: off/heat/cool/auto; fan: auto/on/cycle,",
    "hold: off/on; heat/cool temp is F (default) or C (ex: 20C).\n");
}

/********************************************************************************/

void help_mesg_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<command> [<mesg num>] [<serial port>] [<phone num>]",
    "Send message. Command is show/log/clear/say/phone/send. Clear all if no mesg num",
    "on clear. Send requires serial port number. Phone requires index of phone number.\n");
}

/********************************************************************************/

void help_audio_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<zone> <command> [<argument>]",
    "Send audio command. Command is on/off/mute/vol/src/key. Argument for mute is",
    "on/off, for vol is 0-100, for src is 1-n, for key is 1-40.\n");
}

/********************************************************************************/

void help_user_cmd(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<user setting num> <user setting type> <value>",
    "Set user setting value. Type is number/duration/temp/humidity/date/time/days/level.",
    "number is 1-255; duration is 1-99 seconds or minutes, or 1-18 hours (ex: 10m);\n\ttemp is F (default) or C (ex: 20C); humidity and level are 1-100; date is MM/DD (ex: 01/15);\n\ttime is 24hr HH:MM (ex: 16:30); days is MTWTFSS with - to turn off days (ex: M-W-F--).\n");
}

/********************************************************************************/

void help_getnames(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<filename>]",
    "Displays all names. Optionally download names to",
    "file <filename>. Updates name cache.");
}

/********************************************************************************/

void help_putnames(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<filename>",
    "Upload names from file <filename>. Updates name cache.",
    "");
}

/********************************************************************************/

void help_names(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "",
    "Displays names from name cache.",
    "");
}

/********************************************************************************/

void help_buttons(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "",
    "Displays button names from name cache.",
    "");
}

/********************************************************************************/

void help_log(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "",
    "Displays the event log.",
    "");
}

/********************************************************************************/

void help_sec_code_valid(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<area> <code>",
    "Verifies <code> is valid in <area>. displays the code authority",
    "level and number.\n");
}

/********************************************************************************/

void help_set_time(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<MM/DD/YYYY> <HH:MM> <dow> [DST]",
    "Sets the date and time. dow is Mon, Tue, Wed, Thu, Fri, Sat, Sun. DST",
    "flag indicates if in daylight savings time (i.e. Summer).\n");
}

/********************************************************************************/

void help_getsetup(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<filename>]",
    "Displays all setup. Optionally download setup to",
    "file <filename>.");
}

/********************************************************************************/

void help_putsetup(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<filename>",
    "Upload setup from file <filename>.",
    "");
}

/********************************************************************************/

void help_getprogram(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<filename>]",
    "Displays all programs. Optionally download programs to",
    "file <filename>.");
}

/********************************************************************************/

void help_putprogram(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<filename>",
    "Upload programs from file <filename>.",
    "");
}

/********************************************************************************/

void help_getvoices(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<filename>]",
    "Displays all voices. Optionally download voices to",
    "file <filename>.");
}

/********************************************************************************/

void help_putvoices(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<filename>",
    "Upload voices from file <filename>.",
    "");
}

/********************************************************************************/

void help_stat_summary(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "",
    "Displays the HAI status summary.",
    "");
}

/********************************************************************************/

void help_temp_stat(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<start temp>] [<end temp>]",
    "Displays the temps from a set of thermostats and temperature sensors.",
    "Default is all named. Specify 'all' to include unnamed.");
}

/********************************************************************************/

void help_zone_ready(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "[<start zone>] [<end zone>]",
    "Displays the ready status of a set of zones.",
    "Default is all named. Specify 'all' to include unnamed.");
}

/********************************************************************************/

void help_emergency(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<burglary/fire/aux> [<area>]",
    "Activate keypad emergency. Type is burglary/fire/aux.",
    "");
}

/********************************************************************************/

void help_memo(const char *cmd_text)
{
    printf(HELP_FORMAT, cmd_text, "<play/record>",
    "Send voice memo message play/record command.",
    "");
}

/********************************************************************************/

int strcmp_no_case (const char *s, const char *t)
{
    for (; toupper(*s) == toupper(*t); s++, t++)
    {
        if (*s == '\0')
        {
            return 0;
        }
    }
    return *s-*t;
}

/********************************************************************************/

int strncmp_no_case (const char *s, const char *t, size_t n)
{
    int i;
    for (i = 0; i < n && toupper(*s) == toupper(*t); i++, s++, t++)
    {
        if (*s == '\0' || i == n - 1)
        {
            return 0;
        }
    }
    return *s-*t;
}

