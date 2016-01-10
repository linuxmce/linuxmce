/*
 * dvdudf: parse and read the UDF volume information of a DVD Video
 * Copyright (C) 1999 Christian Wolff for convergence integrated media GmbH
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
 * Removed all CSS related stuff to be able to publish "regionset"
 *   by Mirko Dölle <cooper@linvdr.org>
 */
 
/* This is needed for 64 bit file seek */
// Since older libraries don't seem to support this, i did a workaround
#define _LARGEFILE64_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__OpenBSD__)
# include <sys/dvdio.h>
#elif defined(__linux__)
# include <linux/cdrom.h>
#else
# error "Need the DVD ioctls"
#endif

#include "dvd_udf.h"

#ifndef u8
#define u8 unsigned char
#endif

#ifndef u16
#define u16 unsigned short
#endif

#ifndef u32
#define u32 unsigned long
#endif

#ifndef u64
#define u64 unsigned long long
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

// Maximum length of filenames for UDF
#define MAX_FILE_LEN 2048

// default name for split udf image files
#define SPLITNAME "DVDVIDEO"

FILE* dvdromfile = NULL;  // CD/DVD-ROM block device or image file
char Path[PATH_MAX];
char Filename[PATH_MAX];

struct DVD_Video_Disc {
	int segments;
	int currentsegment;
	long int *segmentlength;
	int segtablelen;
	int phoony_region_mask;
} disc;

struct Partition {
	int valid;
	char VolumeDesc[128];
	u16 Flags;
	u16 Number;
	char Contents[32];
	u32 AccessType;
	u32 Start;
	u32 Length;
} partition;

struct AD {
	u32 Location;
	u32 Length;
	u8 Flags;
	u16 Partition;
};

// for direct data access, LSB first
#define GETN1(p) ((u8)data[p])
#define GETN2(p) ((u16)data[p] | ((u16)data[(p) + 1] << 8))
#define GETN3(p) ((u32)data[p] | ((u32)data[(p) + 1] << 8) | ((u32)data[(p) + 2] << 16))
#define GETN4(p) ((u32)data[p] | ((u32)data[(p) + 1] << 8) | ((u32)data[(p) + 2] << 16) | ((u32)data[(p) + 3] << 24))
#define GETN(p, n, target) memcpy(target, &data[p], n)

#ifdef DVD_AUTH
	dvd_authinfo ai;
	dvd_struct dvds;
	int last_agid = 0;
#endif

// searches for <file> in directory <path>, ignoring case
// returns 0 and full filename in <filename>
// or -1 on file not found
// or -2 on path not found
int udf_find_file(const char *path, const char *file, char *filename) 
{
	DIR *dir;
	struct dirent *ent;

#ifdef UDF_DEBUG
	printf("find file %s / %s\n", path, file);
#endif
	if ((dir = opendir(path)) == NULL) return -2;
	while ((ent = readdir(dir)) != NULL) {
		if (!strcasecmp(ent->d_name, file)) {
			sprintf(filename, "%s%s%s", path, ((path[strlen(path)-1] == '/') ? "" : "/"), ent->d_name);
			closedir(dir);
			return 0;
		}
	}
	closedir(dir);
	return -1;
}

// updates the segment length table of a DVD Video image directory
// stores the length of DVDVIDEO.000 and following into an array
// returns 0 on success, >0 on error
int udf_update_segments(void) 
{
	char file[PATH_MAX];
	char filename[PATH_MAX];
	int err;
	if (disc.currentsegment >= 0) fclose(dvdromfile);
	disc.currentsegment = -1;
	disc.segments = 0;
	err = 0;
	do {
		sprintf(file, "%s.%03d", Filename, disc.segments);
		if (!(err = udf_find_file(Path, file, filename))) {
			if ((dvdromfile = fopen(filename, "r")) == NULL) {
				err = 1;   // file not readable
			} else {
				if (disc.segments >= disc.segtablelen) {
					disc.segtablelen += 100;
					disc.segmentlength = (long int *)realloc(disc.segmentlength, disc.segtablelen * sizeof(long int));
					if (disc.segmentlength == NULL) return 1;
				}
				fseek(dvdromfile, 0L, SEEK_END);
				disc.segmentlength[disc.segments++] = ftell(dvdromfile)/DVD_VIDEO_LB_LEN;
				fclose(dvdromfile);
			}
		}
	} while (!err);
	return 0;
}


