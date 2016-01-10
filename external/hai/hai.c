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
#include <errno.h>
#ifdef GNUREADLINE_SUPPORT
#include <readline/readline.h>
#include <readline/history.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif
#include "hai.h"
#include "hai_net.h"
#include "hai_serial.h"
#include "omni_protocol.h"
#include "help.h"
#include "aes.h"
#ifdef READLINE_SUPPORT
#include "readline.h"
#endif

/* Defines */

#define EINVCMD         __ELASTOMNI + 1
#define EBADCODE        __ELASTOMNI + 2
#define EBADFILE        __ELASTOMNI + 3

#define MAX_LINE        16384
#define MAX_ARG         64

/* Typedefs */
typedef char name_str[MAX_NAME_CHARS];

/* Function prototypes */
void load_parm_file(char **environ);
void dump_parm_file(void);
void load_name_cache(void);
void save_name_cache(void);
int parse_cmdline(int argc, char *argv[]);
int parse_arg(char *arg);
int init_connection(hai_comm_id *id, val32 code);
void term_connection(hai_comm_id *id);
int cmd_loop(hai_comm_id *id);
int do_command(hai_comm_id *id, int argc, char *argv[]);
int do_encrypt(hai_comm_id *id, int argc, char *argv[]);
int do_csv(hai_comm_id *id, int argc, char *argv[]);
void encrypt_bytes(char *key, char *buf_in, int len_in,
    char *buf_out, int *len_out);
void decrypt_bytes(char *key, char *buf_in, int len_in, char *buf_out,
    int *len_out);
int do_sys_info(hai_comm_id *id, int argc, char *argv[]);
int do_sys_stat(hai_comm_id *id, int argc, char *argv[]);
int do_zone_stat(hai_comm_id *id, int argc, char *argv[]);
int do_unit_stat(hai_comm_id *id, int argc, char *argv[]);
int do_sensor_stat(hai_comm_id *id, int argc, char *argv[]);
int do_thermo_stat(hai_comm_id *id, int argc, char *argv[]);
int do_mesg_stat(hai_comm_id *id, int argc, char *argv[]);
int do_usersetting_stat(hai_comm_id *id, int argc, char *argv[]);
int do_events(hai_comm_id *id, int argc, char *argv[]);
int do_unit_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_all_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_counter_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_alc_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_compose_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_upb_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_radiora_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_scene_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_arm_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_button_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_cost_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_saver_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_sensor_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_thermo_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_mesg_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_audio_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_usersetting_cmd(hai_comm_id *id, int argc, char *argv[]);
int do_getnames(hai_comm_id *id, int argc, char *argv[]);
int do_putnames(hai_comm_id *id, int argc, char *argv[]);
int do_names(hai_comm_id *id, int argc, char *argv[]);
int do_buttons(hai_comm_id *id, int argc, char *argv[]);
int do_log(hai_comm_id *id, int argc, char *argv[]);
int do_sec_code_valid(hai_comm_id *id, int argc, char *argv[]);
int do_set_time(hai_comm_id *id, int argc, char *argv[]);
int do_getsetup(hai_comm_id *id, int argc, char *argv[]);
int do_putsetup(hai_comm_id *id, int argc, char *argv[]);
int do_getprogram(hai_comm_id *id, int argc, char *argv[]);
int do_putprogram(hai_comm_id *id, int argc, char *argv[]);
int do_getvoices(hai_comm_id *id, int argc, char *argv[]);
int do_putvoices(hai_comm_id *id, int argc, char *argv[]);
int do_stat_summary(hai_comm_id *id, int argc, char *argv[]);
int do_temp_stat(hai_comm_id *id, int argc, char *argv[]);
int do_zone_ready(hai_comm_id *id, int argc, char *argv[]);
int do_emergency(hai_comm_id *id, int argc, char *argv[]);
int do_memo(hai_comm_id *id, int argc, char *argv[]);
void print_code(FILE *f, int code);
int is_named(int type, int name);
void print_name(FILE *f, int type, int name);
void print_name_fixed(FILE *f, int type, int name);
void print_name_special_zone(int name, char *printStr);

/* Global variables */

char ip[128];
char dev[128];
char haiconf[128];
char password[64];
char hainame[128];
int port = 0, baud = 9600, serial_mode = 0;
unsigned char private_key[16];
val32 code;
val8 code_index;
val8 omni_model;              
val8 omni_major_version;
val8 omni_minor_version;
val8 omni_temp_format;
val8 omni_time_format;
val8 omni_date_format;
/*
  Each panel model has a different number of security zones, "max_zones".
  There are a number of additional zones with status values that vary by model:
  freeze alarm, fire emergency (for each area), police emergency (for each area),
  auxiliary emergency (for each area), duress alarm (for each area),
  battery low trouble, AC power failure trouble, phone line dead trouble,
  digital communicator trouble, fire tamper trouble, fuse trouble.
  */
int max_zones;
int max_zones_status;
int max_units;  
/*
  There are a possible 255 buttons that can be programmed (Buttons 1-255),
  "max_buttons" of which can be named, given a voice description, and executed
  by the user through a console, telephone or PC Access Software.
  Buttons max_buttons+1 to 255 cannot be named or given a voice description,
  and can only be executed through a program. "max_buttons" is different for
  each panel model and ranges from 16 to 128.
  */             
int max_buttons;
int max_buttons_program;             
int max_auxs;                
int max_codes;               
int max_areas;               
int max_thermos;             
int max_mesgs;               
int max_names;               
int max_temps;              
int max_usersettings;
int max_readers;
int name_cache_valid;
int name_cache_save;
int csv;
int dump_config;

struct {
    name_str zone_name_cache[MAX_ZONES];
    name_str unit_name_cache[MAX_UNITS];
    name_str button_name_cache[MAX_BUTTONS];
    name_str code_name_cache[MAX_CODES];
    name_str area_name_cache[MAX_AREAS];
    name_str thermo_name_cache[MAX_THERMOS];
    name_str mesg_name_cache[MAX_MESGS];
    name_str user_name_cache[MAX_USERS];
    name_str reader_name_cache[MAX_READERS];
} name_cache;
name_str *name_index[NUM_NAME_TYPES] =
{
    name_cache.zone_name_cache,
    name_cache.unit_name_cache,
    name_cache.button_name_cache,
    name_cache.code_name_cache,
    name_cache.area_name_cache,
    name_cache.thermo_name_cache,
    name_cache.mesg_name_cache,
    name_cache.user_name_cache,
    name_cache.reader_name_cache
};

const char *dow_text[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const char *month_text[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *sec_text[] = {"off", "day", "night", "away", "vacation",
    "day instant", "night delayed"};
const char *lumina_sec_text[] = {"off", "home", "sleep", "away", "vacation",
    "party", "special"};
const char *zone_text[] = {"Secure", "Not ready", "Trouble"};
const char *latched_text[] = {"Latch Secure", "Latch Tripped", "Latch Reset"};
const char *arming_text[] = {"Disarmed", "Armed", "User bypass",
    "System bypass"};
const char *name_text[] = {"Zone", "Unit", "Button", "Code", "Area",
    "Thermo", "Mesg", "User", "Reader"};
const char *alarm_text[] = {"Burglary", "Fire", "Gas", "Auxiliary", "Freeze",
    "Water", "Duress", "Temp"};
const char *ac_text[] = {"Off", "Heat", "Cool", "Auto", "Emergency Heat"};  
const char *upb_text[] = {"Off", "On", "Set"};  
const char *usersetting_text[] = {"Unused", "Number", "Duration", "Temperature", "Humidity", "Date", "Time", "Days of Week", "Level"};  

cmd_item cmd_list[] =
{
    {"help", "display help info",
    do_help, help_help},
    {"h", "display help info",
    do_help, help_help},
    {"?", "display help info",
    do_help, help_help},
    {"exit", "quit program",
    NULL, NULL},
    {"x", "quit program",
    NULL, NULL},
    {"quit", "quit program",
    NULL, NULL},
    {"q", "quit program",
    NULL, NULL},
    {"encrypt", "display encrypted configuration",
    do_encrypt, help_encrypt},
    {"csv", "change display mode",
    do_csv, help_csv},

    /* Status */
    {"info", "display system information",
    do_sys_info, help_sys_info},
    {"stat", "display system status",
    do_sys_stat, help_sys_stat},
    {"sum", "display status summary",
    do_stat_summary, help_stat_summary},
    {"zones", "display zone status",
    do_zone_stat, help_zone_stat},
    {"ready", "display zone ready status",
    do_zone_ready, help_zone_ready},
    {"units", "display unit status",
    do_unit_stat, help_unit_stat},
    {"sensors", "display temp sensor status",
    do_sensor_stat, help_sensor_stat},
    {"thermos", "display thermostat status",
    do_thermo_stat, help_thermo_stat},
    {"temps", "display temps from thermos & sensors",
    do_temp_stat, help_temp_stat},
    {"mesgs", "display message status",
    do_mesg_stat, help_mesg_stat},
    {"users", "display user setting status",
    do_usersetting_stat, help_usersetting_stat},

    /* System events */
    {"events", "display system events",
    do_events, help_events},

    /* Commands */
    {"unit", "send unit command",
    do_unit_cmd, help_unit_cmd},
    {"all", "send all command",
    do_all_cmd, help_all_cmd},
    {"counter", "send counter command",
    do_counter_cmd, help_counter_cmd},
    {"alc", "send alc command",
    do_alc_cmd, help_alc_cmd},
    {"compose", "send compose command",
    do_compose_cmd, help_compose_cmd},
    {"upb", "send upb command",
    do_upb_cmd, help_upb_cmd},
    {"radiora", "send RadioRA command",
    do_radiora_cmd, help_radiora_cmd},
    {"scene", "send scene command",
    do_scene_cmd, help_scene_cmd},
    {"arm", "send arm command",
    do_arm_cmd, help_arm_cmd},
    {"button", "execute macro button",
    do_button_cmd, help_button_cmd},
    {"cost", "set energy cost",
    do_cost_cmd, help_cost_cmd},
    {"saver", "send saver command",
    do_saver_cmd, help_saver_cmd},
    {"sensor", "set temp sensor setpoint",
    do_sensor_cmd, help_sensor_cmd},
    {"thermo", "send thermostat commands",
    do_thermo_cmd, help_thermo_cmd},
    {"mesg", "send message commands",
    do_mesg_cmd, help_mesg_cmd},
    {"audio", "send audio commands",
    do_audio_cmd, help_audio_cmd},
    {"user", "send user commands",
    do_usersetting_cmd, help_user_cmd},

    /* Names */
    {"getnames", "display/download names",
    do_getnames, help_getnames},
    {"putnames", "upload names",
    do_putnames, help_putnames},
    {"names", "display names",
    do_names, help_names},
    {"buttons", "display button names",
    do_buttons, help_buttons},

    /* Event log */
    {"log", "display event log",
    do_log, help_log},

    /* Validate security codes */
    {"valid", "validate security code",
    do_sec_code_valid, help_sec_code_valid},

    /* Misc. commands */
    {"emergency", "activate keypad emergency",
    do_emergency, help_emergency},
    {"memo", "play/record voice memo",
    do_memo, help_memo},

    /* Undocumented commands */
    {"settime", "set time",
    do_set_time, help_set_time},   
    {"getsetup", "display/download setup",
    do_getsetup, help_getsetup},
    {"putsetup", "upload setup",
    do_putsetup, help_putsetup},
    {"getprogram", "display/download program",
    do_getprogram, help_getprogram},
    {"putprogram", "upload program",
    do_putprogram, help_putprogram},
    {"getvoices", "display/download voices",
    do_getvoices, help_getvoices},
    {"putvoices", "upload voices",
    do_putvoices, help_putvoices},

    /* End of commands */
    {NULL, NULL, NULL, NULL}
};

int max_names_array[NUM_NAME_TYPES] = {MAX_ZONES, MAX_UNITS, MAX_BUTTONS,
    MAX_CODES, MAX_AREAS, MAX_THERMOS, MAX_MESGS, MAX_USERS, MAX_READERS};

/********************************************************************************/

int main(int argc, char **argv, char **environ)
{
    hai_comm_id id;
    int err = 0, nonparm;

    /* Init parms */
    *ip = 0;
    *dev = 0;
    *private_key = 0;
    *code = 0;
    *password = 0;
    *haiconf = 0;
    *hainame = 0;
    csv = 0;
    dump_config = 0;
    name_cache_valid = 0;

    /* Parse command line */
    if ((nonparm = parse_cmdline(argc, argv)) < 0)
    {
        if (nonparm != -999999) {
        printf("error: Unknown parameter %s\n", argv[-nonparm]);
        }
        return -1;
    }

    /* Load parm file */
    load_parm_file(environ);

    /* Display parm file */
    if (dump_config)
        dump_parm_file();

    /* Load name cache */
    load_name_cache();   

    /* Init connection */
    if ((err = init_connection(&id, code)) != 0)
    {
        printf("error initializing connection\n");
        if (err == EHAITIMEOUT)
            goto err_exit;
        else
            goto clean_exit;
    }

    /* Process command(s) */
    if (nonparm == argc)
    {
        /* Loop on commands */
        if ((err = cmd_loop(&id)) != 0)
            goto clean_exit;
    }
    else
        /* Single command */
        err = do_command(&id, argc - nonparm, argv + nonparm);

    /* Term connection */
clean_exit:
    term_connection(&id);

err_exit:
    /* Print error */
    if (err > __ELASTOMNI)
    {
        switch(err)
        {
            case EINVCMD :
                printf("error (%d) : Invalid command\n", err);
                break;
            case EBADCODE :
                printf("error (%d) : Invalid code\n", err);
                break;
            case EBADFILE :
                printf("error (%d) : Bad file format\n", err);
                break;
        }
    }
    else if (err > __ELASTHAI)
    {
        printf("error (%d) : %s\n", err, omni_strerror(err));
    }
    else if (err > __ELASTERROR)
    {
        printf("error (%d) : %s\n", err, hai_net_strerror(err));
    }
    else if (err != 0)
    {
        printf("error (%d) : %s\n", err, strerror(err));
    }

    /* Save name cache */
    save_name_cache();

    return err;
}

/********************************************************************************/

void load_parm_file(char **environ)
{
    char *tok, *parm, *value;
    char buffer[MAX_LINE];
    char line[MAX_LINE];
    FILE *f;

    /* Find parm file path in env */
    if (*haiconf == 0)
    {
        while (*environ != NULL)
        {
			if (strlen(*environ) < MAX_LINE)
			{
            strcpy(buffer, *environ);
            tok = buffer;
            parm = strtok(tok, "=\n\r");
            value = strtok(NULL, "\n\r");

            if (strcmp_no_case(parm, "HAICONF") == 0)
            {
                if (value != NULL)
                    strcpy(haiconf, value);
                break;
            }
			}

            environ++;
        }
    }

    /* Default path */ 
    if (*haiconf == 0)
#ifdef WIN32
        strcpy(haiconf, "c:\\windows\\hai.ini");
#else
        strcpy(haiconf, "/etc/hai.conf");
#endif

    /* Open file */
    if ((f = fopen(haiconf, "r")) == NULL)
    {
        printf("bad config file name (%s)\n", haiconf);
        return;
    }

    /* Process lines */
    while (fgets(line, MAX_LINE, f) != NULL)
    {
        if (line[0] != '#')
            parse_arg(line);
    }

    /* Close file */
    fclose(f);
}

/********************************************************************************/

void dump_parm_file(void)
{
    int i;

    printf("Password: %s\n", password);
    printf("Key: ");
    for (i = 0; i < 15; i++)
        printf("%02X-", private_key[i]);
    printf("%02X\n", private_key[15]);
    printf("IP Address: %s\n", ip);
    printf("Port: %d\n", port);
    printf("Code: %c%c%c%c\n", code[0] + '0', code[1] + '0', code[2] + '0',
        code[3] + '0');
    printf("Config File: %s\n", haiconf);
    printf("Name Cache File: %s\n", hainame);
    printf("Serial Mode: %d\n", serial_mode);
    printf("Serial Device: %s\n", dev);
    printf("Baud: %d\n", baud = 9600);
    printf("CSV: %d\n", csv);
}

/********************************************************************************/

void load_name_cache()
{
    char line[MAX_LINE];
    char *name, *anum, *atype;
    int type, num, index;
    name_str *ncache;
    FILE *f;

    /* Zero name cache */
    name_cache_save = 0;
    for (index = 0; index < MAX_NAMES; index++)
        *name_cache.zone_name_cache[index] = 0;

    /* Check file name */
    if (*hainame == 0)
        return;

    /* Open file */
    if ((f = fopen(hainame, "r")) == NULL)
        return;

    /* Put names */
    index = 0;
    while (fgets(line, MAX_LINE, f) != NULL)
    {
        /* Parse input line */
        atype = strtok(line, "\t \r\n");
        anum = strtok(NULL, "\t \r\n");
        strtok(NULL, "\t \r\n");
        name = strtok(NULL, "\r\n");

        /* Skip blank lines */
        if (atype == NULL)
            continue;

        /* Check line parse */
        if ((anum == NULL) || (name == NULL))
            break;

        /* Lookup type */
        num = atoi(anum);
        for (type = 1; type <= NUM_NAME_TYPES ; type++)
        {
            if (strcmp_no_case(atype, name_text[type - 1]) == 0)
                break;
        }
        if (type > NUM_NAME_TYPES)
            break;
        ncache = name_index[type - 1];

        /* Fill in cache */
        strncpy(ncache[num - 1], name, MAX_NAME_CHARS);
    }
    name_cache_valid = 1;

    /* Close file */
    fclose(f);
}

/********************************************************************************/

void save_name_cache()
{
    int type, num, index;
    FILE *f;

    /* Check file name and save flag */
    if ((*hainame == 0) || (name_cache_save == 0))
        return;

    /* Open file */
    if ((f = fopen(hainame, "w")) == NULL)
        return;

    /* Save names */
    type = 1;
    num = 1;
    for (index = 0; index < MAX_NAMES; index++)
    {
        if ((type < NUM_NAME_TYPES) && (name_cache.zone_name_cache[index] 
            == (void*) name_index[type]))
        {
            type++;
            num = 1;
        }
        
        /* Save name */
        if (*name_cache.zone_name_cache[index] != 0)
        {
            /* Print name info */
            fprintf(f, "%-6s", name_text[type - 1]);
            fprintf(f, " %3d : %s\n", num, name_cache.zone_name_cache[index]);
        }

        /* Increment number */
        num++;
    }
 
    /* Close file */
    fclose(f);  
}

/********************************************************************************/

int parse_cmdline(int argc, char *argv[])
{
    int i, stat;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if ((stat = parse_arg(argv[i] + 1)) != 0)
            {
                 if (stat == -999999) return stat;
                return -i;
        }
        }
        else
            break;
    }

    return i;
}

