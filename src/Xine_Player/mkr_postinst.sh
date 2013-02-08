#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

/usr/pluto/bin/alsaconf-noninteractive

video_driver=xv;
grep -q 'Driver.*i810' /etc/X11/xorg.conf && video_driver=xshm;

xine_conf="
#
# xine config file
#
.version:1

# Entries which are still set to their default values are commented out.
# Remove the '#' at the beginning of the line, if you want to change them.

# Enable deinterlacing by default
# bool, default: 0
#gui.deinterlace_by_default:0

# Configuration experience level
# { Beginner  Advanced  Expert  Master of the known universe }, default: 0
gui.experience_level:Master of the known universe

# Enable OSD support
# bool, default: 1
#gui.osd_enabled:1

# Dismiss OSD time (s)
# numeric, default: 3
#gui.osd_timeout:3

# Ask user for playback with unsupported codec
# bool, default: 0
#gui.play_anyway:0

# Automatically reload old playlist
# bool, default: 0
#gui.playlist_auto_reload:0

# Audio visualization plugin
# { fftgraph  fftscope  oscope  goom }, default: 0
gui.post_audio_plugin:goom

# gui skin theme
# { xinetic }, default: 0
#gui.skin:xinetic

# Change xine's behavior for unexperienced user
# bool, default: 1
#gui.smart_mode:1

# Snapshot location
# string, default: /root
#gui.snapshotdir:/root

# Display splash screen
# bool, default: 1
#gui.splash:1

# Subtitle autoloading
# bool, default: 1
gui.subtitle_autoload:1

# Visual animation style
# { None  Post Plugin  Stream Animation }, default: 1
#gui.visual_anim:Post Plugin

# Windows stacking (more)
# bool, default: 0
#gui.always_layer_above:0

# Audio mixer control method
# { Sound card  Software }, default: 0
#gui.audio_mixer_method:Sound card

# Visiblility behavior of panel
# bool, default: 0
#gui.auto_panel_visibility:0

# Visibility behavior of output window
# bool, default: 0
#gui.auto_video_output_visibility:0

# Deinterlace plugin.
# string, default: tvtime:method=LinearBlend,cheap_mode=1,pulldown=0,use_progressive_frame_flag=1
#gui.deinterlace_plugin:tvtime:method=LinearBlend,cheap_mode=1,pulldown=0,use_progressive_frame_flag=1

# Event sender behavior
# bool, default: 1
#gui.eventer_sticky:1

# Windows stacking
# bool, default: 0
#gui.layer_above:0

# Use unscaled OSD
# bool, default: 1
#gui.osd_use_unscaled:1

# Screensaver wakeup
# numeric, default: 10
#gui.screensaver_timeout:10

# Menu shortcut style
# { Windows style  Emacs style }, default: 0
#gui.shortcut_style:Windows style

# Stream information
# bool, default: 0
#gui.sinfo_auto_update:0

# Skin Server Url
# string, default: http://xine.sourceforge.net/skins/skins.slx
#gui.skin_server_url:http://xine.sourceforge.net/skins/skins.slx

# Chapter hopping
# bool, default: 1
#gui.skip_by_chapter:1

# New stream sizes resize output window
# bool, default: 1
#gui.stream_resize_window:1

# tips timeout (ms)
# numeric, default: 500
#gui.tips_timeout:500

# gui tips visibility
# bool, default: 1
#gui.tips_visible:1

# Synchronized X protocol (debug)
# bool, default: 0
#gui.xsynchronize:0

# Double size for small streams (require stream_resize_window)
# bool, default: 0
#gui.zoom_small_stream:0

# Logo mrl
# string, default: file:/usr/share/xine/skins/xine-ui_logo.mpv
#gui.logo_mrl:file:/usr/share/xine/skins/xine-ui_logo.mpv

# use XVidModeExtension when switching to fullscreen
# bool, default: 0
#gui.use_xvidext:0

# Amplification level
# [0..200], default: 100
#gui.amp_level:100

# gui panel visibility
# bool, default: 1
#gui.panel_visible:1

# numeric, default: 200
gui.panel_x:53

# numeric, default: 100
gui.panel_y:313

# brightness value
# [0..65535], default: 32800
#gui.vo_brightness:32800

# contrast value
# [0..65535], default: 32772
#gui.vo_contrast:32772

# saturation value
# [0..65535], default: 32772
#gui.vo_saturation:32772

gui.control_x:262

gui.control_y:182

gui.eventer_x:653