// reads absolute Logical Block of the disc
// returns number of read bytes on success, 0 or negative error number on error
int UDFReadLB(unsigned long int lb_number, unsigned int block_count, unsigned char *data) 
{
#if defined off64_t
	fpos64_t pos;
#else
	u64 pos;  // 64 bit position
#endif
	int result, segment, segblocks, numread;
	char file[PATH_MAX];
	char filename[PATH_MAX];
	if (disc.segments > 1) {  // split image file
		segment = 0;
		result = 0;
		while (block_count && (segment < disc.segments)) {
			if (disc.segmentlength[segment] <= lb_number) {  // that's not our segment yet
				lb_number -= disc.segmentlength[segment++];    // skip to next segment
			} else {                                  // our segment
				if (disc.currentsegment != segment) {          // segment not open, yet?
					if (disc.currentsegment >= 0) fclose(dvdromfile);
					disc.currentsegment = -1;
					sprintf(file, "%s.%03d", Filename, segment);
					if (udf_find_file(Path, file, filename)) return 0;
					if ((dvdromfile = fopen(filename, "r")) == NULL) return 0;
					disc.currentsegment = segment;               // remember open segment number
				}
				if (fseek(dvdromfile, lb_number * DVD_VIDEO_LB_LEN, SEEK_SET) < 0) break;  // position not found
				segblocks = disc.segmentlength[segment] - lb_number;              // remaining blocks in segment
				if (block_count < segblocks) segblocks = block_count;             // more than requested?
				if ((numread = fread(&data[result], segblocks * DVD_VIDEO_LB_LEN, 1, dvdromfile))) {  // read requested blocks or up to end of segment
					result += segblocks * DVD_VIDEO_LB_LEN;           // add to overall number of read bytes
					block_count -= segblocks;                         // segments done
					lb_number += segblocks;                           // next position
				} else {                                            // read error 
					if (!result) {
						result = ferror(dvdromfile);
						if (result > 0) result = -result;
					}
					break;
				}
			}
		}
		return result;
	} else if (dvdromfile != NULL) {  // block device or one image file
#if defined off64_t
		pos = (fpos64_t)lb_number * (fpos64_t)DVD_VIDEO_LB_LEN;
		if (fsetpos64(dvdromfile, &pos) < 0) return 0; // position not found
#else
		pos = (u64)lb_number * (u64)DVD_VIDEO_LB_LEN;
		fseek(dvdromfile, 0, SEEK_SET);
		while (pos > LONG_MAX) {
			if (fseek(dvdromfile, LONG_MAX, SEEK_CUR)) return 0; // position not found
			pos -= LONG_MAX;
		}
		if (fseek(dvdromfile, (long int)pos, SEEK_CUR)) return 0; // position not found
#endif
		if ((result = fread(data, block_count * DVD_VIDEO_LB_LEN, 1, dvdromfile)) <= 0) {
			result = ferror(dvdromfile);
			clearerr(dvdromfile);
			return ((result > 0) ? -result : result);  // make it negative!
		} else return result * block_count * DVD_VIDEO_LB_LEN;
	} else return 0;
}

int Unicodedecode(u8 *data, int len, char *target) 
{
	int p = 1, i = 0;
	if ((data[0] == 8) || (data[0] == 16)) do {
		if (data[0] == 16) p++;  // ignore MSB of unicode16
		if (p < len) {
			target[i++] = data[p++];
		}
	} while (p < len);
	target[i] = '\0';
	return 0;
}

