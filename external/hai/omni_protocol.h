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

/**
 * @file  omni_protocol.h
 *
 * @brief HAI Omni Protocol Header File
 *
 * Header file containing definitions for using the omni protocol library. The
 * implementation is based on the "HAI Omni-Link Serial Protocol Description
 * Rev H".
 */

#ifndef _OMNI_PROTOCOL_H_
#define _OMNI_PROTOCOL_H_

/* Includes */

#include "hai_net.h"

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/* Defines */

/**
 * @name HAI Omni Protocol Error Codes
 * @anchor OmniErrs
 *
 * These defines are used to report Omni-Link protocol errors.
 *
 * @{
 */

/** Error code for invalid Omni argument */
#define EOMNIARGUMENT           __ELASTHAI + 1
/** Error code for unexpected Omni response */
#define EOMNIRESPONSE           __ELASTHAI + 2
/** Error code for bad CRC */
#define EOMNICRC                __ELASTHAI + 3
/** Error code for end of data */
#define EOMNIEOD                __ELASTHAI + 4
/** Define for max Omni protocol error code */
#define __ELASTOMNI             __ELASTHAI + 10
/** @} */

#define MAX_ZONES               215
#define MAX_UNITS               511
#define MAX_BUTTONS             128
#define MAX_AUXS                176
#define MAX_CODES               99
#define MAX_AREAS               8
#define MAX_THERMOS             64
#define MAX_MESGS               128
#define MAX_USERS               25
#define MAX_READERS             16
#define MAX_NAME_CHARS          16
#define MAX_EVENTS              64
#define MAX_NAMES               (MAX_ZONES + MAX_UNITS + MAX_BUTTONS    \
                                + MAX_CODES + MAX_AREAS + MAX_THERMOS   \
                                + MAX_MESGS+ MAX_USERS + MAX_READERS)
#define MAX_TEMPS               176

/**
 * @name Omni Model Numbers
 * @anchor OmniModelNumbers
 *
 * Model number in sys_info::model returned by omni_sys_info().
 *
 * @{
 */
#define OMNILT                  9       /**< HAI OmniLT */
#define OMNI                    2       /**< HAI Omni */
#define OMNI2                   15      /**< HAI Omni II */
#define OMNI2E                  30      /**< HAI Omni IIe */
#define OMNIPRO                 4       /**< HAI OmniPro */
#define OMNIPRO2                16      /**< HAI OmniPro II */
#define LUMINA                  36      /**< HAI Lumina */
#define LUMINAPRO               37      /**< HAI Lumina Pro */
/** @} */

#define CODE_INVALID            0
#define CODE_MASTER             1
#define CODE_MANAGER            2
#define CODE_USER               3

#define SEC_MODE_OFF            0
#define SEC_MODE_DAY            1
#define NIGHT                   2
#define AWAY                    3
#define VACATION                4
#define DAY_INSTANT             5
#define NIGHT_DELAYED           6

#define LUMINA_HOME             1
#define LUMINA_SLEEP            2
#define LUMINA_AWAY             3
#define LUMINA_VACATION         4
#define LUMINA_PARTY            5
#define LUMINA_SPECIAL          6

/**
 * @name Omni Expansion Status Flags
 * @anchor OmniExpansionStatusFlags
 *
 * Enclosure status flags in expansion::battery returned by 
 * omni_sys_stat().
 *
 * @{
 */ 
#define EXP_STAT_AC             0x01
#define EXP_STAT_BATT           0x02
#define EXP_STAT_COMM           0x80
/** @} */

#define UNIT_OFF                0
#define UNIT_ON                 1
#define UNIT_SCENE_A            2
#define UNIT_SCENE_L            13
#define UNIT_DIM_1              17
#define UNIT_DIM_9              25
#define UNIT_BRIGHTEN_1         33
#define UNIT_BRIGHTEN_9         41
#define UNIT_LEVEL_0            100
#define UNIT_LEVEL_100          200

#define THERMO_COMM_FAIL        0x01
#define THERMO_FREEZE_ALARM     0x02

