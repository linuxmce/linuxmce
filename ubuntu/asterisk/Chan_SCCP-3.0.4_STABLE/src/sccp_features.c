
/*!
 * \file 	sccp_features.c
 * \brief 	SCCP Features Class
 * \author 	Federico Santulli <fsantulli [at] users.sourceforge.net >
 * \note		This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 * \since 	2009-01-16
 *
 * $Date: 2011-08-01 20:24:03 +0200 (Mon, 01 Aug 2011) $
 * $Revision: 2679 $
 */

/*!
 * \remarks	Purpose: 	SCCP Features
 * 		When to use:	Only methods directly related to handling phone features should be stored in this source file.
 *                              Phone Features are Capabilities of the phone, like:
 *                                - CallForwarding
 *                                - Dialing
 *                                - Changing to Do Not Disturb(DND) Status
 *   		Relationships: 	These Features are called by FeatureButtons. Features can in turn call on Actions.
 *
 */

#include "config.h"
#include "common.h"

SCCP_FILE_VERSION(__FILE__, "$Revision: 2679 $")

/*!
 * \brief Handle Call Forwarding
 * \param l SCCP Line
 * \param device SCCP Device
 * \param type CallForward Type (NONE, ALL, BUSY, NOANSWER) as SCCP_CFWD_*
 * \return SCCP Channel
 *
 * \callgraph
 * \callergraph
 *
 * \lock
 * 	- channel
 * 	  - see sccp_channel_set_active()
 * 	  - see sccp_indicate_nolock()
 */
sccp_channel_t *sccp_feat_handle_callforward(sccp_line_t * l, sccp_device_t * device, uint8_t type)
{
	sccp_channel_t *c = NULL;

	struct ast_channel *bridge = NULL;

	sccp_linedevices_t *linedevice;

	if (!l || !device || !device->id || sccp_strlen_zero(device->id)) {
		ast_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return NULL;
	}

	linedevice = sccp_util_getDeviceConfiguration(device, l);

	if (!linedevice) {
		ast_log(LOG_ERROR, "%s: Device does not have line configured \n", DEV_ID_LOG(device));
		return NULL;
	}

	/* if call forward is active and you asked about the same call forward maybe you would disable */
	if ((linedevice->cfwdAll.enabled && type == SCCP_CFWD_ALL)
	    || (linedevice->cfwdBusy.enabled && type == SCCP_CFWD_BUSY)) {

		sccp_line_cfwd(l, device, SCCP_CFWD_NONE, NULL);
		return NULL;
	} else {
		if (type == SCCP_CFWD_NOANSWER) {
			sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "### CFwdNoAnswer NOT SUPPORTED\n");
			sccp_dev_displayprompt(device, 0, 0, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
			return NULL;
		}
	}

	/* look if we have a call  */
	c = sccp_channel_get_active_locked(device);

	if (c) {
		// we have a channel, checking if we created it during previous cfwd loop
		if (c->ss_action == SCCP_SS_GETFORWARDEXTEN) {
			// we have a channel, checking if
			if (c->state == SCCP_CHANNELSTATE_RINGOUT || c->state == SCCP_CHANNELSTATE_CONNECTED || c->state == SCCP_CHANNELSTATE_PROCEED || c->state == SCCP_CHANNELSTATE_BUSY || c->state == SCCP_CHANNELSTATE_CONGESTION) {
				if (c->calltype == SKINNY_CALLTYPE_OUTBOUND) {
					// if we have an outbound call, we can set callforward to dialed number -FS
					if (c->dialedNumber && !sccp_strlen_zero(c->dialedNumber)) {	// checking if we have a number !
						sccp_line_cfwd(l, device, type, c->dialedNumber);
						// we are on call, so no tone has been played until now :)
						//sccp_dev_starttone(device, SKINNY_TONE_ZIPZIP, instance, 0, 0);

						sccp_channel_endcall_locked(c);
						sccp_channel_unlock(c);
						return NULL;
					}
				} else if (c->owner && (bridge = ast_bridged_channel(c->owner))) {	// check if we have an ast channel to get callerid from
					// if we have an incoming or forwarded call, let's get number from callerid :) -FS
					char *number = NULL;

					if (PBX(pbx_get_callerid_name))
						number = PBX(pbx_get_callerid_name) (c);
					if (number) {
						sccp_line_cfwd(l, device, type, number);
						// we are on call, so no tone has been played until now :)
						sccp_dev_starttone(device, SKINNY_TONE_ZIPZIP, linedevice->lineInstance, 0, 0);

						sccp_channel_endcall_locked(c);
						sccp_channel_unlock(c);
						sccp_free(number);
						return NULL;
					}
					// if we where here it's cause there is no number in callerid,, so put call on hold and ask for a call forward number :) -FS
					if (!sccp_channel_hold_locked(c)) {
						// if can't hold  it means there is no active call, so return as we're already waiting a number to dial
						sccp_channel_unlock(c);
						sccp_dev_displayprompt(device, 0, 0, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
						return NULL;
					}
				}
			} else if (c->state == SCCP_CHANNELSTATE_OFFHOOK && sccp_strlen_zero(c->dialedNumber)) {
				// we are dialing but without entering a number :D -FS
				sccp_dev_stoptone(device, linedevice->lineInstance, (c && c->callid) ? c->callid : 0);
				// changing SS_DIALING mode to SS_GETFORWARDEXTEN
				c->ss_action = SCCP_SS_GETFORWARDEXTEN;		/* Simpleswitch will catch a number to be dialed */
				c->ss_data = type;				/* this should be found in thread */
				// changing channelstate to GETDIGITS
				sccp_indicate_locked(device, c, SCCP_CHANNELSTATE_GETDIGITS);
				sccp_channel_unlock(c);
				return c;
			} else {
				// we cannot allocate a channel, or ask an extension to pickup.
				sccp_channel_unlock(c);
				sccp_dev_displayprompt(device, 0, 0, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
				return NULL;
			}
		} else {
			// other call in progress, put on hold before starting cfwd
			int ret = sccp_channel_hold_locked(c);

			sccp_channel_unlock(c);
			if (!ret) {
				ast_log(LOG_ERROR, "%s: Active call '%d' could not be put on hold\n", DEV_ID_LOG(device), c->callid);
				return NULL;
			}
		}
	}
	// if we where here there is no call in progress, so we should allocate a channel.
	c = sccp_channel_allocate_locked(l, device);

	if (!c) {
		ast_log(LOG_ERROR, "%s: Can't allocate SCCP channel for line %s\n", device->id, l->name);
		return NULL;
	}

	c->ss_action = SCCP_SS_GETFORWARDEXTEN;					/* Simpleswitch will catch a number to be dialed */
	c->ss_data = type;							/* this should be found in thread */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(device, c);
	sccp_indicate_locked(device, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate_locked(c)) {
		ast_log(LOG_WARNING, "%s: (handle_callforward) Unable to allocate a new channel for line %s\n", device->id, l->name);
		sccp_indicate_locked(c->device, c, SCCP_CHANNELSTATE_CONGESTION);
		sccp_channel_endcall_locked(c);
		sccp_channel_unlock(c);
		return c;
	}

	sccp_ast_setstate(c, AST_STATE_OFFHOOK);

	if (device->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel_locked(c);
	}

	sccp_channel_unlock(c);

	return c;
}

#ifdef CS_SCCP_PICKUP

/*!
 * \brief Handle Direct Pickup of Line
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 *
 * \lock
 * 	- channel
 * 	  - see sccp_channel_set_active()
 * 	  - see sccp_indicate_nolock()
 */
sccp_channel_t *sccp_feat_handle_directpickup(sccp_line_t * l, uint8_t lineInstance, sccp_device_t * d)
{
	sccp_channel_t *c;

	if (!l || !d || strlen(d->id) < 3) {
		ast_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return NULL;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active_locked(d))) {
		// we have a channel, checking if
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK && (!c->dialedNumber || (c->dialedNumber && sccp_strlen_zero(c->dialedNumber)))) {
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(d, lineInstance, (c && c->callid) ? c->callid : 0);
			// changing SS_DIALING mode to SS_GETFORWARDEXTEN
			c->ss_action = SCCP_SS_GETPICKUPEXTEN;			/* Simpleswitch will catch a number to be dialed */
			c->ss_data = 0;						/* this should be found in thread */
			// changing channelstate to GETDIGITS
			sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_GETDIGITS);
			sccp_channel_unlock(c);
			return c;
		} else {
			/* there is an active call, let's put it on hold first */
			if (!sccp_channel_hold_locked(c)) {
				sccp_channel_unlock(c);
				return NULL;
			}
		}
		sccp_channel_unlock(c);
	}

	c = sccp_channel_allocate_locked(l, d);

	if (!c) {
		ast_log(LOG_ERROR, "%s: (handle_directpickup) Can't allocate SCCP channel for line %s\n", d->id, l->name);
		return NULL;
	}

	c->ss_action = SCCP_SS_GETPICKUPEXTEN;					/* Simpleswitch will catch a number to be dialed */
	c->ss_data = 0;								/* not needed here */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(d, c);
	sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate_locked(c)) {
		ast_log(LOG_WARNING, "%s: (handle_directpickup) Unable to allocate a new channel for line %s\n", d->id, l->name);
		sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_CONGESTION);
		sccp_channel_unlock(c);
		return c;
	}

	sccp_ast_setstate(c, AST_STATE_OFFHOOK);

	if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel_locked(c);
	}

	sccp_channel_unlock(c);

	return c;
}
#endif

