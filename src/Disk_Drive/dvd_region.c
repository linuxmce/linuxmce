/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 * Set / inspect region settings on a DVD drive. This is most interesting
 * on a RPC Phase-2 drive, of course.
 *
 * Usage: dvd_region [ -d device ] [ [ -s ] [ -r region ] ]
 *
 * Jens Axboe <axboe@suse.de>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>

#define DEFAULT_DEVICE "/dev/dvd"

int do_region(int fd, dvd_authinfo *ai, int set, int region)
{
        int ret;
        ai->type = DVD_LU_SEND_RPC_STATE;

        if ((ret = ioctl(fd, DVD_AUTH, ai)) < 0) {
                perror("do_region get");
                return ret;
        }

        if (set) {
                if (region > 8 || region <= 0) {
                        printf("Invalid region (%d)\n", region);
                        return 1;
                }
                printf("Setting drive region can only be done a finite " \
                        "number of times, press CTRL-C now to cancel!\n");
                getchar();

                ai->type = DVD_HOST_SEND_RPC_STATE;
                ai->hrpcs.pdrc = ~(1 << (region - 1));
                printf("Setting region to %d (%x)\n", region, ai->hrpcs.pdrc);
                if ((ret = ioctl(fd, DVD_AUTH, ai)) < 0) {
                        perror("do_region set");
                        return ret;
                }
        }

        return 0;
}

void print_region(dvd_authinfo *ai)
{
        printf("Drive region info:\n");

        printf("Type: ");
        switch (ai->lrpcs.type) {
                case 0:
                        printf("No drive region setting\n");
                        break;
                case 1:
                        printf("Drive region is set\n");
                        break;
                case 2:
                        printf("Drive region is set, with additional " \
                                "restrictions required to make a change\n");
                        break;
                case 3:
                        printf("Drive region has been set permanently, but " \
                                "may be reset by the vendor if necessary\n");
                        break;
                default:
                        printf("Invalid (%x)\n", ai->lrpcs.type);
                        break;
        }

        printf("%d vendor resets available\n", ai->lrpcs.vra);
        printf("%d user controlled changes available\n", ai->lrpcs.ucca);
        printf("Region: ");
        if (ai->lrpcs.region_mask)
                printf("%d playable\n", ffs(~ai->lrpcs.region_mask));
        else
                printf("non-rpc (all)\n");

        printf("RPC Scheme: ");
        switch (ai->lrpcs.rpc_scheme) {
                case 0:
                        printf("The Logical Unit does not enforce Region " \
                                "Playback Controls (RPC)\n");
                        break;
                case 1:
                        printf("The Logical Unit _shall_ adhere to the "
                                "specification and all requirements of the " \
                                "CSS license agreement concerning RPC\n");
                        break;
                default:
                        printf("Reserved (%x)\n", ai->lrpcs.rpc_scheme);
        }
}

int main(int argc, char *argv[])
{
        char device_name[FILENAME_MAX], c, set, get, region;
        dvd_authinfo ai;
        int fd, ret;

        strcpy(device_name, DEFAULT_DEVICE);
        set = 0;
        get = 1;
        while ((c = getopt(argc, argv, "d:sr:")) != EOF) {
                switch (c) {
                        case 'd':
                                strncpy(device_name, optarg, FILENAME_MAX - 1);
                                break;
                        case 's':
                                set = 1;
                                break;
                        case 'r':
                                region = strtoul(optarg, NULL, 10);
                                printf("region %d\n", region);
                                break;
                }
        }

        if ((fd = open(device_name, O_RDONLY | O_NONBLOCK)) < 0) {
                perror("open");
                return 1;
        }

        memset(&ai, 0, sizeof(ai));
        if ((ret = do_region(fd, &ai, 0, 0)))
                return ret;

        if (set)
                if ((ret = do_region(fd, &ai, 1, region)) < 0)
                        return ret;

        print_region(&ai);
        return 0;
}