int UDFEntity(u8 *data, u8 *Flags, char *Identifier) 
{
	*Flags = data[0];
	strncpy(Identifier, &data[1], 5);
	return 0;
}

int UDFDescriptor(u8 *data, u16 *TagID) 
{
	*TagID = GETN2(0);
	// TODO: check CRC 'n stuff
	return 0;
}

int UDFExtentAD(u8 *data, u32 *Length, u32 *Location) 
{
	*Length   = GETN4(0);
	*Location = GETN4(4);
	return 0;
}

int UDFShortAD(u8 *data, struct AD *ad) 
{
	ad->Length = GETN4(0);
	ad->Flags = ad->Length >> 30;
	ad->Length &= 0x3FFFFFFF;
	ad->Location = GETN4(4);
	ad->Partition = partition.Number;  // use number of current partition
	return 0;
}

int UDFLongAD(u8 *data, struct AD *ad) 
{
	ad->Length = GETN4(0);
	ad->Flags = ad->Length >> 30;
	ad->Length &= 0x3FFFFFFF;
	ad->Location = GETN4(4);
	ad->Partition = GETN2(8);
	//GETN(10, 6, Use);
	return 0;
}

int UDFExtAD(u8 *data, struct AD *ad) 
{
	ad->Length = GETN4(0);
	ad->Flags = ad->Length >> 30;
	ad->Length &= 0x3FFFFFFF;
	ad->Location = GETN4(12);
	ad->Partition = GETN2(16);
	//GETN(10, 6, Use);
	return 0;
}

int UDFICB(u8 *data, u8 *FileType, u16 *Flags) 
{
	*FileType = GETN1(11);
	*Flags = GETN2(18);
	return 0;
}


int UDFPartition(u8 *data, u16 *Flags, u16 *Number, char *Contents, u32 *Start, u32 *Length) 
{
	*Flags = GETN2(20);
	*Number = GETN2(22);
	GETN(24, 32, Contents);
	*Start = GETN4(188);
	*Length = GETN4(192);
	return 0;
}

// reads the volume descriptor and checks the parameters
// returns 0 on OK, 1 on error
int UDFLogVolume(u8 *data, char *VolumeDescriptor) 
{
	u32 lbsize, MT_L, N_PM;
	//u8 type, PM_L;
	//u16 sequence;
	//int i, p;
	Unicodedecode(&data[84], 128, VolumeDescriptor);
	lbsize = GETN4(212);  // should be 2048
	MT_L = GETN4(264);    // should be 6
	N_PM = GETN4(268);    // should be 1
	if (lbsize != DVD_VIDEO_LB_LEN) return 1;
	/*
	*Partition1 = 0;
	p = 440;
	for (i = 0; i < N_PM; i++) {
		type = GETN1(p);
		PM_L = GETN1(p + 1);
		if (type == 1) {
			sequence = GETN2(p + 2);
			if (sequence == 1) {
				*Partition1 = GETN2(p + 4);
				return 0;
			}
		}
		p += PM_L;
	}
	return 1;
	*/
	return 0;
}

int UDFFileEntry(u8 *data, u8 *FileType, struct AD *ad) 
{
	u8 filetype;
	u16 flags;
	u32 L_EA, L_AD;
	int p;

	UDFICB(&data[16], &filetype, &flags);
	*FileType = filetype;
	L_EA = GETN4(168);
	L_AD = GETN4(172);
	p = 176 + L_EA;
	while (p < 176 + L_EA + L_AD) {
		switch (flags & 0x0007) {
			case 0: UDFShortAD(&data[p], ad); p += 8;  break;
			case 1: UDFLongAD(&data[p], ad);  p += 16; break;
			case 2: UDFExtAD(&data[p], ad);   p += 20; break;
			case 3:
				switch (L_AD) {
					case 8:  UDFShortAD(&data[p], ad); break;
					case 16: UDFLongAD(&data[p], ad);  break;
					case 20: UDFExtAD(&data[p], ad);   break;
				}
				p += L_AD;
				break;
			default: p += L_AD; break;
		}
	}
	return 0;
}

