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

#include <stdio.h> /* RickM */
#include <string.h>
#include "hai_net.h"
#include "hai_serial.h"
#include "omni_protocol.h"

/* Defines */

/* #define DEBUG_OMNI */

#define OMNI_START                      0x5A

#define OMNI_TYPE_DOWNLOAD_SETUP        0x01
#define OMNI_TYPE_SETUP_DATA            0x02
#define OMNI_TYPE_END_OF_DATA           0x03
#define OMNI_TYPE_UPLOAD_SETUP          0x04
#define OMNI_TYPE_ACK                   0x05
#define OMNI_TYPE_NACK                  0x06
#define OMNI_TYPE_DOWNLOAD_PROGRAM      0x07
#define OMNI_TYPE_PROGRAM_DATA          0x08
#define OMNI_TYPE_UPLOAD_PROGRAM        0x09
#define OMNI_TYPE_DOWNLOAD_NAMES        0x0A
#define OMNI_TYPE_NAME_DATA             0x0B
#define OMNI_TYPE_UPLOAD_NAMES          0x0C
#define OMNI_TYPE_UPLOAD_EVENT_LOG      0x0D
#define OMNI_TYPE_EVENT_LOG_DATA        0x0E
#define OMNI_TYPE_COMMAND               0x0F
#define OMNI_TYPE_SET_TIME              0x10
#define OMNI_TYPE_REQ_SYS_INFO          0x11
#define OMNI_TYPE_SYS_INFO              0x12
#define OMNI_TYPE_REQ_SYS_STATUS        0x13
#define OMNI_TYPE_SYS_STATUS            0x14
#define OMNI_TYPE_REQ_ZONE_STATUS       0x15
#define OMNI_TYPE_ZONE_STATUS           0x16
#define OMNI_TYPE_REQ_UNIT_STATUS       0x17
#define OMNI_TYPE_UNIT_STATUS           0x18
#define OMNI_TYPE_REQ_AUX_STATUS        0x19
#define OMNI_TYPE_AUX_STATUS            0x1A
#define OMNI_TYPE_DOWNLOAD_VOICES       0x1B
#define OMNI_TYPE_VOICE_DATA            0x1C
#define OMNI_TYPE_UPLOAD_VOICES         0x1D
#define OMNI_TYPE_REQ_THERMO_STATUS     0x1E
#define OMNI_TYPE_THERMO_STATUS         0x1F
#define OMNI_TYPE_LOGIN                 0x20
#define OMNI_TYPE_LOGOUT                0x21
#define OMNI_TYPE_REQ_SYS_EVENTS        0x22
#define OMNI_TYPE_SYS_EVENTS            0x23
#define OMNI_TYPE_REQ_MESG_STATUS       0x24
#define OMNI_TYPE_MESG_STATUS           0x25
#define OMNI_TYPE_REQ_SEC_CODE_VALID    0x26
#define OMNI_TYPE_SEC_CODE_VALID        0x27
#define OMNI_TYPE_REQ_STAT_SUMMARY      0x28
#define OMNI_TYPE_STAT_SUMMARY          0x29
#define OMNI_TYPE_REQ_CURRENT_TEMPS     0x2A
#define OMNI_TYPE_CURRENT_TEMPS         0x2B
#define OMNI_TYPE_REQ_ZONE_READY        0x2C
#define OMNI_TYPE_ZONE_READY            0x2D
#define OMNI_TYPE_ACT_KEYPAD_EMERGENCY  0x2E
#define OMNI_TYPE_PLAY_MEMO_MSG         0x2F
#define OMNI_TYPE_REC_MEMO_MSG          0x30
#define OMNI_TYPE_REQ_USERSETTING_STATUS 0x3D
#define OMNI_TYPE_USERSETTING_STATUS     0x3E

/* Typedefs */

typedef struct
{
    val8 start;
    val8 len;
    val8 type;
} omni_msg_header;

typedef struct
{
    omni_msg_header header;
    val16 crc;
} omni_msg;

typedef struct
{
    omni_msg_header header;
    val32 code;
    val16 crc;
} omni_msg_login;