#ifdef CS_SCCP_PICKUP

/*!
 * \brief Handle Direct Pickup of Extension
 * \param c *locked* SCCP Channel
 * \param exten Extension as char
 * \return Success as int
 *
 * \lock
 * 	- asterisk channel
 * 	  - device
 */
int sccp_feat_directpickup_locked(sccp_channel_t * c, char *exten)
{
	int res = 0;

	struct ast_channel *target = NULL;

	struct ast_channel *original = NULL;

	struct ast_channel *tmp = NULL;

	const char *ringermode = NULL;

	sccp_device_t *d;

	char *pickupexten;

	char *name = NULL, *number = NULL;

	sccp_channel_t *tmpChannel;

	if (sccp_strlen_zero(exten)) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (directpickup) zero exten\n");
		return -1;
	}

	if (!c || !c->owner) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (directpickup) no channel\n");
		return -1;
	}

	original = c->owner;

	if (!c->line || !c->device || !c->device->id || sccp_strlen_zero(c->device->id)) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (directpickup) no device\n");
		return -1;
	}

	d = c->device;

	/* copying extension into our buffer */
	pickupexten = strdup(exten);

	while ((target = pbx_channel_walk_locked(target))) {
		sccp_log((DEBUGCAT_FEATURE + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_1 "[SCCP LOOP] in file %s, line %d (%s)\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		sccp_log((DEBUGCAT_FEATURE + DEBUGCAT_HIGH)) (VERBOSE_PREFIX_3 "SCCP: (directpickup)\n" "--------------------------------------------\n" "(pickup target)\n" "exten         = %s\n" "context       = %s\n" "pbx           = off\n" "state         = %d or %d\n" "(current chan)\n" "macro exten   = %s\n" "exten         = %s\n" "macro context = %s\n" "context       = %s\n"
#    if ASTERISK_VERSION_NUMBER >= 10400
							      "dialcontext   = %s\n"
#    endif
							      "pbx           = %s\n" "state         = %d\n" "--------------------------------------------\n", pickupexten, !sccp_strlen_zero(d->pickupcontext) ? d->pickupcontext : "(NULL)", AST_STATE_RINGING, AST_STATE_RING, target->macroexten ? target->macroexten : "(NULL)", target->exten ? target->exten : "(NULL)", target->macrocontext ? target->macrocontext : "(NULL)", target->context ? target->context : "(NULL)",
#    if ASTERISK_VERSION_NUMBER >= 10400
							      target->dialcontext ? target->dialcontext : "(NULL)",
#    endif
							      target->pbx ? "on" : "off", target->_state);

		if ((!strcasecmp(target->macroexten, pickupexten) || !strcasecmp(target->exten, pickupexten)) &&
#    if ASTERISK_VERSION_NUMBER < 10400
		    ((!sccp_strlen_zero(d->pickupcontext) ? (!strcasecmp(target->context, d->pickupcontext)) : 1) || (!sccp_strlen_zero(d->pickupcontext) ? (!strcasecmp(target->macrocontext, d->pickupcontext)) : 1)) &&
#    else
		    ((!sccp_strlen_zero(d->pickupcontext) ? (!strcasecmp(target->dialcontext, d->pickupcontext)) : 1) || (!sccp_strlen_zero(d->pickupcontext) ? (!strcasecmp(target->macrocontext, d->pickupcontext)) : 1)) &&
#    endif
		    (!target->pbx && (target->_state == AST_STATE_RINGING || target->_state == AST_STATE_RING))) {

			tmp = (CS_AST_BRIDGED_CHANNEL(target) ? CS_AST_BRIDGED_CHANNEL(target) : target);

			/* update callinfos */
			tmpChannel = CS_AST_CHANNEL_PVT(target);
			if (tmpChannel) {
				if (PBX(pbx_get_callerid_name))
					name = PBX(pbx_get_callerid_name) (tmpChannel);

				if (PBX(pbx_get_callerid_number))
					number = PBX(pbx_get_callerid_number) (tmpChannel);
			}

			ast_log(LOG_NOTICE, "SCCP: %s callerid is ('%s'-'%s')\n", tmp->name, name ? name : "", number ? number : "");
			tmp = NULL;
			original->hangupcause = AST_CAUSE_CALL_REJECTED;

			res = 0;
			if (d->pickupmodeanswer) {
				if ((res = ast_answer(c->owner))) {		// \todo: remove res in this line: Although the value stored to 'res' is used in the enclosing expression, the value is never actually read from 'res'

					sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (directpickup) Unable to answer '%s'\n", c->owner->name);
					res = -1;
				} else if ((res = ast_queue_control(c->owner, AST_CONTROL_ANSWER))) {
					sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (directpickup) Unable to queue answer on '%s'\n", c->owner->name);
					res = -1;
				}
			}

			if (res == 0) {
				if ((res = ast_channel_masquerade(target, c->owner))) {
					sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (directpickup) Unable to masquerade '%s' into '%s'\n", c->owner->name, target->name);
					res = -1;
				} else {
					sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (directpickup) Pickup on '%s' by '%s'\n", target->name, c->owner->name);
					c->calltype = SKINNY_CALLTYPE_INBOUND;
					sccp_channel_set_callingparty(c, name, number);
					if (d->pickupmodeanswer) {
						sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_CONNECTED);
					} else {
						uint8_t instance;

						instance = sccp_device_find_index_for_line(d, c->line->name);
						sccp_dev_stoptone(d, instance, c->callid);
						sccp_dev_set_speaker(d, SKINNY_STATIONSPEAKER_OFF);

						sccp_device_lock(d);
						d->active_channel = NULL;
						sccp_device_unlock(d);

						c->ringermode = SKINNY_STATION_OUTSIDERING;	// default ring
						ringermode = pbx_builtin_getvar_helper(c->owner, "ALERT_INFO");
						if (ringermode && !sccp_strlen_zero(ringermode)) {
							sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: Found ALERT_INFO=%s\n", ringermode);
							if (strcasecmp(ringermode, "inside") == 0)
								c->ringermode = SKINNY_STATION_INSIDERING;
							else if (strcasecmp(ringermode, "feature") == 0)
								c->ringermode = SKINNY_STATION_FEATURERING;
							else if (strcasecmp(ringermode, "silent") == 0)
								c->ringermode = SKINNY_STATION_SILENTRING;
							else if (strcasecmp(ringermode, "urgent") == 0)
								c->ringermode = SKINNY_STATION_URGENTRING;
						}
						sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_RINGING);
					}
					original->hangupcause = AST_CAUSE_NORMAL_CLEARING;
					ast_setstate(original, AST_STATE_DOWN);
				}
				pbx_channel_unlock(target);
				ast_queue_hangup(original);
			} else {
				pbx_channel_unlock(target);
			}
			if (name)
				sccp_free(name);
			if (number)
				sccp_free(number);
			break;
		} else {
			res = -1;
		}
		pbx_channel_unlock(target);
	}
	sccp_free(pickupexten);
	pickupexten = NULL;
	sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (directpickup) quit\n");
	return res;
}