int UDFFileIdentifier(u8 *data, u8 *FileCharacteristics, char *FileName, struct AD *FileICB) 
{
	u8 L_FI;
	u16 L_IU;
	
	*FileCharacteristics = GETN1(18);
	L_FI = GETN1(19);
	UDFLongAD(&data[20], FileICB);
	L_IU = GETN2(36);
	if (L_FI) Unicodedecode(&data[38 + L_IU], L_FI, FileName);
	else FileName[0] = '\0';
	return 4 * ((38 + L_FI + L_IU + 3) / 4);
}

// Maps ICB to FileAD
// ICB: Location of ICB of directory to scan
// FileType: Type of the file
// File: Location of file the ICB is pointing to
// return 1 on success, 0 on error;
int UDFMapICB(struct AD ICB, u8 *FileType, struct AD *File) 
{
	u8 LogBlock[DVD_VIDEO_LB_LEN];
	u32 lbnum;
	u16 TagID;

	lbnum = partition.Start + ICB.Location;
	do {
		if (UDFReadLB(lbnum++, 1, LogBlock) <= 0) TagID = 0;
		else UDFDescriptor(LogBlock, &TagID);
		if (TagID == 261) {
			UDFFileEntry(LogBlock, FileType, File);
#ifdef UDF_DEBUG
			printf("Found File entry type %d at LB %ld, %ld bytes long\n", *FileType, File->Location, File->Length);
#endif
			return 1;
		};
	} while ((lbnum <= partition.Start + ICB.Location + (ICB.Length-1) / DVD_VIDEO_LB_LEN) && (TagID != 261));
	return 0;
}
	
// Dir: Location of directory to scan
// FileName: Name of file to look for
// FileICB: Location of ICB of the found file
// return 1 on success, 0 on error;
int UDFScanDir(struct AD Dir, char *FileName, struct AD *FileICB) 
{
	u8 LogBlock[DVD_VIDEO_LB_LEN];
	u32 lbnum;
	u16 TagID;
	u8 filechar;
	char filename[MAX_FILE_LEN];
	int p;
	
	// Scan dir for ICB of file
	lbnum = partition.Start + Dir.Location;
	do {
		if (UDFReadLB(lbnum++, 1, LogBlock) <= 0) TagID = 0;
		else {
			p = 0;
			while (p < DVD_VIDEO_LB_LEN) {
				UDFDescriptor(&LogBlock[p], &TagID);
				if (TagID == 257) {
					p += UDFFileIdentifier(&LogBlock[p], &filechar, filename, FileICB);
#ifdef UDF_DEBUG
					printf("Found ICB for file '%s' at LB %ld, %ld bytes long\n", filename, FileICB->Location, FileICB->Length);
#endif
					if (!strcasecmp(FileName, filename)) return 1;
				} else p = DVD_VIDEO_LB_LEN;
			}
		}
	} while (lbnum <= partition.Start + Dir.Location + (Dir.Length-1) / DVD_VIDEO_LB_LEN);
	return 0;
}