/**
 * @name Omni Commands
 * @anchor OmniCommands
 *
 * @see omni_command().
 *
 * @{
 */ 
#define CMD_OFF                 0       /**< Turn off unit */
#define CMD_ON                  1       /**< Turn unit on */
#define CMD_ALL_OFF             2       /**< Turn all units off */
#define CMD_ALL_ON              3       /**< Turn all units on */
#define CMD_LEVEL               9       /**< Set unit level */
#define CMD_LEVEL_TIMED         101     /**< Set unit level with duration */
#define CMD_COUNTER_DEC         10      /**< Decrement counter */
#define CMD_COUNTER_INC         11      /**< Increment counter */
#define CMD_COUNTER_SET         12      /**< Set counter */
#define CMD_ALC                 13      /**< ALC ramp */
#define CMD_COMPOSE             14      /**< Lightolier Compose */
#define CMD_UPB_STAT            15      /**< Req UPB status */
#define CMD_DIM                 16      /**< Dim unit */
#define CMD_BRIGHTEN            32      /**< Brighten unit */
#define CMD_UPB_OFF             28      /**< UPB link deactivate */
#define CMD_UPB_ON              29      /**< UPB link activate */
#define CMD_UPB_SET             30      /**< UPB link store preset */
#define CMD_UPB_LED_OFF         44      /**< UPB unit LED off */
#define CMD_UPB_LED_ON          45      /**< UPB unit LED on */
#define CMD_RADIORA_OFF         46      /**< RadioRA phantom button off */
#define CMD_RADIORA_ON          47      /**< RadioRA phantom button on */
#define CMD_SCENE_OFF           60      /**< Turn scene off */
#define CMD_SCENE_ON            61      /**< Turn scene on */
#define CMD_SCENE_SET           62      /**< Set a scene */
#define CMD_DISARM              48      /**< Disarm area */
#define CMD_DAY                 49      /**< Arm area in day mode */
#define CMD_HOME                49      /**< Arm area in home mode (Lumina only) */
#define CMD_NIGHT               50      /**< Arm area in night mode */
#define CMD_SLEEP               50      /**< Arm area in sleep mode (Lumina only) */
#define CMD_AWAY                51      /**< Arm area in away mode */
#define CMD_VACATION            52      /**< Arm area in vacation mode */
#define CMD_DAY_INSTANT         53      /**< Arm area in day instant mode */
#define CMD_PARTY               53      /**< Arm area in party mode (Lumina only) */
#define CMD_NIGHT_DELAYED       54      /**< Arm area in night delayed mode */
#define CMD_SPECIAL             54      /**< Arm area in special mode (Lumina only) */
#define CMD_BUTTON              7       /**< Run button macro */
#define CMD_COST                8       /**< Set energy cost */
#define CMD_SAVER_OFF           64      /**< Turn off energy saver */
#define CMD_SAVER_ON            65      /**< Turn on energy saver */
#define CMD_THERMO_HEAT         66      /**< Set low/heat setpoint */
#define CMD_THERMO_COOL         67      /**< Set high/cool setpoint */
#define CMD_THERMO_MODE         68      /**< Set thermostat mode */
#define CMD_THERMO_FAN          69      /**< Set thermostat fan mode */
#define CMD_THERMO_HOLD         70      /**< Set thermostat hold mode */
#define CMD_MSG_SHOW            80      /**< Show message */
#define CMD_MSG_LOG             81      /**< Log message */
#define CMD_MSG_CLEAR           82      /**< Clear message */
#define CMD_MSG_SAY             83      /**< Say message */
#define CMD_MSG_PHONE           84      /**< Say message to phone */
#define CMD_MSG_SEND            85      /**< Send message to serial port */
#define CMD_USER_SET            104     /**< Set user setting */
#define CMD_AUDIO               112     /**< Set audio zone state */
#define CMD_AUDIO_VOL           113     /**< Set audio zone volume */
#define CMD_AUDIO_SRC           114     /**< Set audio zone source */
#define CMD_AUDIO_KEY           115     /**< Select audio zone key */
/** @} */

/**
 * @name Omni Command Parameters
 * @anchor OmniCommandParms
 *
 * @see omni_command().
 *
 * @{
 */ 