/*!
 * \brief Handle Group Pickup Feature
 * \param c Asterisk Channel
 * \param data contains the pointer to the SCCP Line
 * \return Success as int
 *
 * \see static int find_channel_by_group(struct ast_channel *c, void *data) from features.c
 */
static int pbx_find_channel_by_group(struct ast_channel *c, void *data)
{
	sccp_line_t *line = data;

	return !c->pbx && (line->pickupgroup & c->callgroup) && ((c->_state == AST_STATE_RINGING) || (c->_state == AST_STATE_RING)) && !c->masq;
}

/*!
 * \brief Handle Group Pickup Feature
 * \param l SCCP Line
 * \param d SCCP Device
 * \return Success as int
 *
 * \lock
 * 	- asterisk channel
 * 	  - channel
 * 	    - see sccp_indicate_nolock()
 *	    - see sccp_device_find_index_for_line()
 *	    - see sccp_dev_stoptone()
 *	    - see sccp_dev_set_speaker()
 * 	    - device
 * 	    - see sccp_indicate_nolock()
 */
int sccp_feat_grouppickup(sccp_line_t * l, sccp_device_t * d)
{
	int res = 0;
	struct ast_channel *target = NULL;
	struct ast_channel *original = NULL;

	const char *ringermode = NULL;

	sccp_channel_t *c;
	char *name = NULL, *number = NULL;

	if (!l || !d || !d->id || sccp_strlen_zero(d->id)) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) no line or device\n");
		return -1;
	}

	if (!l->pickupgroup) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: (grouppickup) pickupgroup not configured in sccp.conf\n", d->id);
		return -1;
	}

	target = sccp_asterisk_channel_search_locked(pbx_find_channel_by_group, l);
	if (target) {
		/* create channel for pickup */
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: Device state is '%s'\n", DEV_ID_LOG(d), devicestatus2str(d->state));
		if (!(c = sccp_channel_find_bystate_on_line_locked(l, SCCP_CHANNELSTATE_OFFHOOK))) {
			c = sccp_channel_allocate_locked(l, d);
			if (!c) {
				ast_log(LOG_ERROR, "%s: (grouppickup) Can't allocate SCCP channel for line %s\n", d->id, l->name);
				pbx_channel_unlock(target);
				return -1;
			}

			if (!sccp_pbx_channel_allocate_locked(c)) {
				ast_log(LOG_WARNING, "%s: (grouppickup) Unable to allocate a new channel for line %s\n", d->id, l->name);
				pbx_channel_unlock(target);
				sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_CONGESTION);
				sccp_channel_unlock(c);
				res = -1;
				return res;
			}
			sccp_channel_set_active(d, c);
			sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_OFFHOOK);

			if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
				sccp_channel_openreceivechannel_locked(c);
			}
		}
		/* done */
		original = c->owner;

#    ifdef CS_AST_CHANNEL_HAS_CID
		if (target->cid.cid_name)
			name = strdup(target->cid.cid_name);
		if (target->cid.cid_num)
			number = strdup(target->cid.cid_num);
#    else
		char *cidtmp = NULL;

		if (target->callerid) {
			cidtmp = strdup(target->callerid);
			ast_callerid_parse(cidtmp, &name, &number);
		}
