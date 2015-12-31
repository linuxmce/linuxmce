/*
 * Some functions for UDF handling, originally part of dvd_disc_20000215.tar.gz
 * from convergence.
 *
 * cleanups (remove every with a relation to CSS)
 *   by Mirko Dölle <cooper@linvdr.org>
 */ 
#ifndef DVD_UDF_H
#define DVD_UDF_H

#define DVD_UDF_VERSION 20000215


/***********************************************************************************/
/* The length of one Logical Block of a DVD Video                                  */
/***********************************************************************************/
#define DVD_VIDEO_LB_LEN 2048

/***********************************************************************************/
/* reads Logical Block of the disc or image                                        */
/*   lb_number: disc-absolute logical block number                                 */
/*   block_count: number of 2048 byte blocks to read                               */
/*   data: pointer to enough allocated memory                                      */
/*   returns number of read bytes on success, 0 or negative error number on error  */
/***********************************************************************************/
int UDFReadLB(unsigned long int lb_number, unsigned int block_count, unsigned char *data);

/***********************************************************************************/
/* looks for a file on the UDF disc/imagefile                                      */
/*   filename: absolute pathname on the UDF filesystem, starting with '/'          */
/*   filesize will be set to the size of the file in bytes, on success             */
/*   returns absolute LB number, or 0 on error                                     */
/***********************************************************************************/
unsigned long int UDFFindFile(char *filename, unsigned long int *filesize);


/************************************/
/* DVD Copy Management:             */
/* RPC - Region Playback Control    */
/************************************/

/***********************************************************************************/
/* Query RPC status of the drive                                                   */
/* type: 0=NONE (no drive region setting)                                          */
/*       1=SET (drive region is set                                                */
/*       2=LAST CHANCE (drive region is set, only one change remains)              */
/*       3=PERM (region set permanently, may be reset by vendor)                   */
/* vra: number of vendor resets available                                          */
/* ucca: number of user controlled changes available                               */
/* region_mask: the bit of the drive's region is set to 0, all other 7 bits to 1   */
/* rpc_scheme: 0=unknown, 1=RPC Phase II, others reserved                          */
/* returns 0 on success, <0 on error                                               */
/***********************************************************************************/
int UDFRPCGet(int *type, int *vra, int *ucca, int *region_mask, int *rpc_scheme);

/***********************************************************************************/
/* Set new Region for drive                                                        */
/* region_mask: the bit of the new drive's region is set to 0, all other 7 bits to 1 */
/***********************************************************************************/
int UDFRPCSet(int region_mask);

/***********************************************************************************/
/* opens block device or image file                                                */
/*   filename: path to the DVD ROM block device or to the image file in UDF format */
/*   returns fileno() of the file on success, or -1 on error                       */
/***********************************************************************************/
int UDFOpenDisc(char *filename);

/***********************************************************************************/
/* closes previously opened block device or image file                             */
/*   returns 0 on success, or -1 on error                                          */
/***********************************************************************************/
int UDFCloseDisc(void);

#endif /* DVD_UDF_H */