gui.eventer_y:680

gui.mrl_browser_x:435

gui.mrl_browser_y:527

gui.setup_x:406

gui.setup_y:221

gui.sinfos_x:400

gui.sinfos_y:408

gui.tvset_x:246

gui.tvset_y:193

# speaker arrangement
# { Mono 1.0  Stereo 2.0  Headphones 2.0  Stereo 2.1  Surround 3.0  Surround 4.0  Surround 4.1  Surround 5.0  Surround 5.1  Surround 6.0  Surround 6.1  Surround 7.1  Pass Through }, default: 1
#audio.speaker_arrangement:Stereo 2.0

# audio driver to use
# { auto  null  alsa  oss  arts  esd  none  file }, default: 0
audio.driver:auto

# OSS audio mixer number, -1 for none
# numeric, default: -1
#audio.mixer_number:-1

# startup audio volume
# [0..100], default: 50
#audio.mixer_volume:50

# OSS audio device name
# { auto  /dev/dsp  /dev/sound/dsp }, default: 0
#audio.oss_device_name:auto

# OSS audio device number, -1 for none
# numeric, default: -1
#audio.oss_device_number:-1

# offset for digital passthrough
# numeric, default: 0
#audio.passthrough_offset:0

# restore volume level at startup
# bool, default: 0
#audio.remember_volume:0

# method to sync audio and video
# { metronom feedback  resample }, default: 0
#audio.av_sync_method:metronom feedback

# always resample to this rate (0 to disable)
# numeric, default: 0
#audio.force_rate:0

# number of audio buffers
# numeric, default: 230
#audio.num_buffers:230

# a/v sync method to use by OSS
# { auto  getodelay  getoptr  softsync  probebuffer }, default: 0
#audio.oss_sync_method:auto

# enable resampling
# { auto  off  on }, default: 0
#audio.resample_mode:auto

audio.alsa_front_device:default

audio.alsa_mixer_name:Master

# deinterlace method (deprecated)
# { none  bob  weave  greedy  onefield  onefield_xv  linearblend }, default: 4
#video.deinterlace_method:onefield

# disable all video scaling
# bool, default: 0
#video.disable_scaling:0

# video driver to use
# { auto  aadxr3  dxr3  xv  xshm  aa  none  vidixfb  vidix  fb }, default: 0
video.driver:$video_driver

# horizontal image position in the output window
# [0..100], default: 50
#video.horizontal_position:50

# vertical image position in the output window
# [0..100], default: 50
#video.vertical_position:50

# pitch alignment workaround
# bool, default: 0
#video.xv_pitch_alignment:0

# enable double buffering
# bool, default: 1
#video.XV_DOUBLE_BUFFER:1

# number of video buffers
# numeric, default: 500
#video.num_buffers:500

# percentage of discarded frames to tolerate
# numeric, default: 10
#video.warn_discarded_threshold:10

# percentage of skipped frames to tolerate
# numeric, default: 10
#video.warn_skipped_threshold:10

# autopaint colour key
# bool, default: 0
#video.xv_autopaint_colorkey:0

# video overlay colour key
# [0..16777215], default: 66046
#video.xv_colorkey:66046

# framebuffer device name
# string, default: 
#video.fb_device:

# DXR3 device number
# numeric, default: 0
#dxr3.device_number:0

# swap odd and even lines
# bool, default: 0
#dxr3.enc_swap_fields:0

# add black bars to correct aspect ratio
# bool, default: 1
#dxr3.enc_add_bars:1

# use smooth play mode for mpeg encoder playback
# bool, default: 1
#dxr3.enc_alt_play_mode:1

# Remember last DVB channel watched
# bool, default: 1
#input.dvb_last_channel_enable:1

# display DVB channel name
# bool, default: 0
#input.dvbdisplaychan:0

# default language for DVD playback
# string, default: en
#input.dvd_language:en

# region the DVD player claims to be in (1 to 8)
# numeric, default: 1
#input.dvd_region:1

# file browsing start location
# string, default: /home/work/pluto-svn/trunk/bin

# network bandwidth
# { 14.4 Kbps (Modem)  19.2 Kbps (Modem)  28.8 Kbps (Modem)  33.6 Kbps (Modem)  34.4 Kbps (Modem)  57.6 Kbps (Modem)  115.2 Kbps (ISDN)  262.2 Kbps (Cable/DSL)  393.2 Kbps (Cable/DSL)  524.3 Kbps (Cable/DSL)  1.5 Mbps (T1)  10.5 Mbps (LAN) }, default: 10
#input.mms_network_bandwidth:1.5 Mbps (T1)

