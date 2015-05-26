/*
 * xine_post_audiochannel.c: xine post plugin
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_post_audiochannel.c,v 1.9 2012-01-09 07:36:52 phintuka Exp $
 *
 */

/*
 * Copyright (C) 2006 the xine project
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * $Id: 
 *
 * audio channel selection post plugin by Petri Hintukainen 01/09/2006
 * based on upmix.c
 *
 * Removes left or right channel from stereo audio track
 * and fills both channels with same data.
 * This is useful with some bi-lingual DVB transmissions where
 * two different languages are sent on same (stereo) audio track.
 *
 */

#include <xine/xine_internal.h>
#include <xine/post.h>


typedef struct audioch_parameters_s {
  int channel;
} audioch_parameters_t;

START_PARAM_DESCR(audioch_parameters_t)
PARAM_ITEM(POST_PARAM_TYPE_BOOL, channel, NULL, 0, 1, 0,
  "select channel (0=left, 1=right)")
END_PARAM_DESCR(audioch_param_descr)
     

typedef struct audioch_post_plugin_s
{
  post_plugin_t  post_plugin;
  xine_post_in_t parameter_input;

  /* Config */
  int channel; /* 0 - left, 1 - right */

  /* Data */
  int channels;

} audioch_post_plugin_t;


/*
 *    Port functions
 */
#if 1
static int audioch_port_open(xine_audio_port_t *port_gen, xine_stream_t *stream,
                             uint32_t bits, uint32_t rate, int mode) 
#else
static int audioch_port_open(xine_audio_port_t *port_gen, xine_stream_t *stream,
			     ao_format_t format)
#endif
{
  post_audio_port_t  *port = (post_audio_port_t *)port_gen;
  audioch_post_plugin_t *this = (audioch_post_plugin_t *)port->post;

  _x_post_rewire(&this->post_plugin);
  _x_post_inc_usage(port);
  
  port->stream = stream;
#if 1
  port->bits = bits;
  port->rate = rate;
  port->mode = mode;

  this->channels = _x_ao_mode2channels(mode);

  return port->original_port->open(port->original_port, stream, bits, rate, mode );
#else
  port->format = format;

  this->num_channels = _x_ao_mode2channels(format.mode);
  
  return port->original_port->open(port->original_port, stream, format);
#endif


}

static void audioch_port_put_buffer (xine_audio_port_t *port_gen, 
				     audio_buffer_t *buf, xine_stream_t *stream) 
{
  post_audio_port_t  *port = (post_audio_port_t *)port_gen;
  audioch_post_plugin_t *this = (audioch_post_plugin_t *)port->post;
  int i;

  if(this->channels == 2) {
#if 1
    int step = buf->format.bits / 8;
#else
    int step = sample_bytes_table[buf->format.sample_format];
#endif
    audio_buffer_t *newbuf = port->original_port->get_buffer(port->original_port); 
    newbuf->num_frames = buf->num_frames;
    newbuf->vpts = buf->vpts;
    newbuf->frame_header_count = buf->frame_header_count;
    newbuf->first_access_unit = buf->first_access_unit;
#if 1
    newbuf->format.bits = buf->format.bits;
    newbuf->format.rate = buf->format.rate;
    newbuf->format.mode = buf->format.mode;
#else
    newbuf->format = buf->format;
#endif
    _x_extra_info_merge( newbuf->extra_info, buf->extra_info);

    switch(step) {
    case 1:
      for(i=0; i<buf->num_frames; i++)
	newbuf->mem[i*2+1] = newbuf->mem[i*2] = buf->mem[i*2+this->channel];
      break;
    case 2:
      for(i=0; i<buf->num_frames; i++)
	((uint16_t*)newbuf->mem)[i*2+1] =
	  ((uint16_t*)newbuf->mem)[i*2] = ((uint16_t*)buf->mem)[i*2+this->channel];
      break;
    case 3:
      for(i=0; i<buf->num_frames*3; i+=3) {
	newbuf->mem[i*2+0] =  newbuf->mem[i*2+3] = buf->mem[i*2+0+3*this->channel];
	newbuf->mem[i*2+1] =  newbuf->mem[i*2+4] = buf->mem[i*2+1+3*this->channel];
	newbuf->mem[i*2+2] =  newbuf->mem[i*2+5] = buf->mem[i*2+2+3*this->channel];
      }
      break;
    case 4:
      for(i=0; i<buf->num_frames; i++)
	((uint32_t*)newbuf->mem)[i*2+1] =
	  ((uint32_t*)newbuf->mem)[i*2] = ((uint32_t*)buf->mem)[i*2+this->channel];
      break;
    default: /* ??? */
      memcpy(newbuf->mem, buf->mem, buf->num_frames*2*buf->format.bits);
      break;
    }

    /* pass data to original port */
    port->original_port->put_buffer(port->original_port, newbuf, stream );

    /* free data from origial buffer */
    buf->num_frames=0; /* UNDOCUMENTED, but hey, it works! Force old audio_out buffer free. */
  }

  port->original_port->put_buffer(port->original_port, buf, stream );  
}