#    endif

		if (original && original->cid.cid_name)
			sccp_copy_string(c->callInfo.originalCalledPartyName, original->cid.cid_name, sizeof(c->callInfo.originalCalledPartyName));
		if (original && original->cid.cid_num)
			sccp_copy_string(c->callInfo.originalCalledPartyNumber, original->cid.cid_num, sizeof(c->callInfo.originalCalledPartyNumber));

		if (target->cid.cid_name) {
			sccp_copy_string(c->callInfo.callingPartyName, name, sizeof(c->callInfo.callingPartyName));
		}
		if (target->cid.cid_num) {
			sccp_copy_string(c->callInfo.callingPartyNumber, number, sizeof(c->callInfo.callingPartyNumber));
		}
		/* we use the  original->cid.cid_name to do the magic */
		if (target->cid.cid_ani) {
			sccp_copy_string(c->callInfo.callingPartyNumber, number, sizeof(c->callInfo.callingPartyNumber));
			sccp_copy_string(c->callInfo.callingPartyName, number, sizeof(c->callInfo.callingPartyName));
		}
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) asterisk remote channel cid_ani = '%s'\n", (target->cid.cid_ani) ? target->cid.cid_ani : "");	/* remote cid_num */
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) asterisk remote channel cid_dnid = '%s'\n", (target->cid.cid_dnid) ? target->cid.cid_dnid : "");
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) asterisk remote channel cid_name = '%s'\n", (target->cid.cid_name) ? target->cid.cid_name : "");
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) asterisk remote channel cid_num = '%s'\n", (target->cid.cid_num) ? target->cid.cid_num : "");
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) asterisk remote channel cid_rdnis = '%s'\n", (target->cid.cid_rdnis) ? target->cid.cid_rdnis : "");

		sccp_channel_t *remote = NULL;

		if ((remote = get_sccp_channel_from_ast_channel(target))) {
			sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) remote channel is SCCP %s -> correct cid\n", remote->owner->name);
			name = strdup(remote->callInfo.callingPartyName);
			number = strdup(remote->callInfo.callingPartyNumber);
			remote = NULL;
		}
		original->hangupcause = AST_CAUSE_CALL_REJECTED;

		res = 0;
		if (d->pickupmodeanswer) {
			if ((res = ast_answer(c->owner))) {			// \todo: remove res in this line: Although the value stored to 'res' is used in the enclosing expression, the value is never actually read from 'res'
				sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) Unable to answer '%s'\n", c->owner->name);
				res = -1;
			} else if ((res = ast_queue_control(c->owner, AST_CONTROL_ANSWER))) {
				sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) Unable to queue answer on '%s'\n", c->owner->name);
				res = -1;
			}
		}

		char tmp[50];							/* save channel name before unlock */

		snprintf(tmp, sizeof(tmp), "%s", target->name);
		pbx_channel_unlock(target);

		if (res == 0) {
			if (ast_channel_masquerade(target, c->owner)) {
				sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) Unable to masquerade '%s' into '%s'\n", c->owner->name, tmp);
				res = -1;					//! \todo remove line : res value is being set to 0 in line 694 any way
			} else {
				sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) Pickup on '%s' by '%s'\n", tmp, c->owner->name);

				c->calltype = SKINNY_CALLTYPE_INBOUND;
				/* \todo: search remote callerid */
				//sccp_channel_set_callingparty(c, name, number);
				if (d->pickupmodeanswer) {
					sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_CONNECTED);
				} else {
					uint8_t instance;

					instance = sccp_device_find_index_for_line(d, c->line->name);
					sccp_dev_stoptone(d, instance, c->callid);
					sccp_dev_set_speaker(d, SKINNY_STATIONSPEAKER_OFF);
					sccp_device_lock(d);
					d->active_channel = NULL;
					sccp_device_unlock(d);

					c->ringermode = SKINNY_STATION_OUTSIDERING;	// default ring
					ringermode = pbx_builtin_getvar_helper(c->owner, "ALERT_INFO");
					if (ringermode && !sccp_strlen_zero(ringermode)) {
						sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: Found ALERT_INFO=%s\n", ringermode);
						if (strcasecmp(ringermode, "inside") == 0)
							c->ringermode = SKINNY_STATION_INSIDERING;
						else if (strcasecmp(ringermode, "feature") == 0)
							c->ringermode = SKINNY_STATION_FEATURERING;
						else if (strcasecmp(ringermode, "silent") == 0)
							c->ringermode = SKINNY_STATION_SILENTRING;
						else if (strcasecmp(ringermode, "urgent") == 0)
							c->ringermode = SKINNY_STATION_URGENTRING;
					}
					sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_RINGING);
				}
				original->hangupcause = AST_CAUSE_ANSWERED_ELSEWHERE;	//AST_CAUSE_NORMAL_CLEARING
				ast_setstate(original, AST_STATE_DOWN);
			}
			ast_hangup(original);
			sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) masquerade succeeded\n");
		}

		if (name)
			sccp_free(name);
		if (number)
			sccp_free(number);
#    ifndef CS_AST_CHANNEL_HAS_CID
		if (cidtmp)
			sccp_free(cidtmp);
#    endif

		res = 0;
		sccp_channel_unlock(c);
	} else {
		sccp_log(DEBUGCAT_FEATURE) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) no channel to pickup\n");
		sccp_dev_displayprompt(d, 1, 0, "No channel for group pickup", 5);
		sccp_dev_starttone(d, SKINNY_TONE_BEEPBONK, 1, 0, 3);

	}
	sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) finished\n");
	return res;
}
#endif

/*!
 * \brief Update Caller ID
 * \param c SCCP Channel
 *
 * \callgraph
 * \callergraph
 */
void sccp_feat_updatecid(sccp_channel_t * c)
{
	struct ast_channel *target = NULL;

	char *name = NULL, *number = NULL;

	if (!c || !c->owner)
		return;

	if (c->calltype == SKINNY_CALLTYPE_OUTBOUND)
		target = c->owner;
	else if (!(target = ast_bridged_channel(c->owner))) {
		return;
	}

	if (PBX(pbx_get_callerid_name))
		name = PBX(pbx_get_callerid_name) (c);

	if (PBX(pbx_get_callerid_number))
		number = PBX(pbx_get_callerid_number) (c);

	sccp_channel_set_callingparty(c, name, number);

	if (name)
		sccp_free(name);
	if (number)
		sccp_free(number);
}

/*!
 * \brief Handle VoiceMail
 * \param d SCCP Device
 * \param lineInstance LineInstance as uint8_t
 *
 * \lock
 * 	- channel
 * 	  - see sccp_dev_displayprompt()
 */