// looks for partition on the disc
//   partnum: number of the partition, starting at 0
//   part: structure to fill with the partition information
//   return 1 if partition found, 0 on error;
int UDFFindPartition(int partnum, struct Partition *part) 
{
	u8 LogBlock[DVD_VIDEO_LB_LEN], Anchor[DVD_VIDEO_LB_LEN];
	u32 lbnum, MVDS_location, MVDS_length;
	u16 TagID;
	//u8 Flags;
	//char Identifier[6];
	u32 lastsector;
	int i, terminate, volvalid;

	// Recognize Volume
	/*
	lbnum = 16;
	do {
		if (UDFReadLB(lbnum++, 1, LogBlock) <= 0) strcpy(Identifier, "");
		else UDFEntity(LogBlock, &Flags, Identifier);
#ifdef UDF_DEBUG
		printf("Looking for NSR02 at LB %ld, found %s\n", lbnum-1, Identifier);
#endif
	} while ((lbnum <= 256) && strcmp("NSR02", Identifier));
#ifdef UDF_DEBUG
	if (strcmp("NSR02", Identifier))  printf("Could not recognize volume. Bad.\n");
	else printf("Found %s at LB %ld. Good.\n", Identifier, lbnum-1);
#endif
	*/

	// Find Anchor
	lastsector = 0;
	lbnum = 256;   // try #1, prime anchor
	terminate = 0;
	while (1) {  // loop da loop
		if (UDFReadLB(lbnum, 1, Anchor) > 0) {
			UDFDescriptor(Anchor, &TagID);
		} else TagID = 0;
		if (TagID != 2) {             // not an anchor?
			if (terminate) return 0;    // final try failed 
			if (lastsector) {           // we already found the last sector
				lbnum = lastsector;       // try #3, alternative backup anchor
				terminate = 1;            // but that's just about enough, then!
			} else {
				// TODO: find last sector of the disc (this is optional)
				if (lastsector) lbnum = lastsector - 256; // try #2, backup anchor
				else return 0;            // unable to find last sector
			}
		} else break;                 // it is an anchor! continue...
	}
	UDFExtentAD(&Anchor[16], &MVDS_length, &MVDS_location);  // main volume descriptor
#ifdef UDF_DEBUG
	printf("MVDS at LB %ld thru %ld\n", MVDS_location, MVDS_location + (MVDS_length-1) / DVD_VIDEO_LB_LEN);
#endif
	
	part->valid = 0;
	volvalid = 0;
	part->VolumeDesc[0] = '\0';
	i = 1;
	do {
		// Find Volume Descriptor
		lbnum = MVDS_location;
		do {
			if (UDFReadLB(lbnum++, 1, LogBlock) <= 0) TagID = 0;
			else UDFDescriptor(LogBlock, &TagID);
#ifdef UDF_DEBUG
			printf("Looking for Descripors at LB %ld, found %d\n", lbnum-1, TagID);
#endif
			if ((TagID == 5) && (!part->valid)) {  // Partition Descriptor
#ifdef UDF_DEBUG
				printf("Partition Descriptor at LB %ld\n", lbnum-1);
#endif
				UDFPartition(LogBlock, &part->Flags, &part->Number, part->Contents, &part->Start, &part->Length);
				part->valid = (partnum == part->Number);
#ifdef UDF_DEBUG
				printf("Partition %d at LB %ld thru %ld\n", part->Number, part->Start, part->Start+part->Length-1);
#endif
			} else if ((TagID == 6) && (!volvalid)) {  // Logical Volume Descriptor
#ifdef UDF_DEBUG
				printf("Logical Volume Descriptor at LB %ld\n", lbnum-1);
#endif
				if (UDFLogVolume(LogBlock, part->VolumeDesc)) {  
					//TODO: sector size wrong!
				} else volvalid = 1;
#ifdef UDF_DEBUG
				printf("Logical Volume Descriptor: %s\n", part->VolumeDesc);  // name of the disc
#endif
			}
		} while ((lbnum <= MVDS_location + (MVDS_length-1) / DVD_VIDEO_LB_LEN) && (TagID != 8) && ((!part->valid) || (!volvalid)));
		if ((!part->valid) || (!volvalid)) UDFExtentAD(&Anchor[24], &MVDS_length, &MVDS_location);  // backup volume descriptor
	} while (i-- && ((!part->valid) || (!volvalid)));
	return (part->valid);  // we only care for the partition, not the volume
}

