/*
 * Copyright (C) 2000-2005 the xine project
 *
 * Copyright (C) 2009 Petri Hintukainen <phintuka@users.sourceforge.net>
 *
 * This file is part of xine, a free video player.
 *
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * Input plugin for BluRay discs / images
 *
 * Requires libbluray from http://www.assembla.com/spaces/libbluray/
 * Tested with SVN revision 103
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* asprintf: */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#include <bluray.h>
#include <libbdnav/navigation.h>

#define LOG_MODULE "input_bluray"
#define LOG_VERBOSE

#define LOG

#define LOGMSG(x...)  xine_log (this->stream->xine, XINE_LOG_MSG, "input_bluray: " x);


#ifdef HAVE_CONFIG_H
# include "xine_internal.h"
# include "input_plugin.h"
#else
# include <xine/xine_internal.h>
# include <xine/input_plugin.h>
#endif

#ifndef EXPORTED
#  define EXPORTED __attribute__((visibility("default")))
#endif

#ifndef MIN
# define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define ALIGNED_UNIT_SIZE 6144
#define PKT_SIZE          192
#define TICKS_IN_MS       45

typedef struct {

  input_class_t   input_class;

  xine_t         *xine;

  /* config */
  char           *mountpoint;
  char           *keyfile;
  char           *device;

} bluray_input_class_t;

typedef struct {
  input_plugin_t        input_plugin;

  xine_stream_t        *stream;
  bluray_input_class_t *class;
  char                 *mrl;
  char                 *disc_root;

  BLURAY               *bdh;
  NAV_TITLE            *nav_title;

} bluray_input_plugin_t;

static int open_clip (bluray_input_plugin_t *this, NAV_CLIP *clip)
{
  /* NOTE: bluray.h bd_select_title() actually opens CLIP, not TITLE ! */

  int clip_id;
  if (sscanf(clip->name, "%d.m2ts", &clip_id) != 1)
    clip_id = 0;

  lprintf("Selecting clip %d: bd_select_title(%d - %s) for title %s\n",
          clip_id, clip_id, clip->name, this->nav_title->name);

  if (!bd_select_title(this->bdh, clip_id)) {
    LOGMSG("bd_select_title(%d) failed: %s\n", clip_id, strerror(errno));
    _x_message(this->stream, XINE_MSG_FILE_NOT_FOUND, this->mrl, NULL);

    return -1;
  }

  lprintf("Clip length: %"PRIu64" bytes\n", (uint64_t)this->bdh->s_size);

  return 1;
}

static int next_clip (bluray_input_plugin_t *this)
{
  /* select clip */

  NAV_CLIP *clip = nav_next_clip(this->nav_title, NULL);
  if (!clip) {
    LOGMSG("nav_next_clip() FAILED\n");
    return -1;
  }
  lprintf("clip change: title %s clip %s (%d clips, %d chapters)\n",
          this->nav_title->name, clip->name,
          this->nav_title->clip_list.count, this->nav_title->chap_list.count);

  /* open clip */

  return open_clip(this, clip);
}

static int open_title (bluray_input_plugin_t *this, int title)
{
  if (this->nav_title)
    nav_title_close(this->nav_title);

  /* open title */

  char mpls[11] = {0};
  snprintf(mpls, sizeof(mpls), "%05d.mpls", MIN(99999, MAX(0, title)));

  this->nav_title = nav_title_open(this->disc_root, mpls);

  if (!this->nav_title) {
    LOGMSG("nav_title_open(%s, %s) FAILED\n", this->disc_root, mpls);
    return -1;
  }

#ifdef LOG
  int ms = this->nav_title->duration / TICKS_IN_MS;
  lprintf("Opened title %s. Length %"PRId64" bytes / %02d:%02d:%02d.%03d\n",
          this->nav_title->name, (int64_t)this->nav_title->packets * PKT_SIZE,
          ms / 3600000, (ms % 3600000 / 60000), (ms % 60000) / 1000, ms % 1000);
#endif

  /* set stream metainfo */

  /* title */
  if (strcmp(this->disc_root, this->class->mountpoint)) {
    char *t = strrchr(this->disc_root, '/');
    if (!t[1])
      while (t > this->disc_root && t[-1] != '/') t--;
    t = strdup(t);
    if (t[strlen(t)-1] ==  '/')
      t[strlen(t)-1] = 0;
    _x_meta_info_set(this->stream, XINE_META_INFO_TITLE, t);
    free(t);
  }

  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_TITLE_NUMBER,   title);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_ANGLE_NUMBER,   this->nav_title->angle);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER, 1);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT,  this->nav_title->chap_list.count);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_HAS_CHAPTERS,       this->nav_title->chap_list.count>0);

  return next_clip(this);
}