typedef struct
{
    omni_msg_header header;
    sys_info data;
    val16 crc;
} omni_msg_sys_info;

typedef struct
{
    omni_msg_header header;
    sys_stat data;
    val16 crc;
} omni_msg_sys_stat;

typedef struct
{
    omni_msg_header header;
    val8 start;
    val8 end;
    val16 crc;
} omni_msg_req_zone_stat;

typedef struct
{
    omni_msg_header header;
    zone_stat data;
    val16 crc;
} omni_msg_zone_stat;

typedef struct
{
    omni_msg_header header;
    val8 start;
    val8 end;
    val16 crc;
} omni_msg_req_unit_stat;

typedef struct
{
    omni_msg_header header;
    val16 start;
    val16 end;
    val16 crc;
} omni_msg_req_unit_stat_big;

typedef struct
{
    omni_msg_header header;
    unit_stat data;
    val16 crc;
} omni_msg_unit_stat;

typedef struct
{
    omni_msg_header header;
    val8 start;
    val8 end;
    val16 crc;
} omni_msg_req_aux_stat;

typedef struct
{
    omni_msg_header header;
    aux_stat data;
    val16 crc;
} omni_msg_aux_stat;

typedef struct
{
    omni_msg_header header;
    val8 start;
    val8 end;
    val16 crc;
} omni_msg_req_thermo_stat;

typedef struct
{
    omni_msg_header header;
    thermo_stat data;
    val16 crc;
} omni_msg_thermo_stat;

typedef struct
{
    omni_msg_header header;
    mesg_stat data;
    val16 crc;
} omni_msg_mesg_stat;

typedef struct
{
    omni_msg_header header;
    val8 start;
    val8 end;
    val16 crc;
} omni_msg_req_usersetting_stat;

typedef struct
{
    omni_msg_header header;
    zone_stat data;
    val16 crc;
} omni_msg_usersetting_stat;

typedef struct
{
    omni_msg_header header;
    sys_events data;
    val16 crc;
} omni_msg_sys_events;

typedef struct
{
    omni_msg_header header;
    val8 cmd;
    val8 p1;
    val16 p2;
    val16 crc;
} omni_msg_unit_cmd;

typedef struct
{
    omni_msg_header header;
    val8 area;
    val32 code;
    val16 crc;
} omni_msg_req_sec_code_valid;

typedef struct
{
    omni_msg_header header;
    name data;
    val16 crc;
} omni_msg_name;

typedef struct
{
    omni_msg_header header;
    elog data;
    val16 crc;
} omni_msg_log;

typedef struct
{
    omni_msg_header header;
    sec_code_valid data;
    val16 crc;
} omni_msg_sec_code_valid;

typedef struct
{
    omni_msg_header header;
    val8 year;
    val8 month;
    val8 day;
    val8 day_of_week;
    val8 hour;
    val8 min;
    val8 flags;
    val16 crc;
} omni_msg_set_time;

typedef struct
{
    omni_msg_header header;
    setup data;
    val16 crc;
} omni_msg_setup;

typedef struct
{
    omni_msg_header header;
    program data;
    val16 crc;
} omni_msg_program;

typedef struct
{
    omni_msg_header header;
    voice data;
    val16 crc;
} omni_msg_voice;

typedef struct
{
    omni_msg_header header;
    zone_ready data;
    val16 crc;
} omni_msg_zone_ready;

typedef struct
{
    omni_msg_header header;
    status_summary data;
    val16 crc;
} omni_msg_status_summary;

typedef struct
{
    omni_msg_header header;
    val8 start;
    val8 end;
    val16 crc;
} omni_msg_req_current_temps;

typedef struct
{
    omni_msg_header header;
    current_temp data;
    val16 crc;
} omni_msg_current_temps;

typedef struct
{
    omni_msg_header header;
    val8 area;
    val8 emergency_type;
    val16 crc;
} omni_msg_activate_keypad_emergency;

/* Function prototypes */

