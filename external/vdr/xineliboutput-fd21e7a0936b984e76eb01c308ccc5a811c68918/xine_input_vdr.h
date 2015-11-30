/*
 * xine_input_vdr.h:  
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_input_vdr.h,v 1.12 2012-03-10 23:51:10 phintuka Exp $
 *
 */

#ifndef __XINE_INPUT_VDR_H_
#define __XINE_INPUT_VDR_H_

#if defined __cplusplus
extern "C" {
#endif

#include <xine/input_plugin.h>

#include "xine_input_vdr_mrl.h"

struct input_plugin_s;
struct vdr_input_plugin_if_s;
struct osd_command_s;
struct frontend_s;

typedef struct vdr_input_plugin_funcs_s {
  /* VDR --> input plugin (only local mode) */
  int  (*push_input_write)  (struct vdr_input_plugin_if_s *, int, uint64_t, const char *, int);
  int  (*push_input_control)(struct vdr_input_plugin_if_s *, const char *);
  int  (*push_input_osd)    (struct vdr_input_plugin_if_s *, struct osd_command_s *);

  /* input plugin --> frontend (only local mode) */
  void (*xine_input_event)  (struct frontend_s *, const char *, const char *);

  /* input plugin --> frontend (remote mode) */
  int  (*intercept_osd)     (struct frontend_s *, struct osd_command_s *);

  /* input plugin --> frontend */
  void *(*fe_control)       (struct frontend_s *, const char *);
  struct frontend_s *fe_handle;

  /* frontend --> input plugin (remote mode) */
  int  (*post_vdr_event)    (struct vdr_input_plugin_if_s *, const char *);
} vdr_input_plugin_funcs_t;

typedef struct vdr_input_plugin_if_s {
  input_plugin_t           input_plugin;
  vdr_input_plugin_funcs_t f;
} vdr_input_plugin_if_t;

#define CONTROL_OK            0
#define CONTROL_UNKNOWN      -1 
#define CONTROL_PARAM_ERROR  -2 
#define CONTROL_DISCONNECTED -3

typedef struct grab_data_s {
  size_t  size;
  char   *data;
} grab_data_t;

#if defined __cplusplus
}
#endif


#endif /*__XINE_INPUT_VDR_H_*/