/*
 * xine plugin interface
 */

static uint32_t bluray_plugin_get_capabilities (input_plugin_t *this_gen)
{
  return INPUT_CAP_SEEKABLE  |
         INPUT_CAP_BLOCK     |
         INPUT_CAP_AUDIOLANG |
         INPUT_CAP_SPULANG;
}

static off_t bluray_plugin_read (input_plugin_t *this_gen, char *buf, off_t len)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (!this || !this->bdh || len < 0)
    return -1;

  if (this->bdh->aacs) {

    /*
     * Split large reads to aligned units
     */

    off_t todo  = len;
    off_t block = MIN(todo, ALIGNED_UNIT_SIZE - (this->bdh->s_pos % ALIGNED_UNIT_SIZE));

    while (block > 0) {
      off_t result = bd_read(this->bdh, (unsigned char *)buf, block);
      if (result != block) {
        if (result < 0) {
          LOGMSG("ERROR: bd_read(aacs, %"PRId64") : got %"PRId64" !\n", block, result);
          return result;
        }
        return len - todo + MAX(0, result);
      }
      todo -= result;
      buf  += result;

      block = MIN(todo, ALIGNED_UNIT_SIZE);
    }

    return len;
  }

  off_t result = bd_read (this->bdh, (unsigned char *)buf, len);

  if (result < 0)
    LOGMSG("bd_read() failed: %s (%d of %d)\n", strerror(errno), (int)result, (int)len);

#if 0
  if (buf[4] != 0x47) {
    LOGMSG("bd_read(): invalid data ? [%02x %02x %02x %02x %02x ...]\n",
          buf[0], buf[1], buf[2], buf[3], buf[4]);
    return 0;
  }
#endif

  return result;
}

static buf_element_t *bluray_plugin_read_block (input_plugin_t *this_gen, fifo_buffer_t *fifo, off_t todo)
{
  buf_element_t *buf = fifo->buffer_pool_alloc (fifo);

  if (todo > (off_t)buf->max_size)
    todo = buf->max_size;

  if (todo > ALIGNED_UNIT_SIZE)
    todo = ALIGNED_UNIT_SIZE;

  if (todo > 0) {

    buf->size = bluray_plugin_read(this_gen, (char*)buf->mem, todo);
    buf->type = BUF_DEMUX_BLOCK;

    if (buf->size > 0)
      return buf;
  }

  buf->free_buffer (buf);
  return NULL;
}

static off_t bluray_plugin_seek (input_plugin_t *this_gen, off_t offset, int origin)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (!this || !this->bdh || !this->nav_title)
    return -1;

  /* convert relative seeks to absolute */

  if (origin == SEEK_CUR) {
    offset = this->bdh->s_pos + offset;
  }
  else if (origin == SEEK_END) {
    if (offset < this->bdh->s_size)
      offset = this->bdh->s_size - offset;
    else
      offset = 0;
  }

  /* clip seek point to nearest random access point */

    uint32_t in_pkt   = offset / PKT_SIZE;
    uint32_t out_pkt  = in_pkt;
    uint32_t out_time = 0;
    nav_packet_search(this->nav_title, in_pkt, &out_pkt, &out_time);
    lprintf("bluray_plugin_seek() seeking to %"PRId64" (packet %d)\n", offset, in_pkt);
    offset = (off_t)PKT_SIZE * (off_t)out_pkt;
    lprintf("Nearest random access point at %"PRId64" (packet %d)\n", offset, out_pkt);

  /* clip to aligned unit start */

  offset -= (offset % ALIGNED_UNIT_SIZE);

  /* seek */

  lprintf("bluray_plugin_seek() seeking to %lld (aligned unit)\n", (long long)offset);

  return bd_seek (this->bdh, offset);
}