#define CMD_COMPOSE_OFF         0
#define CMD_COMPOSE_ON          1
#define CMD_COMPOSE_SCENE       2
#define CMD_MINUTES             100
#define CMD_HOURS               200
#define CMD_USERSETTING          218
#define CMD_COST_LOW            0
#define CMD_COST_MID            1
#define CMD_COST_HIGH           2
#define CMD_COST_CRITICAL       3
#define CMD_THERMO_MODE_OFF     0
#define CMD_THERMO_MODE_HEAT    1
#define CMD_THERMO_MODE_COOL    2
#define CMD_THERMO_MODE_AUTO    3
#define CMD_THERMO_FAN_AUTO     0
#define CMD_THERMO_FAN_ON       1
#define CMD_THERMO_FAN_CYCLE     2
#define CMD_THERMO_HOLD_OFF     0
#define CMD_THERMO_HOLD_ON      255
#define CMD_AUDIO_OFF           0
#define CMD_AUDIO_ON            1
#define CMD_AUDIO_MUTE_OFF      2
#define CMD_AUDIO_MUTE_ON       3
#define CMD_USER_SET_NOTUSED     0
#define CMD_USER_SET_NUMBER      1
#define CMD_USER_SET_DURATION    2
#define CMD_USER_SET_TEMPERATURE 3  
#define CMD_USER_SET_HUMIDITY    4
#define CMD_USER_SET_DATE        5
#define CMD_USER_SET_TIME        6
#define CMD_USER_SET_DAYSOFWEEK  7 
#define CMD_USER_SET_LEVEL       8
/** @} */

/**
 * @name Omni Name Types
 * @anchor OmniNameTypes
 *
 * @{
 */ 
#define NAMES_ZONE              1
#define NAMES_UNIT              2
#define NAMES_BUTTON            3
#define NAMES_CODE              4
#define NAMES_AREA              5
#define NAMES_THERMOSTAT        6
#define NAMES_MESSAGE           7
#define NAMES_USERSETTING       8
#define NAMES_READER            9
#define NUM_NAME_TYPES          9
/** @} */

/**
 * @name Omni Log Event Types
 * @anchor OmniLogEventTypes
 *
 * Log event type in elog::type.
 *
 * @{
 */ 
#define EVENT_ZONE_BYPASS       4
#define EVENT_ZONE_RESTORE      5
#define EVENT_ALL_RESTORE       6
#define EVENT_DISARM            48
#define EVENT_DAY               49
#define EVENT_NIGHT             50
#define EVENT_AWAY              51
#define EVENT_VACATION          52
#define EVENT_DAY_INSTANT       53
#define EVENT_NIGHT_DELAYED     54
#define EVENT_ZONE_TRIPPED      128
#define EVENT_ZONE_TROUBLE      129
#define EVENT_PHONE_ACCESS      130
#define EVENT_PHONE_LOCKOUT     131
#define EVENT_AUTO_BYPASS       132
#define EVENT_TRBL_CLEARED      133
#define EVENT_PC_ACCESS         134
#define EVENT_ALARM_ACTIVE      135
#define EVENT_ALARM_RESET       136
#define EVENT_RESET             137
#define EVENT_MESSAGE           138
/** @} */

/**
 * @name Omni Misc. Code types
 * @anchor OmniCodeTypes
 *
 * Code type in elog::p1.
 *
 * @{
 */ 
#define CODE_DURESS             251
#define CODE_KEYSWITCH          252
#define CODE_QUICK_ARM          253
#define CODE_PC_ACCESS          254
#define CODE_PROGRAM            255
/** @} */

/**
 * @name Omni System Event Macros
 * @anchor OmniSystemEventMacros
 *
 * These macros are used to parse system events info
 * from sys_events::events.
 *
 * @see omni_sys_events().
 *
 * @{
 */ 
