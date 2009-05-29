/*
 * xine_input_vdr.h:  
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_input_vdr.h,v 1.5 2008/03/31 20:36:03 phintuka Exp $
 *
 */

#ifndef __XINE_INPUT_VDR_H_
#define __XINE_INPUT_VDR_H_

#if defined __cplusplus
extern "C" {
#endif

struct input_plugin_s;
struct osd_command_s;
struct frontend_s;

typedef struct vdr_input_plugin_funcs_s {
  /* VDR --> input plugin (only local mode) */
  int  (*push_input_write)(struct input_plugin_s *, const char *, int);
  int  (*push_input_control)(struct input_plugin_s *, const char *);
  int  (*push_input_osd)(struct input_plugin_s *, struct osd_command_s *);
  /* input plugin --> frontend (only local mode) */
  void (*xine_input_event)(const char *, const char *);
  /* input plugin --> frontend (remote mode) */
  int  (*intercept_osd)(struct frontend_s *, struct osd_command_s *);
  /* input plugin --> frontend */
  void *(*fe_control)(struct frontend_s *, const char *);
  struct frontend_s *fe_handle;
  /* frontend --> input plugin (remote mode) */
  int  (*input_control)(struct input_plugin_s *, const char *, const char *, int, int);
} vdr_input_plugin_funcs_t;

#define CONTROL_OK            0
#define CONTROL_UNKNOWN      -1 
#define CONTROL_PARAM_ERROR  -2 
#define CONTROL_DISCONNECTED -3

typedef struct grab_data_s {
  int size;
  char *data;
} grab_data_t;

#if defined __cplusplus
}
#endif


#endif /*__XINE_INPUT_VDR_H_*/

