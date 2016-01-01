/**
 * IdentifierCDDB - identify an inserted disc against FreeDB
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com> - class structure
 * Author: phenigma <phenigma@hotmail.com> - functionality - based on example in libcddb2
 */


#include <stdlib.h>
#include <string.h>
#include <cddb/cddb.h>

#include "IdentifierCDDB.h"
#include "OutputMiscTab.h"

using namespace DCE;
using namespace std;

/* free pointer when it's pointing at something */
#define FREE_NOT_NULL(p) if (p) { free(p); p = NULL; }

/* return string, or return null if string is blank */
#define STR_OR_NULL(s) ((s) ? s : "(null)")

// forward declarations
cddb_disc_t *do_read(cddb_conn_t *conn, const char *category, int discid, int quiet);
cddb_disc_t *cd_read(char *device);
cddb_disc_t *cd_create(int dlength, int tcount, int *foffset, int use_time);

IdentifierCDDB::IdentifierCDDB(string sPath) : IdentifierBase(sPath)
{
    FREE_NOT_NULL(conn);
    FREE_NOT_NULL(disc);
    FREE_NOT_NULL(device);
    FREE_NOT_NULL(category);
}

IdentifierCDDB::~IdentifierCDDB()
{
  conn = NULL;   /* libcddb connection structure */
  disc = NULL;   /* libcddb disc structure */
  charset = NULL;
  use_cd = 1;          /* use CD-ROM to retrieve disc data */
  device = NULL;       /* device to use if use_cd == 1. NULL means
                                     to find a suitable CD-ROM drive. */
  category = NULL;     /* category command-line argument */
  discid = 0;          /* disc ID command-line argument or calculated */
}

bool IdentifierCDDB::Init()
{
  if ( !IdentifierBase::Init() )
  {
    return false;
  }

  /* much of this is based on the libcddb2 example application */
  /* create a new cddb connection */
  conn = cddb_new();

  if (!conn)
  {
    // error_exit(GENERIC_ERROR, "unable to create connection structure");
    return false;
  }

  return true;
}

bool IdentifierCDDB::Identify()
{
    if (!conn)
    {
      // error_exit(GENERIC_ERROR, "unable to create connection structure");
      return false;
    }

    // check if file exists, todo: replace with local check
    if ( !IdentifierBase::Identify() )
    {
      return false;
    }

    disc = cd_read((char *)m_sPath.c_str());
    if (!disc)
    {
        //error_exit(GENERIC_ERROR, "could not read CD in CD-ROM drive");
        return false;
    }

    int matches;
    cddb_disc_calc_discid(disc);
    matches = cddb_query(conn, disc);
    if (matches == -1)
    {
        //error_exit(cddb_errno(conn), "could not query");
        return false;
    }
    else if (matches == 0)
    {
        //error_exit(CDDB_ERR_DISC_NOT_FOUND, "no matching discs found");
        return false;
    }

    /* Get the disc information needed for the read command.
       Afterwards we destroy the current disc because do_read
       will return a new disc. */
    category = strdup(cddb_disc_get_category_str(disc));
    discid = cddb_disc_get_discid(disc);
    cddb_disc_destroy(disc);

    int quiet = 1;

    disc = do_read(conn, category, discid, quiet);
    if (!disc)
    {
        //error_exit(cddb_errno(conn), "could not read disc data");
        return false;
    }

    /* The category string is no longer needed because it is now
       also present in the disc structure. */
    FREE_NOT_NULL(category);

    return true;
}

