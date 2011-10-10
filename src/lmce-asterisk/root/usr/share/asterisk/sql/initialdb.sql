/*
 * Fill in LinuxMCE asterisk database
 * with default values
 *
 * v0.1 - 23/09/2011 - foxi352 - initial version
 * 
*/

--
-- Don't leave semi commmited stuff if ever script breaks
--
SET AUTOCOMMIT=0;
START TRANSACTION;

--
-- Switch to LinuxMCE's asterisk db
--
USE asterisk;

--
-- Insert static sip.conf in  DB
--	
DELETE FROM ast_config WHERE filename = 'sip.conf';
INSERT INTO ast_config (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES
(0, 0, 0, 'sip.conf', 'general', 'bindport', '5060'),
(0, 1, 0, 'sip.conf', 'general', 'bindaddr', '0.0.0.0'),
(0, 2, 0, 'sip.conf', 'general', 'disallow', 'all'),
(0, 3, 0, 'sip.conf', 'general', 'allow', 'ulaw'),
(0, 4, 0, 'sip.conf', 'general', 'allow', 'ulaw'),
(0, 5, 0, 'sip.conf', 'general', 'context', 'from-sip-external'),
(0, 6, 0, 'sip.conf', 'general', 'callerid', 'Unknown'),
(0, 7, 0, 'sip.conf', 'general', 'tos', '0x68'),
(0, 8, 0, 'sip.conf', 'general', 'notifyringing', 'yes'),
(0, 9, 0, 'sip.conf', 'general', 'notifyhold', 'yes'),
(0, 10, 0, 'sip.conf', 'general', 'limitonpeers', 'yes'),
(0, 11, 0, 'sip.conf', 'general', 'rtcachefriends', 'yes'),
(0, 12, 0, 'sip.conf', 'general', 'rtautoclear', 'yes');

--
-- Insert static iax.conf in  DB
--	
DELETE FROM ast_config WHERE filename = 'iax.conf';
INSERT INTO ast_config (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES
(0, 0, 0, 'iax.conf', 'general', 'bindport', '4569'),
(0, 1, 0, 'iax.conf', 'general', 'bindaddr', '0.0.0.0'),
(0, 2, 0, 'iax.conf', 'general', 'disallow', 'all'),
(0, 3, 0, 'iax.conf', 'general', 'allow', 'ulaw'),
(0, 4, 0, 'iax.conf', 'general', 'allow', 'alaw'),
(0, 5, 0, 'iax.conf', 'general', 'allow', 'gsm'),
(0, 6, 0, 'iax.conf', 'general', 'context', 'from-sip-external'),
(0, 7, 0, 'iax.conf', 'general', 'mailboxdetail', 'yes'),
(0, 8, 0, 'iax.conf', 'general', 'dtmfmode', 'auto'),
(0, 9, 0, 'iax.conf', 'general', 'rtcachefriends', 'yes'),
(0, 10, 0, 'iax.conf', 'general', 'rtautoclear', 'yes'),
(0, 11, 0, 'iax.conf', 'general', 'tos', '0x18'),
(0, 12, 0, 'iax.conf', 'general', 'jitterbuffer', 'yes');

--
-- Insert static rtp.conf in  DB
--	
DELETE FROM ast_config WHERE filename = 'rtp.conf';
INSERT INTO ast_config (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES
(0, 0, 0, 'rtp.conf', 'general', 'rtpstart', '10000'),
(0, 1, 0, 'rtp.conf', 'general', 'rtpend', '20000');

--
-- Insert static musiconhold.conf in  DB
--	
DELETE FROM ast_config WHERE filename = 'musiconhold.conf';
INSERT INTO ast_config (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES
(0, 0, 0, 'musiconhold.conf', 'default', 'mode', 'files'),
(0, 1, 0, 'musiconhold.conf', 'default', 'directory', '/var/lib/asterisk/mohmp3'),
(1, 0, 0, 'musiconhold.conf', 'none', 'mode', 'files'),
(1, 1, 0, 'musiconhold.conf', 'none', 'directory', '/var/lib/asterisk/mohmp3/none');

--
-- Insert static voicemail.conf in  DB
--	
DELETE FROM ast_config WHERE filename = 'voicemail.conf';
INSERT INTO ast_config (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES
(0, 0, 0, 'voicemail.conf', 'general', 'format', 'wav|gsm'),
(0, 1, 0, 'voicemail.conf', 'general', 'serveremail', 'LinuxMCE@dcerouter'),
(0, 2, 0, 'voicemail.conf', 'general', 'maxlogins', '3'),
(0, 3, 0, 'voicemail.conf', 'general', 'emaildateformat', '%A, %B %d, %Y at %r'),
(0, 4, 0, 'voicemail.conf', 'general', 'sendvoicemail', 'yes'),
(0, 5, 0, 'voicemail.conf', 'general', 'skipms', '3000'),
(0, 6, 0, 'voicemail.conf', 'general', 'maxsilence', '10'),
(0, 7, 0, 'voicemail.conf', 'general', 'silencethreshold', '128'),
(0, 8, 0, 'voicemail.conf', 'general', 'maxlogins', '3'),
(1, 0, 0, 'voicemail.conf', 'zonemessages', 'eastern', 'America/New_York|''vm-received'' Q ''digits/at'' IMp'),
(1, 1, 0, 'voicemail.conf', 'zonemessages', 'central', 'America/Chicago|''vm-received'' Q ''digits/at'' IMp'),
(1, 2, 0, 'voicemail.conf', 'zonemessages', 'central24', 'America/Chicago|''vm-received'' q ''digits/at'' H N ''hours'''),
(1, 3, 0, 'voicemail.conf', 'zonemessages', 'military', 'Zulu|''vm-received'' q ''digits/at'' H N ''hours'' ''phonetic/z_p'''),
(1, 4, 0, 'voicemail.conf', 'zonemessages', 'european', 'Europe/Copenhagen|''vm-received'' a d b ''digits/at'' HM');

--
-- Insert static meetme.conf in  DB
--	
DELETE FROM ast_config WHERE filename = 'meetme.conf';
INSERT INTO ast_config (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES
(0, 0, 0, 'meetme.conf', 'general', 'audiobuffers', '32');

DELETE FROM meetme;
INSERT INTO meetme (confno,pin,adminpin,members,starttime,endtime) 
VALUES ("997","","","0","2010-01-01 00:01","2999-12-31 23:59");

--
-- Insert realtime extensions in  DB
--	

-- Delete applications and recreate latest versions of them
DELETE FROM extensions WHERE context='app-echo-test';
DELETE FROM extensions WHERE context='app-speakingclock';
DELETE FROM extensions WHERE context='app-speakextennum';
ALTER TABLE extensions AUTO_INCREMENT=1;

-- *43: echo test
INSERT INTO extensions (context, exten, priority, app, appdata) VALUES
('app-echo-test', '*43', 1, 'Answer', ''),
('app-echo-test', '*43', 2, 'Wait', '1'),
('app-echo-test', '*43', 3, 'Playback', 'demo-echotest'),
('app-echo-test', '*43', 4, 'Echo', ''),
('app-echo-test', '*43', 5, 'Playback', 'demo-echodone'),
('app-echo-test', '*43', 6, 'Hangup', '');

-- *60: speaking clock
INSERT INTO extensions (context, exten, priority, app, appdata) VALUES
('app-speakingclock', '*60', 1, 'Answer', ''),
('app-speakingclock', '*60', 2, 'Wait', '1'),
('app-speakingclock', '*60', 3, 'Set', 'NumLoops=0'),
('app-speakingclock', '*60', 4, 'Set', 'FutureTime=$[${EPOCH} + 11]'),
('app-speakingclock', '*60', 5, 'Playback', 'at-tone-time-exactly'),
('app-speakingclock', '*60', 6, 'GotoIf', '$["${TIMEFORMAT}" = "kM"]?9'),
('app-speakingclock', '*60', 7, 'SayUnixTime', '${FutureTime},,IM \'and\' S \'seconds\' p'),
('app-speakingclock', '*60', 8, 'Goto', '10'),
('app-speakingclock', '*60', 9, 'SayUnixTime', '${FutureTime},,kM \'and\' S \'seconds\''),
('app-speakingclock', '*60', 10, 'Set', 'TimeLeft=$[${FutureTime} - ${EPOCH}]'),
('app-speakingclock', '*60', 11, 'GotoIf', '$[${TimeLeft} < 1]?14'),
('app-speakingclock', '*60', 12, 'Wait', '1'),
('app-speakingclock', '*60', 13, 'Goto', '10'),
('app-speakingclock', '*60', 14, 'Playback', 'beep'),
('app-speakingclock', '*60', 15, 'Wait', '5'),
('app-speakingclock', '*60', 16, 'Set', 'NumLoops=$[${NumLoops} + 1]'),
('app-speakingclock', '*60', 17, 'GotoIf', '$[${NumLoops} < 5]?4'),
('app-speakingclock', '*60', 18, 'Playback', 'goodbye'),
('app-speakingclock', '*60', 19, 'Hangup', '');

-- *65: say your extension number
INSERT INTO extensions (context, exten, priority, app, appdata) VALUES
('app-speakextennum', '*65', 1, 'Answer', ''),
('app-speakextennum', '*65', 2, 'Wait', '1'),
('app-speakextennum', '*65', 3, 'Macro', 'user-callerid'),
('app-speakextennum', '*65', 4, 'Playback', 'your'),
('app-speakextennum', '*65', 5, 'Playback', 'extension'),
('app-speakextennum', '*65', 6, 'Playback', 'number'),
('app-speakextennum', '*65', 7, 'Playback', 'is'),
('app-speakextennum', '*65', 8, 'SayDigits', '${AMPUSER}'),
('app-speakextennum', '*65', 9, 'Wait', '2'),
('app-speakextennum', '*65',10, 'Hangup', '');

--
-- Everything went fine until here, so let's commit to db
--
COMMIT;