#define EVENT_BUTTON(a)         (((a) & 0xFF00) == 0x0000)
#define EVENT_BUTTON_NUM(a)     ((a) & 0x00FF)
#define EVENT_PROLINK(a)        (((a) & 0xFF80) == 0x0100)
#define EVENT_PROLINK_NUM(a)    ((a) & 0x007F)
#define EVENT_CENTRALITE(a)     (((a) & 0xFF80) == 0x0180)
#define EVENT_CENTRALITE_NUM(a) ((a) & 0x007F)
#define EVENT_ALARM(a)          (((a) & 0xFF00) == 0x0200)
#define EVENT_ALARM_TYPE(a)     (((a) >> 4) & 0x000F)
#define EVENT_ALARM_AREA(a)     ((a) & 0x000F)
#define EVENT_ZONE(a)           (((a) & 0xFC00) == 0x0400)
#define EVENT_ZONE_NUM(a)       ((a) & 0x01FF)
#define EVENT_ZONE_STATE(a)     ((a) & 0x0200)
#define EVENT_UNIT(a)           (((a) & 0xFC00) == 0x0800)
#define EVENT_UNIT_NUM(a)       ((a) & 0x01FF)
#define EVENT_UNIT_STATE(a)     ((a) & 0x0200)
#define EVENT_COMPOSE(a)        (((a) & 0xF000) == 0x7000)
#define EVENT_COMPOSE_SCENE     2
#define EVENT_COMPOSE_STATE(a)  (((a) >> 8) & 0x000F)
#define EVENT_COMPOSE_CODE(a)   (((a) >> 4) & 0x000F)
#define EVENT_COMPOSE_NUM(a)    (((a) & 0x000F) + 1)
#define EVENT_X10(a)            (((a) & 0xFC00) == 0x0C00)
#define EVENT_X10_ALL(a)        ((a) & 0x0100)
#define EVENT_X10_STATE(a)      ((a) & 0x0200)
#define EVENT_X10_HOUSE(a)      (((a) >> 4) & 0x000F)
#define EVENT_X10_UNIT(a)       ((a) & 0x000F)
#define EVENT_ARM(a)            ((((a) & 0xF000) != 0x0000) && (((a) & 0xE000) != 0x7000))
#define EVENT_ARM_MODE(a)       (((a) >> 12) & 0x0007)
#define EVENT_ARM_AREA(a)       (((a) >> 8) & 0x000F)
#define EVENT_ARM_CODE(a)       ((a) & 0x00FF)
#define EVENT_ARM_DELAY(a)      ((a) & 0x8000)
#define EVENT_SWITCH(a)         ((((a) & 0xF000) == 0xF000) && (((a) & 0x0C00) != 0x0C00))
#define EVENT_SWITCH_STATE(a)   (((a) >> 8) & 0x000F)
#define EVENT_SWITCH_UNIT(a)    ((a) & 0x00FF)
#define EVENT_UPBLINK(a)        (((a) & 0xFC00) == 0xFC00)
#define EVENT_UPBLINK_CMD(a)    (((a) >> 8) & 0x0003)
#define EVENT_UPBLINK_NUM(a)    ((a) & 0x00FF)
#define EVENT_ALL(a)            (((a) & 0xFFE0) == 0x03E0)
#define EVENT_ALL_STATE(a)      ((a) & 0x0010)
#define EVENT_ALL_AREA(a)       ((a) & 0x000F)
#define EVENT_PHONE_DEAD(a)     ((a) == 0x0300)
#define EVENT_PHONE_RING(a)     ((a) == 0x0301)
#define EVENT_PHONE_OFF(a)      ((a) == 0x0302)
#define EVENT_PHONE_ON(a)       ((a) == 0x0303)
#define EVENT_AC_OFF(a)         ((a) == 0x0304)
#define EVENT_AC_ON(a)          ((a) == 0x0305)
#define EVENT_BATT_LOW(a)       ((a) == 0x0306)
#define EVENT_BATT_OK(a)        ((a) == 0x0307)
#define EVENT_DCM_TRBL(a)       ((a) == 0x0308)
#define EVENT_DCM_OK(a)         ((a) == 0x0309)
#define EVENT_ENG_LOW(a)        ((a) == 0x030A)
#define EVENT_ENG_MID(a)        ((a) == 0x030B)
#define EVENT_ENG_HIGH(a)       ((a) == 0x030C)
#define EVENT_ENG_CRIT(a)       ((a) == 0x030D)
/** @} */