/*
 *    Parameter functions
 */

static xine_post_api_descr_t *audioch_get_param_descr(void)
{
  return &audioch_param_descr;
}

static int audioch_set_parameters(xine_post_t *this_gen, void *param_gen)
{
  audioch_post_plugin_t *this = (audioch_post_plugin_t *)this_gen;
  audioch_parameters_t *param = (audioch_parameters_t *)param_gen;

  this->channel = param->channel;
  return 1;
}

static int audioch_get_parameters(xine_post_t *this_gen, void *param_gen)
{
  audioch_post_plugin_t *this = (audioch_post_plugin_t *)this_gen;
  audioch_parameters_t *param = (audioch_parameters_t *)param_gen;

  param->channel = this->channel;
  return 1;
}

static char *audioch_get_help(void) {
  return _("The audiochannel plugin is meant to take stereo audio and \n"
           "remove left or right channel by replacing both channels \n"
           "with the same data. \n"
	   "\n"
           "Parameters \n"
           "  channel:  Select channel (left=0, right=1) \n"
           "\n"
         );
}


/*
 *    Open / Close
 */

static void audioch_dispose(post_plugin_t *this_gen)
{
  if (_x_post_dispose(this_gen)) 
    free(this_gen);
}

static post_plugin_t *audioch_open_plugin(post_class_t *class_gen, 
					  int inputs,
					  xine_audio_port_t **audio_target,
					  xine_video_port_t **video_target)
{
  audioch_post_plugin_t *this = calloc(1, sizeof(audioch_post_plugin_t));
  post_in_t             *input;
  post_out_t            *output;
  post_audio_port_t     *port;
  xine_post_in_t        *input_param;

  static xine_post_api_t post_api =
    { audioch_set_parameters,  audioch_get_parameters, 
      audioch_get_param_descr, audioch_get_help };

  if (!this || !audio_target || !audio_target[0] ) {
    free(this);
    return NULL;
  }

  _x_post_init(&this->post_plugin, 1, 0);

  port = _x_post_intercept_audio_port(&this->post_plugin, 
				      audio_target[ 0 ], 
				      &input, &output);

  port->new_port.open  = audioch_port_open;
  port->new_port.put_buffer = audioch_port_put_buffer;

  input->xine_in.name   = "audio in";
  output->xine_out.name = "audio out";

  this->post_plugin.xine_post.audio_input[ 0 ] = &port->new_port;
  this->post_plugin.dispose = audioch_dispose;

  input_param       = &this->parameter_input;
  input_param->name = "parameters";
  input_param->type = XINE_POST_DATA_PARAMETERS;
  input_param->data = &post_api;
#if XINE_VERSION_CODE >= 10102
  xine_list_push_back(this->post_plugin.input, input_param);
#else
  xine_list_append_content(this->post_plugin.input, input_param);
#endif

  this->channel = 0;

  this->channels = 0;

  return &this->post_plugin;
}

/*
 *    Plugin class
 */

#if POST_PLUGIN_IFACE_VERSION < 10
static char *audioch_get_identifier(post_class_t *class_gen)
{
  return "audiochannel";
}

static char *audioch_get_description(post_class_t *class_gen)
{
  return "Select audio channel";
}

static void audioch_class_dispose(post_class_t *class_gen)
{
  free(class_gen);
}
#endif

static void *audioch_init_plugin(xine_t *xine, void *data)
{
  post_class_t *class = calloc(1, sizeof(post_class_t));
  
  if(!class)
    return NULL;

  class->open_plugin     = audioch_open_plugin;
#if POST_PLUGIN_IFACE_VERSION < 10
  class->get_identifier  = audioch_get_identifier;
  class->get_description = audioch_get_description;
  class->dispose         = audioch_class_dispose;
#else
  class->identifier      = "audiochannel";
  class->description     = N_("Select audio channel");
  class->dispose         = default_post_class_dispose;
#endif

  return class;
}

static post_info_t audioch_info = { XINE_POST_TYPE_AUDIO_FILTER };

#ifndef NO_INFO_EXPORT
plugin_info_t xine_plugin_info[] __attribute__((visibility("default"))) =
{
  /* type, API, "name", version, special_info, init_function */
  { PLUGIN_POST, POST_PLUGIN_IFACE_VERSION, "audiochannel", XINE_VERSION_CODE, &audioch_info, &audioch_init_plugin },
  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};
#endif