static int omni_send_msg(hai_comm_id *id, int type, void *msg, int len);
static int omni_recv_msg(hai_comm_id *id, int *type, void *msg, int *len);
static int crc16(const void *data, int len);

/********************************************************************************/

HAIEXPORT int omni_login(hai_comm_id *id, val32 code)
{
    omni_msg_login req;
    omni_msg resp;
    int err, type, len;

    /* Send login message */
    memcpy (req.code, code, 4);
    if ((err = omni_send_msg(id, OMNI_TYPE_LOGIN,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_logout(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send logout message */
    if ((err = omni_send_msg(id, OMNI_TYPE_LOGOUT,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_sys_info(hai_comm_id *id, sys_info *data)
{
    omni_msg req;
    omni_msg_sys_info resp;
    int err, type, len;

    /* Send request system information */
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_SYS_INFO,
            &req, sizeof(req))) != 0)
        return err;

    /* Get system information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_SYS_INFO)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(sys_info));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_sys_stat(hai_comm_id *id, sys_stat *data)
{
    omni_msg req;
    omni_msg_sys_stat resp;
    int err, type, len;

    /* Send request system status */
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_SYS_STATUS,
            &req, sizeof(req))) != 0)
        return err;

    /* Get system status */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_SYS_STATUS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(sys_stat));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_zone_stat(hai_comm_id *id, int start, int end, zone_stat *data)
{
    omni_msg_req_zone_stat req;
    omni_msg_zone_stat resp;
    int err, type, len;

    /* Send request zone status */
    SET8(req.start, start);
    SET8(req.end, end);
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_ZONE_STATUS,
            &req, sizeof(req))) != 0)
        return err;

    /* Get zone information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ZONE_STATUS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(zone_stat));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_unit_stat(hai_comm_id *id, int start, int end, unit_stat *data)
{
    omni_msg_req_unit_stat req;
    omni_msg_req_unit_stat_big req_big;
    omni_msg_unit_stat resp;
    int err, type, len;

    /* Send request unit status */
    if ((start < 256) && (end < 256))
    {
        SET8(req.start, start);
        SET8(req.end, end);
        if ((err = omni_send_msg(id, OMNI_TYPE_REQ_UNIT_STATUS,
                &req, sizeof(req))) != 0)
            return err;
    }
    else
    {
        SET16(req_big.start, start);
        SET16(req_big.end, end);
        if ((err = omni_send_msg(id, OMNI_TYPE_REQ_UNIT_STATUS,
                &req_big, sizeof(req_big))) != 0)
            return err;
    }

    /* Get unit information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_UNIT_STATUS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(unit_stat));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_aux_stat(hai_comm_id *id, int start, int end, aux_stat *data)
{
    omni_msg_req_aux_stat req;
    omni_msg_aux_stat resp;
    int err, type, len;

    /* Send request aux status */
    SET8(req.start, start);
    SET8(req.end, end);
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_AUX_STATUS,
            &req, sizeof(req))) != 0)
        return err;

    /* Get aux information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_AUX_STATUS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(aux_stat));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_thermo_stat(hai_comm_id *id, int start, int end,
    thermo_stat *data)
{
    omni_msg_req_thermo_stat req;
    omni_msg_thermo_stat resp;
    int err, type, len;

    /* Send request thermo status */
    SET8(req.start, start);
    SET8(req.end, end);
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_THERMO_STATUS,
            &req, sizeof(req))) != 0)
        return err;

    /* Get thermostat information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_THERMO_STATUS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(thermo_stat));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_mesg_stat(hai_comm_id *id, mesg_stat *data, int *num)
{
    omni_msg req;
    omni_msg_mesg_stat resp;
    int err, type, len;

    /* Send request message status */
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_MESG_STATUS,
            &req, sizeof(req))) != 0)
        return err;

    /* Get message information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_MESG_STATUS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(mesg_stat));
    *num = (resp.header.len - 2) * 4;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_usersetting_stat(hai_comm_id *id, int start, int end, usersetting_stat *data)
{
    omni_msg_req_usersetting_stat req;
    omni_msg_usersetting_stat resp;
    int err, type, len;

    /* Send request user setting status */
    SET8(req.start, start);
    SET8(req.end, end);
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_USERSETTING_STATUS,
            &req, sizeof(req))) != 0)
        return err;

    /* Get user setting information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;

    if (type != OMNI_TYPE_USERSETTING_STATUS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(usersetting_stat));

    return 0;
};

/********************************************************************************/