/********************************************************************************/

int parse_arg(char *arg)
{
    char *tok, *parm, *value;
    char buffer[MAX_LINE];
    char key[16];

    /* Check arg */
    if (arg == NULL)
        return 0;
    if (strlen(arg) == 0)
        return 0;
    if ((*arg == '\n') || (*arg == '\r'))
        return 0;

    /* Parse argument */
    strcpy(buffer, arg);
    tok = buffer;
    parm = strtok(tok, "=\n\r");

    /* First check for flags that don't have values (-h and -r) */

    /* Test for help */
    if ((strcmp_no_case("help", parm) == 0) || (strcmp("h", parm) == 0)
        || (strcmp_no_case("?", parm) == 0))
    {
        printf("\nHAI command Program (ver 0.7)\n\n");
        printf("Format:   hai [opts] [command]\n\n");
        printf("Options:   -w=password\n");
        printf("           -f=config file\n");       
        printf("           -i=HAI ip address\n");
        printf("          -ix=encrypted HAI ip address\n");       
        printf("           -p=HAI port\n");
        printf("          -px=encrypted HAI port\n");
        printf("           -k=key\n");
        printf("          -kx=encrypted key\n");
        printf("           -c=user code\n");
        printf("          -cx=encrypted user code\n");
        printf("           -n=name cache file\n");       
        printf("           -s=serial path\n");
        printf("           -b=baud rate\n");
        printf("           -r display comma separated values (CSV) display\n");       
        printf("           -d dump config file\n");      
        printf("           -h help\n\n");
        printf("Commands: 'help' to see info on commands\n\n");
        printf("Example:  hai -i=192.168.0.101 -p=4369 info\n\n");

        exit(0);
    }

    /* Test for csv mode */
    else if ((strcmp_no_case("csv", parm) == 0) || (strcmp("r", parm) == 0))
    {
        csv = 1;
        return 0;
    }

    /* Test for dump mode */
    else if ((strcmp_no_case("dump", parm) == 0) || (strcmp("d", parm) == 0))
    {
        dump_config = 1;
        return 0;
    }

    /* Now check for options that must have "=" with no spaces */
    
    value = strtok(NULL, "\n\r");

    if (value == NULL)
    {
        printf("error: parameter -%s not followed by \"=\"\n", buffer);
        return -999999;
    }
    
    /* Test for password */
    if ((strcmp_no_case("password", parm) == 0) || (strcmp("w", parm) == 0))
    {
        if (value != NULL)
            strcpy(password, value);
    }

    /* Test for config file */
    else if ((strcmp_no_case("file", parm) == 0) || (strcmp("f", parm) == 0))
    {
        if (value != NULL)
            strcpy(haiconf, value);
    }

    /* Test for ip */
    else if ((strcmp_no_case("ip", parm) == 0) || (strcmp("i", parm) == 0))
    {
        if (value != NULL)
            strcpy(ip, value);
    }

    /* Test for encrypted ip */
    else if ((strcmp_no_case("ipx", parm) == 0) || (strcmp("ix", parm) == 0))
    {
        /* Get password */
        if (*password == 0)
        {
            char *p;
            
            printf("Password: ");
            if (fgets(password, 64, stdin) != NULL)
            {
            if ((p = strchr(password, '\r')) != 0)
                *p = 0;
            if ((p = strchr(password, '\n')) != 0)
                *p = 0;            
        }
        }

        /* Prepare key */
        memset(key, 0, sizeof(key));
        if (strlen(password) > 16)
            memcpy(key, password, 16);
        else
            memcpy(key, password, strlen(password));  

        if (value != NULL)
        {
            int j, len;
            char *ptr = value;
            char sec_buffer[128];

            for (j = 0; j < 128; j++)
            {
                sec_buffer[j] = (char) strtol(ptr, &ptr, 16);
                if (*ptr == ':')
                    ptr++;
                else
                    break;
            }

            len = 128;
            decrypt_bytes(key, sec_buffer, j + 1, ip, &len);
        }          
    }

    /* Test for port */
    else if ((strcmp_no_case("port", parm) == 0) || (strcmp("p", parm) == 0))
    {
        if (value != NULL)
            port = atoi(value);
    }

    /* Test for encrypted port */
    else if ((strcmp_no_case("portx", parm) == 0) || (strcmp("px", parm) == 0))
    {
         /* Get password */
        if (*password == 0)
        {
            char *p;
            
            printf("Password: ");
            if (fgets(password, 64, stdin) != NULL)
            {
            if ((p = strchr(password, '\r')) != 0)
                *p = 0;
            if ((p = strchr(password, '\n')) != 0)
                *p = 0;
        }
        }
        
        /* Prepare key */
        memset(key, 0, sizeof(key));
        if (strlen(password) > 16)
            memcpy(key, password, 16);
        else
            memcpy(key, password, strlen(password));  

        if (value != NULL)
        {
            int j, len;
            char *ptr = value;
            char buffer[128];
            char sec_buffer[128];

            for (j = 0; j < 128; j++)
            {
                sec_buffer[j] = (char) strtol(ptr, &ptr, 16);
                if (*ptr == ':')
                    ptr++;
                else
                    break;
            }

            len = sizeof(buffer);
            memset(buffer, 0, len);
            decrypt_bytes(key, sec_buffer, j + 1, buffer, &len);
            
            port = atoi(buffer);
        }          
    }

    /* Test for key */
    else if ((strcmp_no_case("key", parm) == 0) || (strcmp("k", parm) == 0))
    {
        if (value != NULL)
        {
            int j;
            char *ptr = value;

            for (j = 0; j < 16; j++)
            {
                private_key[j] = (char) strtol(ptr, &ptr, 16);
                if (*ptr == ':')
                    ptr++;
            }
        }
    }

    /* Test for encrypted key */
    else if ((strcmp_no_case("keyx", parm) == 0) || (strcmp("kx", parm) == 0))
    {
         /* Get password */
        if (*password == 0)
        {
            char *p;
            
            printf("Password: ");
            if (fgets(password, 64, stdin) != NULL)
            {
            if ((p = strchr(password, '\r')) != 0)
                *p = 0;
            if ((p = strchr(password, '\n')) != 0)
                *p = 0;
        }
        }

        /* Prepare key */
        memset(key, 0, sizeof(key));
        if (strlen(password) > 16)
            memcpy(key, password, 16);
        else
            memcpy(key, password, strlen(password));  

        if (value != NULL)
        {
            int j, len;
            char *ptr = value;
            char sec_buffer[128];

            for (j = 0; j < 128; j++)
            {
                sec_buffer[j] = (char) strtol(ptr, &ptr, 16);
                if (*ptr == ':')
                    ptr++;
                else
                    break;
            }

            len = sizeof(private_key);
            memset(private_key, 0, len);
            decrypt_bytes(key, sec_buffer, j + 1, (char *)private_key, &len);
        }          
    }

    /* Test for code */
    else if ((strcmp_no_case("code", parm) == 0) || (strcmp("c", parm) == 0))
    {
        if (value != NULL)
        {
            int j, ncode;

            ncode = atoi(value);

            for (j = 3; j >= 0; j--)
            {
                code[j] = ncode % 10;
                ncode /= 10;
            }
        }
    }

    /* Test for encrypted code */
    else if ((strcmp_no_case("codex", parm) == 0) || (strcmp("cx", parm) == 0))
    {
         /* Get password */
        if (*password == 0)
        {
            char *p;
            
            printf("Password: ");
            if (fgets(password, 64, stdin) != NULL)
            {
            if ((p = strchr(password, '\r')) != 0)
                *p = 0;
            if ((p = strchr(password, '\n')) != 0)
                *p = 0;
        }
        }

        /* Prepare key */
        memset(key, 0, sizeof(key));
        if (strlen(password) > 16)
            memcpy(key, password, 16);
        else
            memcpy(key, password, strlen(password));  

        if (value != NULL)
        {
            int j, len;
            char *ptr = value;
            char sec_buffer[128];

            for (j = 0; j < 128; j++)
            {
                sec_buffer[j] = (char) strtol(ptr, &ptr, 16);
                if (*ptr == ':')
                    ptr++;
                else
                    break;
            }

            len = sizeof(code);
            memset(code, 0, len);
            decrypt_bytes(key, sec_buffer, j + 1,  (char *)code, &len);
        }          
    }

    /* Test for config file */
    else if ((strcmp_no_case("name", parm) == 0) || (strcmp("n", parm) == 0))
    {
        if (value != NULL)
            strcpy(hainame, value);
    }

    /* Test for serial */
    else if ((strcmp_no_case("serial", parm) == 0) || (strcmp("s", parm) == 0))
    {
        if (value != NULL)
        {
            strcpy(dev, value);
            serial_mode = 1;
        }
    }

    /* Test for baudrate */
    else if ((strcmp_no_case("baud", parm) == 0) || (strcmp("b", parm) == 0))
    {
        if (value != NULL)
            baud = atoi(value);
    }

    /* Must be unknown */
    else
        return -1;

    return 0;
}

/********************************************************************************/

int init_connection(hai_comm_id *id, val32 code)
{
    int err;
    sys_info si;
    sec_code_valid data;

    /* Create connection */
    if (serial_mode)
    {
#ifdef HAISERIAL_SUPPORT
        /* Create serial connection */
        if ((err = hai_serial_open(id, dev, baud)) != 0)
            return err;
#endif
    }
    else
    {
#ifdef WIN32
        WORD sockVersion;
        WSADATA wsaData;
        sockVersion = MAKEWORD(1, 1); 
        if ((err = WSAStartup(sockVersion, &wsaData)) != 0)
            return err;
#endif

        /* Create network connection */
        if ((err = hai_net_open(id, ip, port, private_key)) != 0)
        {
            printf("error opening hai network connection\n");
            return err;
    }
    }

    /* Send login message */
    if ((err = omni_login(id, code)) != 0)
    {
        printf("error logging in\n");
        return err;
    }

    /* Request system info */
    if ((err = omni_sys_info(id, &si)) != 0)
    {
        printf("error getting system info\n");
        return err;
    }
    omni_model = si.model;
    omni_major_version = si.major;
    omni_minor_version = si.minor;

    switch(omni_model)
    {
        case OMNILT :
            max_zones = 25;
            max_zones_status = 36;
            max_units = 36;    
            max_buttons = 16;
            max_buttons_program = 255;
            max_auxs = 25;     
            max_codes = 8;    
            max_areas = 0;    
            max_thermos = 2;  
            max_mesgs = 16;    
            max_temps = 25;
            max_usersettings = 0;
            max_readers = 0;
            break;
        case OMNI2E :
            max_zones = 48;  
            max_zones_status = 63;  
            max_units = 128;    
            max_buttons = 64;  
            max_buttons_program = 255;
            max_auxs = 48;     
            max_codes = 16;    
            max_areas = 2;    
            max_thermos = 4;  
            max_mesgs = 64;    
            max_temps = 48; 
            max_usersettings = 10;
            max_readers = 4;   
            break;
        case OMNIPRO2:
            max_zones = 176;  
            max_zones_status = 215;  
            max_units = 511;    
            max_buttons = 128;  
            max_buttons_program = 255;
            max_auxs = 176;     
            max_codes = 99;    
            max_areas = 8;    
            max_thermos = 64;  
            max_mesgs = 128;    
            max_temps = 176; 
            max_usersettings = 25;
            max_readers = 16;   
            break;
        case LUMINA:
            max_zones = 48;   
            max_zones_status = 54; 
            max_units = 128;    
            max_buttons = 64;  
            max_buttons_program = 255;
            max_auxs = 48;     
            max_codes = 16;    
            max_areas = 1;    
            max_thermos = 4;  
            max_mesgs = 64;    
            max_temps = 48; 
            max_usersettings = 10;
            max_readers = 4;   
            break;
        case LUMINAPRO:
            max_zones = 176;  
            max_zones_status = 182;  
            max_units = 511;    
            max_buttons = 128; 
            max_buttons_program = 255; 
            max_auxs = 176;     
            max_codes = 99;    
            max_areas = 1;    
            max_thermos = 64;  
            max_mesgs = 128;    
            max_temps = 176;  
            max_usersettings = 25;
            max_readers = 16;  
            break;
        case OMNI :
            max_zones = 32;  
            max_zones_status = 45;  
            max_units = 64;    
            max_buttons = 32;  
            max_buttons_program = 255;
            max_auxs = 32;     
            max_codes = 16;    
            max_areas = 2;    
            max_thermos = 4;  
            max_mesgs = 0;    
            max_temps = 32; 
            max_usersettings = 0;
            max_readers = 0;   
            break;
        case OMNI2:
            max_zones = 48;   
            max_zones_status = 63; 
            max_units = 128;    
            max_buttons = 64; 
            max_buttons_program = 255; 
            max_auxs = 48;     
            max_codes = 16;    
            max_areas = 2;    
            max_thermos = 4;  
            max_mesgs = 64;    
            max_temps = 48; 
            max_usersettings = 10;
            max_readers = 4;   
            break;
        case OMNIPRO:
            max_zones = 96;   
            max_zones_status = 133; 
            max_units = 255;    
            max_buttons = 64; 
            max_buttons_program = 255; 
            max_auxs = 96;     
            max_codes = 99;    
            max_areas = 8;    
            max_thermos = 64;  
            max_mesgs = 128;    
            max_temps = 96; 
            max_usersettings = 0;
            max_readers = 0;   
            break;
    }
    max_names =  max_zones + max_units + max_buttons + max_codes + max_areas
        + max_thermos + max_mesgs;

    /* Request code validation */
    if ((err = omni_sec_code_valid(id, 0, code, &data)) != 0)
    {
        printf("error validating code\n");
        return err;
    }
    if (data.authority == CODE_INVALID)
        return EBADCODE;
    else
        code_index = GET8(data.number);

    return 0;
}

/********************************************************************************/

void term_connection(hai_comm_id *id)
{
    /* Send logout message */
    omni_logout(id);

    /* Close connection */
    if (serial_mode)
    {
#ifdef HAISERIAL_SUPPORT
        /* Close serial connection */
        hai_serial_close(id);
#endif
    }
    else
    {
        /* Close network connection */
        hai_net_close(id);
    }
}

/********************************************************************************/

int cmd_loop(hai_comm_id *id)
{
#if defined(GNUREADLINE_SUPPORT) || defined(READLINE_SUPPORT)        
    char *line;
#else
    char line[MAX_LINE];
#endif
    int argc, err;
    char *argv[MAX_ARG];

    /* Perform interactive loop */
    while (1)
    {
        /* Get input */
#if defined(GNUREADLINE_SUPPORT)        
        if ((line = readline("hai> ")) == NULL)
            break;        
        add_history(line);
#elif defined(READLINE_SUPPORT)
        printf("hai> ");
        if ((line = readline(stdin)) == NULL)
            break;
#else
        printf("hai> ");
        if (fgets(line, MAX_LINE, stdin) == NULL)
            break;
#endif

        /* Parse command */
        argc = 0;
        argv[argc] = strtok(line, " \r\n");
        while (argv[argc] != NULL)
        {
            argc++;
            argv[argc] = strtok(NULL, " \r\n");
        }

        /* Check for null command */
        if (argc == 0)
        {
#if defined(GNUREADLINE_SUPPORT) || defined(READLINE_SUPPORT)        
            free(line);
#endif
            continue;
        }

        /* Check for exit */
        if (strcmp_no_case("exit", argv[0]) == 0 ||
            strcmp_no_case("quit", argv[0]) == 0 ||
            strcmp_no_case("x", argv[0]) == 0    ||
            strcmp_no_case("q", argv[0]) == 0)
        {
#if defined(GNUREADLINE_SUPPORT) || defined(READLINE_SUPPORT)        
            free(line);
#endif
            break;
        }
        if (strcmp_no_case(".", argv[0]) == 0)
        {
#if defined(GNUREADLINE_SUPPORT) || defined(READLINE_SUPPORT)        
            free(line);
#endif
            break;
        }

        /* Do command */
        err = do_command(id, argc, argv);

        /* Check for timeout */
        /* Also check for CRD errors and try again. I get these often, but this does the trick...RickM */
        if ((err == EOMNIRESPONSE) || (err == EHAITIMEOUT) || err == EOMNICRC)
        {
            /* Reconnect */
            term_connection(id);
            init_connection(id, code);
       
            err = do_command(id, argc, argv);    
        }

#if defined(GNUREADLINE_SUPPORT) || defined(READLINE_SUPPORT)        
        free(line);
#endif

        /* Print error message */
        if (err > __ELASTOMNI)
        {
            switch(err)
            {
                case EINVCMD :
                    printf("error (%d) : Invalid command\n", err);
                    break;
                case EBADCODE :
                    printf("error (%d) : Invalid code\n", err);
                    break;
                case EBADFILE :
                    printf("error (%d) : Bad file format\n", err);
                    break;
            }
        }
        else if (err > __ELASTHAI)
            printf("error (%d) : %s\n", err, omni_strerror(err));
        else if (err > __ELASTERROR)
            printf("error (%d) : %s\n", err, hai_net_strerror(err));
        else if (err != 0)
            printf("error (%d) : %s\n", err, strerror(err));

        printf("\n");
    }

    return 0;
}