/**
 * @name Omni Emergency Types
 * @anchor OmniEmergencyTypes
 *
 * @{
 */ 
#define EMERGENCY_BURGLARY      1
#define EMERGENCY_FIRE          2
#define EMERGENCY_AUX           3
/** @} */

/**
 * @name Omni Temp Macros
 * @anchor OmniTempMacros
 *
 * @{
 */ 
#define GETF(a)             ((((((int)GET8(a)) * 9) + 4) / 10) - 40)
/* #define GETC(a)             ((((((int)GET8(a)) * 5) + 4) / 10) - 40) */
#define GETC(a)             ((((int)GET8(a)) / 2.0) - 40)
/* #define PUTF(a)             (((10 * ((a) + 40)) + 4) / 9) */
/* #define PUTC(a)             (((10 * ((a) + 40)) + 4) / 5) */
#define PUTF(a)             (int)(((10 * ((a) + 40)) + 4.5) / 9)
#define PUTC(a)             (int)(((10 * ((a) + 40)) + 2.5) / 5)
/** @} */

/**
 * Minimum year for omni_set_time() and EPOC for some fields
 * in omni_sys_stat(). 
 */ 
#define TIME_YEAR_BASE      2000

/**
 * Indicates currently in daylight savings time (i.e. summer)
 * in the @a flag argument of omni_set_time().
 */ 
#define TIME_DST            0x01

/* Typedefs */

/**
 * Structure for return of system info.
 *
 * @brief System Info Structure
 *
 * @see omni_sys_info().
 */
typedef struct
{
    /** Omni model number. See @ref OmniModelNumbers. */
    val8 model;
    /** Firmware version major number. */
    val8 major;
    /** Firmware version minor number. */
    val8 minor;
    /** Firmware version rev number. */
    val8 rev;
    /** Local phone number. */
    val8 phone[25];
} sys_info;

/**
 * Structure for return of expansion enclosure info.
 *
 * @brief Expansion Info Structure
 *
 * @see omni_sys_stat().
 */
typedef struct
{
    /** Enclosure status. See @ref OmniExpansionStatusFlags. */
    val8 status;
    /** Enclosure battery reading. */
    val8 battery;
} expansion;

/**
 * Structure for return of the system status.
 *
 * @brief System Status Structure
 */
typedef struct
{
    val8 date_valid;
    val8 year;
    val8 month;
    val8 day;
    val8 day_of_week;
    val8 hour;
    val8 minute;
    val8 second;
    val8 dst;
    val8 sunrise_hour;
    val8 sunrise_minute;
    val8 sunset_hour;
    val8 sunset_minute;
    val8 battery;
    val8 security_mode[8];
    expansion expansion_enclosure[8];
} sys_stat;

/**
 * Structure for return of the status of a single zone.
 *
 * @brief Individual Zone Status Structure
 */
typedef struct
{
    val8 status;
    val8 loop;
} zone;

/**
 * Structure for return of the status of a set of zones.
 *
 * @brief Zones Status Structure
 */
typedef struct
{
    zone zones[MAX_ZONES];
} zone_stat;

/**
 * Structure for return of the status of a single unit.
 *
 * @brief Individual Unit Status Structure
 */
typedef struct
{
    val8 status;
    val16 time;
} unit;

/**
 * Structure for return of the status of a set of units.
 *
 * @brief Zones Status Structure
 */
typedef struct
{
    unit units[MAX_UNITS];
} unit_stat;

/**
 * Structure for return of the status of a single temp sensor.
 *
 * @brief Individual Sensor Status Structure
 */
typedef struct
{
    val8 status;
    val8 temp;
    val8 low_setpoint;
    val8 high_setpoint;
} aux;

/**
 * Structure for return of the status of a set of temp sensors.
 *
 * @brief Sensor Status Structure
 */
typedef struct
{
    aux auxs[MAX_AUXS];
} aux_stat;

/**
 * Structure for return of the status of a single thermostat.
 *
 * @brief Individual Thermostat Status Structure
 */
