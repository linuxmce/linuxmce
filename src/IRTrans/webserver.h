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
void ProcessWebRequest (SOCKET sock);
void ParseRequest (char* data,SOCKET sock);
int GetHtmlPage (char *mem,char *page);
void GetHtmlParameter (char *pnt,char *name,char *value);
void GetIndexPage (char *mem,char *lang);
void GetDevicesPage (char *mem,char *lang,char *parm);
void GenerateErrorPage (char *mem,int error);
void GenerateHtmlHeader (char *mem,char *data);
int GenerateImageHeader (char *mem,char *data,char *type,int len);
void GetIRTransType (char ver,char typ[]);
void GetDeviceConfigPage (char *mem,char *lang,int id);
void GetSendPage (char *mem,char *lang,char *parm);