/********************************************************************************/

int do_command(hai_comm_id *id, int argc, char *argv[])
{
    cmd_item *cmd = cmd_list;

    while (cmd->cmd_text != NULL)
    {
        if (strcmp_no_case(cmd->cmd_text, argv[0]) == 0)
        {
            if (cmd->func != NULL)
            {
                return ((cmd->func)(id, argc, argv));
            }
            break;
        }
        cmd++;
    }

    return EINVCMD;
}

/********************************************************************************/

int do_encrypt(hai_comm_id *id, int argc, char *argv[])
{
    char buffer[1024];
    char sec_buffer[1024];
    char key[16];
    int i, len;

    /* Check arguments */
    if (argc < 2)
        return EINVAL;
    if (strlen(argv[1]) < 1)
        return EINVAL;       

    /* Prepare key */
    memset(key, 0, sizeof(key));
    if (strlen(argv[1]) > 16)
        memcpy(key, argv[1], 16);
    else
        memcpy(key, argv[1], strlen(argv[1]));  

    /* Encrypt ip address */
    if (*ip != 0)
    {
        len = 1024;
        encrypt_bytes(key, ip, strlen(ip) + 1, sec_buffer, &len);
        printf("ipx=");
        for (i = 0; i < (len - 1); i++)
            printf("%02x:", sec_buffer[i] & 0xff);
        printf("%02x\n", sec_buffer[len - 1] & 0xff);
    }

    /* Encrypt port */
    if (*ip != 0)
    {
        sprintf(buffer, "%d", port);
        len = strlen(buffer);
        encrypt_bytes(key, buffer, strlen(buffer) + 1, sec_buffer, &len);
        printf("portx=");
        for (i = 0; i < (len - 1); i++)
            printf("%02x:", sec_buffer[i] & 0xff);
        printf("%02x\n", sec_buffer[len - 1] & 0xff);
    }

    /* Encrypt key */
    if (*private_key != 0)
    {
        len = 16;
        encrypt_bytes(key, (char *)private_key, 16, sec_buffer, &len);
        printf("keyx=");
        for (i = 0; i < (len - 1); i++)
            printf("%02x:", sec_buffer[i] & 0xff);
        printf("%02x\n", sec_buffer[len - 1] & 0xff);
    }

    /* Encrypt code */
    if (*code != 0)
    {
        len = 4;
        encrypt_bytes(key, (char *)code, 4, sec_buffer, &len);
        printf("codex=");
        for (i = 0; i < (len - 1); i++)
            printf("%02x:", sec_buffer[i] & 0xff);
        printf("%02x\n", sec_buffer[len - 1] & 0xff);
    }
    
    return 0;
}

/********************************************************************************/

int do_csv(hai_comm_id *id, int argc, char *argv[])
{
    /* Check arguments */
    if (argc < 2)
        return EINVAL;

    /* Parse options */
    if (strcmp_no_case(argv[1], "on") == 0)
        csv = 1;
    else if (strcmp_no_case(argv[1], "off") == 0)
        csv = 0;
    else
        return EINVAL;

    return 0;
}

/********************************************************************************/

void encrypt_bytes(char *key, char *buf_in, int len_in, char *buf_out,
    int *len_out)
{
    char buffer[1024];
    aes_t aes;
  
    /* Copy data */
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, buf_in, len_in);

    /* Encrypt message */
    memset(buf_out, 0, *len_out);
    *len_out = (len_in + 15) & ~0xF;
    MakeKey(&aes, key, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
    Encrypt(&aes, buffer, buf_out, *len_out, ECB);    
}

/********************************************************************************/

void decrypt_bytes(char *key, char *buf_in, int len_in, char *buf_out,
    int *len_out)
{
    char buffer[1024];
    aes_t aes;
  
    /* Copy data */
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, buf_in, len_in);

    /* Encrypt message */
    memset(buf_out, 0, *len_out);
    *len_out = (len_in + 15) & ~0xF;
    MakeKey(&aes, key, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
    Decrypt(&aes, buffer, buf_out, *len_out, ECB);    
}

/********************************************************************************/

int do_sys_info(hai_comm_id *id, int argc, char *argv[])
{
    sys_info data;
    int err;

    /* Request system info */
    if ((err = omni_sys_info(id, &data)) != 0)
        return err;

    /* Print model */
    if (!csv)
        printf("Model: HAI ");
    switch (data.model)
    {
        case OMNILT :
            printf("OmniLT");
            break;
        case OMNI :
            printf("Omni");
            break;
        case OMNI2 :
            printf("OmniII");
            break;
        case OMNI2E :
            printf("Omni IIe");
            break;
        case OMNIPRO :
            printf("OmniPro");
            break;
        case OMNIPRO2 :
            printf("OmniPro II");
            break;
        case LUMINA:
            printf("Lumina");
            break;
        case LUMINAPRO:
            printf("Lumina Pro");
            break;
        default :
            printf("Unknown");
            break;
    }
    if (!csv)
        printf("\n");

    /* Print firmware version */
    if (!csv)
        printf("Version: %d.%d", data.major, data.minor);
    else
        printf(",%d.%d", data.major, data.minor);
    if ((data.rev != 0) && (data.rev < 0x80))
        printf("%c", data.rev + 'A' - 1);
    else if (data.rev != 0)
        printf("X%d", 0x100 - data.rev);
    if (!csv)
        printf("\n");

    /* Print my phone number */
    if (!csv)
        printf("Phone: %s\n", data.phone);
    else
        printf(",%s\n", data.phone);

    return 0;
}

/********************************************************************************/

int do_sys_stat(hai_comm_id *id, int argc, char *argv[])
{
    sys_stat data;
    int err, i;
    int security_modes = 1, expansion_enclosures = 0;

    /* Calc model parameters */
    switch (omni_model)
    {
        case OMNI :
        case OMNI2 :
        case OMNI2E :
            security_modes = 2;
            expansion_enclosures = 0;
            break;
        case OMNIPRO :
            security_modes = 8;
            expansion_enclosures = 4;
            break;
        case OMNIPRO2 :
            security_modes = 8;
            expansion_enclosures = 8;
            break;
        case LUMINAPRO:
            security_modes = 1;
            expansion_enclosures = 8;
            break;
    }

    /* Request system status */
    if ((err = omni_sys_stat(id, &data)) != 0)
        return err;

    /* Print time/date */
    if (!csv)
    {
        printf("Time: ");
        if (data.date_valid)
        {
            printf("%s ", dow_text[data.day_of_week - 1]);
            printf("%s ", month_text[data.month - 1]);
            printf("%d ", data.day);
            printf("%02d:%02d:%02d ", data.hour, data.minute, data.second);
            if (data.dst)
                printf("(DST) ");
            printf("%d\n", data.year + TIME_YEAR_BASE);
        }
        else
            printf("not valid\n");
    }
    else
    {
        if (data.date_valid)
        {
            printf("Time,%s,", dow_text[data.day_of_week - 1]);
            printf("%s,", month_text[data.month - 1]);
            printf("%d,", data.day);
            printf("%02d:%02d:%02d,", data.hour, data.minute, data.second);
            if (data.dst)
                printf("DST,");
            else
                printf(",");
            printf("%d\n", data.year + TIME_YEAR_BASE);
        }
        else
            printf(",,,,,\n");
    }

    /* Print Sunrise/Sunset */
    if (!csv)
    {
        if (data.date_valid)
        {
            printf("Sunrise: %02d:%02d\n", data.sunrise_hour, data.sunrise_minute);
            printf("Sunset:  %02d:%02d\n", data.sunset_hour, data.sunset_minute);
        }
    }
    else
    {
        if (data.date_valid)
        {
            printf("Sun,%02d:%02d,", data.sunrise_hour, data.sunrise_minute);
            printf("%02d:%02d\n", data.sunset_hour, data.sunset_minute);
        }
        else
            printf(",\n");
    }

    /* Print battery reading */
    if (!csv)
        printf("Battery Reading: %d\n", data.battery);
    else
        printf("Battery,%d\n", data.battery);   

    /* Print security modes */
    if ((omni_model == LUMINA) || (omni_model == LUMINAPRO))
    {
    if (!csv)
            printf("Security mode: %s\n", lumina_sec_text[data.security_mode[0]]);
        else
            printf("Security Mode,%s\n", lumina_sec_text[data.security_mode[0]]);
    }
    else
    {
        if (!csv)
        {
            printf("Security Modes:\n");
        for (i = 0; i < security_modes; i++)
            printf("    Area %d : %s\n", i + 1, sec_text[data.security_mode[i]]);
    }
    else
    {
            printf("Security Modes,");
        for (i = 0; i < security_modes; i++)
        {
            printf("%s", sec_text[data.security_mode[i]]);
            if (i < (security_modes - 1))
                printf(",");
            else
                printf("\n");            
        }
    }
    }

    /* Expansion status */
    if (!csv)
    {
        if (expansion_enclosures)
        {
            printf("Expansion Enclosures:\n");
            for (i = 0; i < expansion_enclosures; i++)
            {
                printf("    %d : Battery=%d", i + 1,
                    data.expansion_enclosure[i].battery);
                if (data.expansion_enclosure[i].status & EXP_STAT_AC)
                    printf(", AC off");
                if (data.expansion_enclosure[i].status & EXP_STAT_BATT)
                    printf(", Batt low");
                if (data.expansion_enclosure[i].status & EXP_STAT_COMM)
                    printf(", Comm failure");
                printf("\n");
            }
        }
    }
    else
    {
        printf("Expansion Enclosures,");
        for (i = 0; i < expansion_enclosures; i++)
        {
            if (expansion_enclosures)
                printf("%d,%d", data.expansion_enclosure[i].battery,
                    data.expansion_enclosure[i].status);
            if (i < (expansion_enclosures - 1))
                printf(",");
            else
                printf("\n");
        }
    }

    return 0;
}

/********************************************************************************/

int do_zone_stat(hai_comm_id *id, int argc, char *argv[])
{
    zone_stat data;
    int first, last, start, end;
    int err, i;
    int printAll = 0;

    /* Check arguments */
    if (argc > 3)
        return EINVAL;

    /* Setup arguments */
    if (argc > 1 && strcmp_no_case(argv[1], "all") == 0) {
         printAll = 1;
    }
    if (argc == 1 || printAll)
    {
        first = 1;
        last = max_zones_status;
    }
    else
    {
        first = atoi(argv[1]);
        if (argc > 2)
            last = atoi(argv[2]);
        else
            last = first;
    }

    /* Check range */
    if ((first < 1) || (last < first) || (last > max_zones_status)) {
         printf("error: zone not in range from 1 to %d\n", max_zones_status);
         return EINVAL;
    }

    /* Init loop parms */
    start = first;
    end = start + 50;
    if (end > last)
        end = last;

    /* Loop to get all zones */
    while(start <= last)
    {
        /* Request zone status */
        if ((err = omni_zone_stat(id, start, end, &data)) != 0)
            return err;

        /* Print zone status */
        for (i = 0; i < (end - start + 1); i++)
        {
            int current = ((data.zones[i].status >> 0)& 0x03);
            int latched = ((data.zones[i].status >> 2)& 0x03);
            int arming  = ((data.zones[i].status >> 4)& 0x03);
            int trouble = ((data.zones[i].status >> 6)& 0x01);

            /* Skip unnamed */
            if (name_cache_valid && !printAll && (argc == 1) && !is_named(NAMES_ZONE, i + start))
                continue;

            if (!csv)
            {
                printf("%3d : ", i + start);
                print_name_fixed(stdout, NAMES_ZONE, i + start);
                printf(" : %03d, %9s, %13s, %13s", data.zones[i].loop,
                    zone_text[current], latched_text[latched], arming_text[arming]);
                if (trouble)
                    printf(" Latched Trouble");
                printf("\n");
             }
             else
             {
                printf("%d,", i + start);
                print_name(stdout, NAMES_ZONE, i + start);
                printf(",%d,%s,%s,%s,", data.zones[i].loop,
                    zone_text[current], latched_text[latched], arming_text[arming]);
                if (trouble)
                    printf("LT\n");
                printf("\n");
             }
        }

        /* Next block */
        start = end + 1;
        end = start + 50;
        if (end > last)
           end = last;        
    }

    return 0;
}

/********************************************************************************/

int do_unit_stat(hai_comm_id *id, int argc, char *argv[])
{
    unit_stat data;
    int first, last, start, end;
    int err, i;
    int printAll = 0;

    /* Check arguments */
    if (argc > 3)
        return EINVAL;

    /* Setup arguments */
    if (argc > 1 && strcmp_no_case(argv[1], "all") == 0) {
         printAll = 1;
    }
    if (argc == 1 || printAll)
    {
        first = 1;
        last = max_units;
    }
    else
    {
        first = atoi(argv[1]);
        if (argc > 2)
            last = atoi(argv[2]);
        else
            last = first;
    }

    /* Check range */
    if ((first < 1) || (last < first) || (last > max_units)) {
         printf("error: unit not in range from 1 to %d\n", max_units);
         return EINVAL;
    }

    /* Init loop parms */
    start = first;
    end = start + 50;
    if (end > last)
        end = last;

                
    /* Loop to get all units */
    while(start <= last)
    {
        /* Request unit status */
        if ((err = omni_unit_stat(id, start, end, &data)) != 0)
            return err;

        /* Print unit status */
        for (i = 0; i < (end - start + 1); i++)
        {
            /* Skip unnamed */
            if (name_cache_valid && !printAll && (argc == 1) && !is_named(NAMES_UNIT, i + start))
                continue;

            if (!csv)
            {
                printf("%3d : ", i + start);
                print_name_fixed(stdout, NAMES_UNIT, i + start);
                printf(" : %03d, ", data.units[i].status);
                if (data.units[i].status == UNIT_OFF)
                    printf("Off");
                else if (data.units[i].status == UNIT_ON)
                    printf("On");
                else if ((data.units[i].status >= UNIT_SCENE_A)
                        && (data.units[i].status <= UNIT_SCENE_L))
                    printf("Scene %c", data.units[i].status - UNIT_SCENE_A + 'A');
                else if ((data.units[i].status >= UNIT_DIM_1)
                        && (data.units[i].status <= UNIT_DIM_9))
                    printf("Dim %d", data.units[i].status - UNIT_DIM_1 + 1);
                else if ((data.units[i].status >= UNIT_BRIGHTEN_1)
                        && (data.units[i].status <= UNIT_BRIGHTEN_9))
                    printf("Brighten %d", data.units[i].status - UNIT_BRIGHTEN_1 + 1);
                else if ((data.units[i].status >= UNIT_LEVEL_0)
                        && (data.units[i].status <= UNIT_LEVEL_100))
                    printf("Level %d", data.units[i].status - UNIT_LEVEL_0);
                if (GET16(data.units[i].time) != 0)
                    printf(" for %d secs", GET16(data.units[i].time));
                printf("\n");
            }
            else
            {
                printf("%d,", i + start);
                print_name(stdout, NAMES_UNIT, i + start);
                printf(",%d,", data.units[i].status);
                if (data.units[i].status == UNIT_OFF)
                    printf("Off");
                else if (data.units[i].status == UNIT_ON)
                    printf("On");
                else if ((data.units[i].status >= UNIT_SCENE_A)
                        && (data.units[i].status <= UNIT_SCENE_L))
                    printf("Scene %c", data.units[i].status - UNIT_SCENE_A + 'A');
                else if ((data.units[i].status >= UNIT_DIM_1)
                        && (data.units[i].status <= UNIT_DIM_9))
                    printf("Dim %d", data.units[i].status - UNIT_DIM_1 + 1);
                else if ((data.units[i].status >= UNIT_BRIGHTEN_1)
                        && (data.units[i].status <= UNIT_BRIGHTEN_9))
                    printf("Brighten %d", data.units[i].status - UNIT_BRIGHTEN_1 + 1);
                else if ((data.units[i].status >= UNIT_LEVEL_0)
                        && (data.units[i].status <= UNIT_LEVEL_100))
                    printf("Level %d", data.units[i].status - UNIT_LEVEL_0);
                printf(",%d\n", GET16(data.units[i].time));
            }
        }

        /* Next block */
        start = end + 1;
        end = start + 50;
        if (end > last)
           end = last;        
    }

    return 0;
}