typedef struct
{
    val8 status;
    val8 temp;
    val8 heat_setpoint;
    val8 cool_setpoint;
    val8 sys_mode;
    val8 fan_mode;
    val8 hold_status;
} thermo;

/**
 * Structure for return of the status of a set of thermostats.
 *
 * @brief Thermostat Status Structure
 */
typedef struct
{
    thermo thermos[MAX_THERMOS];
} thermo_stat;

/**
 * Structure for return of the status of a set of messages.
 *
 * @brief Message Status Structure
 */
typedef struct
{
    val8 memo;
    val8 mesgs[MAX_MESGS / 4];
} mesg_stat;

/**
 * Structure for return of the status of a single user setting.
 *
 * @brief Individual User Setting Status Structure
 */
typedef struct
{
    val8 type;
    val8 data1;
    val8 data2;
} usersetting;

/**
 * Structure for return of the status of a set of user settings.
 *
 * @brief User Settings Status Structure
 */
typedef struct
{
    usersetting usersettings[MAX_USERS];
} usersetting_stat;

/**
 * Structure for return of the system events.
 *
 * @brief System Event Structure
 */
typedef struct
{
    val16 events[MAX_EVENTS];
} sys_events;

/**
 * Structure for a name with index < 256.
 *
 * @brief Short Name Structure
 */
typedef struct
{
    val8 number;
    val8 text[MAX_NAME_CHARS];
} name_short;

/**
 * Structure for a name with index >= 256.
 *
 * @brief Long Name Structure
 */
typedef struct
{
    val16 number;
    val8 text[MAX_NAME_CHARS];
} name_long;

/**
 * Structure for send/recv of a name.
 *
 * @brief Name Structure
 */
typedef struct
{
    val8 type;
    union
    {
        name_short ns;
        name_long nl;
    };
} name;

/**
 * Structure for return of the event log.
 *
 * @brief Event Log Structure
 */
typedef struct
{
    val8 number;
    val8 date_valid;
    val8 month;
    val8 day;
    val8 hour;
    val8 minute;
    val8 type;
    val8 p1;
    val16 p2;
} elog;

/**
 * Structure for return of a code validation.
 *
 * @brief Security Code Validation Structure
 */
typedef struct
{
    val8 number;
    val8 authority;
} sec_code_valid;

/**
 * Structure for send/recv of a setup.
 *
 * @brief Setup Structure
 */
typedef struct
{
    val16 address;
    val8 data[32];
} setup;

/**
 * Structure for send/recv of a program.
 *
 * @brief Program Structure
 */
typedef struct
{
    val16 number;
    val8 data[14];
} program;

/**
 * Structure for a voice with index < 256.
 *
 * @brief Short Voice Structure
 */
typedef struct
{
    val8 number;
    val16 data[7];
} voice_short;

/**
 * Structure for a voice with index >= 256.
 *
 * @brief Long Voice Structure
 */
typedef struct
{
    val16 number;
    val16 data[7];
} voice_long;

/**
 * Structure for send/recv of a voice.
 *
 * @brief Voice Structure
 */
typedef struct
{
    val8 type;
    union
    {
        voice_short vs;
        voice_long vl;
    };
} voice;

/**
 * Structure for return of area status.
 *
 * @brief Area Status Structure
 */
typedef struct
{
    val8 security_mode;
    val8 alarm_status;
} area_status;

/**
 * Structure for return of OmniLT status summary.
 *
 * @brief OmniLT Status Summary Structure
 */
typedef struct
{
    val8 resv;
    area_status area;
    val8 flags;
} status_summary_omnilt;

/**
 * Structure for return of Omni II and Omni IIe status summary.
 *
 * @brief Omni II/IIe Status Summary Structure
 */
typedef struct
{
    val8 resv;
    area_status area[2];
    val8 flags[7];
} status_summary_omni2;

/**
 * Structure for return of OmniPro II status summary.
 *
 * @brief OmniPro II Status Summary Structure
 */
typedef struct
{
    val8 msg;
    area_status area[8];
    val8 flags[15];
} status_summary_omnipro2;

