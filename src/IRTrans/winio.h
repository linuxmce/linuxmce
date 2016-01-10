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

HANDLE hCom;
HANDLE hComEvent;

void SetSerialTimeout (word time);

int		OpenSerialPort (char device[]);
void	WriteSerialString (byte pnt[],int len);
int		ReadSerialString (byte pnt[],int len,word timeout);
int	WriteSerialStringEx (DEVICEINFO *dev,byte pnt[],int len);
int		ReadSerialStringEx (DEVICEINFO *dev,byte pnt[],int len,word timeout);
int		OpenSerialPortEx (char Pname[],HANDLE *port);
int		GetSerialAvailableEx (DEVICEINFO *dev);