/********************************************************************************/

int do_sensor_stat(hai_comm_id *id, int argc, char *argv[])
{
    aux_stat data;
    int first, last, start, end;
    int err, i;
    int printAll = 0;

    /* Check arguments */
    if (argc > 3)
        return EINVAL;

    /* Setup arguments */
    if (argc > 1 && strcmp_no_case(argv[1], "all") == 0) {
         printAll = 1;
    }
    if (argc == 1 || printAll)
    {
        first = 1;
        last = max_auxs;
    }
    else
    {
        first = atoi(argv[1]);
        if (argc > 2)
            last = atoi(argv[2]);
        else
            last = first;
    }

    /* Check range */
    if ((first < 1) || (last < first) || (last > max_auxs)) {
         printf("error: sensor not in range from 1 to %d\n", max_auxs);
         return EINVAL;
    }

    /* Init loop parms */
    start = first;
    end = start + 25;
    if (end > last)
        end = last;

    /* Loop to get all sensors */
    while(start <= last)
    {
        /* Request aux status */
        if ((err = omni_aux_stat(id, start, end, &data)) != 0)
            return err;

        /* Print aux status */
        for (i = 0; i < (end - start + 1); i++)
        {
            /* Skip ones that are not temp sensors */
            if (data.auxs[i].temp == 0)
                continue;
            /* Skip unnamed */
            if (name_cache_valid && !printAll && (argc == 1) && !is_named(NAMES_ZONE, i + start))
                continue;

            if (!csv)
            {
                printf("%3d : ",  i + start);
                print_name_fixed(stdout, NAMES_ZONE, i + start);
                printf(" : %s\n",  data.auxs[i].status ? "On" : "Off");
                printf("   Temp: %3dF, %5.1fC\n", GETF(data.auxs[i].temp),
                    GETC(data.auxs[i].temp));
                printf("   High: %3dF, %5.1fC\n", GETF(data.auxs[i].high_setpoint),
                    GETC(data.auxs[i].high_setpoint));
                printf("   Low:  %3dF, %5.1fC\n", GETF(data.auxs[i].low_setpoint),
                    GETC(data.auxs[i].low_setpoint));
            }
            else
            {
                printf("%d,",  i + start);
                print_name(stdout, NAMES_ZONE, i + start);
                printf(",%s,",  data.auxs[i].status ? "On" : "Off");
                printf("%d,%.1f,", GETF(data.auxs[i].temp),
                    GETC(data.auxs[i].temp));
                printf("%d,%.1f,", GETF(data.auxs[i].high_setpoint),
                    GETC(data.auxs[i].high_setpoint));
                printf("%d,%.1f\n", GETF(data.auxs[i].low_setpoint),
                    GETC(data.auxs[i].low_setpoint));
            }
        }
        
        /* Next block */
        start = end + 1;
        end = start + 25;
        if (end > last)
           end = last;        
    }

    return 0;
}

/********************************************************************************/

int do_thermo_stat(hai_comm_id *id, int argc, char *argv[])
{
    thermo_stat data;
    int first, last, start, end;
    int err, i;
    int printAll = 0;

    /* Check arguments */
    if (argc > 3)
        return EINVAL;

    /* Setup arguments */
    if (argc > 1 && strcmp_no_case(argv[1], "all") == 0) {
         printAll = 1;
    }
    if (argc == 1 || printAll)
    {
        first = 1;
        last = max_thermos;
    }
    else
    {
        first = atoi(argv[1]);
        if (argc > 2)
            last = atoi(argv[2]);
        else
            last = first;
    }

    /* Check range */
    if ((first < 1) || (last < first) || (last > max_thermos)) {
         printf("error: thermo not in range from 1 to %d\n", max_thermos);
         return EINVAL;
    }

    /* Init loop parms */
    start = first;
    end = start + 25;
    if (end > last)
        end = last;

    /* Loop to get all thermostats */
    while(start <= last)
    {
        /* Request thermo status */
        if ((err = omni_thermo_stat(id, start, end, &data)) != 0)
            return err;

        /* Print thermo status */
        for (i = 0; i < (end - start + 1); i++)
        {
            /* Skip unused */
            if (!printAll && (argc == 1) && (data.thermos[i].temp == 0))
                continue;

            /* Skip unnamed */
            if (name_cache_valid && !printAll && (argc == 1) && !is_named(NAMES_THERMOSTAT, i + start))
                continue;

            if (!csv)
            {
                printf("%2d : ",  i + start);
                print_name_fixed(stdout, NAMES_THERMOSTAT, i + start);
                printf(" : %s",  ac_text[data.thermos[i].sys_mode]);
                printf(", Fan %s",  data.thermos[i].fan_mode ? (data.thermos[i].fan_mode == 1 ? "On" : "Cycle") : "Auto");
                printf(", Hold %s",  data.thermos[i].hold_status ? "On" : "Off");       
                if (data.thermos[i].status & THERMO_COMM_FAIL)
                    printf(" (Comms Failure)");
                if (data.thermos[i].status & THERMO_FREEZE_ALARM)
                    printf(" (Freeze Alarm)");
                printf("\n   Temp: %3dF, %5.1fC\n", GETF(data.thermos[i].temp),
                    GETC(data.thermos[i].temp));
                printf("   Heat: %3dF, %5.1fC\n", GETF(data.thermos[i].heat_setpoint),
                    GETC(data.thermos[i].heat_setpoint));
                printf("   Cool: %3dF, %5.1fC\n", GETF(data.thermos[i].cool_setpoint),
                    GETC(data.thermos[i].cool_setpoint));
            }
            else
            {
                printf("%d,",  i + start);
                print_name(stdout, NAMES_THERMOSTAT, i + start);
                printf(",%s,",  ac_text[data.thermos[i].sys_mode]);
                printf("%s,",  data.thermos[i].fan_mode ? (data.thermos[i].fan_mode == 1 ? "On" : "Cycle") : "Auto");
                printf("%s,",  data.thermos[i].hold_status ? "On" : "Off");                      
                printf("%d,%.1f,", GETF(data.thermos[i].temp),
                    GETC(data.thermos[i].temp));
                printf("%d,%.1f,", GETF(data.thermos[i].heat_setpoint),
                    GETC(data.thermos[i].heat_setpoint));
                printf("%d,%.1f,", GETF(data.thermos[i].cool_setpoint),
                    GETC(data.thermos[i].cool_setpoint));
                if (data.thermos[i].status & THERMO_COMM_FAIL)
                    printf("CF");
                printf(",");               
                if (data.thermos[i].status & THERMO_FREEZE_ALARM)
                    printf("FA");
                printf("\n");
            }
        }
    
        /* Next block */
        start = end + 1;
        end = start + 25;
        if (end > last)
           end = last;        
    }

    return 0;
}

/********************************************************************************/

int do_mesg_stat(hai_comm_id *id, int argc, char *argv[])
{
    mesg_stat data;
    int first, last;
    int err, num, i;
    int printAll = 0;

    /* Check arguments */
    if (argc > 3)
        return EINVAL;

    /* Setup arguments */
    if (argc > 1 && strcmp_no_case(argv[1], "all") == 0) {
         printAll = 1;
    }
    if (argc == 1 || printAll)
    {
        first = 1;
        last = max_mesgs;
    }
    else
    {
        first = atoi(argv[1]);
        if (argc > 2)
            last = atoi(argv[2]);
        else
            last = first;
    }

    /* Check range */
    if ((first < 1) || (last < first) || (last > max_mesgs)) {
         printf("error: mesg not in range from 1 to %d\n", max_mesgs);
         return EINVAL;
    }

    /* Request message status */
    if ((err = omni_mesg_stat(id, &data, &num)) != 0)
        return err;

    /* Print message status */
    if (data.memo && !csv)
        printf("Memo pending\n"); 

    /* Adjust for num */
    if (first > num)
        return 0;
    if (last > num)
        last = num;
        
    for (i = (first - 1); i < last; i++)
    {
        int displayed = (data.mesgs[i / 4] >> (6 - ((i % 4) * 2))) & 0x01;
        int not_ack = (data.mesgs[i / 4] >> (6 - ((i % 4) * 2))) & 0x02;

        /* Skip unnamed */
        if (name_cache_valid && !printAll && (argc == 1) && !is_named(NAMES_MESSAGE, i + 1))
            continue;

        if (!csv)
        {
            printf("%3d : ",  i + 1);
            print_name_fixed(stdout, NAMES_MESSAGE, i + 1);
            if (displayed)
                printf(" : displayed, %sacknowledged\n",  not_ack 
                    ? "not " : "    ");
            else
                printf(" : not displayed\n");
        }
        else
        {
            printf("%d,",  i + 1);
            print_name(stdout, NAMES_MESSAGE, i + 1);
            if (displayed)
                printf(",displayed,%sacknowledged\n",  not_ack 
                    ? "not " : "    ");
            else
                printf(",not displayed,\n");
        }
    }
        
    return 0;
}

/********************************************************************************/

int do_usersetting_stat(hai_comm_id *id, int argc, char *argv[])
{
    usersetting_stat data;
    int first, last, start, end, value;
    int err, i;
    int printAll = 0;

    if (omni_major_version < 3) {
         printf("error (%d) : Invalid command: Firmware v3.0 or greater required.\n", EINVCMD);
         return 0;
    }
    
    /* Check arguments */
    if (argc > 3)
        return EINVAL;
    
    /* Setup arguments */
    if (argc > 1 && strcmp_no_case(argv[1], "all") == 0) {
         printAll = 1;
    }
    if (argc == 1 || printAll)
    {
        first = 1;
        last = max_usersettings;
    }
    else
    {
        first = atoi(argv[1]);
        if (argc > 2)
            last = atoi(argv[2]);
        else
            last = first;
    }

    /* Check range */
    if ((first < 1) || (last < first) || (last > max_usersettings)) {
         printf("error: usersetting not in range from 1 to %d\n", max_usersettings);
         return EINVAL;
    }

    /* Init loop parms */
    start = first;
    end = start + 50;
    if (end > last)
        end = last;

    /* Loop to get all usersettings */
    while(start <= last)
    {
        /* Request usersetting status */
        if ((err = omni_usersetting_stat(id, start, end, &data)) != 0)
            return err;

        /* Print usersetting status */
        for (i = 0; i < (end - start + 1); i++)
        {
            val8 dow;
            int type = (data.usersettings[i].type);
            /* Skip unnamed */
            if (name_cache_valid && !printAll && (argc == 1) && !is_named(NAMES_USERSETTING, i + start))
                continue;
/*
  The user setting types are:
        0                   Not Used
        1                   Number
        2                   Duration
        3                   Temperature
        4                   Humidity
        5                   Date
        6                   Time
        7                   Days of Week
        8                   Level

Numbers are reported as 0-255 in the low byte.
For durations, 1-99 = 1-99 seconds, 101-199 = 1-99 minutes, and 201-218 = 1-18 hours.
Temperatures are reported in the Omni temperature format (see Appendix B).
Humidity is reported in the Omni temperature format where Fahrenheit temperatures 0-100 correspond to 0-100% relative humidity
(see Appendix B).
Dates are reported with the month in the high byte and the day in the low byte.
Times are reported with the hour in the high byte and the minute in the low byte.
Days of the week are reported in the low byte, with bit 1 set for Monday, bit 2 set for Tuesday, and so on with bit 7 set for Sunday.
Bit 0 is not used.
Levels are reported as 0-100 in the low byte.

High byte is data1; Low byte is data2.
 */
            if (!csv) {
                printf("%3d : ", i + start);
                print_name_fixed(stdout, NAMES_USERSETTING, i + start);
                printf(" : %12s: ", usersetting_text[type]);
            } else {
                printf("%d,", i + start);
                print_name(stdout, NAMES_USERSETTING, i + start);
                printf(",");
            }
            switch (type)
            {
            case CMD_USER_SET_NOTUSED:
                 /* Nothing more to print */
                 printf("\n");
                 break;
            case CMD_USER_SET_NUMBER:
                 printf("%d\n", data.usersettings[i].data2);
                 break;
            case CMD_USER_SET_DURATION:
                 value = data.usersettings[i].data2;
                 if (value < 100) {
                      printf("%d", value);
                      if (!csv) {
                           printf(" seconds");
                      }
                 } else if (value < 200) {
                      printf("%d", value - 100);
                      if (!csv) {
                           printf(" minutes");
                      }
                 } else {
                      printf("%d", value - 200);
                      if (!csv) {
                           printf(" hours");
                      }
                 }
                 printf("\n");
                 break;
            case CMD_USER_SET_TEMPERATURE:
                 if (!csv) {
                      printf("%3dF, %5.1fC\n",
                             GETF(data.usersettings[i].data2),
                             GETC(data.usersettings[i].data2));
                 } else {
                      printf("%d,%.1f\n",
                             GETF(data.usersettings[i].data2),
                             GETC(data.usersettings[i].data2));
                 }
                 break;
            case CMD_USER_SET_HUMIDITY:
                 /* This is a work-around for a strange OmniLink/UDP issue. If you send 0% humidity, it later comes back from the controller as a binary 0 instead of the decimal 44 we sent to indicate 0 degrees F (0% humidity). */
                 if (data.usersettings[i].data2 == 0) {
                      data.usersettings[i].data2 = 44;
                 }
                 printf("%d", GETF(data.usersettings[i].data2));
                 if (!csv) {
                      printf("%%");
                 }
                 printf("\n");
                 printf("data1 %02x data2 %02x", data.usersettings[i].data1, data.usersettings[i].data2);
                 break;
            case CMD_USER_SET_DATE:
                 if (!csv) {
                      printf("%s %d\n", month_text[data.usersettings[i].data1 - 1],
                             data.usersettings[i].data2);
                 } else {
                      printf("%s,%d\n", month_text[data.usersettings[i].data1 - 1],
                             data.usersettings[i].data2);
                 }
                 break;
            case CMD_USER_SET_TIME:
                 printf("%02d:%02d\n", data.usersettings[i].data1,
                        data.usersettings[i].data2);
                 break;
            case CMD_USER_SET_DAYSOFWEEK: /* Days of Week */
                 dow = data.usersettings[i].data2;
                 printf("%c", dow & 0x02 ? 'M' : '-');
                 printf("%c", dow & 0x04 ? 'T' : '-');
                 printf("%c", dow & 0x08 ? 'W' : '-');
                 printf("%c", dow & 0x10 ? 'T' : '-');
                 printf("%c", dow & 0x20 ? 'F' : '-');
                 printf("%c", dow & 0x40 ? 'S' : '-');
                 printf("%c", dow & 0x80 ? 'S' : '-');
                 printf("\n");
                 break;
            case CMD_USER_SET_LEVEL:
                 printf("%d", data.usersettings[i].data2);
                 if (!csv) {
                      printf("%%");
                 }
                 printf("\n");
                 break;
            }
        }

        /* Next block */
        start = end + 1;
        end = start + 50;
        if (end > last)
           end = last;  
    }

    return 0;
}

/********************************************************************************/