/**
 * Structure for return of Lumina status summary.
 *
 * @brief Lumina Status Summary Structure
 */
typedef struct
{
    val8 resv;
    area_status area[1];
    val8 flags[7];
} status_summary_lumina;

/**
 * Structure for return of Lumina status summary.
 *
 * @brief Lumina Status Summary Structure
 */
typedef struct
{
    val8 msg;
    area_status area[1];
    val8 flags[15];
} status_summary_luminapro;

/**
 * Structure for return of status summary.
 *
 * @brief Status Summary Structure
 */
typedef struct
{
    val8 date_valid;
    val8 year;
    val8 month;
    val8 day;
    val8 day_of_week;
    val8 hour;
    val8 minute;
    val8 outdoor_temp;
    union
    {
        status_summary_omnilt omnilt;
        status_summary_omni2 omni2;
        status_summary_omnipro2 omnipro2;
        status_summary_lumina lumina;
        status_summary_luminapro luminapro;
    };
} status_summary;


/**
 * Structure for return of the current temps.
 *
 * @brief Current Temp Structure
 */
typedef struct
{
    val8 temp[MAX_TEMPS];
} current_temp;

/**
 * Structure for return of the zone ready status.
 *
 * @brief Zone Ready Status Structure
 */
typedef struct
{
    val8 ready[MAX_ZONES / 8];
} zone_ready;

/* Funtion prototypes */