// looks for a file on the UDF disc/imagefile
// filename has to be the absolute pathname on the UDF filesystem, starting with /
// filesize will be set to the size of the file in bytes, on success
// returns absolute LB number, or 0 on error
unsigned long int UDFFindFile(char *filename, unsigned long int *filesize) 
{
	u8 LogBlock[DVD_VIDEO_LB_LEN];
	u32 lbnum;
	u16 TagID;
	struct AD RootICB, File, ICB;
	char tokenline[MAX_FILE_LEN];
	char *token;
	u8 filetype;
	
	int Partition = 0;  // Partition number to look for the file, 
										  // 0 is this is the standard location for DVD Video
	
	*filesize = 0;
	tokenline[0] = '\0';
	strcat(tokenline, filename);

	// Find partition
	if (!UDFFindPartition(Partition, &partition)) return 0;
	// Find root dir ICB
	lbnum = partition.Start;
	do {
		if (UDFReadLB(lbnum++, 1, LogBlock) <= 0) TagID = 0;
		else UDFDescriptor(LogBlock, &TagID);
#ifdef UDF_DEBUG
		printf("Found TagID %d at LB %ld\n", TagID, lbnum-1);
#endif
		if (TagID == 256) {  // File Set Descriptor
			UDFLongAD(&LogBlock[400], &RootICB);
		}
	} while ((lbnum < partition.Start + partition.Length) && (TagID != 8) && (TagID != 256));
	if (TagID!=256) return 0;
	if (RootICB.Partition != Partition) return 0;
	
	// Find root dir
	if (!UDFMapICB(RootICB, &filetype, &File)) return 0;
	if (filetype != 4) return 0;  // root dir should be dir
#ifdef UDF_DEBUG
	printf("Root Dir found at %ld\n", File.Location);
#endif

	// Tokenize filepath
	token = strtok(tokenline, "/");
	while (token != NULL) {
#ifdef UDF_DEBUG
		printf("looking for token %s\n", token);
#endif
		if (!UDFScanDir(File, token, &ICB)) return 0;
		if (!UDFMapICB(ICB, &filetype, &File)) return 0;
		token = strtok(NULL, "/");
	}
	*filesize = File.Length;
	return partition.Start + File.Location;
}



// DVD Copy Management: 
// RPC - Region Playback Control

// Query RPC status of the drive
// type: 0=NONE (no drive region setting)
//       1=SET (drive region is set
//       2=LAST CHANCE (drive region is set, only one change remains)
//       3=PERM (region set permanently, may be reset by vendor)
// vra: number of vendor resets available
// ucca: number of user controlled changes available
// region_mask: the bit of the drive's region is set to zero, all other 7 bits to one
// rpc_scheme: 0=unknown, 1=RPC Phase II, others reserved
// returns 0 on success, <0 on error
int UDFRPCGet(int *type, int *vra, int *ucca, int *region_mask, int *rpc_scheme) 
{
	int ret;
	if (disc.segments) {
		*type = 1;
		*vra = 4;
		*ucca = 5;
		*region_mask = disc.phoony_region_mask;
		*rpc_scheme = 0;
	} else {
#ifdef DVD_LU_SEND_RPC_STATE
		if (dvdromfile==NULL) return 0;
		ai.type = DVD_LU_SEND_RPC_STATE;
		if ((ret = ioctl(fileno(dvdromfile), DVD_AUTH, &ai)) < 0) return ret;
		*type = ai.lrpcs.type;
		*vra = ai.lrpcs.vra;
		*ucca = ai.lrpcs.ucca;
		*region_mask = ai.lrpcs.region_mask;
		*rpc_scheme = ai.lrpcs.rpc_scheme;
#else
		return -1;
#endif
	}
	return 0;
}

// Set new Region for drive
// region_mask: the bit of the new drive's region is set to zero, all other 7 bits to one
int UDFRPCSet(int region_mask) 
{
	int ret;
	if (disc.segments) {
		disc.phoony_region_mask = region_mask;
	} else {
#ifdef DVD_HOST_SEND_RPC_STATE
		if (dvdromfile == NULL) return 0;
		ai.type = DVD_HOST_SEND_RPC_STATE;
		ai.hrpcs.pdrc = region_mask & 0xFF;
		if ((ret = ioctl(fileno(dvdromfile), DVD_AUTH, &ai)) < 0) return ret;
#else
		return -1;
#endif
	}
	return 0;
}