int do_events(hai_comm_id *id, int argc, char *argv[])
{
    sys_events data;
    int err, i, num, event;

    /* Request system events */
    if ((err = omni_sys_events(id, &data, &num)) != 0)
        return err;

    for (i = 0; i < num; i++)
    {
        event = GET16(data.events[i]);

        if (EVENT_BUTTON(event))
        {
            printf("User Macro ");
            print_name(stdout, NAMES_BUTTON, EVENT_BUTTON_NUM(event));
        }
        else if (EVENT_PROLINK(event))
        {
            printf("Pro-Link Message % d", EVENT_PROLINK_NUM(event));
        }
        else if (EVENT_CENTRALITE(event))
        {
            printf("Centralite Switch %d", EVENT_CENTRALITE_NUM(event));
        }
        else if (EVENT_ALARM(event))
        {
            printf("%s Alarm in ", alarm_text[EVENT_ALARM_TYPE(event)]);
            print_name(stdout, NAMES_AREA, EVENT_ALARM_AREA(event));
       }
        else if (EVENT_ZONE(event))
        {
            print_name(stdout, NAMES_ZONE, EVENT_ZONE_NUM(event));
            printf(" %s", EVENT_ZONE_STATE(event) ? "On" : "Off");
        }
        else if (EVENT_UNIT(event))
        {
            print_name(stdout, NAMES_UNIT, EVENT_UNIT_NUM(event));
            printf(" %s", EVENT_UNIT_STATE(event) ? "On" : "Off");
        }
        else if (EVENT_COMPOSE(event))
        {
            if (EVENT_COMPOSE_STATE(event) < EVENT_COMPOSE_SCENE)
                printf("Compose %c%d %s", EVENT_COMPOSE_CODE(event) + 'A',
                    EVENT_COMPOSE_NUM(event), EVENT_COMPOSE_STATE(event) ? "On"
                    : "Off");
            else
                printf("Compose %c%d Scene %c", EVENT_COMPOSE_CODE(event) + 'A',
                    EVENT_COMPOSE_NUM(event), EVENT_COMPOSE_STATE(event)
                    - EVENT_COMPOSE_SCENE + 'A');
        }
        else if (EVENT_X10(event))
        {
            if (EVENT_X10_ALL(event))   /* HAI bug workaround */
                printf("X-10 All %c %s", ((EVENT_X10_HOUSE(event) + 1) & 0xF)
                    + 'A', EVENT_X10_STATE(event) ? "On" : "Off");
            else
                printf("X-10 %c%d %s", EVENT_X10_HOUSE(event) + 'A',
                    EVENT_X10_UNIT(event) + 1, EVENT_X10_STATE(event) ? "On"
                    : "Off");
        }
        else if (EVENT_ARM(event))
        {
            if ((omni_model == LUMINA) || (omni_model == LUMINAPRO))
                printf("Arm %s in ", lumina_sec_text[EVENT_ARM_MODE(event)]);
            else
            printf("Arm %s in ", sec_text[EVENT_ARM_MODE(event)]);
            print_name(stdout, NAMES_AREA, EVENT_ARM_AREA(event));
            print_code(stdout, EVENT_ARM_CODE(event));
            printf(" (%s)", EVENT_ARM_DELAY(event) ? "start" : "end");
        }
        else if (EVENT_SWITCH(event))
        {
            print_name(stdout, NAMES_UNIT, EVENT_SWITCH_UNIT(event));
            if (EVENT_SWITCH_STATE(event) <= 1)
                printf(" Switch %s", EVENT_SWITCH_STATE(event) ? "On" : "Off");
            else
                printf(" Switch %d", EVENT_SWITCH_STATE(event));
        }
        else if (EVENT_UPBLINK(event))
        {
            printf("UPB Link %d %s", EVENT_UPBLINK_NUM(event),
                    upb_text[EVENT_UPBLINK_CMD(event)]);
        }        
        else if (EVENT_ALL(event))
        {
            printf("All %s in ", EVENT_ALL_STATE(event) ? "On" : "Off");
            print_name(stdout, NAMES_AREA, EVENT_ALL_AREA(event));
        }
        else if (EVENT_PHONE_DEAD(event))
            printf("Phone Line Dead");
        else if (EVENT_PHONE_RING(event))
            printf("Phone Line Ring");
        else if (EVENT_PHONE_OFF(event))
            printf("Phone Line Off Hook");
        else if (EVENT_PHONE_ON(event))
            printf("Phone Line On Hook");
        else if (EVENT_AC_OFF(event))
            printf("AC Power Off");
        else if (EVENT_AC_ON(event))
            printf("AC Power On");
        else if (EVENT_BATT_LOW(event))
            printf("Battery Low");
        else if (EVENT_BATT_OK(event))
            printf("Battery Okay");
        else if (EVENT_DCM_TRBL(event))
            printf("DCM Trouble");
        else if (EVENT_DCM_OK(event))
            printf("DCM Okay");
        else if (EVENT_ENG_LOW(event))
            printf("Energy Cost Low");
        else if (EVENT_ENG_MID(event))
            printf("Energy Cost Mid");
        else if (EVENT_ENG_HIGH(event))
            printf("Energy Cost High");
        else if (EVENT_ENG_CRIT(event))
            printf("Energy Cost Critical");
        else
            printf("Unknown event 0x%04x", event);

        printf("\n");
    }

    return 0;
}

/********************************************************************************/