/**
 * This function logs into the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   code        4-byte array of code digits
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_login(hai_comm_id *id, val32 code);
/**
 * This function logs out of the Omni.
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_logout(hai_comm_id *id);
/**
 * This function obtains system info from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_sys_info(hai_comm_id *id, sys_info *data);
/**
 * This function obtains system status from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_sys_stat(hai_comm_id *id, sys_stat *data);
/**
 * This function obtains zone status from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   start       starting zone number
 * @param   end         ending zone number
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_zone_stat(hai_comm_id *id, int start, int end,
    zone_stat *data);
/**
 * This function obtains unit status from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   start       starting unit number
 * @param   end         ending unit number
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_unit_stat(hai_comm_id *id, int start, int end,
    unit_stat *data);
/**
 * This function obtains temp sensor status from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   start       starting temp sensor number
 * @param   end         ending temp sensor number
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_aux_stat(hai_comm_id *id, int start, int end,
    aux_stat *data);
/**
 * This function obtains thermostat status from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   start       starting thermostat number
 * @param   end         ending thermostat number
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_thermo_stat(hai_comm_id *id, int start, int end,
    thermo_stat *data);
/**
 * This function obtains message status from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 * @param   num         pointer to return number of messages
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_mesg_stat(hai_comm_id *id, mesg_stat *data, int *num);
/**
 * This function obtains the user settings from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   start       starting user setting number
 * @param   end         ending user setting number
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_usersetting_stat(hai_comm_id *id, int start, int end, usersetting_stat *data);
/**
 * This function obtains the system events from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 * @param   num         pointer to return number of events
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_sys_events(hai_comm_id *id, sys_events *data, int *num);
/**
 * This function sends a command to the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   cmd         command
 * @param   p1          parameter one
 * @param   p2          parameter two
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_command(hai_comm_id *id, int cmd, int p1, int p2);
/**
 * This function gets the first name from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 * @param   nl          0 if name struct is short, else ~0
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_first_name(hai_comm_id *id, name *data, int *nl);
/**
 * This function gets the next name from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 * @param   nl          0 if name struct is short, else ~0
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_next_name(hai_comm_id *id, name *data, int *nl);
/**
 * This function starts the name upload process. All names
 * in the controller are erased!
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_start_put_name(hai_comm_id *id);
/**
 * This function sends a name to the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   type        type of name
 * @param   num         index number of name
 * @param   name        pointer to struct for name data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_put_name(hai_comm_id *id,  int type, int num,
    const char *name);
/**
 * This function ends the name upload process.
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_end_put_name(hai_comm_id *id);
/**
 * This function gets the first log entry from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_first_log(hai_comm_id *id, elog *data);
/**
 * This function gets the next log entry from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_next_log(hai_comm_id *id, elog *data);
/**
 * This function validates a security code.
 *
 * @param   id          pointer to HAI communication ID
 * @param   area        area (0 = any)
 * @param   code        4-byte code to validate
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_sec_code_valid(hai_comm_id *id, val8 area, val32 code,
    sec_code_valid *data);
/**
 * This function sets the time in the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   year        year (4-digit)
 * @param   month       month (1-12)
 * @param   day         day (1-31)
 * @param   dow         day of the week (1=Mon,7=Sun)
 * @param   hour        hour (0-23)
 * @param   min         minute (0-59)
 * @param   flags       flags (1=DST)
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_set_time(hai_comm_id *id, int year, val8 month, val8 day,
    val8 dow, val8 hour, val8 min, val8 flags);
/**
 * This function gets the first setup from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_first_setup(hai_comm_id *id, setup *data);
/**
 * This function gets the next setup from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_next_setup(hai_comm_id *id, setup *data);
/**
 * This function starts the setup upload process. All setup
 * data in the controller is erased!
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_start_put_setup(hai_comm_id *id);
/**
 * This function sends a setup item to the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for setup data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_put_setup(hai_comm_id *id,  setup *data);
/**
 * This function ends the setup upload process.
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_end_put_setup(hai_comm_id *id);
/**
 * This function gets the first program from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_first_program(hai_comm_id *id, program *data);
/**
 * This function gets the next program from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_next_program(hai_comm_id *id, program *data);
/**
 * This function starts the program upload process. All program
 * data in the controller is erased!
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_start_put_program(hai_comm_id *id);
/**
 * This function sends a program item to the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for program data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_put_program(hai_comm_id *id,  program *data);
/**
 * This function ends the program upload process.
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_end_put_program(hai_comm_id *id);
/**
 * This function gets the first voice from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 * @param   vl          0 if voice struct is short, else ~0
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_first_voice(hai_comm_id *id, voice *data, int *vl);
/**
 * This function gets the next voice from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 * @param   vl          0 if voice struct is short, else ~0
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_get_next_voice(hai_comm_id *id, voice *data, int *vl);
/**
 * This function starts the voice upload process. All voices
 * in the controller are erased!
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_start_put_voice(hai_comm_id *id);
/**
 * This function sends a voice to the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   type        type of voice
 * @param   num         index number of voice
 * @param   data        pointer to struct for voice data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_put_voice(hai_comm_id *id,  int type, int num,
    val16 *data);
/**
 * This function ends the voice upload process.
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_end_put_voice(hai_comm_id *id);
/**
 * This function obtains status summary from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_stat_summary(hai_comm_id *id, status_summary *data);
/**
 * This function obtains current temps for thermostats and sensors.
 *
 * @param   id          pointer to HAI communication ID
 * @param   start       starting zone
 * @param   end         ending zone
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_current_temps(hai_comm_id *id, int start, int end,
    current_temp *data);
/**
 * This function obtains zone ready status from the Omni.
 *
 * @param   id          pointer to HAI communication ID
 * @param   data        pointer to struct for return data
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_zone_ready(hai_comm_id *id, zone_ready *data);
/**
 * This function activates a keypad emergency.
 *
 * @param   id          pointer to HAI communication ID
 * @param   area        area
 * @param   emergency   emergency type to activate
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_act_keypad_emergency(hai_comm_id *id, val8 area,
    val8 emergency);
/**
 * This function plays the voice memo message.
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_play_memo_msg(hai_comm_id *id);
/**
 * This function records the voice memo message.
 *
 * @param   id          pointer to HAI communication ID
 *
 * @return Returns an error code (0 = success). Refer to 
 * @ref OmniErrs "HAI Omni Protocol Error Codes".
 */
HAIIMPORT int omni_rec_memo_msg(hai_comm_id *id);
/**
 * This function return an error string associated with @a err.
 *
 * @param   err         error code
 *
 * @return Returns a text string which describes @a err.
 */
HAIIMPORT const char *omni_strerror(int err);

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif /* _OMNI_PROTOCOL_H_ */