HAIEXPORT int omni_sys_events(hai_comm_id *id, sys_events *data, int *num)
{
    omni_msg req;
    omni_msg_sys_events resp;
    int err, type, len;

    /* Send request system events */
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_SYS_EVENTS,
            &req, sizeof(req))) != 0)
        return err;

    /* Get system events */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_SYS_EVENTS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(sys_events));
    *num = (resp.header.len - 1) / 2;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_command(hai_comm_id *id, int cmd, int p1, int p2)
{
    omni_msg_unit_cmd req;
    omni_msg resp;
    int err, type, len;

    /* Send command */
    SET8(req.cmd, cmd);
    SET8(req.p1, p1);
    SET16(req.p2, p2);
    if ((err = omni_send_msg(id, OMNI_TYPE_COMMAND,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_first_name(hai_comm_id *id, name *data, int *nl)
{
    omni_msg req;
    omni_msg_name resp;
    int err, type, len;

    /* Send request name upload */
    if ((err = omni_send_msg(id, OMNI_TYPE_UPLOAD_NAMES,
            &req, sizeof(req))) != 0)
        return err;

    /* Get name */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_NAME_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(name));
    *nl = ((len == 20) || (len == 17));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_next_name(hai_comm_id *id, name *data, int *nl)
{
    omni_msg req;
    omni_msg_name resp;
    int err, type, len;

    /* Send acknowledge */
    if ((err = omni_send_msg(id, OMNI_TYPE_ACK,
            &req, sizeof(req))) != 0)
        return err;

    /* Get name */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
    {
        if (err == EHAITIMEOUT)
        {
            /* Send negative acknowledge */
            if ((err = omni_send_msg(id, OMNI_TYPE_NACK,
                    &req, sizeof(req))) != 0)
                return err;

            /* Get name again */
            len = sizeof(resp);
            if ((err = omni_recv_msg(id, &type,
                    &resp, &len)) != 0)
                return err;       
        }
        else
            return err;    
    }

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_NAME_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(name));
    *nl = ((len == 20) || (len == 17));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_start_put_name(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send start name download */
    if ((err = omni_send_msg(id, OMNI_TYPE_DOWNLOAD_NAMES,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_put_name(hai_comm_id *id,  int type, int num, const char *name)
{
    omni_msg_name req;
    omni_msg resp;
    int err, len, len2;

    /* Determine max name length */
    if ((type == NAMES_ZONE) || (type == NAMES_MESSAGE) || (type == NAMES_USERSETTING) || (type == NAMES_READER))
        len = 15;
    else
        len = 12;

    /* Setup data */
    SET8(req.data.type, type);
    if (num < 256)
    {
        SET8(req.data.ns.number, num);
        strncpy((char *)(req.data.ns.text), name, len);
        req.data.ns.text[len] = 0;
        len += 8;
    }
    else
    {
        SET16(req.data.nl.number, num);
        strncpy((char *)(req.data.nl.text), name, len);
        req.data.nl.text[len] = 0;
        len += 9;
    }

    /* Send name */
    if ((err = omni_send_msg(id, OMNI_TYPE_NAME_DATA,
            &req, len)) != 0)
        return err;

    /* Get acknowledgement */
    len2 = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len2)) != 0)
        return err;
    if (type == OMNI_TYPE_NACK)
    {
        /* Send name again */
        if ((err = omni_send_msg(id, OMNI_TYPE_NAME_DATA,
                &req, len)) != 0)
            return err;

        /* Get acknowledgement */
        len2 = sizeof(resp);
        if ((err = omni_recv_msg(id, &type,
                &resp, &len2)) != 0)
            return err;
    }
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_end_put_name(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send end name download */
    if ((err = omni_send_msg(id, OMNI_TYPE_END_OF_DATA,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_first_log(hai_comm_id *id, elog *data)
{
    omni_msg req;
    omni_msg_log resp;
    int err, type, len;

    /* Send request event log upload */
    if ((err = omni_send_msg(id, OMNI_TYPE_UPLOAD_EVENT_LOG,
            &req, sizeof(req))) != 0)
        return err;

    /* Get event */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_EVENT_LOG_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(elog));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_next_log(hai_comm_id *id, elog *data)
{
    omni_msg req;
    omni_msg_log resp;
    int err, type, len;

    /* Send acknowledge */
    if ((err = omni_send_msg(id, OMNI_TYPE_ACK,
            &req, sizeof(req))) != 0)
        return err;

    /* Get event */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
    {
        if (err == EHAITIMEOUT)
        {
            /* Send negative acknowledge */
            if ((err = omni_send_msg(id, OMNI_TYPE_NACK,
                    &req, sizeof(req))) != 0)
                return err;

            /* Get event again */
            len = sizeof(resp);
            if ((err = omni_recv_msg(id, &type,
                    &resp, &len)) != 0)
                return err;

        }
        else
            return err;
    }
        
    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_EVENT_LOG_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(elog));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_sec_code_valid(hai_comm_id *id, val8 area, val32 code,
    sec_code_valid *data)
{
    omni_msg_req_sec_code_valid req;
    omni_msg_sec_code_valid resp;
    int err, type, len;

    /* Send request security code validation */
    SET8(req.area, area);
    memcpy(req.code, code, 4);
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_SEC_CODE_VALID,
            &req, sizeof(req))) != 0)
        return err;

    /* Get security code validation */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_SEC_CODE_VALID)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(sec_code_valid));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_set_time(hai_comm_id *id, int year, val8 month, val8 day,
    val8 dow, val8 hour, val8 min, val8 flags)
{
    omni_msg_set_time req;
    omni_msg resp;
    int err, type, len;

    /* Set time */
    SET8(req.year, year - TIME_YEAR_BASE);
    SET8(req.month, month);
    SET8(req.day, day);
    SET8(req.day_of_week, dow);
    SET8(req.hour, hour);
    SET8(req.min, min);
    SET8(req.flags, flags);
    if ((err = omni_send_msg(id, OMNI_TYPE_SET_TIME,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_first_setup(hai_comm_id *id, setup *data)
{
    omni_msg req;
    omni_msg_setup resp;
    int err, type, len;

    /* Send request setup upload */
    if ((err = omni_send_msg(id, OMNI_TYPE_UPLOAD_SETUP,
            &req, sizeof(req))) != 0)
        return err;

    /* Get name */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_SETUP_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(setup));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_next_setup(hai_comm_id *id, setup *data)
{
    omni_msg req;
    omni_msg_setup resp;
    int err, type, len;

    /* Send acknowledge */
    if ((err = omni_send_msg(id, OMNI_TYPE_ACK,
            &req, sizeof(req))) != 0)
        return err;

    /* Get setup */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
    {
        if (err == EHAITIMEOUT)
        {
            /* Send negative acknowledge */
            if ((err = omni_send_msg(id, OMNI_TYPE_NACK,
                    &req, sizeof(req))) != 0)
                return err;

            /* Get setup again */
            len = sizeof(resp);
            if ((err = omni_recv_msg(id, &type,
                    &resp, &len)) != 0)
                return err;       
        }
        else
            return err;    
    }

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_SETUP_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(setup));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_start_put_setup(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send start setup download */
    if ((err = omni_send_msg(id, OMNI_TYPE_DOWNLOAD_SETUP,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_put_setup(hai_comm_id *id,  setup *data)
{
    omni_msg_setup req;
    omni_msg resp;
    int err, len, type;

    /* Setup data */
    memcpy(&req.data, data, sizeof(setup));

    /* Send data */
    if ((err = omni_send_msg(id, OMNI_TYPE_SETUP_DATA,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type == OMNI_TYPE_NACK)
    {
        /* Send data again */
        if ((err = omni_send_msg(id, OMNI_TYPE_SETUP_DATA,
                &req, sizeof(req))) != 0)
            return err;

        /* Get acknowledgement */
        len = sizeof(resp);
        if ((err = omni_recv_msg(id, &type,
                &resp, &len)) != 0)
            return err;
    }
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_end_put_setup(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send end setup download */
    if ((err = omni_send_msg(id, OMNI_TYPE_END_OF_DATA,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_first_program(hai_comm_id *id, program *data)
{
    omni_msg req;
    omni_msg_program resp;
    int err, type, len;

    /* Send request program upload */
    if ((err = omni_send_msg(id, OMNI_TYPE_UPLOAD_PROGRAM,
            &req, sizeof(req))) != 0)
        return err;

    /* Get name */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_PROGRAM_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(program));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_next_program(hai_comm_id *id, program *data)
{
    omni_msg req;
    omni_msg_program resp;
    int err, type, len;

    /* Send acknowledge */
    if ((err = omni_send_msg(id, OMNI_TYPE_ACK,
            &req, sizeof(req))) != 0)
        return err;

    /* Get program */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
    {
        if (err == EHAITIMEOUT)
        {
            /* Send negative acknowledge */
            if ((err = omni_send_msg(id, OMNI_TYPE_NACK,
                    &req, sizeof(req))) != 0)
                return err;

            /* Get program again */
            len = sizeof(resp);
            if ((err = omni_recv_msg(id, &type,
                    &resp, &len)) != 0)
                return err;       
        }
        else
            return err;    
    }

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_PROGRAM_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(program));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_start_put_program(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send start program download */
    if ((err = omni_send_msg(id, OMNI_TYPE_DOWNLOAD_PROGRAM,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_put_program(hai_comm_id *id,  program *data)
{
    omni_msg_program req;
    omni_msg resp;
    int err, len, type;

    /* program data */
    memcpy(&req.data, data, sizeof(program));

    /* Send data */
    if ((err = omni_send_msg(id, OMNI_TYPE_PROGRAM_DATA,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type == OMNI_TYPE_NACK)
    {
        /* Send data again */
        if ((err = omni_send_msg(id, OMNI_TYPE_PROGRAM_DATA,
                &req, sizeof(req))) != 0)
            return err;

        /* Get acknowledgement */
        len = sizeof(resp);
        if ((err = omni_recv_msg(id, &type,
                &resp, &len)) != 0)
            return err;
    }
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_end_put_program(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send end program download */
    if ((err = omni_send_msg(id, OMNI_TYPE_END_OF_DATA,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_first_voice(hai_comm_id *id, voice *data, int *vl)
{
    omni_msg req;
    omni_msg_voice resp;
    int err, type, len;

    /* Send request voice upload */
    if ((err = omni_send_msg(id, OMNI_TYPE_UPLOAD_VOICES,
            &req, sizeof(req))) != 0)
        return err;

    /* Get voice */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_VOICE_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(voice));
    *vl = (len == 18);

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_get_next_voice(hai_comm_id *id, voice *data, int *vl)
{
    omni_msg req;
    omni_msg_voice resp;
    int err, type, len;

    /* Send acknowledge */
    if ((err = omni_send_msg(id, OMNI_TYPE_ACK,
            &req, sizeof(req))) != 0)
        return err;

    /* Get voice */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
    {
        if (err == EHAITIMEOUT)
        {
            /* Send negative acknowledge */
            if ((err = omni_send_msg(id, OMNI_TYPE_NACK,
                    &req, sizeof(req))) != 0)
                return err;

            /* Get voice again */
            len = sizeof(resp);
            if ((err = omni_recv_msg(id, &type,
                    &resp, &len)) != 0)
                return err;       
        }
        else
            return err;    
    }

    if (type == OMNI_TYPE_END_OF_DATA)
        return EOMNIEOD;
    if (type != OMNI_TYPE_VOICE_DATA)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(voice));
    *vl = (len == 18);

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_start_put_voice(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send start voice download */
    if ((err = omni_send_msg(id, OMNI_TYPE_DOWNLOAD_VOICES,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_put_voice(hai_comm_id *id,  int type, int num, val16 *data)
{
    omni_msg_voice req;
    omni_msg resp;
    int err, len, len2;

    /* Setup data */
    len = 14;
    SET8(req.data.type, type);
    if (num < 256)
    {
        SET8(req.data.vs.number, num);
        memcpy(req.data.vs.data, data, len);
        len += 8;
    }
    else
    {
        SET16(req.data.vl.number, num);
        memcpy(req.data.vl.data, data, len);
        len += 9;
    }

    /* Send voice */
    if ((err = omni_send_msg(id, OMNI_TYPE_VOICE_DATA,
            &req, len)) != 0)
        return err;

    /* Get acknowledgement */
    len2 = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len2)) != 0)
        return err;
    if (type == OMNI_TYPE_NACK)
    {
        /* Send voice again */
        if ((err = omni_send_msg(id, OMNI_TYPE_VOICE_DATA,
                &req, len)) != 0)
            return err;

        /* Get acknowledgement */
        len2 = sizeof(resp);
        if ((err = omni_recv_msg(id, &type,
                &resp, &len2)) != 0)
            return err;
    }
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_end_put_voice(hai_comm_id *id)
{
    omni_msg req, resp;
    int err, type, len;

    /* Send end voice download */
    if ((err = omni_send_msg(id, OMNI_TYPE_END_OF_DATA,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_stat_summary(hai_comm_id *id, status_summary *data)
{
    omni_msg req;
    omni_msg_status_summary resp;
    int err, type, len;

    /* Send request status summary */
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_STAT_SUMMARY,
            &req, sizeof(req))) != 0)
        return err;

    /* Get status summary */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_STAT_SUMMARY)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(status_summary));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_current_temps(hai_comm_id *id, int start, int end,
    current_temp *data)
{
    omni_msg_req_current_temps req;
    omni_msg_current_temps resp;
    int err, type, len;

    /* Send request current temps */
    SET8(req.start, start);
    SET8(req.end, end);
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_CURRENT_TEMPS,
            &req, sizeof(req))) != 0)
        return err;

    /* Get current temp information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_CURRENT_TEMPS)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(current_temp));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_zone_ready(hai_comm_id *id, zone_ready *data)
{
    omni_msg req;
    omni_msg_zone_ready resp;
    int err, type, len;

    /* Send request zone ready status */
    if ((err = omni_send_msg(id, OMNI_TYPE_REQ_ZONE_READY,
            &req, sizeof(req))) != 0)
        return err;

    /* Get current zone ready information */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ZONE_READY)
        return EOMNIRESPONSE;

    /* Return results */
    memcpy(data, &resp.data, sizeof(zone_ready));

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_act_keypad_emergency(hai_comm_id *id, val8 area,
    val8 emergency)
{
    omni_msg_activate_keypad_emergency req;
    omni_msg resp;
    int err, type, len;

    /* Send activate keypad emergency command */
    SET8(req.area, area);
    SET8(req.emergency_type, emergency);
    if ((err = omni_send_msg(id, OMNI_TYPE_ACT_KEYPAD_EMERGENCY,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_play_memo_msg(hai_comm_id *id)
{
    omni_msg req;
    omni_msg resp;
    int err, type, len;

    /* Send play memo command */
    if ((err = omni_send_msg(id, OMNI_TYPE_PLAY_MEMO_MSG,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT int omni_rec_memo_msg(hai_comm_id *id)
{
    omni_msg req;
    omni_msg resp;
    int err, type, len;

    /* Send record memo command */
    if ((err = omni_send_msg(id, OMNI_TYPE_REC_MEMO_MSG,
            &req, sizeof(req))) != 0)
        return err;

    /* Get acknowledgement */
    len = sizeof(resp);
    if ((err = omni_recv_msg(id, &type,
            &resp, &len)) != 0)
        return err;
    if (type != OMNI_TYPE_ACK)
        return EOMNIRESPONSE;

    return 0;
}

/********************************************************************************/

HAIEXPORT const char *omni_strerror(int err)
{
    switch (err)
    {
        case EOMNIARGUMENT :
            return "Bad omni function argument";
        case EOMNIRESPONSE :
            return "Unexpected response from Omni";
        case EOMNICRC :
            return "Bad CRC from Omni";
        case EOMNIEOD :
            return "End of Omni data";
    }

    return NULL;
}

/********************************************************************************/

static int omni_send_msg(hai_comm_id *id, int type, void *msg, int len)
{
    omni_msg_header *header = msg;
    unsigned char *crc_ptr = (unsigned char*) msg + len - 2;
    int crc, err;

    /* Setup header */
    SET8(header->start, OMNI_START);
    SET8(header->type, type);
    SET8(header->len, len - 4);

    /* Calc CRC */
    crc = crc16((unsigned char*) msg + 1, len - 3);
    crc_ptr[0] = crc & 0xFF;
    crc_ptr[1] = (crc >> 8) & 0xFF;

#ifdef DEBUG_OMNI
    {
        int i;

        printf("Tx: ");
        for (i = 0 ; i < GET8(header->len) + 4; i++)
            printf("0x%02x ", ((unsigned char*) msg)[i]);
        printf("\n");
    }
#endif

    /* Send message */
    if (id->serial_mode)
    {
#ifdef HAISERIAL_SUPPORT
        /* Send serial message */
        if ((err = hai_serial_send_msg(id, msg, len)) != 0)
            return err;
#endif
    }
    else
    {
        /* Send network message */
        if ((err = hai_net_send_msg(id, OMNI_LINK_MESSAGE,
                msg, len)) != 0)
            return err;
    }

    return 0;
}

/********************************************************************************/

static int omni_recv_msg(hai_comm_id *id, int *type, void *msg, int *len)
{
    int err, crc;
    omni_msg_header *header = msg;
    unsigned char *crc_ptr;

    /* Recv message */
    if (id->serial_mode)
    {
#ifdef HAISERIAL_SUPPORT
        int temp_len;

        /* Recv serial header */
        temp_len = sizeof(omni_msg_header);
        if ((err = hai_serial_recv_msg(id, msg, &temp_len)) != 0)
            return err;

        /* Check header */
        if (temp_len != sizeof(omni_msg_header))
            return EOMNIRESPONSE;
        if (GET8(header->start) != OMNI_START)
            return EOMNIRESPONSE;

        /* Recv serial message */
        temp_len = GET8(header->len) + 1;
        if ((err = hai_serial_recv_msg(id, (unsigned char*) msg
                + sizeof(omni_msg_header), &temp_len)) != 0)
            return err;
#endif
    }
    else
    {
        /* Recv network message */
        if ((err = hai_net_recv_msg(id, (hai_msg_type*) type, msg, len)) != 0)
            return err;
    }

#ifdef DEBUG_OMNI
    {
        int i;

        printf("Rx: ");
        for (i = 0 ; i < GET8(header->len) + 4; i++)
            printf("0x%02x ", ((unsigned char*) msg)[i]);
        printf("\n");
    }
#endif

    /* Check header */
    if (GET8(header->start) != OMNI_START)
        return EOMNIRESPONSE;

    /* Return data */
    *type = GET8(header->type);
    *len = GET8(header->len);

    /* Check CRC */
    crc_ptr = (unsigned char*) msg + *len + 2;
    crc = crc16((unsigned char*) msg + 1, *len + 1);
    if ((crc_ptr[0] != (crc & 0xFF)) || (crc_ptr[1] != ((crc >> 8) & 0xFF)))
        return EOMNICRC;

    return 0;
}

/********************************************************************************/

static int crc16(const void *data, int len)
{
    unsigned char *buf = (unsigned char*) data;
    unsigned short crc = 0;

    while (len > 0)
    {
        int i;

        crc ^= *buf;
        for (i = 0; i < 8; i++)
        {
            int flag;

            flag = ((crc & 1) != 0);
            crc >>= 1;
            if (flag)
                crc ^= 0xA001;
        }
        len--;
        buf++;
    }

    return crc;
}