# device used for CD audio
# string, default: /dev/cdrom
#input.cdda_device:/dev/cdrom

# query CDDB
# bool, default: 1
#input.cdda_use_cddb:1

# slow down disc drive to this speed factor
# numeric, default: 4
#input.drive_slowdown:4

# device used for DVD playback
# string, default: /dev/dvd
#input.dvd_device:/dev/dvd

# raw device set up for DVD access
# string, default: /dev/rdvd
#input.dvd_raw_device:/dev/rdvd

# read-ahead caching
# bool, default: 1
#input.dvd_use_readahead:1

# list hidden files
# bool, default: 0
#input.file_hidden_files:0

# Domains, where to ignore the HTTP proxy
# string, default: 
#input.http_no_proxy:

# HTTP proxy host
# string, default: 
#input.http_proxy_host:

# HTTP proxy password
# string, default: 
#input.http_proxy_password:

# HTTP proxy port
# numeric, default: 80
#input.http_proxy_port:80

# HTTP proxy username
# string, default: 
#input.http_proxy_user:

# device used for WinTV-PVR 250/350 (pvr plugin)
# string, default: /dev/video0
#input.pvr_device:/dev/video0

# v4l radio device
# string, default: /dev/v4l/radio0
#input.v4l_radio_device_path:/dev/v4l/radio0

# v4l video device
# string, default: /dev/v4l/video0
#input.v4l_video_device_path:/dev/v4l/video0

# device used for VCD playback
# string, default: /dev/cdrom
#input.vcd_device:/dev/cdrom

# Remember last DVB channel watched
# numeric, default: -1
#input.dvb_last_channel_watched:-1

# CSS decryption method
# { key  disc  title }, default: 0
#input.css_decryption_method:key

# unit for seeking
# { seek in program chain  seek in program }, default: 0
#input.dvd_seek_behaviour:seek in program chain

# unit for the skip action
# { skip program  skip part  skip title }, default: 0
#input.dvd_skip_behaviour:skip program

# MMS protocol
# { auto  TCP  HTTP }, default: 0
#input.mms_protocol:auto

# CDDB cache directory
# string, default: /root/.xine/cddbcache
#input.cdda_cddb_cachedir:/root/.xine/cddbcache

# CDDB server port
# numeric, default: 8880
#input.cdda_cddb_port:8880

# CDDB server name
# string, default: freedb.freedb.org
#input.cdda_cddb_server:freedb.freedb.org

# path to the title key cache
# string, default: /root/.dvdcss/
input.css_cache_path:/home/.dvdcss/

# use A/52 dynamic range compression
# bool, default: 0
#codec.a52_dynrng:0

# downmix audio to 2 channel surround stereo
# bool, default: 0
#codec.a52_surround_downmix:0

# A/52 volume
# [0..200], default: 100
#codec.a52_level:100

# MPEG-4 postprocessing quality
# [0..6], default: 3
#codec.ffmpeg_pp_quality:3

# path to RealPlayer codecs
# string, default: unknown
codec.real_codecs_path:/usr/lib/win32

# path to Win32 codecs
# string, default: /usr/lib/win32
#codec.win32_path:/usr/lib/win32

# frames per second to generate
# numeric, default: 10
#post.goom_fps:10

# goom image height
# numeric, default: 240
#post.goom_height:240

# goom image width
# numeric, default: 320
#post.goom_width:320

# colorspace conversion method
# { Fast but not photorealistic  Slow but looks better }, default: 0
#post.goom_csc_method:Fast but not photorealistic

# priority for a/52 decoder
# numeric, default: 0
#decoder.a/52_priority:0

# priority for bitplane decoder
# numeric, default: 0
#decoder.bitplane_priority:0

# priority for dts decoder
# numeric, default: 0
#decoder.dts_priority:0

# priority for dvaudio decoder
# numeric, default: 0
#decoder.dvaudio_priority:0

# priority for dxr3-mpeg2 decoder
# numeric, default: 0
#decoder.dxr3-mpeg2_priority:0

# priority for dxr3-spudec decoder
# numeric, default: 0
#decoder.dxr3-spudec_priority:0

# priority for faad decoder
# numeric, default: 0
#decoder.faad_priority:0

# priority for ffmpeg-wmv8 decoder
# numeric, default: 0
#decoder.ffmpeg-wmv8_priority:0