// returns the tab delimited string of data
string IdentifierCDDB::GetIdentifiedData()
{
    string sRet = "";

    // fabricate return string from disc data here
    if (!disc)
    {
        //error_exit(cddb_errno(conn), "could not read disc data");
        return sRet;
    }

    category = strdup(cddb_disc_get_category_str(disc));
    discid = cddb_disc_get_discid(disc);

    OutputMiscTab Disc(discid);

    // for each found item Disc.addAttribute(track,attributetype,section,wholename);

    // MISC-TAB format?
        // The format is as follows.  Each line (\n terminated) contains the following:
        // The first line only is the disc id
        // Track \t PK_AttributeType \t Section \t Name/Description
        // Track refers to separate tracks on a cd, which, if ripped, would be separate files.
        // Section, however, refers to chapters within a DVD--there is only file, but it has sections

    // CDDB-TAB format
	// CDDBID Performer\tAlbum\tGenre\tTitle\t

    cddb_track_t *track = NULL; /* libcddb track structure */

    // !!!! This is the 'Album Artist' which can often be different than the
    //  'Track Artist' which can vary due to guest artists. -phenigma
    /* 1. The artist name, disc title and extended data. */
    printf("Artist:   %s\n", STR_OR_NULL(cddb_disc_get_artist(disc)));
    string sDiscArtist = STR_OR_NULL(cddb_disc_get_artist(disc));

    printf("Title:    %s\n", STR_OR_NULL(cddb_disc_get_title(disc)));
    string sDiscTitle = STR_OR_NULL(cddb_disc_get_title(disc));

    // LMCE has no method of specifically storing this ext data at the moment
    // display it regardless for now
    const char *s = cddb_disc_get_ext_data(disc);
    if (s) {
        printf("Ext.data: %s\n", s);
    }

    /* 2. The music genre.  This field is similar to the category
       field initialized above.  It can be the same but it does not
       have to be.  The category can only be come from a set of CDDB
       predefined categories.  The genre can be any string. */
    printf("Genre:    %s\n", STR_OR_NULL(cddb_disc_get_genre(disc)));
    string sGenre = STR_OR_NULL(cddb_disc_get_genre(disc));

    /* 3. The disc year. */
    printf("Year:     %d\n", cddb_disc_get_year(disc));
    int iYear = cddb_disc_get_year(disc);

    /* 4. The disc length and the number of tracks.  The length field
       is given in seconds. */
    int iDiscLength = cddb_disc_get_length(disc);
    int iTracks = cddb_disc_get_track_count(disc);
    printf("Length:   %d:%02d (%d seconds)\n", (iDiscLength / 60), (iDiscLength % 60), iDiscLength);
    printf("%d tracks\n", cddb_disc_get_track_count(disc));



// ***********************  track iteration here
    /* 5. The tracks.  Track iteration can either be done by counting
       from 0 to (track_count - 1) and using the cddb_disc_get_track
       function.  Or by using the built-in iterator functions
       cddb_disc_get_track_first and cddb_disc_get_track_next.  We'll
       use the latter approach in this example. */


// lmce stores data per track, not per disc... per se.  both are possible but
// this needs a little more research to implement properly.
    for (track = cddb_disc_get_track_first(disc);

         track != NULL;
         track = cddb_disc_get_track_next(disc)) {

        /* 5.a. The track number on the disc.  This track number
           starts counting at 1.  So this is not the same number as
           the one used in cddb_disc_get_track. */
        printf("  [%02d]", cddb_track_get_number(track));
	int iTrackNum = cddb_track_get_number(track);

        /* 5.b. The track artist name and title. */
        printf(" '%s'", cddb_track_get_title(track));
        s = cddb_track_get_artist(track);
        if (s) {
            printf(" by %s", s);
        }

        /* 5.c. The track length. */
        int iTrackLength = cddb_track_get_length(track);
        if (iTrackLength != -1) {
            printf(" (%d:%02d)", (iTrackLength / 60), (iTrackLength % 60));
        } else {
            printf(" (unknown)");
        }

        /* 5.d. The extended track data. */
        s = cddb_track_get_ext_data(track);
        if (s) {
            printf(" [%s]\n", s);
        } else {
            printf("\n");
        }
    }
// ***********************  track iteration done

    return sRet;
}

// tells lmce what format the returned data will be in
IdentifierBase::eIdentityType IdentifierCDDB::GetIdentityType()
{
  return MISC_TAB;
}






// do_read.c && cd_access.c from libcddb2 examples


/*
    $Id: cd_access.c,v 1.14 2006/09/29 16:01:42 airborne Exp $

    Copyright (C) 2003, 2004, 2005 Kris Verbeeck <airborne@advalvas.be>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the
    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA  02111-1307, USA.
*/

#include <stdlib.h>
#include <string.h>
#include <cdio/cdio.h>

#define STR_OR_NULL(s) ((s) ? s : "(null)")

cddb_disc_t *do_read(cddb_conn_t *conn, const char *category, int discid, int quiet)
{
    cddb_disc_t *disc = NULL;   /* libcddb disc structure */
    int success;

    /* Create a new disc structure. */
    disc = cddb_disc_new();

    /* If the pointer is NULL then an error occured (out of memory).
       Otherwise we continue. */
    if (disc) {
        /* Initialize the category of the disc.  This function
           converts a string into a category ID as used by libcddb.
           If the specified string does not match any of the known
           categories, then the category is set to 'misc'. */
        cddb_disc_set_category_str(disc, category);

        /* Initialize the ID of the disc. */
        cddb_disc_set_discid(disc, discid);

        /* Try reading the rest of the disc data.  This information
           will be retrieved from the server or read from the cache
           depending on the connection settings. */
        success = cddb_read(conn, disc);

        /* If an error occured then the return value will be false and the
           internal libcddb error number will be set. */
        if (!success) {
            /* Print an explanatory message on stderr.  Other routines are
               available for retrieving the message without printing it or
               printing it on a stream other than stderr. */
            if (!quiet) {
                cddb_error_print(cddb_errno(conn));
            }
            /* Destroy the disc. */
            cddb_disc_destroy(disc);
            /* And return NULL to signal an error. */
            return NULL;
        }
    }

    return disc;
}


