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
int		DBReadCommandFile (char remote[]);
int		DBStoreRemote (FILE *fp,char newremote[]);
int		DBStoreTimings (FILE *fp);
int		DBStoreCommands (FILE *fp);
int		DBStoreCCFCode (char cd[]);
void	DBShowStatus (void);
void    ReadRoutingTable (void);
void	ReadSwitches (void);
void	ReadAppConfig (void);
void	WriteSwitches (void);
int		FindSwitch (word id,word num,char *rem,char *com,word *mode);
int		StoreSwitch (word id,word num,char *rem,char *com,word mode);
int		DBStoreRooms (FILE *fp);
void	DBStoreRouting (FILE *fp,char section[],ROUTING **pnt,long *cnt);
void	FreeDatabaseMemory (void);

FILE	*DBOpenFile (char remote[],char mode[]);
char	*DBReadString (char ln[],FILE *fp,long *fpos);
char	*DBFindSection (FILE *fp,char section[],char data[],char end[],long *fpos);
void	StoreIRTiming (IRTIMING *irp,char data[]);
void	ConvertLcase (char *pnt,int len);
int		GetFunctionCode (byte type,char *com);
int		GetKeyCode (char *com);


int		FindLineSection (char ln[],char section[]);
void	StorePulseTiming (IRTIMING *irp,char cmd[],char data[]);

FILE	*ASCIIOpenRemote (char name[],NETWORKCLIENT *client);
void	ASCIITimingSample (FILE *fp,NETWORKCLIENT *client);
int		ASCIIStoreCommand (FILE *fp,IRDATA *ird,char name[],int timing,int seq_number);
int		ASCIIFindToggleSeq (FILE *fp,IRDATA *ird,char name[]);
int		ASCIIStoreTiming (FILE *fp,IRDATA *ird,NETWORKLEARNSTAT *stat);
int		ASCIIStoreRAW (FILE *fp,IRRAW *ird,char name[]);
void	ASCIIStoreTimingParam (FILE *fp,IRDATA *ird,int timing);
int		ASCIIFindCommand (FILE *fp,char name[],NETWORKCLIENT *client);
int		CompareTiming (IRDATA *ird,IRTIMING *irt);
int		ASCIIFindTiming (FILE *fp,IRDATA *ird);
void	GetRemoteAddressMask (int num);

int		DBFindCommandName (byte command[],char remote[],char name[],byte address,long *remote_num,long *command_num,word *command_num_rel,int start);
int		DBFindRemoteCommand (char remote[],char command[],long *cmd_num,long *rem_num);
int		DBFindRemoteCommandEx(char remote[],char command[],IRDATA *ir);
int		DBGetIRCode (long cmd_num,IRDATA *ir,long idx,long *mac_len,long *mac_pause,long *rpt_len);
int		DBFindCommand (char command[],long remote);
int		DBFindRemote (char remote[]);
void	GetNumericCode (char command[],char numeric[],char rem[],char com[]);
void	GetRemoteDatabase (REMOTEBUFFER *buf,long offset);
void	GetCommandDatabase (COMMANDBUFFER *buf,char remote[],long offset);
