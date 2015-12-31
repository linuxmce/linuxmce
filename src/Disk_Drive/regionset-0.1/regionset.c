/*
 * regionset: Region Code Manager
 * Copyright (C) 1999 Christian Wolff for convergence integrated media GmbH
 * 1999-12-13
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 * 
 * The author can be reached at scarabaeus@convergence.de, 
 * the project's page is at http://linuxtv.org/dvd/
 *
 * Minor changes (see ChangeLog) by Mirko Dölle <cooper@linvdr.org>
 */

#include <stdio.h>
#include "dvd_udf.h"

#define DEFAULTDEVICE "/dev/dvd"
#define VERSION "0.1"

int main (int argc, char* argv[]) 
{
	int i,err,type,vra,ucca,region_mask,rpc_scheme;
	char ch[3];
        printf("regionset version %s -- reads/sets region code on DVD drives\n", VERSION);
	setbuf(stdin,NULL);
	if (argc>1) err=UDFOpenDisc(argv[1]);
	else err=UDFOpenDisc(DEFAULTDEVICE);
	if (err<0) {
		printf("ERROR: Could not open disc \"%s\"!\n",((argc>=1)?argv[1]:DEFAULTDEVICE));
                printf("Ensure that there is any readable CD or DVD in the drive.\n");
		return 1;
	}
	if (UDFRPCGet(&type,&vra,&ucca,&region_mask,&rpc_scheme)) {
		printf("ERROR: Could not retreive region code settings of the drive!\n");
	} else {
		printf("Current Region Code settings:\n");
		printf("RPC Phase: %s\n",((rpc_scheme==0)?"Unknown (I)":((rpc_scheme==1)?"II":"Reserved (III?)")));
		printf("type: %s\n",((type==0)?"NONE":((type==1)?"SET":((type==2)?"LAST CHANCE":"PERMANENT"))));
		printf("vendor resets available: %d\n",vra);
		printf("user controlled changes resets available: %d\n",ucca);
		printf("drive plays discs from region(s):");
		for (i=0; i<8; i++) {
			if (!(region_mask&(1<<i))) printf(" %d",i+1);
		}
		printf(", mask=0x%02X\n\n",region_mask);
	}
	printf("Would you like to change the region setting of your drive? [y/n]:");
	fgets(ch,3,stdin);
	if ((ch[0]=='y') || (ch[0]=='Y')) {
		printf("Enter the new region number for your drive [1..8]:");
		fgets(ch,3,stdin);
		if ((ch[0]<'0') || (ch[0]>'8')) {
			printf("ERROR: Illegal region code!\n");
			goto ERROR;
		}
		if (ch[0]=='0') i=0;
		else i=~(1<<(ch[0]-'1'));
		if (i==region_mask) {
			printf("Identical region code already set, aborting!\n");
			goto ERROR;
		}
		printf("New mask: 0x%02X, correct? [y/n]:",i);
		fgets(ch,3,stdin);
		if ((ch[0]=='y') || (ch[0]=='Y')) {
			if (UDFRPCSet(i)) {
				printf("ERROR: Region code could not be set!\n");
				goto ERROR;
			} else {
				printf("Region code set successfully!\n");
			}
		}
	}
	UDFCloseDisc();
	return 0;
	ERROR:
	UDFCloseDisc();
	return 1;
}