static off_t bluray_plugin_get_current_pos (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  return this->bdh ? this->bdh->s_pos : 0;
}

static off_t bluray_plugin_get_length (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  return this->bdh ? this->bdh->s_size : -1;
}

static uint32_t bluray_plugin_get_blocksize (input_plugin_t *this_gen)
{
  return ALIGNED_UNIT_SIZE;
}

static const char* bluray_plugin_get_mrl (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  return this->mrl;
}

static int bluray_plugin_get_optional_data (input_plugin_t *this_gen, void *data, int data_type)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (!this || !this->stream || !data)
    return INPUT_OPTIONAL_UNSUPPORTED;

  switch (data_type) {

    case INPUT_OPTIONAL_DATA_DEMUXER:
#ifdef HAVE_CONFIG_H
      *(const char **)data = "mpeg-ts";
#else
      *(const char **)data = "mpeg-ts-hdmv";
#endif
      return INPUT_OPTIONAL_SUCCESS;

    /*
     * audio track language:
     * - channel number can be mpeg-ts PID (0x1100 ... 0x11ff)
     */
    case INPUT_OPTIONAL_DATA_AUDIOLANG:
      if (this->nav_title) {
        int        channel = *((int *)data);
        CLPI_PROG *prog    = &this->nav_title->clip_list.clip->cl->program.progs[0];
        int i, n = 0;
        for (i=0 ; i < prog->num_streams; i++)
          if (prog->streams[i].pid >= 0x1100 && prog->streams[i].pid < 0x1200) {
            /* audio stream #n */
            if (channel == n || channel == prog->streams[i].pid) {
              memcpy(data, prog->streams[i].lang, 4);

              lprintf("INPUT_OPTIONAL_DATA_AUDIOLANG: ch %d pid %x: %s\n",
                      channel, prog->streams[i].pid, prog->streams[i].lang);

              return INPUT_OPTIONAL_SUCCESS;
            }
            n++;
          }
      }
      return INPUT_OPTIONAL_UNSUPPORTED;

    /*
     * SPU track language:
     * - channel number can be mpeg-ts PID (0x1200 ... 0x12ff)
     */
    case INPUT_OPTIONAL_DATA_SPULANG:
      if (this->nav_title) {
        int        channel = *((int *)data);
        CLPI_PROG *prog    = &this->nav_title->clip_list.clip->cl->program.progs[0];
        int i, n = 0;
        for (i=0 ; i < prog->num_streams; i++)
          if (prog->streams[i].pid         >= 0x1200 && prog->streams[i].pid         <  0x1300 &&
              prog->streams[i].coding_type >= 0x90   && prog->streams[i].coding_type <= 0x92) {
            /* subtitle stream #n */
            if (channel == n || channel == prog->streams[i].pid) {

              memcpy(data, prog->streams[i].lang, 4);

              lprintf("INPUT_OPTIONAL_DATA_SPULANG: ch %d pid %x: %s\n",
                      channel, prog->streams[i].pid, prog->streams[i].lang);

              return INPUT_OPTIONAL_SUCCESS;
            }
            n++;
          }
      }
      return INPUT_OPTIONAL_UNSUPPORTED;

    default:
      return DEMUX_OPTIONAL_UNSUPPORTED;
    }

  return INPUT_OPTIONAL_UNSUPPORTED;
}

static void bluray_plugin_dispose (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (this->bdh)
    bd_close(this->bdh);

  if (this->nav_title)
    nav_title_close(this->nav_title);

  free (this->mrl);
  free (this->disc_root);

  free (this);
}