void sccp_feat_voicemail(sccp_device_t * d, uint8_t lineInstance)
{

	sccp_channel_t *c;

	sccp_line_t *l;

	sccp_log((DEBUGCAT_CORE | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "%s: Voicemail Button pressed on line (%d)\n", d->id, lineInstance);

	c = sccp_channel_get_active_locked(d);
	if (c) {
		if (!c->line || sccp_strlen_zero(c->line->vmnum)) {
			sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "%s: No voicemail number configured on line %d\n", d->id, lineInstance);
			sccp_channel_unlock(c);
			return;
		}
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK || c->state == SCCP_CHANNELSTATE_DIALING) {
			sccp_copy_string(c->dialedNumber, c->line->vmnum, sizeof(c->dialedNumber));
			SCCP_SCHED_DEL(sched, c->digittimeout);
			sccp_pbx_softswitch_locked(c);
			sccp_channel_unlock(c);
			return;
		}

		sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
		sccp_channel_unlock(c);
		return;
	}

	if (!lineInstance)
		l = sccp_line_find_byinstance(d, 1);
	else
		l = sccp_line_find_byinstance(d, lineInstance);

	if (!l) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", d->id, lineInstance);
		return;
	}
	if (!sccp_strlen_zero(l->vmnum)) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "%s: Dialing voicemail %s\n", d->id, l->vmnum);
		sccp_channel_newcall(l, d, l->vmnum, SKINNY_CALLTYPE_OUTBOUND);
	} else {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "%s: No voicemail number configured on line %d\n", d->id, lineInstance);
	}
}

/*!
 * \brief Handle Divert/Transfer Call to VoiceMail
 * \param d SCCP Device
 * \param l SCCP Line
 * \param c SCCP Channel
 */
void sccp_feat_idivert(sccp_device_t * d, sccp_line_t * l, sccp_channel_t * c)
{
	int instance;

	if (!l) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: TRANSVM pressed but no line found\n", d->id);
		sccp_dev_displayprompt(d, 0, 0, "No line found to transfer", 5);
		return;
	}
	if (!l->trnsfvm) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: TRANSVM pressed but not configured in sccp.conf\n", d->id);
		return;
	}
	if (!c || !c->owner) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: TRANSVM with no channel active\n", d->id);
		sccp_dev_displayprompt(d, 0, 0, "TRANSVM with no channel active", 5);
		return;
	}

	if (c->state != SCCP_CHANNELSTATE_RINGING && c->state != SCCP_CHANNELSTATE_CALLWAITING) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: TRANSVM pressed in no ringing state\n", d->id);
		return;
	}

	sccp_log((DEBUGCAT_CORE | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "%s: TRANSVM to %s\n", d->id, l->trnsfvm);
#ifdef CS_AST_HAS_AST_STRING_FIELD
	ast_string_field_set(c->owner, call_forward, l->trnsfvm);
#else
	sccp_copy_string(c->owner->call_forward, l->trnsfvm, sizeof(c->owner->call_forward));
#endif
	instance = sccp_device_find_index_for_line(d, l->name);
	sccp_device_sendcallstate(d, instance, c->callid, SKINNY_CALLSTATE_PROCEED, SKINNY_CALLPRIORITY_LOW, SKINNY_CALLINFO_VISIBILITY_DEFAULT);	/* send connected, so it is not listed as missed call */
	ast_setstate(c->owner, AST_STATE_BUSY);
	ast_queue_control(c->owner, AST_CONTROL_BUSY);
}

/*!
 * \brief Handle Conference List
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \return Success as int
 */
void sccp_feat_conflist(sccp_device_t * d, sccp_line_t * l, uint8_t lineInstance, sccp_channel_t * c)
{
#ifdef CS_SCCP_CONFERENCE
	sccp_conference_show_list(c->conference, c);
#else
	sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
#endif
}

/*!
 * \brief Handle Conference
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \return Success as int
 *
 * \lock
 * 	- device
 * 	  - see sccp_device_numberOfChannels()
 * 	- device->selectedChannels
 * 	  - see sccp_conference_addParticipant()
 * 	- device->buttonconfig
 * 	  - see sccp_line_find_byname_wo()
 * 	  - line->channels
 * 	    - see sccp_conference_addParticipant()
 */
void sccp_feat_conference(sccp_device_t * d, sccp_line_t * l, uint8_t lineInstance, sccp_channel_t * c)
{
#ifdef CS_SCCP_CONFERENCE
	sccp_buttonconfig_t *config = NULL;

	sccp_channel_t *channel = NULL;

	sccp_selectedchannel_t *selectedChannel = NULL;

	sccp_line_t *line = NULL;

	boolean_t selectedFound = FALSE;

	if (!d || !c)
		return;

	sccp_device_lock(d);
	uint8_t num = sccp_device_numberOfChannels(d);

	sccp_device_unlock(d);
	if (num < 2) {
		sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
		return;
	}

	/* Always create a new conference until we learn to clean up (-DD) */
	//if (!d->conference) {
	d->conference = sccp_conference_create(c);
	//}

	/* if we have selected channels, add this to conference */
	SCCP_LIST_LOCK(&d->selectedChannels);
	SCCP_LIST_TRAVERSE(&d->selectedChannels, selectedChannel, list) {
		selectedFound = TRUE;

		if (NULL != selectedChannel->channel) {
			//if (c != channel) {
			/*
			   if (channel->state == SCCP_CHANNELSTATE_HOLD)
			   sccp_channel_resume_locked(d, channel, FALSE);

			 */
			sccp_conference_addParticipant(d->conference, channel);
			//} else {
			ast_log(LOG_NOTICE, "%s: not adding our own active channel to device.\n", DEV_ID_LOG(d));
			//}

		} else {
			ast_log(LOG_NOTICE, "%s: not adding NULL channel to conference.\n", DEV_ID_LOG(d));
		}
	}
	SCCP_LIST_UNLOCK(&d->selectedChannels);

	/* If no calls were selected, add all calls to the conference, across all lines. */
	if (FALSE == selectedFound) {
		SCCP_LIST_LOCK(&d->buttonconfig);
		SCCP_LIST_TRAVERSE(&d->buttonconfig, config, list) {
			if (config->type == LINE) {
				line = sccp_line_find_byname_wo(config->button.line.name, FALSE);
				if (!line)
					continue;

				SCCP_LIST_LOCK(&line->channels);
				SCCP_LIST_TRAVERSE(&line->channels, channel, list) {
					if (channel->device == d) {
						/* Make sure not to add the moderator channel (ourselves) twice. */
						//if (c != channel) {
						/*
						   if (channel->state == SCCP_CHANNELSTATE_HOLD)
						   sccp_channel_resume_locked(d, channel, FALSE);

						 */
						sccp_conference_addParticipant(d->conference, channel);
						//} else {
						//      ast_log(LOG_NOTICE, "%s: not adding our own active channel to device.\n", DEV_ID_LOG(d));
						//}
					}
				}
				SCCP_LIST_UNLOCK(&line->channels);
			}
		}
		SCCP_LIST_UNLOCK(&d->buttonconfig);
	}
#else
	sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
	ast_log(LOG_NOTICE, "%s: conference not enabled\n", DEV_ID_LOG(d));
#endif
}