#define libcdio_error_exit(...) // error_exit(GENERIC_ERROR, "libcdio: " __VA_ARGS__)

cddb_disc_t *cd_read(char *device)
{
    cddb_disc_t *disc = NULL;   /* libcddb disc structure */

#ifndef HAVE_LIBCDIO

    fprintf(stderr, "*** libcdio support missing, CD access failed ***\n");
    fprintf(stderr, "*** see http://www.gnu.org/software/libcdio/ ***\n");

#else

    CdIo_t *cdio;               /* libcdio CD access structure */
    track_t cnt, t;             /* track counters */
    lba_t lba;                  /* Logical Block Address */
    int *foffset = NULL;        /* list of frame offsets */

    /* Get the name of the default CD-ROM device. */
    if (!device) {
        device = cdio_get_default_device(NULL);
        if (!device) {
            libcdio_error_exit("unable to get default CD device");
        }
    }
    printf("CD-ROM device: %s\n", device);

    /* Load the appropriate driver and open the CD-ROM device for reading. */
    cdio = cdio_open(device, DRIVER_UNKNOWN);
    if (!cdio) {
        libcdio_error_exit("unable to open CD device");
    }

    /* Get the track count for the CD. */
    cnt = cdio_get_num_tracks(cdio);
    if (cnt == 0) {
        libcdio_error_exit("no audio tracks on CD");
    }
    printf("CD contains %d track(s)\n", cnt);

    /* Reserve some memory for the frame offsets. */
    foffset = calloc(cnt, sizeof(int));

    /* Now we go and fetch the track data. */
    for (t = 1; t <= cnt; t++) {

        /* We only want to process audio CDs. */
        if (cdio_get_track_format(cdio, t) != TRACK_FORMAT_AUDIO) {
            libcdio_error_exit("track %d is not an audio track", t);
        }

        /* Get frame offset of next track. */
        lba = cdio_get_track_lba(cdio, t);
        if (lba == CDIO_INVALID_LBA) {
            libcdio_error_exit("track %d has invalid Logical Block Address", t);
        }

        /* Add this offset to the list. */
        foffset[t - 1] = lba;
    }

    /* Now all we still have to do, is calculate the length of the
       disc in seconds.  We use the LEADOUT_TRACK for this. */
    lba = cdio_get_track_lba(cdio, CDIO_CDROM_LEADOUT_TRACK);
    if (lba == CDIO_INVALID_LBA) {
        libcdio_error_exit("LEADOUT_TRACK has invalid Logical Block Address");
    }

    /* Now we have to create the libcddb disc structure. */
    disc = cd_create(FRAMES_TO_SECONDS(lba), cnt, foffset, 0);

    /* Free all resources held by libcdio CD access structure. */
    cdio_destroy(cdio);

    /* more clean up */
    FREE_NOT_NULL(foffset);
    FREE_NOT_NULL(device);

#endif

    return disc;
}

cddb_disc_t *cd_create(int dlength, int tcount, int *foffset, int use_time)
{
    int i;
    cddb_disc_t *disc;
    cddb_track_t *track;

    /* Create a new disc structure. */
    disc = cddb_disc_new();

    /* If the pointer is NULL then an error occured (out of memory).
       Otherwise we continue. */
    if (disc) { 
        /* Initialize the disc length in the structure. */
        cddb_disc_set_length(disc, dlength);

        /* Now we have to add the basic track data. */
        for (i = 0; i < tcount; i++) {
            /* Create a new libcddb track structure for this track. */
            track = cddb_track_new();

            /* If the pointer is NULL then an error occured (out of
               memory).  Otherwise we continue. */
            if (!track) { 
                /* Destroy the disc because we can not return half of
                   it.  Return NULL to signal failure. */
                cddb_disc_destroy(disc);
                return NULL;
            }

            /* First add the track to the disc. */
            cddb_disc_add_track(disc, track);

            if (use_time) {
                /* Set length in track structure.  Since this track is
                   already part of a disc, the frame offset for the
                   track will be calculated automatically when we set
                   its length. */
                cddb_track_set_length(track, foffset[i]);
            } else {
                /* Set frame offset in track structure. */
                cddb_track_set_frame_offset(track, foffset[i]);
            }
        }
    }
    return disc;
}