static int bluray_plugin_open (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this  = (bluray_input_plugin_t *) this_gen;
  int                    title = -1, chapter = 0;

  lprintf("bluray_plugin_open\n");

  /* validate mrl */

  if (strncasecmp (this->mrl, "bluray:", 7))
    return -1;

  if (!strcasecmp (this->mrl, "bluray:") ||
      !strcasecmp (this->mrl, "bluray:/") ||
      !strcasecmp (this->mrl, "bluray://") ||
      !strcasecmp (this->mrl, "bluray:///")) {

    this->disc_root = strdup(this->class->mountpoint);

  } else if (!strncasecmp (this->mrl, "bluray:/", 8)) {

    if (!strncasecmp (this->mrl, "bluray:///", 10))
      this->disc_root = strdup(this->mrl + 9);
    else if (!strncasecmp (this->mrl, "bluray://", 9))
      this->disc_root = strdup(this->mrl + 8);
    else
      this->disc_root = strdup(this->mrl + 7);

    _x_mrl_unescape(this->disc_root);

    if (this->disc_root[strlen(this->disc_root)-1] != '/') {
      char *end = strrchr(this->disc_root, '/');
      if (end && end[1])
        if (sscanf(end, "/%d.%d", &title, &chapter) < 1)
          title = -1;
      *end = 0;
    }

  } else {
    return -1;
  }

  /* if title was not in mrl, find the main title */

  if (title < 0) {
    char *main_title = nav_find_main_title(this->disc_root);
    title = 0;
    if (main_title) {
      if (sscanf(main_title, "%d.mpls", &title) != 1)
        title = 0;
      lprintf("main title: %s (%d) \n", main_title, title);
    } else {
      LOGMSG("nav_find_main_title(%s) failed\n", this->disc_root);
    }
  }

  /* open libbluray */

  /* replace ~/ in keyfile path */
  char *keyfile = NULL;
  if (this->class->keyfile && !strncmp(this->class->keyfile, "~/", 2))
    if (asprintf(&keyfile, "%s/%s", xine_get_homedir(), this->class->keyfile + 2) < 0)
      keyfile = NULL;
  /* open */
  if (! (this->bdh = bd_open (this->disc_root, keyfile ?: this->class->keyfile))) {
    LOGMSG("bd_open(\'%s\') failed: %s\n", this->disc_root, strerror(errno));
    free(keyfile);
    return -1;
  }
  free(keyfile);
  lprintf("bd_open(\'%s\') OK\n", this->disc_root);

  /* select title */

  if (open_title(this, title) < 0)
    return -1;

  /* jump to chapter */

  if (chapter > 0) {
    uint32_t out_pkt = 0;
    NAV_CLIP *clip = nav_chapter_search(this->nav_title, chapter, &out_pkt);
    bluray_plugin_seek(&this->input_plugin, (off_t)(clip->title_pkt + out_pkt) * PKT_SIZE, SEEK_SET);

    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER, chapter);
  }

  return 1;
}

static input_plugin_t *bluray_class_get_instance (input_class_t *cls_gen, xine_stream_t *stream,
                                                  const char *mrl)
{
  bluray_input_plugin_t *this;

  lprintf("bluray_class_get_instance\n");

  if (strncasecmp (mrl, "bluray:", 7))
    return NULL;

  this = (bluray_input_plugin_t *) calloc(1, sizeof (bluray_input_plugin_t));

  this->stream = stream;
  this->class  = (bluray_input_class_t*)cls_gen;
  this->mrl    = strdup(mrl);

  this->input_plugin.open               = bluray_plugin_open;
  this->input_plugin.get_capabilities   = bluray_plugin_get_capabilities;
  this->input_plugin.read               = bluray_plugin_read;
  this->input_plugin.read_block         = bluray_plugin_read_block;
  this->input_plugin.seek               = bluray_plugin_seek;
  this->input_plugin.get_current_pos    = bluray_plugin_get_current_pos;
  this->input_plugin.get_length         = bluray_plugin_get_length;
  this->input_plugin.get_blocksize      = bluray_plugin_get_blocksize;
  this->input_plugin.get_mrl            = bluray_plugin_get_mrl;
  this->input_plugin.get_optional_data  = bluray_plugin_get_optional_data;
  this->input_plugin.dispose            = bluray_plugin_dispose;
  this->input_plugin.input_class        = cls_gen;

  return &this->input_plugin;
}

