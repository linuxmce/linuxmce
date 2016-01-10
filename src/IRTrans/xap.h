/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
void xAP_SendHeartbeat (void);
void xAP_SendData (char data[]);
void xAP_SendIREvent (char remote[],char command[],int bus,int adr);
void xAP_EventReceived (void);
void xAP_ProcessHeader (char data[]);
int  xAP_GetItem (char data[],char item[],char value[]);
int  xAP_GetLine (char data[],char line[],int pos);

extern SOCKET xAP_rcv;
extern SOCKET xAP_send;
extern int xAP_rcv_port;
extern int xAP_last_hbeat;


#define XAP_UID			"FF2121"
#define XAP_HBEAT		60
#define XAP_ADR			"IRTrans.irserver"
#define XAP_VERSION		12