/*!
 * \brief Handle Join a Conference
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_feat_join(sccp_device_t * d, sccp_line_t * l, uint8_t lineInstance, sccp_channel_t * c)
{
	sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
}

/*!
 * \brief Handle 3-Way Phone Based Conferencing on a Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 *
 * \lock
 * 	- channel
 * 	  - see sccp_indicate_nolock()
 * 	- channel
 * 	  - see sccp_channel_set_active()
 * 	  - see sccp_indicate_nolock()
 * 	  - see sccp_pbx_channel_allocate()
 * 	  - see sccp_channel_openreceivechannel()
 */
sccp_channel_t *sccp_feat_handle_meetme(sccp_line_t * l, uint8_t lineInstance, sccp_device_t * d)
{
	sccp_channel_t *c;

	if (!l || !d || !d->id || sccp_strlen_zero(d->id)) {
		ast_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return NULL;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active_locked(d))) {
		// we have a channel, checking if
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK && (!c->dialedNumber || (c->dialedNumber && sccp_strlen_zero(c->dialedNumber)))) {
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(d, lineInstance, (c && c->callid) ? c->callid : 0);
			// changing SS_DIALING mode to SS_GETFORWARDEXTEN
			c->ss_action = SCCP_SS_GETMEETMEROOM;			/* Simpleswitch will catch a number to be dialed */
			c->ss_data = 0;						/* this should be found in thread */
			// changing channelstate to GETDIGITS
			sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_GETDIGITS);
			sccp_channel_unlock(c);
			return c;
			/* there is an active call, let's put it on hold first */
		} else if (!sccp_channel_hold_locked(c)) {
			sccp_channel_unlock(c);
			return NULL;
		}
		sccp_channel_unlock(c);
	}

	c = sccp_channel_allocate_locked(l, d);

	if (!c) {
		ast_log(LOG_ERROR, "%s: (handle_meetme) Can't allocate SCCP channel for line %s\n", DEV_ID_LOG(d), l->name);
		return NULL;
	}

	c->ss_action = SCCP_SS_GETMEETMEROOM;					/* Simpleswitch will catch a number to be dialed */
	c->ss_data = 0;								/* not needed here */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(d, c);
	sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate_locked(c)) {
		ast_log(LOG_WARNING, "%s: (handle_meetme) Unable to allocate a new channel for line %s\n", d->id, l->name);
		sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_CONGESTION);
		sccp_channel_unlock(c);
		return c;
	}

	sccp_ast_setstate(c, AST_STATE_OFFHOOK);

	if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel_locked(c);
	}

	/* removing scheduled dial */
	SCCP_SCHED_DEL(sched, c->digittimeout);

	if (!(c->digittimeout = sccp_sched_add(sched, GLOB(firstdigittimeout) * 1000, sccp_pbx_sched_dial, c))) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: Unable to schedule dialing in '%d' ms\n", GLOB(firstdigittimeout));
	}

	sccp_channel_unlock(c);

	return c;
}

struct meetmeAppConfig {
	char *appName;
	char *defaultMeetmeOption;
};

struct meetmeAppConfig meetmeApps[] = {
	{"MeetMe", "qd"},
	{"ConfBridge", "Mac"},
	{"Konference", "MTV"}
};

/*!
 * \brief a Meetme Application Thread
 * \param data Data
 * \author Federico Santulli
 *
 * \lock
 * 	- channel
 *	  - see sccp_indicate_nolock()
 *	  - see sccp_channel_set_calledparty()
 *	  - see sccp_channel_setSkinnyCallstate()
 *	  - see sccp_channel_send_callinfo()
 *	  - see sccp_indicate_nolock()
*/
static void *sccp_feat_meetme_thread(void *data)
{
	sccp_channel_t *c = data;

	sccp_device_t *d = NULL;

	struct meetmeAppConfig *app = NULL;

	char ext[AST_MAX_EXTENSION];

	char context[AST_MAX_CONTEXT];

	char meetmeopts[AST_MAX_CONTEXT];

#if ASTERISK_VERSION_NUMBER >= 10600
#    define SCCP_CONF_SPACER ','
#endif

#if ASTERISK_VERSION_NUMBER >= 10400 && ASTERISK_VERSION_NUMBER < 10600
#    define SCCP_CONF_SPACER '|'
#endif

#if ASTERISK_VERSION_NUMBER >= 10400
	unsigned int eid = ast_random();
#else
	unsigned int eid = random();

#    define SCCP_CONF_SPACER '|'
#endif

	d = c->device;

	/* searching for meetme app */
	uint32_t i;

	for (i = 0; i < sizeof(meetmeApps) / sizeof(struct meetmeAppConfig); i++) {
		if (pbx_findapp(meetmeApps[i].appName)) {
			app = &(meetmeApps[i]);
			sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "SCCP: using '%s' for meetme\n", meetmeApps[i].appName);
			break;
		}
	}
	/* finish searching for meetme app */

	if (!app) {								// \todo: remove res in this line: Although the value stored to 'res' is used in the enclosing expression, the value is never actually read from 'res'
		ast_log(LOG_WARNING, "SCCP: No MeetMe application available!\n");
		sccp_channel_lock(c);
		sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_DIALING);
		sccp_channel_set_calledparty(c, SKINNY_DISP_CONFERENCE, c->dialedNumber);
		sccp_channel_setSkinnyCallstate(c, SKINNY_CALLSTATE_PROCEED);
		sccp_channel_send_callinfo(d, c);
		sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_INVALIDCONFERENCE);
		sccp_channel_unlock(c);
		return NULL;
	}
	// SKINNY_DISP_CAN_NOT_COMPLETE_CONFERENCE
	if (c && c->owner) {
		if (!c->owner->context || sccp_strlen_zero(c->owner->context))
			return NULL;
		if (!sccp_strlen_zero(c->line->meetmeopts)) {
			snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, c->line->meetmeopts);
		} else if (!sccp_strlen_zero(d->meetmeopts)) {
			snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, d->meetmeopts);
		} else if (!sccp_strlen_zero(GLOB(meetmeopts))) {
			snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, GLOB(meetmeopts));
		} else {
			snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, app->defaultMeetmeOption);
		}

		sccp_copy_string(context, c->owner->context, sizeof(context));
		snprintf(ext, sizeof(ext), "sccp_meetme_temp_conference_%ud", eid);

		if (!ast_exists_extension(NULL, context, ext, 1, NULL)) {
			ast_add_extension(context, 1, ext, 1, NULL, NULL, app->appName, meetmeopts, NULL, "sccp_feat_meetme_thread");
			ast_log(LOG_WARNING, "SCCP: create extension exten => %s,%d,%s(%s)\n", ext, 1, app->appName, meetmeopts);
		}

		sccp_copy_string(c->owner->exten, ext, sizeof(c->owner->exten));

		sccp_channel_lock(c);
		sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_DIALING);
		sccp_channel_set_calledparty(c, SKINNY_DISP_CONFERENCE, c->dialedNumber);
		sccp_channel_setSkinnyCallstate(c, SKINNY_CALLSTATE_PROCEED);
		sccp_channel_send_callinfo(d, c);
		sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_CONNECTEDCONFERENCE);
		sccp_channel_unlock(c);

		if (ast_pbx_run(c->owner)) {
			sccp_channel_lock(c);
			sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_INVALIDCONFERENCE);
			sccp_channel_unlock(c);
			ast_log(LOG_WARNING, "SCCP: SCCP_CHANNELSTATE_INVALIDCONFERENCE\n");
		}
		ast_context_remove_extension(context, ext, 1, NULL);
	}

	return NULL;
}