/*
 * plugin class
 */

static void mountpoint_change_cb(void *data, xine_cfg_entry_t *cfg)
{
  bluray_input_class_t *this = (bluray_input_class_t *) data;

  this->mountpoint = cfg->str_value;
}

static void device_change_cb(void *data, xine_cfg_entry_t *cfg)
{
  bluray_input_class_t *this = (bluray_input_class_t *) data;

  this->device = cfg->str_value;
}

static void keyfile_change_cb(void *data, xine_cfg_entry_t *cfg)
{
  bluray_input_class_t *this = (bluray_input_class_t *) data;

  this->keyfile = cfg->str_value;
}

static const char *bluray_class_get_description (input_class_t *this_gen)
{
  return _("BluRay input plugin");
}

static const char *bluray_class_get_identifier (input_class_t *this_gen)
{
  return "bluray";
}

static char **bluray_class_get_autoplay_list (input_class_t *this_gen, int *num_files)
{
  static char *autoplay_list[] = { "bluray:/", NULL };

  *num_files = 1;

  return autoplay_list;
}

static int bluray_class_eject_media (input_class_t *this_gen)
{
#if 0
  bluray_input_class_t *this = (bluray_input_class_t*) this_gen;

  return media_eject_media (this->xine, this->device);
#endif
  return 1;
}

static void bluray_class_dispose (input_class_t *this_gen)
{
  bluray_input_class_t *this   = (bluray_input_class_t *) this_gen;
  config_values_t      *config = this->xine->config;

  config->unregister_callback(config, "media.bluray.mountpoint");
  config->unregister_callback(config, "media.bluray.device");
  config->unregister_callback(config, "media.bluray.keyfile");

  free (this);
}

static void *bluray_init_plugin (xine_t *xine, void *data)
{
  config_values_t      *config = xine->config;
  bluray_input_class_t *this   = (bluray_input_class_t *) calloc(1, sizeof (bluray_input_class_t));

  this->xine = xine;

  this->input_class.get_instance       = bluray_class_get_instance;
  this->input_class.get_identifier     = bluray_class_get_identifier;
  this->input_class.get_description    = bluray_class_get_description;
  this->input_class.get_dir            = NULL;
  this->input_class.get_autoplay_list  = bluray_class_get_autoplay_list;
  this->input_class.dispose            = bluray_class_dispose;
  this->input_class.eject_media        = bluray_class_eject_media;

  this->mountpoint = config->register_filename(config, "media.bluray.mountpoint",
                                               "/mnt/bluray", XINE_CONFIG_STRING_IS_DIRECTORY_NAME,
                                               _("BluRay mount point"),
                                               _("Default mount location for BluRay discs."),
                                               0, mountpoint_change_cb, (void *) this);
  this->device = config->register_filename(config, "media.bluray.device",
                                           "/dev/dvd", XINE_CONFIG_STRING_IS_DIRECTORY_NAME,
                                           _("device used for BluRay playback"),
                                           _("The path to the device "
                                             "which you intend to use for playing BluRy discs."),
                                           0, device_change_cb, (void *) this);
  this->keyfile = config->register_filename(config, "media.bluray.keyfile",
                                            "~/.xine/aacskeys.bin", XINE_CONFIG_STRING_IS_DIRECTORY_NAME,
                                            _("AACS key file"),
                                            _("Location of libaacs key file."),
                                            0, keyfile_change_cb, (void *) this);

  return this;
}

/*
 * exported plugin catalog entry
 */

const plugin_info_t xine_plugin_info[] EXPORTED = {
  /* type, API, "name", version, special_info, init_function */
  { PLUGIN_INPUT | PLUGIN_MUST_PRELOAD, 17, "BLURAY", XINE_VERSION_CODE, NULL, bluray_init_plugin },
  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};