// open block device or image file
// returns fileno() of the file on success, or -1 on error
int UDFOpenDisc(char *filename) 
{
	struct stat filestat;
	int i;
	char tempfilename[PATH_MAX];
	if (filename == NULL) return -1;
	Path[0] = '\0';
	disc.currentsegment = -1;
	disc.segmentlength = NULL;
	disc.segtablelen = 0;
	disc.phoony_region_mask = 0x00;
	if (strlen(Filename) < (PATH_MAX-4)) {
		strcpy(tempfilename, filename);
	} else {
		strncpy(tempfilename, filename, PATH_MAX-4);
	}
	if (stat(tempfilename, &filestat) < 0) {
		if (tempfilename[strlen(tempfilename)-1] != '.') strcat(tempfilename, ".");
		strcat(tempfilename, "000");
	}
	if (stat(tempfilename, &filestat) >= 0) {
		if (S_ISDIR(filestat.st_mode)) {  // directory?  dir/DVDVIDEO.000 obligatory
#ifdef UDF_DEBUG
printf("is directory, looking for %s\n", SPLITNAME ".000");
#endif
			if (tempfilename[strlen(tempfilename)-1] != '/') strcat(tempfilename, "/");
			strcat(tempfilename, SPLITNAME ".000");
			if (stat(tempfilename, &filestat) < 0) return -1;  // file not found
		}
		if (S_ISBLK(filestat.st_mode)) {  // block device?
#ifdef UDF_DEBUG
printf("is block device.\n");
#endif
			disc.segments = 0;
			strcpy(Filename, tempfilename);
		} else if (S_ISREG(filestat.st_mode)) {  // image file? tempfilename.000 for split image obligatory
			i = strlen(tempfilename);
			while ((--i >= 0) && (tempfilename[i] != '/'));
			if ((i >= 0) && (tempfilename[i] == '/')) { // slash in name?
				strncpy(Path, tempfilename, i + 1);         // separate there
				strcpy(Filename, &tempfilename[i + 1]);
			} else {                            // no slash
				strcpy(Path, "./");                // in local directory
				strcpy(Filename, tempfilename);
			}
			if (strncmp(&Filename[strlen(Filename)-4], ".000", 4)) {  // does Filename end on 000?
				strcpy(tempfilename, Path);
				strcat(tempfilename, Filename);
				strcat(tempfilename, ".000");  // or do we have an Filename.000?
				if (stat(tempfilename, &filestat) >= 0) {
					disc.segments = 0;  // then it might be split file
				} else {
					disc.segments = 1;  // else one image file
				}
			} else {  // ends on 000, chop that off
				i = strlen(Filename)-4;
				Filename[i] = '\0';                   // cut off extension
				disc.segments = 0;
			}
			if (!disc.segments) {
				strcpy(tempfilename, Path);
				strcat(tempfilename, Filename);
				strcat(tempfilename, ".001");  // do we have a second segment?
				if (stat(tempfilename, &filestat) >= 0) {
#ifdef UDF_DEBUG
printf("is split image files.\n");
#endif
					udf_update_segments();
					if (disc.segments == 1) disc.segmentlength[disc.segments++]=0;  // dummy segment
				} else {
					disc.segments = 1;
					strcat(Filename, ".000");
				}
			}
			if (disc.segments == 1) {
#ifdef UDF_DEBUG
printf("is one image file.\n");
#endif
			}
		}
	} else {
#ifdef UDF_DEBUG
printf("stat failed. block device?\n");
#endif
		disc.segments = 0;
		strcpy(Filename, tempfilename);
	}
	if (disc.segments <= 1) {
		strcpy(tempfilename, Path);
		strcat(tempfilename, Filename);
		if ((dvdromfile = fopen(tempfilename, "r")) != NULL) {
			return fileno(dvdromfile);
		}
	} else {
		return 0;
	}
	return -1;
}

// closes previously opened block device or image file
// returns 0 on success, or -1 on error
int UDFCloseDisc(void) 
{
	fclose(dvdromfile);
	return 0;
}