/*!
 * \brief Start a Meetme Application Thread
 * \param c SCCP Channel
 * \author Federico Santulli
 */
void sccp_feat_meetme_start(sccp_channel_t * c)
{
	pthread_attr_t attr;

	pthread_t t;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ast_pthread_create(&t, &attr, sccp_feat_meetme_thread, c) < 0) {
		ast_log(LOG_WARNING, "SCCP: Cannot create a MeetMe thread (%s).\n", strerror(errno));
	}
	pthread_attr_destroy(&attr);
}

/*!
 * \brief Handle Barging into a Call
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 *
 * \lock
 * 	- channel
 * 	  - see sccp_indicate_nolock()
 */
sccp_channel_t *sccp_feat_handle_barge(sccp_line_t * l, uint8_t lineInstance, sccp_device_t * d)
{
	sccp_channel_t *c;

	if (!l || !d || !d->id || sccp_strlen_zero(d->id)) {
		ast_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return NULL;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active_locked(d))) {
		// we have a channel, checking if
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK && (!c->dialedNumber || (c->dialedNumber && sccp_strlen_zero(c->dialedNumber)))) {
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(d, lineInstance, (c && c->callid) ? c->callid : 0);
			// changing SS_DIALING mode to SS_GETFORWARDEXTEN
			c->ss_action = SCCP_SS_GETBARGEEXTEN;			/* Simpleswitch will catch a number to be dialed */
			c->ss_data = 0;						/* this should be found in thread */
			// changing channelstate to GETDIGITS
			sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_GETDIGITS);
			sccp_channel_unlock(c);
			return c;
		} else if (!sccp_channel_hold_locked(c)) {
			/* there is an active call, let's put it on hold first */
			sccp_channel_unlock(c);
			return NULL;
		}
		sccp_channel_unlock(c);
	}

	c = sccp_channel_allocate_locked(l, d);

	if (!c) {
		ast_log(LOG_ERROR, "%s: (handle_barge) Can't allocate SCCP channel for line %s\n", d->id, l->name);
		return NULL;
	}

	c->ss_action = SCCP_SS_GETBARGEEXTEN;					/* Simpleswitch will catch a number to be dialed */
	c->ss_data = 0;								/* not needed here */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(d, c);
	sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate_locked(c)) {
		ast_log(LOG_WARNING, "%s: (handle_barge) Unable to allocate a new channel for line %s\n", d->id, l->name);
		sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_CONGESTION);
		sccp_channel_unlock(c);
		return c;
	}

	sccp_ast_setstate(c, AST_STATE_OFFHOOK);

	if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel_locked(c);
	}
	sccp_channel_unlock(c);

	return c;
}

/*!
 * \brief Barging into a Call Feature
 * \param c SCCP Channel
 * \param exten Extention as char
 * \return Success as int
 *
 * \todo implement barge feature
 */
int sccp_feat_barge(sccp_channel_t * c, char *exten)
{
	uint8_t instance = sccp_device_find_index_for_line(c->device, c->line->name);

	sccp_dev_displayprompt(c->device, instance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
	return 1;
}

/*!
 * \brief Handle Barging into a Conference
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 *
 * \lock
 * 	- channel
 * 	  - see sccp_indicate_nolock()
 * 	- channel
 * 	  - see sccp_channel_set_active()
 * 	  - see sccp_indicate_nolock()
 */
sccp_channel_t *sccp_feat_handle_cbarge(sccp_line_t * l, uint8_t lineInstance, sccp_device_t * d)
{
	sccp_channel_t *c;

	if (!l || !d || strlen(d->id) < 3) {
		ast_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return NULL;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active_locked(d))) {
		// we have a channel, checking if
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK && (!c->dialedNumber || (c->dialedNumber && sccp_strlen_zero(c->dialedNumber)))) {
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(d, lineInstance, (c && c->callid) ? c->callid : 0);
			// changing SS_DIALING mode to SS_GETFORWARDEXTEN
			c->ss_action = SCCP_SS_GETCBARGEROOM;			/* Simpleswitch will catch a number to be dialed */
			c->ss_data = 0;						/* this should be found in thread */
			// changing channelstate to GETDIGITS
			sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_GETDIGITS);
			sccp_channel_unlock(c);
			return c;
		} else {
			/* there is an active call, let's put it on hold first */
			if (!sccp_channel_hold_locked(c)) {
				sccp_channel_unlock(c);
				return NULL;
			}
		}
		sccp_channel_unlock(c);
	}

	c = sccp_channel_allocate_locked(l, d);

	if (!c) {
		ast_log(LOG_ERROR, "%s: (handle_cbarge) Can't allocate SCCP channel for line %s\n", d->id, l->name);
		return NULL;
	}

	c->ss_action = SCCP_SS_GETCBARGEROOM;					/* Simpleswitch will catch a number to be dialed */
	c->ss_data = 0;								/* not needed here */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(d, c);
	sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate_locked(c)) {
		ast_log(LOG_WARNING, "%s: (handle_cbarge) Unable to allocate a new channel for line %s\n", d->id, l->name);
		sccp_indicate_locked(d, c, SCCP_CHANNELSTATE_CONGESTION);
		sccp_channel_unlock(c);
		return c;
	}

	sccp_ast_setstate(c, AST_STATE_OFFHOOK);

	if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel_locked(c);
	}

	sccp_channel_unlock(c);

	return c;
}

/*!
 * \brief Barging into a Conference Feature
 * \param c SCCP Channel
 * \param conferencenum Conference Number as char
 * \return Success as int
 *
 * \todo implement conference barge
 */
