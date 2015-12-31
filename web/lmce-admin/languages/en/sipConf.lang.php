<?php
$TEXT_SIP_CONF_CONST='SIP Configuration';
$TEXT_BINDPORT_CONST='Bindport';
$TEXT_BINDADDR_CONST='Bind address';
$TEXT_VIDEOSUPPORT_CONST='Enable video support';
$TEXT_CONTEXT_CONST='Context';
$TEXT_CALLERID_CONST='Caller ID';
$TEXT_TOS_CONST='TOS';
$TEXT_NOTIFYRINGING_CONST='Notify ringing';
$TEXT_NOTIFYHOLD_CONST='Notify hold';
$TEXT_LIMITONPEERS_CONST='Limit on peers';
$TEXT_RTCACHEFRIENDS_CONST='RT Cache friends';
$TEXT_RTUPDATE_CONST='Enable RT update';
$TEXT_TCPENABLE_CONST='Enable TCP';
$TEXT_TCPBINDADDR_CONST='TCP bind address';
$TEXT_REGISTERATTEMPTS_CONST='Register attempts';
$TEXT_REGISTERTIMEOUT_CONST='Register timeout';
$TEXT_ALWAYSAUTHREJECT_CONST='Alway auth reject';
$TEXT_VALID_INT_CONST='Not a valid integer';
$TEXT_VALID_IP_CONST='Not a valid IP Address';
$TEXT_BINDPORT_TIP='Number : UDP Port to bind to (listen on). Used to be port in Asterisk v1.0.x. Default 5060.';
$TEXT_BINDADDR_TIP='IP_Address : IP Address to bind to (listen on). Default 0.0.0.0 (all network interfaces).';
$TEXT_VIDEOSUPPORT_TIP='yes|no : Turn on support for SIP video (peer specific setting added in SVN Dec 21 2005, bug 5427. Default no.';
$TEXT_CONTEXT_TIP='<context_name> : If type=user, the Context for the inbound call from this SIP user definition. If type=peer, the Context in the dialplan for outbound calls from this SIP peer definition. If type=friend the context used for both inbound and outbound calls through the SIP entities definition. If no type=user entry matches an inbound call, then a type=peer or type=friend will match if the hostname or IP address defined in host= matches.';
$TEXT_CALLERID_TIP='<string> : Caller ID information used when nothing else is available. Defaults to asterisk.';
$TEXT_TOS_TIP='<value> : Set IP QoS parameters for outgoing media streams (numeric values are also accepted, like tos=184 )';
$TEXT_NOTIFYRINGING_TIP='yes|no : Notify subscription on RINGING state. Default yes. (New in v1.2.x).';
$TEXT_NOTIFYHOLD_TIP='Specifies whether Asterisk should notify subscriptions on HOLD state';
$TEXT_LIMITONPEERS_TIP='Default is NO; this setting applies to the [general] section of sip.conf.';
$TEXT_RTCACHEFRIENDS_TIP='If rtcachefriends is turned on, Asterisk will cache friends that come from the realtime engine, just as if they had come from sip.conf. This often helps with items such as message-waiting indications on realtime peers:';
$TEXT_RTUPDATE_TIP='If set to yes Asterisk will update the IP address, origination port, and registration period of a peer upon registration. Defaults to yes:';
$TEXT_TCPENABLE_TIP='Enable server for incoming TCP connections (default is no)';
$TEXT_TCPBINDADDR_TIP='IP address for TCP server to bind to (0.0.0.0 binds to all interfaces) Optionally add a port number, 192.168.1.1:5062 (default is port 5060)';
$TEXT_REGISTERATTEMPTS_TIP='Number of registration attempts before we give up 0 = continue forever, hammering the other server until it accepts the registration Default is 0 tries, continue forever';
$TEXT_REGISTERTIMEOUT_TIP='retry registration calls every 20 seconds (default)';
$TEXT_ALWAYSAUTHREJECT_TIP='When an incoming INVITE or REGISTER is to be rejected, for any reason, always reject with an identical response equivalent to valid username and invalid password or hash instead of letting the requester know whether there was a matching user or peer for their request.  This reduces the ability of an attacker to scan for valid SIP usernames. This option is set to >yes< by default.';
$TEXT_LANGUAGE_TIP='Default language setting for all users/peers e.g. en, fr, de ...';


?>