# priority for ffmpegaudio decoder
# numeric, default: 0
#decoder.ffmpegaudio_priority:0

# priority for ffmpegvideo decoder
# numeric, default: 0
#decoder.ffmpegvideo_priority:0

# priority for flacdec decoder
# numeric, default: 0
#decoder.flacdec_priority:0

# priority for gsm610 decoder
# numeric, default: 0
#decoder.gsm610_priority:0

# priority for image decoder
# numeric, default: 0
#decoder.image_priority:0

# priority for mad decoder
# numeric, default: 0
#decoder.mad_priority:0

# priority for mpeg2 decoder
# numeric, default: 0
#decoder.mpeg2_priority:0

# priority for nsf decoder
# numeric, default: 0
#decoder.nsf_priority:0

# priority for pcm decoder
# numeric, default: 0
#decoder.pcm_priority:0

# priority for qta decoder
# numeric, default: 0
#decoder.qta_priority:0

# priority for qtv decoder
# numeric, default: 0
#decoder.qtv_priority:0

# priority for real decoder
# numeric, default: 0
#decoder.real_priority:0

# priority for realadec decoder
# numeric, default: 0
#decoder.realadec_priority:0

# priority for rgb decoder
# numeric, default: 0
#decoder.rgb_priority:0

# priority for speex decoder
# numeric, default: 0
#decoder.speex_priority:0

# priority for spucc decoder
# numeric, default: 0
#decoder.spucc_priority:0

# priority for spucmml decoder
# numeric, default: 0
#decoder.spucmml_priority:0

# priority for spudec decoder
# numeric, default: 0
#decoder.spudec_priority:0

# priority for sputext decoder
# numeric, default: 0
#decoder.sputext_priority:0

# priority for vorbis decoder
# numeric, default: 0
#decoder.vorbis_priority:0

# priority for win32a decoder
# numeric, default: 0
#decoder.win32a_priority:0

# priority for win32v decoder
# numeric, default: 0
#decoder.win32v_priority:0

# priority for yuv decoder
# numeric, default: 0
#decoder.yuv_priority:0

# subtitle size
# { tiny  small  normal  large  very large  huge }, default: 1
misc.spu_subtitle_size:large

# subtitle vertical offset
# numeric, default: 0
#misc.spu_vertical_offset:0

# palette (foreground-border-background) to use for subtitles and OSD
# { white-black-transparent  white-none-transparent  white-none-translucid  yellow-black-transparent }, default: 0
#misc.osd_text_palette:white-black-transparent

# font for subtitles
# string, default: sans
#misc.spu_font:sans

# encoding of the subtitles
# string, default: iso-8859-1
#misc.spu_src_encoding:iso-8859-1

# use unscaled OSD if possible
# bool, default: 1
#misc.spu_use_unscaled_osd:1

# media format detection strategy
# { default  reverse  content  extension }, default: 0
#misc.demux_strategy:default

# memcopy method used by xine
# { probe  libc  kernel  mmx  mmxext  sse }, default: 0
misc.memcpy_method:sse

# default duration of subtitle display in seconds
# numeric, default: 4
#misc.sub_timeout:4

# allow implicit changes to the configuration (e.g. by MRL)
# bool, default: 0
#misc.implicit_config:0

# directory for saving streams
# string, default: 
#misc.save_dir:

# automatically advance track/entry
# bool, default: 1
#vcd.autoadvance:1

# default type to use on VCD autoplay
# { track  entry  segment  playlist }, default: 3
#vcd.autoplay:playlist

# position slider range
# { auto  track  entry }, default: 0
#vcd.length_reporting:auto

# show 'rejected' LIDs
# bool, default: 0
#vcd.show_rejected:0

# format string for stream comment field
# string, default: %P - Track %T
#vcd.comment_format:%P - Track %T

# debug flag mask
# numeric, default: 0
#vcd.debug:0

# default CD drive used for VCD when none given
# string, default: 
#vcd.default_device:

# format string for display banner
# string, default: %F - %I %N%L%S, disk %c of %C - %v %A
#vcd.title_format:%F - %I %N%L%S, disk %c of %C - %v %A
"

mkdir -p /etc/pluto
echo "$xine_conf" >/etc/pluto/xine.conf

if [[ -d /root/.dvdcss ]]; then
	mkdir -p /home/.dvdcss
	mv /root/.dvdcss/* /home/.dvdcss/* 2>/dev/null || :
	rm -rf /root/.dvdcss
fi