int sccp_feat_cbarge(sccp_channel_t * c, char *conferencenum)
{
	uint8_t instance = sccp_device_find_index_for_line(c->device, c->line->name);

	sccp_dev_displayprompt(c->device, instance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
	return 1;
}

/*!
 * \brief Hotline/Adhoc Feature
 *
 * Setting the hotline/adhoc/PLAR Feature on a device, will make it connect to a predefined extension as soon as the Receiver
 * is picked up or the "New Call" Button is pressed. No number has to be given.
 *
 * \param d SCCP Device
 * \param line SCCP Line
 *
 * \lock
 * 	- channel
 */
void sccp_feat_adhocDial(sccp_device_t * d, sccp_line_t * line)
{
	sccp_channel_t *c = NULL;

	if (!d || !d->session || !line)
		return;

	sccp_log((DEBUGCAT_FEATURE | DEBUGCAT_LINE)) (VERBOSE_PREFIX_3 "%s: handling hotline\n", d->id);
	c = sccp_channel_get_active_locked(d);
	if (c) {
		if ((c->state == SCCP_CHANNELSTATE_DIALING) || (c->state == SCCP_CHANNELSTATE_OFFHOOK)) {
			sccp_copy_string(c->dialedNumber, line->adhocNumber, sizeof(c->dialedNumber));

			SCCP_SCHED_DEL(sched, c->digittimeout);
			sccp_pbx_softswitch_locked(c);

			sccp_channel_unlock(c);

			return;
		}
		sccp_channel_unlock(c);
		sccp_pbx_senddigits(c, line->adhocNumber);
	} else {
		// Pull up a channel
		if (GLOB(hotline)->line) {
			sccp_channel_newcall(line, d, line->adhocNumber, SKINNY_CALLTYPE_OUTBOUND);
		}
	}
}

/*!
 * \brief Handler to Notify Features have Changed
 * \param device SCCP Device
 * \param featureType SCCP Feature Type
 * 
 * \lock
 * 	- see sccp_hint_handleFeatureChangeEvent() via sccp_event_fire()
 * 	- see sccp_util_handleFeatureChangeEvent() via sccp_event_fire()
 */
void sccp_feat_changed(sccp_device_t * device, sccp_feature_type_t featureType)
{
	if (device) {

		sccp_featButton_changed(device, featureType);

		sccp_event_t *event = ast_malloc(sizeof(sccp_event_t));

		memset(event, 0, sizeof(sccp_event_t));

		event->type = SCCP_EVENT_FEATURE_CHANGED;
		event->event.featureChanged.device = device;
		event->event.featureChanged.featureType = featureType;
		sccp_event_fire((const sccp_event_t **)&event);

		if (SCCP_FEATURE_MONITOR == featureType) {
			/* Special case for monitor */
			sccp_moo_t *r;

			int status = (device->monitorFeature.status) || (device->mwilight & (1 << 0));

			REQ(r, SetLampMessage);
			r->msg.SetLampMessage.lel_stimulus = htolel(SKINNY_STIMULUS_VOICEMAIL);
			//r->msg.SetLampMessage.lel_stimulusInstance = 0;
			r->msg.SetLampMessage.lel_lampMode = htolel(status ? device->mwilamp : SKINNY_LAMP_OFF);
			sccp_dev_send(device, r);
			sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: Turn %s the MWI light (monitor feature change)\n", DEV_ID_LOG(device), (device->mwilight & (1 << 0)) ? "ON" : "OFF");
		}

	}
}

/*!
 * \brief Handler to Notify Channel State has Changed
 * \param device SCCP Device
 * \param channel SCCP Channel
 */
void sccp_feat_channelStateChanged(sccp_device_t * device, sccp_channel_t * channel)
{
	uint8_t state;

	if (!channel || !device)
		return;

	state = channel->state;
	switch (state) {
	case SCCP_CHANNELSTATE_CONNECTED:
		/* We must update the status here. Not change it. (DD) */
		/*
		   if (device->monitorFeature.enabled && device->monitorFeature.status != channel->monitorEnabled) {
		   sccp_feat_monitor(device, channel);
		   }
		 */
		break;
	case SCCP_CHANNELSTATE_DOWN:
	case SCCP_CHANNELSTATE_ONHOOK:
	case SCCP_CHANNELSTATE_BUSY:
	case SCCP_CHANNELSTATE_CONGESTION:
	case SCCP_CHANNELSTATE_INVALIDNUMBER:
	case SCCP_CHANNELSTATE_ZOMBIE:
		/* Todo: In the event a call is terminated,
		   the channel monitor should be turned off (it implicitly is by ending the call),
		   and the feature button should be reset to disabled state. */
		device->monitorFeature.status = 0;
		sccp_feat_changed(device, SCCP_FEATURE_MONITOR);
		break;

	default:
		break;
	}

}

int checkMonCond(void *v);

int checkMonCond(void *v)
{
	struct ast_channel *c = (struct ast_channel *)v;

	if (NULL == c)
		return 1;

	return (c->monitor) ? 1 : 0;
}

/*!
 * \brief Feature Monitor
 * \param device SCCP Device
 * \param line SCCP Line
 * \param lineInstance Line Instance as Uint32_t
 * \param channel SCCP Channel
 */
void sccp_feat_monitor(sccp_device_t * device, sccp_line_t * line, const uint32_t lineInstance, sccp_channel_t * channel)
{
#if ASTERISK_VERSION_NUMBER >= 10600
#    ifdef CS_SCCP_FEATURE_MONITOR
	struct ast_call_feature *feat;

	struct ast_channel *bridge;

	struct ast_frame f;

#        if ASTERISK_VERSION_NUMBER >= 10400
	f = ast_null_frame;
#        else
	f = NULL;
#        endif
	f.frametype = AST_FRAME_DTMF;

	unsigned int j;

	bridge = ast_bridged_channel(channel->owner);

	if (!bridge) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "Recording requested, but we have no bridged peer.\n");
		return;
	}

	ast_rdlock_call_features();
	feat = ast_find_call_feature("automon");
	if (!feat || sccp_strlen_zero(feat->exten)) {
		sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "Recording requested, but no One Touch Monitor registered. (See features.conf)\n");
		ast_unlock_call_features();
		return;
	}

	/*! 
	 * \todo We could do monitoring directly.
	 * So we would be able to do it without ugly dtmf packets. 
	 * At the moment we have to pause a bit to wait for activity on the channel.
	 */

	f.len = 100;
	for (j = 0; j < strlen(feat->exten); j++) {
		f.subclass = feat->exten[j];
		ast_queue_frame(channel->owner, &f);
	}
	ast_unlock_call_features();

	//int res = ast_safe_sleep_conditional(bridge, 1000, &checkMonCond, bridge);
	//usleep(20000);

	// Look at asterisk/main/features.c and asterisk/res/res_monitor.c to understand (-DD).
	//channel->monitorEnabled = (bridge->monitor) ? 1 : 0;
	device->monitorFeature.status = (device->monitorFeature.status) ? 0 : 1;

	sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_FEATURE | DEBUGCAT_FEATURE_BUTTON)) (VERBOSE_PREFIX_3 "Recording requested. Do we? %s!\n", device->monitorFeature.status ? "YES" : "NO");
	sccp_feat_changed(device, SCCP_FEATURE_MONITOR);
#    endif
#endif
}