int do_unit_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, level, step, p1 = 0, p2;
    int err;
    char *time = NULL;
    char *user = NULL;

    /* Check arguments */
    if (argc < 3)
        return EINVAL;

    /* Parse unit */
    p2 = atoi(argv[1]);
    if ((p2 < 0) || (p2 > max_units)) {
         printf("error: unit not in range from 1 to %d\n", max_units);
         return EINVAL;
    }

    /* Parse command */
    if (strcmp_no_case(argv[2], "off") == 0)
    {
        cmd = CMD_OFF;
        time = argv[3]; 
        if (argc > 4 && strcmp_no_case(time, "user") == 0)
             user = argv[4];
    }
    else if (strcmp_no_case(argv[2], "on") == 0)
    {
        cmd = CMD_ON;
        time = argv[3]; 
        if (argc > 4 && strcmp_no_case(time, "user") == 0)
             user = argv[4];
    }
    else if (strcmp_no_case(argv[2], "dim") == 0)
    {
        if (argc < 4)
            return EINVAL;
        step = atoi(argv[3]);
        if ((step < 1) || (step > 9))
             return EINVAL;
        cmd = CMD_DIM + step;
        time = argv[4];
        if (argc > 5 && strcmp_no_case(time, "user") == 0)
             user = argv[5];
    }
    else if (strcmp_no_case(argv[2], "brighten") == 0)
    {
        if (argc < 4)
            return EINVAL;
        step = atoi(argv[3]);
        if ((step < 1) || (step > 9))
             return EINVAL;
        cmd = CMD_BRIGHTEN + step;
        time = argv[4];
        if (argc > 5 && strcmp_no_case(time, "user") == 0)
             user = argv[5];
    }
    else if (strcmp_no_case(argv[2], "level") == 0)
    {
        if (argc < 4)
            return EINVAL;
        cmd = CMD_LEVEL_TIMED;
        level = atoi(argv[3]);
        if ((level < 0) || (level > 100))
             return EINVAL;
        p2 = p2 | (level << 9);
        time = argv[4];
        if (argc > 5 && strcmp_no_case(time, "user") == 0)
             user = argv[5];
    }
    else if (strcmp_no_case(argv[2], "ramp") == 0)
    {
        if (argc < 4)
            return EINVAL;
        cmd = CMD_ALC;
        level = atoi(argv[3]);
        if ((level < 0) || (level > 100))
             return EINVAL;
        p2 = p2 | (level << 9);
        time = argv[4];
        if (argc > 5 && strcmp_no_case(time, "user") == 0)
             user = argv[5];
    }
    else
        return EINVAL;

    /* Parse time */
    if (time != NULL)
    {
        if (strcmp_no_case(time, "user") == 0)
        {
             p1 = atoi(user) + CMD_USERSETTING;
        }
        else
        {
        p1 = atoi(time);

        if (strchr(time, 'm') != 0)
            p1 += CMD_MINUTES;
        else if (strchr(time, 'h') != 0)
            p1 += CMD_HOURS;
    }
    }

    /* Send command */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_all_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, area;
    int err;

    /* Check arguments */
    if (argc < 2)
        return EINVAL;

    /* Parse command */
    if (strcmp_no_case(argv[1], "off") == 0)
        cmd = CMD_ALL_OFF;
    else if (strcmp_no_case(argv[1], "on") == 0)
        cmd = CMD_ALL_ON;
    else
        return EINVAL;

    /* Parse area */
    if (max_areas == 0)
        area = 0;
    else if (max_areas == 1)
        area = 1;
    else
    {
    if (argc == 3)
        area = atoi(argv[2]);
    else
        area = 0;
    }

    /* Check range */
    if (area > max_areas) {
         printf("error: area not in range from 1 to %d\n", max_areas);
         return EINVAL;
    }

    /* Send command */
    if ((err = omni_command(id, cmd, 0, area)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_counter_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p1 = 0, p2;
    int err;

    /* Check arguments */
    if ((argc < 3) || (argc > 4))
        return EINVAL;

    /* Parse counter */
    p2 = atoi(argv[1]);

    /* Parse command */
    if (strcmp_no_case(argv[2], "dec") == 0)
        cmd = CMD_COUNTER_DEC;
    else if (strcmp_no_case(argv[2], "inc") == 0)
        cmd = CMD_COUNTER_INC;
    else if (strcmp_no_case(argv[2], "set") == 0)
    {
        if (argc != 4)
            return EINVAL;
   
        cmd = CMD_COUNTER_SET;
        p1 = atoi(argv[3]);
    }
    else
        return EINVAL;

    /* Send command */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_alc_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int unit, level, p1, p2;
    int err;
    char *time = NULL;
    char *user = NULL;

    /* Check arguments */
    if (argc < 4)
        return EINVAL;

    /* Parse unit */
    unit = atoi(argv[1]);
    if ((unit < 0) || (unit > max_units))
        return EINVAL;
       
    /* Parse level */
    level = atoi(argv[2]);
    if ((level < 0) || (level > 100))
        return EINVAL;

    /* Calc p2 */
    p2 = (level << 9) | unit;

    /* Parse time */
    time = argv[3];
    if (argc > 4 && strcmp_no_case(time, "user") == 0)
         user = argv[4];
    if (time != NULL)
    {
        if (strcmp_no_case(time, "user") == 0)
        {
             p1 = atoi(user) + CMD_USERSETTING;
        }
        else
        {
             p1 = atoi(time);

             if (strchr(time, 'm') != 0)
        p1 += CMD_MINUTES;
             else if (strchr(time, 'h') != 0)
        p1 += CMD_HOURS;
        }
    }

    /* Send command */
    if ((err = omni_command(id, CMD_ALC, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_compose_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int p1 = 0, p2;
    int err;

    /* Check arguments */
    if ((argc < 3) || (argc > 4))
        return EINVAL;

    /* Parse unit */
    p2 = atoi(argv[1]);

    /* Parse command */
    if (strcmp_no_case(argv[2], "off") == 0)
        p1 = CMD_COMPOSE_OFF;
    else if (strcmp_no_case(argv[2], "on") == 0)
        p1 = CMD_COMPOSE_ON;
    else if (strcmp_no_case(argv[2], "scene") == 0)
    {
       /* Check arguments */
        if (argc != 4)
            return EINVAL;

       p1 = CMD_COMPOSE_SCENE + *argv[2] - 'A';
    }
    else
        return EINVAL;

    /* Send command */
    if ((err = omni_command(id, CMD_COMPOSE, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_upb_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p1 = 0, p2;
    int err;

    /* Check arguments */
    if ((argc < 3) || (argc > 5))
        return EINVAL;

    /* Parse unit */
    p2 = atoi(argv[1]);

    /* Parse command */
    if (strcmp_no_case(argv[2], "stat") == 0)
        cmd = CMD_UPB_STAT;
    else if (strcmp_no_case(argv[2], "link") == 0)
    {
       /* Check arguments */
        if (argc != 4)
            return EINVAL;

        if (strcmp_no_case(argv[3], "off") == 0)
            cmd = CMD_UPB_OFF;
        else if (strcmp_no_case(argv[3], "on") == 0)
            cmd = CMD_UPB_ON;
        else if (strcmp_no_case(argv[3], "set") == 0)
            cmd = CMD_UPB_SET;
        else
            return EINVAL;
    }
    else if (strcmp_no_case(argv[2], "led") == 0)
    {
       /* Check arguments */
        if (argc != 5)
            return EINVAL;

        /* Parse LED */
        p1 = atoi(argv[3]);

       if (strcmp_no_case(argv[4], "off") == 0)
            cmd = CMD_UPB_LED_OFF;
        else if (strcmp_no_case(argv[4], "on") == 0)
            cmd = CMD_UPB_LED_ON;
        else
            return EINVAL;
    }
    else
        return EINVAL;

    /* Send command */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_radiora_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p2;
    int err;

    /* Check arguments */
    if (argc != 3)
        return EINVAL;

    /* Parse scene */
    p2 = atoi(argv[1]);

    /* Parse command */
    if (strcmp_no_case(argv[2], "off") == 0)
        cmd = CMD_RADIORA_OFF;
    else if (strcmp_no_case(argv[2], "on") == 0)
        cmd = CMD_RADIORA_ON;
    else
        return EINVAL;

    /* Send command */
    if ((err = omni_command(id, cmd, 0, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_scene_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p2;
    int err;

    /* Check arguments */
    if (argc != 3)
        return EINVAL;

    /* Parse scene */
    p2 = atoi(argv[1]);

    /* Parse command */
    if (strcmp_no_case(argv[2], "off") == 0)
        cmd = CMD_SCENE_OFF;
    else if (strcmp_no_case(argv[2], "on") == 0)
        cmd = CMD_SCENE_ON;
    else if (strcmp_no_case(argv[2], "set") == 0)
        cmd = CMD_SCENE_SET;
    else
        return EINVAL;

    /* Send command */
    if ((err = omni_command(id, cmd, 0, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_arm_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, area;
    int err;

    /* Check arguments */
    if ((argc < 2) || (argc > 3))
        return EINVAL;

    /* Parse mode */
    if ((omni_model == LUMINA) || (omni_model == LUMINAPRO))
    {
        if (strcmp_no_case(argv[1], "home") == 0)
            cmd = CMD_HOME;
        else if (strcmp_no_case(argv[1], "sleep") == 0)
            cmd = CMD_SLEEP;
        else if (strcmp_no_case(argv[1], "away") == 0)
            cmd = CMD_AWAY;
        else if (strcmp_no_case(argv[1], "vac") == 0)
            cmd = CMD_VACATION;
        else if (strcmp_no_case(argv[1], "party") == 0)
            cmd = CMD_PARTY;
        else if (strcmp_no_case(argv[1], "special") == 0)
            cmd = CMD_SPECIAL;
        else
            return EINVAL;
    }
    else
    {
    if (strcmp_no_case(argv[1], "off") == 0)
        cmd = CMD_DISARM;
    else if (strcmp_no_case(argv[1], "disarm") == 0)
        cmd = CMD_DISARM;
    else if (strcmp_no_case(argv[1], "day") == 0)
        cmd = CMD_DAY;
    else if (strcmp_no_case(argv[1], "night") == 0)
        cmd = CMD_NIGHT;
    else if (strcmp_no_case(argv[1], "away") == 0)
        cmd = CMD_AWAY;
    else if (strcmp_no_case(argv[1], "vac") == 0)
        cmd = CMD_VACATION;
    else if (strcmp_no_case(argv[1], "dayi") == 0)
        cmd = CMD_DAY_INSTANT;
    else if (strcmp_no_case(argv[1], "nightd") == 0)
        cmd = CMD_NIGHT_DELAYED;
    else
        return EINVAL;
    }

    /* Parse area */
    if (max_areas == 0)
        area = 0;
    else if (max_areas == 1)
        area = 1;
    else {
         if (argc == 3) {
              area = atoi(argv[2]);
              /* Check range */
              if (area > max_areas) {
                   printf("error: area not in range from 1 to %d\n", max_areas);
                   return EINVAL;
              }
         } else {
              area = 0;
         }
    }
    
    /* Send command */
    if ((err = omni_command(id, cmd, code_index, area)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_button_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int button;
    int err;

    /* Check arguments */
    if (argc != 2)
        return EINVAL;

    /* Parse button */
    button = atoi(argv[1]);

    /* Check range */
    if ((button < 1) || button > max_buttons_program) {
         printf("error: button not in range from 1 to %d\n", max_buttons_program);
        return EINVAL;
    }

    /* Send command */
    if ((err = omni_command(id, CMD_BUTTON, 0, button)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_cost_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int p1;
    int err;

    /* Check arguments */
    if (argc != 2)
        return EINVAL;

    /* Parse cost */
    if (strcmp_no_case(argv[1], "low") == 0)
        p1 = CMD_COST_LOW;
    else if (strcmp_no_case(argv[1], "mid") == 0)
        p1 = CMD_COST_MID;
    else if (strcmp_no_case(argv[1], "high") == 0)
        p1 = CMD_COST_HIGH;
    else if (strcmp_no_case(argv[1], "critical") == 0)
        p1 = CMD_COST_CRITICAL;
    else
        return EINVAL;

    /* Send command */
    if ((err = omni_command(id, CMD_COST, p1, 0)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_saver_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p1, p2;
    int err;
    char *time;

    /* Check arguments */
    if (argc < 3)
        return EINVAL;

    /* Parse saver */
    p2 = atoi(argv[1]);

    /* Parse command */
    time = argv[3];
    if (strcmp_no_case(argv[2], "off") == 0)
        cmd = CMD_SAVER_OFF;
    else if (strcmp_no_case(argv[2], "on") == 0)
        cmd = CMD_SAVER_ON;
    else
        return EINVAL;

    /* Parse time */
    if (time == NULL)
        p1 = 0;
    else
    {
        p1 = atoi(time);

        if (strchr(time, 'm') != 0)
            p1 += CMD_MINUTES;
        else if (strchr(time, 'h') != 0)
            p1 += CMD_HOURS;
    }

    /* Send command */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_sensor_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p1 = 0, p2 = 0;
    int err;
    const char *units;
    double temp;

    /* Check arguments */
    if (argc != 4)
        return EINVAL;

    /* Parse device */
    if (strcmp_no_case(argv[1], "all") == 0) {
        p2 = 0;
    } else {
        p2 = atoi(argv[1]);
        /* Check range */
        if ((p2 < 1) || p2 > max_auxs) {
             printf("error: sensor not in range from 1 to %d\n", max_auxs);
             return EINVAL;
        }
    }

    /* Parse command */
    if (strcmp_no_case(argv[2], "low") == 0)
        cmd = CMD_THERMO_HEAT;
    else if (strcmp_no_case(argv[2], "high") == 0)
        cmd = CMD_THERMO_COOL;
    else if (strcmp_no_case(argv[2], "heat") == 0)
        cmd = CMD_THERMO_HEAT;
    else if (strcmp_no_case(argv[2], "cool") == 0)
        cmd = CMD_THERMO_COOL;
    else
        return EINVAL;

    /* Parse arguments */
/*     p1 = PUTF(atoi(argv[3])); */ /* RickM */
    temp = atof(argv[3]);
    units = argv[3] + (strlen(argv[3]) - 1);
    /* printf("temp=%f units=%s\n", temp, units); */ /* RickM */
    if (!isalpha(units[0]) || strcmp_no_case(units, "f") == 0) {
         p1 = (int)(PUTF(temp));
    } else if (strcmp_no_case(units, "c") == 0) {
         p1 = (int)(PUTC(temp));
    } else {
         return EINVAL;
    }
    /* printf("temp=%f units=%s p2=%d tc=%f tf=%f\n", temp, units, p1, (double)GETC(p1), (double)GETF(p1)); */ /* RickM */

    /* Send command */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_thermo_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p1 = 0, p2 = 0;
    int err;
    const char *units;
    double temp;

    /* Check arguments */
    if (argc != 4)
        return EINVAL;

    /* Parse device */
    if (strcmp_no_case(argv[1], "all") == 0) {
        p2 = 0;
    } else {
        p2 = atoi(argv[1]);
        /* Check range */
        if ((p2 < 1) || p2 > max_thermos) {
             printf("error: thermo not in range from 1 to %d\n", max_thermos);
             return EINVAL;
        }
    }

    /* Parse command */
    if (strcmp_no_case(argv[2], "mode") == 0)
        cmd = CMD_THERMO_MODE;
    else if (strcmp_no_case(argv[2], "fan") == 0)
        cmd = CMD_THERMO_FAN;
    else if (strcmp_no_case(argv[2], "hold") == 0)
        cmd = CMD_THERMO_HOLD;
    else if (strcmp_no_case(argv[2], "heat") == 0)
        cmd = CMD_THERMO_HEAT;
    else if (strcmp_no_case(argv[2], "cool") == 0)
        cmd = CMD_THERMO_COOL;
    else if (strcmp_no_case(argv[2], "low") == 0)
        cmd = CMD_THERMO_HEAT;
    else if (strcmp_no_case(argv[2], "high") == 0)
        cmd = CMD_THERMO_COOL;
    else
        return EINVAL;

    /* Parse arguments */
    switch (cmd)
    {
        case CMD_THERMO_MODE :
            if (strcmp_no_case(argv[3], "off") == 0)
                p1 = CMD_THERMO_MODE_OFF;            
            else if (strcmp_no_case(argv[3], "heat") == 0)
                p1 = CMD_THERMO_MODE_HEAT;            
            else if (strcmp_no_case(argv[3], "cool") == 0)
                p1 = CMD_THERMO_MODE_COOL;            
            else if (strcmp_no_case(argv[3], "auto") == 0)
                p1 = CMD_THERMO_MODE_AUTO;            
            else
                return EINVAL;
            break;
         case CMD_THERMO_FAN :
            if (strcmp_no_case(argv[3], "auto") == 0)
                p1 = CMD_THERMO_FAN_AUTO;            
            else if (strcmp_no_case(argv[3], "on") == 0)
                p1 = CMD_THERMO_FAN_ON;            
            else if (strcmp_no_case(argv[3], "cycle") == 0) {
                if (omni_major_version < 3) {
                     return EINVAL;
                }
                p1 = CMD_THERMO_FAN_CYCLE;
            }
            else
                return EINVAL;
            break;
        case CMD_THERMO_HOLD :
            if (strcmp_no_case(argv[3], "off") == 0)
                p1 = CMD_THERMO_HOLD_OFF;            
            else if (strcmp_no_case(argv[3], "on") == 0)
                p1 = CMD_THERMO_HOLD_ON;            
            else
                return EINVAL;
            break;
        case CMD_THERMO_HEAT :
        case CMD_THERMO_COOL :
          /*   p1 = PUTF(atoi(argv[3])); */ /* RickM */
             temp = atof(argv[3]);
             units = argv[3] + (strlen(argv[3]) - 1);
             /* printf("temp=%f units=%s\n", temp, units); */ /* RickM */
             if (!isalpha(units[0]) || strcmp_no_case(units, "f") == 0) {
                  p1 = (int)(PUTF(temp));
             } else if (strcmp_no_case(units, "c") == 0) {
                  p1 = (int)(PUTC(temp));
             } else {
                  return EINVAL;
             }
             /* printf("temp=%f units=%s p2=%d tc=%f tf=%f\n", temp, units, p1, (double)GETC(p1), (double)GETF(p1)); */ /* RickM */
            break;
    }

    /* Send command */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_mesg_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p1 = 0, p2 = 0;
    int err;

    /* Check arguments */
    if ((argc < 2) || (argc > 4))
        return EINVAL;

    /* Parse command */
    if (strcmp_no_case(argv[1], "show") == 0)
        cmd = CMD_MSG_SHOW;
    else if (strcmp_no_case(argv[1], "log") == 0)
        cmd = CMD_MSG_LOG;
    else if (strcmp_no_case(argv[1], "clear") == 0)
        cmd = CMD_MSG_CLEAR;
    else if (strcmp_no_case(argv[1], "say") == 0)
        cmd = CMD_MSG_SAY;
    else if (strcmp_no_case(argv[1], "phone") == 0)
        cmd = CMD_MSG_PHONE;
    else if (strcmp_no_case(argv[1], "send") == 0)
        cmd = CMD_MSG_SEND;
    else
        return EINVAL;

    /* Parse arguments */
    switch (cmd)
    {
        case CMD_MSG_SHOW :
        case CMD_MSG_LOG :
        case CMD_MSG_SAY :
            if (argc != 3)
                return EINVAL;
            p2 = atoi(argv[2]);
            break;
        case CMD_MSG_CLEAR :
            if (argc > 3)
                return EINVAL;
            if (argc == 3)
                p2 = atoi(argv[2]);
            break;
         case CMD_MSG_PHONE :
         case CMD_MSG_SEND :
            if (argc != 4)
                return EINVAL;
            p2 = atoi(argv[2]);
            p1 = atoi(argv[3]);           
            break;
    }

    /* Check range */
    if ((p2 < 1) || p2 > max_mesgs) {
         printf("error: mesg not in range from 1 to %d\n", max_mesgs);
         return EINVAL;
    }

    /* Send command */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_audio_cmd(hai_comm_id *id, int argc, char *argv[])
{
    int cmd, p1 = 0, p2;
    int err;

    /* Check arguments */
    if (argc < 3)
        return EINVAL;

    /* Parse zone */
    p2 = atoi(argv[1]);

    /* TODO: how to check if p2 is within limits? */

    /* Parse command */
    if (strcmp_no_case(argv[2], "off") == 0)
    {
        cmd = CMD_AUDIO;
        p1 = CMD_AUDIO_OFF; 
    }
    else if (strcmp_no_case(argv[2], "on") == 0)
    {
        cmd = CMD_AUDIO;
        p1 = CMD_AUDIO_ON; 
    }
    else if (strcmp_no_case(argv[2], "mute") == 0)
    {
        cmd = CMD_AUDIO;
        if (argc < 4)
            return EINVAL;

        if (strcmp_no_case(argv[3], "off") == 0)
            p1 = CMD_AUDIO_MUTE_OFF; 
        if (strcmp_no_case(argv[3], "on") == 0)
            p1 = CMD_AUDIO_MUTE_ON; 
        else
            return EINVAL;
    }
    else if (strcmp_no_case(argv[2], "vol") == 0)
    {
        cmd = CMD_AUDIO_VOL;
        if (argc < 4)
            return EINVAL;
        p1 = atoi(argv[3]);
        if ((p1 < 0) || (p1 > 100))
            return EINVAL;
    }
    else if (strcmp_no_case(argv[2], "src") == 0)
    {
        cmd = CMD_AUDIO_SRC;
        if (argc < 4)
            return EINVAL;
        p1 = atoi(argv[3]);
        if (p1 < 1)
            return EINVAL;
    }
    else if (strcmp_no_case(argv[2], "key") == 0)
    {
        cmd = CMD_AUDIO_KEY;
        if (argc < 4)
            return EINVAL;
        p1 = atoi(argv[3]);
        if ((p1 < 1) || (p1 > 40))
            return EINVAL;
    }
    else
        return EINVAL;

    /* Send command */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_usersetting_cmd(hai_comm_id *id, int argc, char *argv[])
{
    usersetting_stat data;
    int cmd, i, p1 = 0, p2 = 0, utype;
    int err;
    val8 month, day, hour, min;
    const char *units;
    double temp;
    const char *days="MTWTFSS";

    if (omni_major_version < 3) {
         printf("error (%d) : Invalid command: Firmware v3.0 or greater required.\n", EINVCMD);
         return 0;
    }
    
    /* Check arguments */
    if (argc != 4)
        return EINVAL;
    /* printf("args=%s/%s/%s/%s\n", argv[0], argv[1], argv[2], argv[3]); */

    /* Parse arguments */
    /* user setting number */
    p1 = atoi(argv[1]);
    
    /* Check range */
    if ((p1 < 1) || p1 > max_usersettings) {
         printf("error: usersetting not in range from 1 to %d\n", max_usersettings);
         return EINVAL;
    }

    /* Get actual usersetting type */
    if ((err = omni_usersetting_stat(id, p1, p1, &data)) != 0)
         return err;
    utype = (data.usersettings[0].type);
    /* printf("User setting %d is type: %d (%s)\n", p1, utype, usersetting_text[utype]); */

    switch (utype)
    {
    case CMD_USER_SET_NOTUSED:
         /* Can't set an unused user setting */
         return EINVAL;
         break;
    case CMD_USER_SET_NUMBER:
         if (strncmp_no_case(argv[2], "number", 3) != 0)
              return EINVAL;
         p2 = atoi(argv[3]);
         break;
    case CMD_USER_SET_DURATION:
         if (strncmp_no_case(argv[2], "duration", 3) != 0)
              return EINVAL;
         p2 = atoi(argv[3]);
         if (p2 < 10) {
              units = argv[3] + 1;
         } else if (p2 < 100) {
              units = argv[3] + 2;
         } else if (p2 < 1000) {
              units = argv[3] + 3;
         } else {
              return EINVAL;
         }
         /* printf("units=%s\n", units); */ /* RickM */
         if (strncmp_no_case(units, "s", 1) == 0) {
              /* value is OK for seconds */
         } else if (strncmp_no_case(units, "m", 1) == 0) {
              /* Add 100 for minutes */
              p2 += 100;
         } else if (strncmp_no_case(units, "h", 1) == 0) {
              /* Add 200 for hours */
              p2 += 200;
         } else {
              return EINVAL;
         }
         break;
    case CMD_USER_SET_TEMPERATURE:
         if (strncmp_no_case(argv[2], "temp", 3) != 0)
              return EINVAL;
         temp = atof(argv[3]);
         units = argv[3] + (strlen(argv[3]) - 1);
         /* printf("temp=%f units=%s\n", temp, units); */ /* RickM */
         if (!isalpha(units[0]) || strcmp_no_case(units, "f") == 0) {
              p2 = (int)(PUTF(temp));
         } else if (strcmp_no_case(units, "c") == 0) {
              p2 = (int)(PUTC(temp));
         } else {
              return EINVAL;
         }
         /* printf("temp=%f units=%s p2=%d tc=%f tf=%f\n", temp, units, p2, (double)GETC(p2), (double)GETF(p2)); */ /* RickM */
         break;
    case CMD_USER_SET_HUMIDITY:
         if (strncmp_no_case(argv[2], "humidity", 3) != 0)
              return EINVAL;
         p2 = PUTF(atoi(argv[3]));
         /* printf("p2=%02x\n", p2); */
         break;
    case CMD_USER_SET_DATE:
         if (strncmp_no_case(argv[2], "date", 3) != 0)
              return EINVAL;
         if ((strlen(argv[3]) != 5) || (argv[3][2] != '/'))
              return EINVAL; 
         month = atoi(argv[3]);
         day = atoi(argv[3] + 3);
         p2 = month*256 + day;
         /* printf("month=%d day=%d\n", month, day); */ /* RickM */
         break;
    case CMD_USER_SET_TIME:
         if (strncmp_no_case(argv[2], "time", 3) != 0)
              return EINVAL;  
         if ((strlen(argv[3]) != 5) || (argv[3][2] != ':'))
              return EINVAL;   
         hour = atoi(argv[3]);
         min = atoi(argv[3] + 3);
         p2 = hour*256 + min;
         /* printf("hour=%d min=%d\n", hour, min); */ /* RickM */
         break;
    case CMD_USER_SET_DAYSOFWEEK:
         if (strncmp_no_case(argv[2], "days", 3) != 0)
              return EINVAL;
         if (strlen(argv[3]) != 7)
              return EINVAL;
         for (i = 0; i < 7; ++i) {
              if (argv[3][i] != '-' && strncmp_no_case(argv[3]+i, days+i, 1) != 0) {
                   return EINVAL;
              }
         }
         p2 = 0;
         p2 = argv[3][0] == '-' ? p2 : p2 | 0x02;
         p2 = argv[3][1] == '-' ? p2 : p2 | 0x04;
         p2 = argv[3][2] == '-' ? p2 : p2 | 0x08;
         p2 = argv[3][3] == '-' ? p2 : p2 | 0x10;
         p2 = argv[3][4] == '-' ? p2 : p2 | 0x20;
         p2 = argv[3][5] == '-' ? p2 : p2 | 0x40;
         p2 = argv[3][6] == '-' ? p2 : p2 | 0x80;
         /* printf("days=%s %02x", argv[3], p2); */ /* RickM */
         break;
    case CMD_USER_SET_LEVEL:
         if (strncmp_no_case(argv[2], "level", 3) != 0)
              return EINVAL;
         p2 = atoi(argv[3]);
         break;
    }
         
    /* Send command */
    cmd = CMD_USER_SET;
/*    printf("cmd/p1/p2: %d/%d/%d\n", cmd, p1, p2); */ /* RickM */
    if ((err = omni_command(id, cmd, p1, p2)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_getnames(hai_comm_id *id, int argc, char *argv[])
{
    name data;
    int err, nl, index;
    name_str *ncache;
    FILE *f = stdout;

    /* Check arguments */
    if ((argc < 1) || (argc > 2))
        return EINVAL;

    /* Open file */
    if (argc == 2)
    {
        if ((f = fopen(argv[1], "w")) == NULL)
        {
            printf("error opening file %s\n", argv[1]);
            return errno;
    }
    }

    /* Start transfer */
    err = omni_get_first_name(id, &data, &nl);

    /* Zero name cache */
    if (*hainame != 0)
        name_cache_save = 1;
    for (index = 0; index < MAX_NAMES; index++)
        *name_cache.zone_name_cache[index] = 0;
    
    /* Get names */
    while (err == 0)
    {
        /* Print name info */
        fprintf(f, "%-6s", name_text[data.type - 1]);
        if (nl) {
            fprintf(f, " %3d : %s\n", GET16(data.nl.number), data.nl.text);
        } else {
            fprintf(f, " %3d : %s\n", GET8(data.ns.number), data.ns.text);       
        }

        /* Copy to name cache */
        ncache = name_index[data.type - 1];
    if (nl) {
            strncpy(ncache[GET16(data.nl.number) - 1], (char *)(data.nl.text),
                MAX_NAME_CHARS);
    } else {
            strncpy(ncache[GET8(data.ns.number) - 1], (char *)(data.ns.text),
                MAX_NAME_CHARS);
    }

        err = omni_get_next_name(id, &data, &nl);
    }
    if (err == EOMNIEOD)
        err = 0;
    name_cache_valid = 1;

    /* Close file */
    if (argc == 2)
        fclose(f);

    return err;
}

/********************************************************************************/

int do_names(hai_comm_id *id, int argc, char *argv[])
{
    int type, i;

    /* Check arguments */
    if (argc > 1)
        return EINVAL;

    /* Loop over names */
    for (type = 0; type < NUM_NAME_TYPES; type++)
    {
        for (i = 1; i < max_names_array[type]; i++)
        {
            /* Skip unnamed */
            if (!is_named(type + 1, i))
                continue;
                
            /* Print name info */
            if (!csv)
            {
                printf("%-6s", name_text[type]);
                printf(" %3d : ", i);       
                print_name(stdout, type + 1, i);
                printf("\n");
            }
            else
            {
                printf("%s,", name_text[type]);
                printf("%d,", i);       
                print_name(stdout, type + 1, i);
                printf("\n");
            }            
        }
    }

    return 0;
}

/********************************************************************************/

int do_buttons(hai_comm_id *id, int argc, char *argv[])
{
    int i;

    /* Check arguments */
    if (argc > 1)
        return EINVAL;

    /* Loop over button names */
    for (i = 1; i <= max_buttons; i++)
    {
        /* Skip unnamed */
        if (!is_named(NAMES_BUTTON, i))
            continue;
            

        /* Print name info */
        if (!csv)
        {
        printf("%3d : ", i);
        print_name(stdout, NAMES_BUTTON, i);
        printf("\n");
    }
        else
        {
            printf("%3d,", i);
            print_name(stdout, NAMES_BUTTON, i);
            printf("\n");
        }           
    }

    return 0;
}

/********************************************************************************/

int do_putnames(hai_comm_id *id, int argc, char *argv[])
{
    char line[MAX_LINE];
    char *name, *anum, *atype;
    int err, type, num, index;
    name_str *ncache;
    FILE *f;

    /* Check arguments */
    if (argc != 2)
        return EINVAL;

    /* Open file */
    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("error opening file %s\n", argv[1]);
        return errno;
    }

    /* Start transfer of names */
    if ((err = omni_start_put_name(id)) != 0)
        return err;

    /* Zero name cache */
    name_cache_save = 1;
    if (*hainame != 0)
    {
        for (index = 0; index < MAX_NAMES; index++)
            *name_cache.zone_name_cache[index] = 0;
    }
        
    /* Put names */
    while ((err == 0) && (fgets(line, MAX_LINE, f) != NULL))
    {
        /* Parse input line */
        atype = strtok(line, "\t \r\n");
        anum = strtok(NULL, "\t \r\n");
        strtok(NULL, "\t \r\n");
        name = strtok(NULL, "\r\n");

        /* Skip blank lines */
        if (atype == NULL)
            continue;

        /* Check line parse */
        if ((anum == NULL) || (name == NULL))
        {
            err = EBADFILE;
            break;
        }

        /* Lookup type */
        num = atoi(anum);
        for (type = 1; type <= NUM_NAME_TYPES ; type++)
        {
            if (strcmp_no_case(atype, name_text[type - 1]) == 0)
                break;
        }
        if (type > NUM_NAME_TYPES)
        {
            err = EBADFILE;
            break;
        }
        ncache = name_index[type - 1];

        /* Fill in cache */
        strncpy(ncache[num - 1], name, MAX_NAME_CHARS);

        /* Send */
        err = omni_put_name(id, type, num, name);
    }
    omni_end_put_name(id);

    /* Close file */
    fclose(f);

    return err;
}

/********************************************************************************/

int do_log(hai_comm_id *id, int argc, char *argv[])
{
    elog data;
    int err, p1, p2;

    /* Check arguments */
    if (argc != 1)
        return EINVAL;

    /* Get first name */
    err = omni_get_first_log(id, &data);
    while (err == 0)
    {
        /* Print header */
        if (!csv)
            printf("%3d : ", data.number);
        else
            printf("%d,", data.number);        
        if (data.date_valid)
        {
            if (!csv)
            {
                printf("%3s ", month_text[data.month - 1]);
                printf("%2d ", data.day);
                printf("%02d:%02d ", data.hour, data.minute);
            }
            else
            {
                printf("%s ", month_text[data.month - 1]);
                printf("%d ", data.day);
                printf("%02d:%02d,", data.hour, data.minute);
            }
        }
        else
            if (!csv)
                printf("<invalid>    ");
            else
                printf(",");           

        /* Print event log entry */
        p1 = GET8(data.p1);
        p2 = GET16(data.p2);
        switch (data.type)
        {
            case EVENT_ZONE_BYPASS :
                print_name(stdout, NAMES_ZONE, p2);
                printf(" bypassed");
                print_code(stdout, p1);
                break;
            case EVENT_ZONE_RESTORE :
                print_name(stdout, NAMES_ZONE, p2);
                printf(" restored");
                print_code(stdout, p1);
            break;
            case EVENT_ALL_RESTORE :
                if (p2 == 0)
                    printf("All zones restored");
                else
                {
                    printf("All ");
                    print_name(stdout, NAMES_AREA, p2);                    
                    printf(" zones restored");
                }
                print_code(stdout, p1);
                break;
            case EVENT_DISARM :
            case EVENT_DAY :
            case EVENT_NIGHT :
            case EVENT_AWAY :
            case EVENT_VACATION :
            case EVENT_DAY_INSTANT :
            case EVENT_NIGHT_DELAYED :
                if (p2 == 0)
                    printf("Armed in %s mode", sec_text[data.type - EVENT_DISARM]);
                else
                {
                    print_name(stdout, NAMES_AREA, p2);
                    printf(" armed in %s mode", sec_text[data.type - EVENT_DISARM]);
                }
                print_code(stdout, p1);
                break;
            case EVENT_ZONE_TRIPPED :
                print_name(stdout, NAMES_ZONE, p2);            
                printf(" tripped");
                break;
            case EVENT_ZONE_TROUBLE :
                print_name(stdout, NAMES_ZONE, p2);
                printf(" trouble");
                break;
            case EVENT_PHONE_ACCESS :
                printf("Remote phone access");
                print_code(stdout, p1);
                break;
            case EVENT_PHONE_LOCKOUT :
                printf("Remote phone lockout");
                break;
            case EVENT_AUTO_BYPASS :
                print_name(stdout, NAMES_ZONE, p2);            
                printf(" bypassed");
                break;
            case EVENT_TRBL_CLEARED :
                print_name(stdout, NAMES_ZONE, p2);            
                printf(" trouble cleared");
                break;
            case EVENT_PC_ACCESS :
                printf("PC access");
                print_code(stdout, p1);
                break;
            case EVENT_ALARM_ACTIVE :
                printf("%s alarm activated in ", alarm_text[p1 - 1]);
                print_name(stdout, NAMES_AREA, p2);                
                break;
            case EVENT_ALARM_RESET :
                if (p1 == 0)
                    printf("Alarm reset in ");
                else
                    printf("%s alarm reset in ", alarm_text[p1 - 1]);
                print_name(stdout, NAMES_AREA, p2);                
                break;
            case EVENT_RESET :
                printf("System reset");
                break;
            case EVENT_MESSAGE :
                print_name(stdout, NAMES_MESSAGE, p2);                
                printf(" logged");
                break;
        }
        printf("\n");

        err = omni_get_next_log(id, &data);
    }

    return 0;
}

/********************************************************************************/

int do_sec_code_valid(hai_comm_id *id, int argc, char *argv[])
{
    sec_code_valid data;
    int err, i, ncode;
    val8 area;
    val32 code;

    /* Check arguments */
    if (argc != 3)
        return EINVAL;

    /* Setup arguments */
    area = atoi(argv[1]);
    ncode = atoi(argv[2]);
    for (i = 3; i >= 0; i--)
    {
        code[i] = ncode % 10;
        ncode /= 10;
    }

    /* Check range */
    if (area > max_areas) {
         printf("error: area not in range from 1 to %d\n", max_areas);
         return EINVAL;
    }

    /* Request security code validation */
    if ((err = omni_sec_code_valid(id, area, code, &data)) != 0)
        return err;

    /* Print authority */
    if (!csv)
        printf("Authority: ");
    switch (data.authority)
    {
        case CODE_MASTER :
            printf("Master");
            break;
        case CODE_MANAGER :
            printf("Manager");
            break;
        case CODE_USER :
            printf("User");
            break;
        default :
        case CODE_INVALID   :
            printf("Invalid");
            break;
    }
    if (csv)
        printf(",");

    /* Print number */
    if (data.authority != CODE_INVALID)
    {
        if (!csv)
            printf("\nNumber: %d", data.number);
        else
            printf("%d", data.number);        
    }
    printf("\n");

    return 0;
}

/********************************************************************************/

int do_set_time(hai_comm_id *id, int argc, char *argv[])
{
    int err, i, year;
    val8 month, day, dow, hour, min, flags;

    /* Check arguments */
    if ((argc < 4) || (argc > 5))
        return EINVAL;
    if ((strlen(argv[1]) != 10) || (argv[1][2] != '/') || (argv[1][5] != '/'))
        return EINVAL;   
    if ((strlen(argv[2]) != 5) || (argv[2][2] != ':'))
        return EINVAL;   

    /* Setup date */
    month = atoi(argv[1]);
    day = atoi(argv[1] + 3);
    year = atoi(argv[1] + 6);
    if (year < TIME_YEAR_BASE)
        return EINVAL;      

    /* Setup time */
    hour = atoi(argv[2]);
    min = atoi(argv[2] + 3);

    /* Setup dow */
    dow = 0;
    for (i = 0; i < 7; i++)
    {
        if (strcmp_no_case(dow_text[i], argv[3]) == 0)
            dow = i + 1;
    }
    if (dow == 0)
        return EINVAL;        

    /* Setup flags */
    if ((argc == 5) && (strcmp_no_case(argv[4], "DST") == 0))
        flags = TIME_DST;
    else
        flags = 0;

    /* Request set time */
    if ((err = omni_set_time(id, year, month, day, dow, hour, min, flags)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_getsetup(hai_comm_id *id, int argc, char *argv[])
{
    setup data;
    int err, i;
    FILE *f = stdout;

    /* Check arguments */
    if ((argc < 1) || (argc > 2))
        return EINVAL;

    /* Open file */
    if (argc == 2)
    {
        if ((f = fopen(argv[1], "w")) == NULL)
        {
            printf("error opening file %s\n", argv[1]);
            return errno;
    }
    }

    /* Get setup */
    err = omni_get_first_setup(id, &data);
    while (err == 0)
    {
        unsigned char *d = (unsigned char*) &data.data[0];

        /* Print setup info */
        fprintf(f, "0x%04x - ", GET16(data.address));     
        for (i = 0 ; i < 16; i++)
            fprintf(f, "%02x ", d[i]);
        fprintf(f, "\n       - ");
        for (i = 0 ; i < 16; i++)
            fprintf(f, "%02x ", d[i + 16]);
        fprintf(f, "\n");

        err = omni_get_next_setup(id, &data);
    }
    if (err == EOMNIEOD)
        err = 0;

    /* Close file */
    if (argc == 2)
        fclose(f);

    return err;
}

/********************************************************************************/

int do_putsetup(hai_comm_id *id, int argc, char *argv[])
{
    setup data;
    char line[MAX_LINE];
    char *bytes;
    int i, err, val, byte = 0;
    FILE *f;

    /* Check arguments */
    if (argc != 2)
        return EINVAL;

    /* Open file */
    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("error opening file %s\n", argv[1]);
        return errno;
    }

    /* Send setup */
    if ((err = omni_start_put_setup(id)) != 0)
        return err;
    while ((err == 0) && (fgets(line, MAX_LINE, f) != NULL))
    {
        /* Find data */
        if ((bytes = strchr(line, '-')) == NULL)
        {
            err = EBADFILE;
            break;
        }
        bytes += 2;

        /* Find address */
        if (line[0] == '0')
        {
            val = strtol(line, NULL, 16);
            SET16(data.address, val);
            byte = 0;
        }

        /* Copy data */
        for (i = 0; i < 16; i++)
        {
            val = strtol(bytes, &bytes, 16);
            SET8(data.data[byte], val);
            byte++;
        }       
    
        /* Send */
        if (byte == 32) 
            err = omni_put_setup(id, &data);
    }
    omni_end_put_setup(id);

    /* Close file */
    fclose(f);

    return 0;
}

/********************************************************************************/

int do_getprogram(hai_comm_id *id, int argc, char *argv[])
{
    program data;
    int err, i;
    FILE *f = stdout;

    /* Check arguments */
    if ((argc < 1) || (argc > 2))
        return EINVAL;

    /* Open file */
    if (argc == 2)
    {
        if ((f = fopen(argv[1], "w")) == NULL)
        {
            printf("error opening file %s\n", argv[1]);
            return errno;
    }
    }

    /* Get programs */
    err = omni_get_first_program(id, &data);
    while (err == 0)
    {
        unsigned char *d = (unsigned char*) &data.data[0];

        /* Print program info */
        fprintf(f, "0x%04x - ", GET16(data.number));     
        for (i = 0 ; i < 14; i++)
            fprintf(f, "%02x ", d[i]);
        fprintf(f, "\n");

        err = omni_get_next_program(id, &data);
    }
    if (err == EOMNIEOD)
        err = 0;

    /* Close file */
    if (argc == 2)
        fclose(f);

    return err;
}

/********************************************************************************/

int do_putprogram(hai_comm_id *id, int argc, char *argv[])
{
    program data;
    char line[MAX_LINE];
    char *bytes;
    int i, err, val;
    FILE *f;

    /* Check arguments */
    if (argc != 2)
        return EINVAL;

    /* Open file */
    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("error opening file %s\n", argv[1]);
        return errno;
    }

    /* Send programs */
    if ((err = omni_start_put_program(id)) != 0)
        return err;
    while ((err == 0) && (fgets(line, MAX_LINE, f) != NULL))
    {
        /* Find data */
        if ((bytes = strchr(line, '-')) == NULL)
        {
            err = EBADFILE;
            break;
        }
        bytes += 2;

        /* Find number */
        val = strtol(line, NULL, 16);
        SET16(data.number, val);

        /* Copy data */
        for (i = 0; i < 14; i++)
        {
            val = strtol(bytes, &bytes, 16);
            SET8(data.data[i], val);
        }       
    
        /* Send */
        err = omni_put_program(id, &data);
    }
    omni_end_put_program(id);

    /* Close file */
    fclose(f);

    return 0;
}

/********************************************************************************/

int do_getvoices(hai_comm_id *id, int argc, char *argv[])
{
    voice data;
    int err, i, vl;
    FILE *f = stdout;

    /* Check arguments */
    if ((argc < 1) || (argc > 2))
        return EINVAL;

    /* Open file */
    if (argc == 2)
    {
        if ((f = fopen(argv[1], "w")) == NULL)
        {
            printf("error opening file %s\n", argv[1]);
            return errno;
    }
    }

    /* Get voices */
    err = omni_get_first_voice(id, &data, &vl);
    while (err == 0)
    {
        val16 *d = NULL;

        fprintf(f, "%-6s", name_text[data.type - 1]);
        
        if (vl)
        {
            fprintf(f, " %3d :", GET16(data.vl.number));
            d = (val16*) &data.vl.data[0];
        }
        else
        {
           fprintf(f, " %3d :", GET8(data.vs.number));
            d = (val16*) &data.vs.data[0];
        }   

        /* Print voices info */
        for (i = 0 ; i < 7; i++)
            fprintf(f, " %04x", GET16(d[i]));
        fprintf(f, "\n");

        err = omni_get_next_voice(id, &data, &vl);
    }
    if (err == EOMNIEOD)
        err = 0;

    /* Close file */
    if (argc == 2)
        fclose(f);

    return err;
}

/********************************************************************************/

int do_putvoices(hai_comm_id *id, int argc, char *argv[])
{
    val16 data[7];
    char line[MAX_LINE];
    char *bytes, *atype, *anum;
    int i, err, val, type, num;
    FILE *f;

    /* Check arguments */
    if (argc != 2)
        return EINVAL;

    /* Open file */
    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("error opening file %s\n", argv[1]);
        return errno;
    }

    /* Send voicess */
    if ((err = omni_start_put_voice(id)) != 0)
        return err;
    while ((err == 0) && (fgets(line, MAX_LINE, f) != NULL))
    {
        /* Parse input line */
        atype = strtok(line, "\t \r\n");
        anum = strtok(NULL, "\t \r\n");
        strtok(NULL, "\t \r\n");
        bytes = strtok(NULL, "\r\n");

        /* Skip blank lines */
        if (atype == NULL)
            continue;

        /* Check line parse */
        if ((anum == NULL) || (bytes == NULL))
        {
            err = EBADFILE;
            break;
        }

        /* Lookup type */
        num = atoi(anum);
        for (type = 1; type <= NUM_NAME_TYPES ; type++)
        {
            if (strcmp_no_case(atype, name_text[type - 1]) == 0)
                break;
        }
        if (type > NUM_NAME_TYPES)
        {
            err = EBADFILE;
            break;
        }

        /* Copy data */
        for (i = 0; i < 7; i++)
        {
            val = strtol(bytes, &bytes, 16);
            SET16(data[i], val);
        }       
    
        /* Send */
        err = omni_put_voice(id, type, num, data);
    }
    omni_end_put_voice(id);

    /* Close file */
    fclose(f);

    return 0;
}

/********************************************************************************/

int do_stat_summary(hai_comm_id *id, int argc, char *argv[])
{
    status_summary data;
    int err, i, j;

    /* Request system status */
    if ((err = omni_stat_summary(id, &data)) != 0)
        return err;

    /* Print time/date */
    if (!csv)
    {
        printf("Time: ");
        if (data.date_valid)
        {
            printf("%s ", dow_text[data.day_of_week - 1]);
            printf("%s ", month_text[data.month - 1]);
            printf("%d ", data.day);
            printf("%02d:%02d ", data.hour, data.minute);
            printf("%d\n", data.year + TIME_YEAR_BASE);
        }
        else
            printf("not valid\n");
    }
    else
    {
        printf("Time,");
        if (data.date_valid)
        {
            printf("%s,", dow_text[data.day_of_week - 1]);
            printf("%s,", month_text[data.month - 1]);
            printf("%d,", data.day);
            printf("%02d:%02d,", data.hour, data.minute);
            printf("%d\n", data.year + TIME_YEAR_BASE);
        }
        else
            printf(",,,,\n");
    }


    /* Print outdoor temp */
    if (!csv)
        printf("Outdoor Temp: %3dF, %5.1fC\n", GETF(data.outdoor_temp),
            GETC(data.outdoor_temp));
    else
        printf("Outdoor Temp,%d,%.1f\n", GETF(data.outdoor_temp),
            GETC(data.outdoor_temp));

    /* Print area and flag status */
    switch(omni_model)
    {
        case OMNILT :
            {
                status_summary_omnilt *sdata = &(data.omnilt);
                
                /* Print area status */
                if (!csv)
                {
                    printf("Security Modes:\n");
                    printf("    Area %d : ", 1);
                    if (sdata->area.security_mode & 0x8)
                        printf("Arming ");
                    printf("%s", sec_text[sdata->area.security_mode & 0x7]);
                    for (j = 0; j < 8; j++)
                    {
                        if (sdata->area.alarm_status & (1 << j))
                            printf(",%s", alarm_text[j]);
                    }   
                    printf("\n");
                }
                else
                {
                    printf("Security Modes,");
                    if (sdata->area.security_mode & 0x8)
                        printf("Arming ");
                    printf("%s\n", sec_text[sdata->area.security_mode & 0x7]);
                    printf("Alarm Status,");
                    printf("%02X\n", sdata->area.alarm_status);
                }
                /* Print flag status */
                if (!csv)
                {
                    printf("Flags:\n");
                    printf("    Flag %03d-%03d : %02X\n", 29, max_units, sdata->flags);
                }
                else
                {
                    printf("Flags,");
                    printf("%02X\n", sdata->flags);
                }
            }    
            break;
        case OMNI2 :
        case OMNI2E :
            {
                status_summary_omni2 *sdata = &(data.omni2);
                int flags = 7;
                int first_flag = 73;
                
                /* Print area status */
                if (!csv)
                {
                    printf("Security Modes:\n");
                    for (i = 0; i < max_areas; i++)
                    {
                        printf("    Area %d : ", i + 1);
                        if (sdata->area[i].security_mode & 0x8)
                            printf("Arming ");
                        printf("%s", sec_text[sdata->area[i].security_mode & 0x7]);
                        for (j = 0; j < 8; j++)
                        {
                            if (sdata->area[i].alarm_status & (1 << j))
                                printf(",%s", alarm_text[j]);
                        }   
                        printf("\n");
                    }
                }
                else
                {
                    printf("Security Modes,");
                    for (i = 0; i < max_areas; i++)
                    {
                        if (sdata->area[i].security_mode & 0x8)
                            printf("Arming ");
                        printf("%s", sec_text[sdata->area[i].security_mode & 0x7]);
                        if (i < (max_areas - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                    printf("Alarm Status,");
                    for (i = 0; i < max_areas; i++)
                    {
                        printf("%02X", sdata->area[i].alarm_status);
                        if (i < (max_areas - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                }
                /* Print flag status */
                if (!csv)
                {
                    printf("Flags:\n");
                    for (i = 0; i < (flags - 1); i++)
                        printf("    Flag %03d-%03d : %02X\n", (i * 8) + first_flag, (i * 8)
                            + first_flag + 7, sdata->flags[i]);
                    printf("    Flag %03d-%03d : %02X\n", (i * 8) + first_flag, max_units, 
                        sdata->flags[(flags - 1)]);
                }
                else
                {
                    printf("Flags,");
                    for (i = 0; i < flags; i++)
                    {
                        printf("%02X", sdata->flags[i]);
                        if (i < (flags - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                }
            }    
            break;
        case OMNIPRO2 :
            {
                status_summary_omnipro2 *sdata = &(data.omnipro2);
                int flags = 15;
                int first_flag = 393;

                /* Print message status */
                if (!csv)
                    printf("Memo Message: %s\n", (sdata->msg & 0x01) ? "Not played" : "Played");
                else
                    printf("Memo Message,%s\n", (sdata->msg & 0x01) ? "Not played" : "Played");
                
                /* Print area status */
                if (!csv)
                {
                    printf("Security Modes:\n");
                    for (i = 0; i < max_areas; i++)
                    {
                        printf("    Area %d : ", i + 1);
                        if (sdata->area[i].security_mode & 0x8)
                            printf("Arming ");
                        printf("%s", sec_text[sdata->area[i].security_mode & 0x7]);
                        for (j = 0; j < 8; j++)
                        {
                            if (sdata->area[i].alarm_status & (1 << j))
                                printf(",%s", alarm_text[j]);
                        }   
                        printf("\n");
                    }
                }
                else
                {
                    printf("Security Modes,");
                    for (i = 0; i < max_areas; i++)
                    {
                        if (sdata->area[i].security_mode & 0x8)
                            printf("Arming ");
                        printf("%s", sec_text[sdata->area[i].security_mode & 0x7]);
                        if (i < (max_areas - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                    printf("Alarm Status,");
                    for (i = 0; i < max_areas; i++)
                    {
                        printf("%02X", sdata->area[i].alarm_status);
                        if (i < (max_areas - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                }
                /* Print flag status */
                if (!csv)
                {
                    printf("Flags:\n");
                    for (i = 0; i < (flags - 1); i++)
                        printf("    Flag %03d-%03d : %02X\n", (i * 8) + first_flag, (i * 8)
                            + first_flag + 7, sdata->flags[i]);
                    printf("    Flag %03d-%03d : %02X\n", (i * 8) + first_flag, max_units, 
                        sdata->flags[(flags - 1)]);
                }
                else
                {
                    printf("Flags,");
                    for (i = 0; i < flags; i++)
                    {
                        printf("%02X", sdata->flags[i]);
                        if (i < (flags - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                }
            }    
            break;
        case LUMINA :
            {
                status_summary_lumina *sdata = &(data.lumina);
                int flags = 7;
                int first_flag = 73;
                
                /* Print area status */
                if (!csv)
                {
                    printf("Security Modes:\n");
                    for (i = 0; i < max_areas; i++)
                    {
                        printf("    Area %d : ", i + 1);
                        if (sdata->area[i].security_mode & 0x8)
                            printf("Arming ");
                        printf("%s", lumina_sec_text[sdata->area[i].security_mode & 0x7]);
                        for (j = 0; j < 8; j++)
                        {
                            if (sdata->area[i].alarm_status & (1 << j))
                                printf(",%s", alarm_text[j]);
                        }   
                        printf("\n");
                    }
                }
                else
                {
                    printf("Security Modes,");
                    for (i = 0; i < max_areas; i++)
                    {
                        if (sdata->area[i].security_mode & 0x8)
                            printf("Arming ");
                        printf("%s", lumina_sec_text[sdata->area[i].security_mode & 0x7]);
                        if (i < (max_areas - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                    printf("Alarm Status,");
                    for (i = 0; i < max_areas; i++)
                    {
                        printf("%02X", sdata->area[i].alarm_status);
                        if (i < (max_areas - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                }
                /* Print flag status */
                if (!csv)
                {
                    printf("Flags:\n");
                    for (i = 0; i < (flags - 1); i++)
                        printf("    Flag %03d-%03d : %02X\n", (i * 8) + first_flag, (i * 8)
                            + first_flag + 7, sdata->flags[i]);
                    printf("    Flag %03d-%03d : %02X\n", (i * 8) + first_flag, max_units, 
                        sdata->flags[(flags - 1)]);
                }
                else
                {
                    printf("Flags,");
                    for (i = 0; i < flags; i++)
                    {
                        printf("%02X", sdata->flags[i]);
                        if (i < (flags - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                }
            }    
            break;
        case LUMINAPRO:
            {
                status_summary_luminapro *sdata = &(data.luminapro);
                int flags = 15;
                int first_flag = 393;
                
                /* Print message status */
                if (!csv)
                    printf("Memo Message: %s\n", (sdata->msg & 0x01) ? "Not played" : "Played");
                else
                    printf("Memo Message,%s\n", (sdata->msg & 0x01) ? "Not played" : "Played");
                
                /* Print area status */
                if (!csv)
                {
                    printf("Security Modes:\n");
                    for (i = 0; i < max_areas; i++)
                    {
                        printf("    Area %d : ", i + 1);
                        if (sdata->area[i].security_mode & 0x8)
                            printf("Arming ");
                        printf("%s", lumina_sec_text[sdata->area[i].security_mode & 0x7]);
                        for (j = 0; j < 8; j++)
                        {
                            if (sdata->area[i].alarm_status & (1 << j))
                                printf(",%s", alarm_text[j]);
                        }   
                        printf("\n");
                    }
                }
                else
                {
                    printf("Security Modes,");
                    for (i = 0; i < max_areas; i++)
                    {
                        if (sdata->area[i].security_mode & 0x8)
                            printf("Arming ");
                        printf("%s", lumina_sec_text[sdata->area[i].security_mode & 0x7]);
                        if (i < (max_areas - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                    printf("Alarm Status,");
                    for (i = 0; i < max_areas; i++)
                    {
                        printf("%02X", sdata->area[i].alarm_status);
                        if (i < (max_areas - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                }
                /* Print flag status */
                if (!csv)
                {
                    printf("Flags:\n");
                    for (i = 0; i < (flags - 1); i++)
                        printf("    Flag %03d-%03d : %02X\n", (i * 8) + first_flag, (i * 8)
                            + first_flag + 7, sdata->flags[i]);
                    printf("    Flag %03d-%03d : %02X\n", (i * 8) + first_flag, max_units, 
                        sdata->flags[(flags - 1)]);
                }
                else
                {
                    printf("Flags,");
                    for (i = 0; i < flags; i++)
                    {
                        printf("%02X", sdata->flags[i]);
                        if (i < (flags - 1))
                            printf(",");
                        else
                            printf("\n");            
                    }
                }
            }    
            break;
    }

    return 0;
}

/********************************************************************************/

int do_temp_stat(hai_comm_id *id, int argc, char *argv[])
{
    current_temp data;
    int first, last, start, end;
    int err, i;
    int printAll = 0;

    /* Check arguments */
    if (argc > 3)
        return EINVAL;

    /* Setup arguments */
    if (argc > 1 && strcmp_no_case(argv[1], "all") == 0) {
         printAll = 1;
    }
    if (argc == 1 || printAll)
    {
        first = 1;
        last = max_temps;
    }
    else
    {
        first = atoi(argv[1]);
        if (argc > 2)
            last = atoi(argv[2]);
        else
            last = first;
    }

    /* Check range */
    if ((first < 1) || (last < first) || (last > max_temps)) {
         printf("error: temp not in range from 1 to %d\n", max_temps);
         return EINVAL;
    }

    /* Init loop parms */
    start = first;
    end = start + 100;
    if (end > last)
        end = last;

    /* Loop to get all temps */
    while(start <= last)
    {
        /* Request temps */
        if ((err = omni_current_temps(id, start, end, &data)) != 0)
            return err;

        /* Print temps */
        for (i = 0; i < (end - start + 1); i++)
        {
            /* Skip ones that are not temp types */
            if ((data.temp[i] == 0))
                continue;
            
            /* Skip unnamed */
            if (name_cache_valid && !printAll && (argc == 1) && !is_named(NAMES_THERMOSTAT, i + start) &&
                !is_named(NAMES_ZONE, i + start))
                continue;

            if (!csv)
            {
                printf("%3d : ",  i + start);
                if (is_named(NAMES_THERMOSTAT, i + start))
                    print_name_fixed(stdout, NAMES_THERMOSTAT, i + start);
                else
                    print_name_fixed(stdout, NAMES_ZONE, i + start);
                printf(" : %3dF, %5.1fC\n", GETF(data.temp[i]),
                    GETC(data.temp[i]));
            }
            else
            {
                printf("%d,",  i + start);
                if (is_named(NAMES_THERMOSTAT, i + start))
                    print_name(stdout, NAMES_THERMOSTAT, i + start);
                else
                    print_name(stdout, NAMES_ZONE, i + start);
                printf(",%d,%.1f\n", GETF(data.temp[i]),
                    GETC(data.temp[i]));
            }
        }
        
        /* Next block */
        start = end + 1;
        end = start + 100;
        if (end > last)
           end = last;        
    }

    return 0;
}

/********************************************************************************/

int do_zone_ready(hai_comm_id *id, int argc, char *argv[])
{
    zone_ready data;
    int first, last;
    int err, i;
    int printAll = 0;

    /* Check arguments */
    if (argc > 3)
        return EINVAL;

    /* Setup arguments */
    if (argc > 1 && strcmp_no_case(argv[1], "all") == 0) {
         printAll = 1;
    }
    if (argc == 1 || printAll)
    {
        first = 1;
        last = max_zones_status;
    }
    else
    {
        first = atoi(argv[1]);
        if (argc > 2)
            last = atoi(argv[2]);
        else
            last = first;
    }

    /* Check range */
    if ((first < 1) || (last < first) || (last > max_zones_status)) {
         printf("error: zone not in range from 1 to %d\n", max_zones_status);
         return EINVAL;
    }

    /* Request zone ready status */
    if ((err = omni_zone_ready(id, &data)) != 0)
        return err;

    /* Print zone ready status */
    for (i = first; i <= last; i++)
    {
        int status = data.ready[(i - 1) / 8] & (0x80 >> ((i - 1) % 8));

        /* Skip unnamed */
        if (name_cache_valid && !printAll && (argc == 1) && !is_named(NAMES_ZONE, i))
             continue;

        if (!csv)
        {
            printf("%3d : ", i);
            print_name_fixed(stdout, NAMES_ZONE, i);
            if (status)           
                printf(" : Not Ready\n");
            else
                printf(" : Ready\n");          
         }
         else
         {
            printf("%d,", i);
            print_name(stdout, NAMES_ZONE, i);
            if (status)           
                printf(",Not Ready\n");
            else
                printf(",Ready\n");          
         }
    }

    return 0;
}

/********************************************************************************/

int do_emergency(hai_comm_id *id, int argc, char *argv[])
{
    int emergency, area;
    int err;

    /* Check arguments */
    if (argc < 2)
        return EINVAL;

    /* Parse command */
    if (strcmp_no_case(argv[1], "burglary") == 0)
        emergency = EMERGENCY_BURGLARY;
    else if (strcmp_no_case(argv[1], "fire") == 0)
        emergency = EMERGENCY_FIRE;
    else if (strcmp_no_case(argv[1], "aux") == 0)
        emergency = EMERGENCY_AUX;
    else
        return EINVAL;

    /* Parse area */
    if (argc == 3)
        area = atoi(argv[2]);
    else
        area = 1;
    
    /* Check range */
    if (area > max_areas) {
         printf("error: area not in range from 1 to %d\n", max_areas);
         return EINVAL;
    }

    /* Send command */
    if ((err = omni_act_keypad_emergency(id, area, emergency)) != 0)
        return err;

    return 0;
}

/********************************************************************************/

int do_memo(hai_comm_id *id, int argc, char *argv[])
{
    int err;

    /* Check arguments */
    if (argc < 2)
        return EINVAL;

    /* Parse command */
    if (strcmp_no_case(argv[1], "play") == 0)
    {
        if ((err = omni_play_memo_msg(id)) != 0)
            return err;
    }
    else if (strcmp_no_case(argv[1], "record") == 0)
    {
        if ((err = omni_rec_memo_msg(id)) != 0)
            return err;
    }
    else
        return EINVAL;

    return 0;
}

/********************************************************************************/

void print_code(FILE *f, int code)
{
    if (code == CODE_DURESS)
        fprintf(f, " with duress code");
    else if (code == CODE_KEYSWITCH)
        fprintf(f, " with keyswith");
    else if (code == CODE_QUICK_ARM)
        fprintf(f, " with quick arm");
    else if (code == CODE_PC_ACCESS)
        fprintf(f, " with PC access");
    else if (code == CODE_PROGRAM)
        fprintf(f, " by program");
    else
    {
        fprintf(f, " by ");
        print_name(f, NAMES_CODE, code);
    }
}

/********************************************************************************/

int is_named(int type, int name)
{
    name_str *ncache = name_index[type - 1];

    return (*ncache[name - 1] != 0);  
}

/********************************************************************************/

void print_name(FILE *f, int type, int name)
{
    char printStr[33];
    name_str *ncache = name_index[type - 1];

    if (type == NAMES_ZONE && name > max_zones) {
         print_name_special_zone(name, printStr);
         fprintf(f, "%s", printStr);
         return;
    }
    
    if (is_named(type, name))
        fprintf(f, "%s", ncache[name - 1]);
    else
        fprintf(f, "%s %d", name_text[type - 1], name);
}

/********************************************************************************/

void print_name_fixed(FILE *f, int type, int name)
{
    char printStr[33];
    name_str *ncache = name_index[type - 1];

    if (type == NAMES_ZONE && name > max_zones) {
         print_name_special_zone(name, printStr);
         fprintf(f, "%16s", printStr);
         return;
    }
    
    if (is_named(type, name))
        fprintf(f, "%16s", ncache[name - 1]);
    else
        fprintf(f, "%12s %3d", name_text[type - 1], name);
}


/********************************************************************************/

void print_name_special_zone(int name, char *printStr)
{
    int status, area, emergency;
    const char *emergency_text[] = {"fire", "police", "auxiliary", "duress"};

    if (name == max_zones + 1) {
         sprintf (printStr, "%s", "Freeze alarm");
    } else if (max_areas > 0 && name >= max_zones + 2 && name <= max_zones + 1 + (max_areas * 4)) {
         status = name - max_zones - 1;
         area = (status % 4 + 1) % 2 + 1;
         emergency = (status - 1) / max_areas;
         sprintf (printStr, "%s %d %s", "Area", area, emergency_text[emergency]);
    } else {
         status = name - max_zones - (max_areas * 4) - 1;
         if (status == 1) {
              sprintf (printStr, "%s", "Battery low");
         } else if (status == 2) {
              sprintf (printStr, "%s", "AC power failure");
         } else if (status == 3) {
              sprintf (printStr, "%s", "Phone line dead");
         } else if (status == 4) {
              sprintf (printStr, "%s", "Communicator");
         } else if (status == 5) {
              switch(omni_model)
              {
              case OMNILT :
              case OMNI2:
              case OMNI2E :
              case OMNIPRO2:
                   sprintf (printStr, "%s", "Fire tamper");
                   break;
              case LUMINA:
              case LUMINAPRO:
                   sprintf (printStr, "%s", "Fuse");
                   break;
              case OMNIPRO:
                   break;
              case OMNI :
                   break;
              }
         } else if (status == 6) {
              switch(omni_model)
              {
              case OMNILT :
              case OMNI2:
              case OMNI2E :
              case OMNIPRO2:
                   sprintf (printStr, "%s", "Fuse");
                   break;
              case LUMINA:
              case LUMINAPRO:
              case OMNIPRO:
              case OMNI :
                   break;
              }
         }
    }
    return;
}
