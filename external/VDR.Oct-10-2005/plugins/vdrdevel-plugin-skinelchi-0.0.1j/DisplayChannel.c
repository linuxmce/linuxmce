#ifdef VIDEOSTATUS
#include <sys/ioctl.h>
#endif

#include "DisplayChannel.h"
#include "setup.h"
#include <vdr/menu.h>

#include "symbols/audio.xpm"
#include "symbols/dolbydigital.xpm"
#include "symbols/encrypted.xpm"
#include "symbols/radio.xpm"
#include "symbols/recording.xpm"
#include "symbols/teletext.xpm"
#include "symbols/vps.xpm"

#define Gap          5

cBitmap cSkinElchiDisplayChannel::bmVps(vps_xpm);
cBitmap cSkinElchiDisplayChannel::bmTeletext(teletext_xpm);
cBitmap cSkinElchiDisplayChannel::bmRadio(radio_xpm);
cBitmap cSkinElchiDisplayChannel::bmAudio(audio_xpm);
cBitmap cSkinElchiDisplayChannel::bmDolbyDigital(dolbydigital_xpm);
cBitmap cSkinElchiDisplayChannel::bmEncrypted(encrypted_xpm);
cBitmap cSkinElchiDisplayChannel::bmRecording(recording_xpm);

#ifdef VIDEOSTATUS
#include <linux/dvb/video.h>
extern video_status videostatus;
extern int fd_video;
extern video_size_t vs;
#define DEV_DVB_VIDEO     "/dev/dvb/adapter%d/video%d"
#endif

// --- cSkinElchiDisplayChannel --------------------------------------------

cSkinElchiDisplayChannel::cSkinElchiDisplayChannel(bool WithInfo)
{
#ifdef VIDEOSTATUS
//  fd_video = - 1;
  if (SkinElchiSetup.videostatus && cDevice::PrimaryDevice()->HasDecoder()) {
    if (fd_video != -1) {
        close(fd_video);
        fd_video = -1;
        }
    char *dev = NULL;
    asprintf(&dev, DEV_DVB_VIDEO, cDevice::PrimaryDevice()->CardIndex(), 0);
    fd_video = open(dev, O_RDONLY | O_NONBLOCK);
    free(dev);
    }
#endif
  const cFont *smallfont = cFont::GetFont(fontSml);
  const cFont *font = cFont::GetFont(fontOsd);
  present = NULL;
  track = NULL;
  oldpresentvps = -1;
  lastSeen = -1;
  withInfo = WithInfo;
  lineHeight = font->Height();
  bg = Theme.Color(clrBackground);
  message = false;
  if (SkinElchiSetup.logo) {
    logox = 64;
    logoy = 48;
    }
  else
    logox = logoy = 0;

  x0 = 0;
  x1 = SkinElchiSetup.logo ? 16 : 0;
  x2 = x1 + lineHeight - lineHeight / 2;
  x3 = logox + lineHeight - lineHeight / 2;
  x4 = x2 + font->Width("00:00");
  x5 = x4 + lineHeight / 2 + Gap;
  x6 = x3 + font->Width("00000-") + smallfont->Width("_000 00.00 00:00");
  x7 = x6 + lineHeight - lineHeight / 2;
  x11 = Setup.OSDWidth;
  x8 = x11 - lineHeight;
  x10 = x8 + lineHeight / 2;
  x9 = x10 - lineHeight - bmRecording.Width() - 3 - bmEncrypted.Width() - 3 - bmDolbyDigital.Width() - 3 - bmAudio.Width() - 3 - bmTeletext.Width() - 3 - bmVps.Width() - 3;
  y0 = 0;
  y1 = SkinElchiSetup.logo ? 16 : 0;
  y3 = y1 + lineHeight;
  y2 = y3 - lineHeight / 2;
  y4 = y3 + 3;
  y6 = y4 + lineHeight;
  y5 = y6 - lineHeight / 2;
  y9 = y6 + (withInfo ? lineHeight * 4 : 0);
  y7 = y9 - lineHeight;
  y8 = y9 - lineHeight / 2;

  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + (Setup.ChannelInfoPos ? 0 : Setup.OSDHeight - y9));

  if (withInfo) {
    tArea Areas[] = { { 0, 0, x11 - 1, y9 - 1, 8 } };
    if ((osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) && (SkinElchiSetup.singlearea)) {
    	osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (SkinElchiSetup.logo) {
	    for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++) {
		osd->GetBitmap(0)->SetColor(i,SkinElchiSetup.ThemeColors[i]);
		}
    	    osd->DrawRectangle(logox, y0, x11 - 1, y1 - 1, clrTransparent);
    	    osd->DrawRectangle(x0, logoy, x1 - 1, y9 - 1, clrTransparent);
    	    }
        }
    else {
        if (SkinElchiSetup.logo) {
    	    tArea Areas[] = {{ logox, y1, x11 - 1, y6 - 1, 4 },
			    { x1, logoy, logox - 1, y6 - 1, 4 }, 
			    { x0, y0, logox - 1, logoy - 1, 8 },
			    { x1, y6, x11 - 1, y9 - 1, 4 },};
    	    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        }
	else {
    	    tArea Areas[] = { { x0, y0, x11 - 1, y6 - 1, 4 },
			    { x0, y6, x11 - 1, y9 - 1, 4 }, };
    	    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
	}
    }

    if (SkinElchiSetup.logo) 
	osd->DrawRectangle(logox, y1, x3 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
    else {
	osd->DrawRectangle(logox, y1, x3 - 1, y2 - 1, clrTransparent);
	osd->DrawEllipse  (logox, y1, x3 - 1, y2 - 1, Theme.Color(clrChannelDateBg), 2);
	osd->DrawRectangle(logox, y2, x3 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
	}

    osd->DrawRectangle(x3, y1, x6 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
    osd->DrawRectangle(x6, y1, x10 - 1, y3 - 1, bg);
    osd->DrawEllipse  (x6, y1, x7 - 1, y3 - 1, Theme.Color(clrChannelDateBg), 5);

    osd->DrawRectangle(x10, y1, x11 - 1, y2 - 1, clrTransparent);
    osd->DrawEllipse  (x10, y1, x11 - 1, y2 - 1, bg, 1);
    osd->DrawRectangle(x10, y2, x11 - 1, y3 - 1, bg);

    osd->DrawRectangle(logox, y3, x11 - 1, y4 - 1, bg);

    if (SkinElchiSetup.logo) 
	osd->DrawRectangle(x1, logoy, logox - 1, y6 - 1, Theme.Color(clrChannelNameBg));

    osd->DrawRectangle(logox, y4, x3 - 1, y6 - 1, Theme.Color(clrChannelNameBg));

    osd->DrawRectangle(x3, y4, x9 - 1, y6 - 1, Theme.Color(clrChannelNameBg));

    osd->DrawRectangle(x9, y4, x11 - 1, y6 - 1, bg);
    osd->DrawEllipse  (x9, y4, x9 + lineHeight / 2 - 1, y6 - 1, Theme.Color(clrChannelNameBg), 5);

    osd->DrawRectangle(x1, y6, x4 - 1, y8 - 1, Theme.Color(clrChannelEpgTimeBg));
    osd->DrawRectangle(x4, y6, x11 - 1, y8 - 1, Theme.Color(clrChannelEpgTitleBg));
  
    osd->DrawRectangle(x1, y8, x2 - 1, y9 - 1, clrTransparent);
    osd->DrawEllipse  (x1, y8, x2 - 1, y9 - 1, Theme.Color(clrChannelEpgTimeBg), 3);

    osd->DrawRectangle(x2, y8, x4 - 1, y9 - 1, Theme.Color(clrChannelEpgTimeBg));
    osd->DrawRectangle(x4, y8, x10 - 1, y9 - 1, Theme.Color(clrChannelEpgTitleBg));

    osd->DrawRectangle(x10, y8, x11 - 1, y9 - 1, clrTransparent);
    osd->DrawEllipse  (x10, y8, x11 - 1, y9 - 1, Theme.Color(clrChannelEpgTitleBg), 4);
    }
  else {
    tArea Areas[] = { { 0, 0, x11 - 1, y9 - 1, 8 } };
    if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
    	osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (SkinElchiSetup.logo) {
	    for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++) {
		osd->GetBitmap(0)->SetColor(i,SkinElchiSetup.ThemeColors[i]);
		}
    	    osd->DrawRectangle(logox, y0, x11 - 1, y1 - 1, clrTransparent);
    	    osd->DrawRectangle(x0, logoy, x1 - 1, y9 - 1, clrTransparent);
    	    }
        }
    else {
	if (SkinElchiSetup.logo) {
    	    tArea Areas[] ={{ logox, y1, x11 - 1, y9 - 1, 4 },
			    { x1, logoy, logox - 1, y9 - 1, 4 }, 
			    { x0, y0, logox - 1, logoy - 1, 8 },};
    	    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));

    	    osd->DrawRectangle(logox, y0, x11 - 1, y1 - 1, clrTransparent);
    	    osd->DrawRectangle(x0, logoy, x1 - 1, y9 - 1, clrTransparent);
        }
	else {
    	    tArea Areas[] = { { x0, y0, x11 - 1, y9 - 1, 4 } };
    	    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
	}
    }

    if (SkinElchiSetup.logo) {
        osd->DrawRectangle(logox, y1, x3 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
        }
    else {
        osd->DrawRectangle(logox, y1, x3 - 1, y2 - 1, clrTransparent);
        osd->DrawEllipse  (logox, y1, x3 - 1, y2 - 1, Theme.Color(clrChannelDateBg), 2);
        osd->DrawRectangle(logox, y2, x3 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
    }
    osd->DrawRectangle(x3, y1, x6 - 1, y3 - 1, Theme.Color(clrChannelDateBg));
    osd->DrawRectangle(x6, y1, x10 - 1, y3 - 1, bg);
    osd->DrawEllipse  (x6, y1, x7 - 1, y3 - 1, Theme.Color(clrChannelDateBg), 5);

    osd->DrawRectangle(x10, y1, x11 - 1, y2 - 1, clrTransparent);
    osd->DrawEllipse  (x10, y1, x11 - 1, y2 - 1, bg, 1);
    osd->DrawRectangle(x10, y2, x11 - 1, y3 - 1, bg);

    osd->DrawRectangle(logox, y3, x11 - 1, y4 - 1, bg);

    if (SkinElchiSetup.logo) {
	osd->DrawRectangle(x1, logoy, logox - 1, y5 - 1, Theme.Color(clrChannelNameBg));
        osd->DrawRectangle(x2, y5, logox - 1, y6 - 1, Theme.Color(clrChannelNameBg));
        osd->DrawRectangle(logox, y4, x3 - 1, y6 - 1, Theme.Color(clrChannelNameBg));
    }
    else
        osd->DrawRectangle(logox, y4, x3 - 1, y5 - 1, Theme.Color(clrChannelNameBg));

    osd->DrawRectangle(x1, y5, x2 - 1, y6 - 1, clrTransparent);
    osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, Theme.Color(clrChannelNameBg), 3);

    osd->DrawRectangle(x3, y4, x9 - 1, y6 - 1, Theme.Color(clrChannelNameBg));

    osd->DrawRectangle(x9, y4, x11 - 1, y6 - 1, bg);
    osd->DrawEllipse  (x9, y4, x9 + lineHeight / 2 - 1, y6 - 1, Theme.Color(clrChannelNameBg), 5);

    osd->DrawRectangle(x9 + lineHeight / 2, y4, x11 - 1, y5 - 1, bg);
    osd->DrawRectangle(x9 + lineHeight / 2, y5, x10 - 1, y6 - 1, bg);

    osd->DrawRectangle(x10, y5, x11 - 1, y6 - 1, clrTransparent);
    osd->DrawEllipse  (x10, y5, x11 - 1, y6 - 1, bg, 4);

    }

    if (SkinElchiSetup.logo) {
        osd->DrawRectangle(x0, y0, logox - 1, logoy - 1, Theme.Color(clrChannelNameBg));
        osd->DrawRectangle(logox, y0 + 2, logox + 2 - 1, logoy + 2 - 1, bg);
        osd->DrawRectangle(x1, logoy, logox - 1, logoy + 2 - 1, bg);
    }
}

cSkinElchiDisplayChannel::~cSkinElchiDisplayChannel()
{
#ifdef VIDEOSTATUS
  if (fd_video != -1) {
    close(fd_video);
    fd_video = -1;
    }
#endif
  delete osd;
}

void cSkinElchiDisplayChannel::SetChannel(const cChannel *Channel, int Number)
{
  const cFont *smallfont = cFont::GetFont(fontSml);
  const cFont *font = cFont::GetFont(fontOsd);
  int lineOffset = (font->Height() - smallfont->Height()) / 2;
  oldpresentvps = -1;
  osd->DrawRectangle(x9 + lineHeight / 2, y4, x10 - 1, y6 - 1, bg);
  int x = x10;
  lineHeight = font->Height();
  if (Channel) {
    char number[6];
    if (!Channel->GroupSep()) {
	int d = 3;
        bool rec = cRecordControls::Active();
        x -= bmRecording.Width();
	osd->DrawBitmap(x, y4 + (y6 - y4 - bmRecording.Height()) / 2, bmRecording, Theme.Color(rec ? clrChannelSymbolRecFg : clrChannelSymbolOff), rec ? Theme.Color(clrChannelSymbolRecBg) : bg);
        x -= bmEncrypted.Width() + d;
        osd->DrawBitmap(x, y4 + (y6 - y4 - bmEncrypted.Height()) / 2, bmEncrypted, Theme.Color(Channel->Ca() ? clrChannelSymbolOn : clrChannelSymbolOff), bg);
	x -= bmDolbyDigital.Width() + d;
	osd->DrawBitmap(x, y4 + (y6 - y4 - bmDolbyDigital.Height()) / 2, bmDolbyDigital, Theme.Color(Channel->Dpid(0) ? clrChannelSymbolOn : clrChannelSymbolOff), bg);
        x -= bmAudio.Width() + d;
        osd->DrawBitmap(x, y4 + (y6 - y4 - bmAudio.Height()) / 2, bmAudio, Theme.Color(Channel->Apid(1) ? clrChannelSymbolOn : clrChannelSymbolOff), bg);
	if (Channel->Vpid()) {
    	    x -= bmTeletext.Width() + d;
    	    osd->DrawBitmap(x, y4 + (y6 - y4 - bmTeletext.Height()) / 2, bmTeletext, Theme.Color(Channel->Tpid() ? clrChannelSymbolOn : clrChannelSymbolOff), bg);
    	    }
	else if (Channel->Apid(0)) {
    	    x -= bmRadio.Width() + d;
    	    osd->DrawBitmap(x, y4 + (y6 - y4 - bmRadio.Height()) / 2, bmRadio, Theme.Color(clrChannelSymbolOn), bg);
    	    }
	setchannel_x = x;
	snprintf(number, sizeof(number), "%d%s", Number ? Number : Channel->Number(), Number ? "-" : "");
        }
    else
	snprintf(number, sizeof(number), "#");

    osd->DrawText(x3, y1, number, Theme.Color(clrChannelNumberFg), Theme.Color(clrChannelDateBg), font, font->Width("00000-"));
    osd->DrawText(x3, y4 + lineOffset, Channel->Name(), Theme.Color(clrChannelNameFg), Theme.Color(clrChannelNameBg), smallfont, x9 - x3);
    if (SkinElchiSetup.logo) {
        if (font->Width(Channel->Name()) != 0 ) {
	    cBitmap bmp(logox, logoy, 8);
	    char *filename;
	    asprintf(&filename, "%s/logos/%s%s", cPlugin::ConfigDirectory("skinelchi"), Channel->Name(),".xpm");
	    if (bmp.LoadXpm(filename) && (bmp.Width() == logox) && (bmp.Height() == logoy)) {
		if (osd->GetBitmap(2)) {
		    osd->GetBitmap(2)->Reset();
		}
		else {
		    osd->GetBitmap(0)->Reset();
		    for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++) {
			osd->GetBitmap(0)->SetColor(i,SkinElchiSetup.ThemeColors[i]);
			}
    		    }
		osd->DrawBitmap(0, 0, bmp);
		}
	    else
		osd->DrawRectangle(0, 0, logox - 1, logoy - 1, Theme.Color(clrChannelNameBg));
	    free(filename);
	    }
    	else
	    osd->DrawRectangle(0, 0, logox - 1, logoy - 1, Theme.Color(clrChannelNameBg));
	}
    }
}

void cSkinElchiDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following)
{
  if (present != Present) {
    lastSeen = -1;
    present = Present;
    }
  else if ((lastSeen < 0) || (oldpresentvps == -1)) {
    present = Present;
    }
  if (!withInfo)
    return;
  const cFont *smallfont = cFont::GetFont(fontSml);
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  osd->DrawRectangle(x1, y6, x4 - 1, y9 - 1, Theme.Color(clrChannelEpgTimeBg));
  osd->DrawRectangle(x1, y8, x2 - 1, y9 - 1, clrTransparent);
  osd->DrawEllipse  (x1, y8, x2 - 1, y9 - 1, Theme.Color(clrChannelEpgTimeBg), 3);
  osd->DrawRectangle(x5, y6, x10 - 1, y9 - 1, Theme.Color(clrChannelEpgTitleBg));
  for (int i = 0; i < 2; i++) {
    const cEvent *e = !i ? Present : Following;
    if (e) {
        osd->DrawText(x1 + 4, y6 + 2 * i * lineHeight, e->GetTimeString(), Theme.Color(clrChannelEpgTimeFg), Theme.Color(clrChannelEpgTimeBg), font);
        osd->DrawText(x5 + 2, y6 + 2 * i * lineHeight, e->Title(), Theme.Color(clrChannelEpgTitle), Theme.Color(clrChannelEpgTitleBg), font, x8 - x5 - 2);
        osd->DrawText(x5 + 2, y6 + (2 * i + 1) * lineHeight, e->ShortText(), Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTitleBg), smallfont, x8 - x5 - 2);
        }
    }
}

void cSkinElchiDisplayChannel::SetMessage(eMessageType Type, const char *Text)
{
  const cFont *font = cFont::GetFont(fontOsd);
  if (Text) {
    char *text;
    asprintf(&text, "%s", Text);

    if (!strcasecmp(text, tr("Going to sleep in about one minute"))) {
	asprintf(&text, "\"%s\" <- Sleeptimer-Message received -> not printed on screen", Text);
	esyslog(text);
	}
    else {
	if (!message)
	    osd->SaveRegion(logox, y4, x11 - 1, y6 - 1);
	if (withInfo) {
	    if (SkinElchiSetup.logo)
		osd->DrawRectangle(x1, y5, logox-1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
	    osd->DrawRectangle(logox, y5, x3-1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
	    osd->DrawRectangle(x10, y4, x11 - 1, y6 - 1, bg);
	    }
	else {
	    if (SkinElchiSetup.logo) {
	        osd->DrawRectangle(x1, y5, x2 - 1, y6 - 1, clrTransparent);
	        osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 3);
		osd->DrawRectangle(x2, y5, logox - 1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
	    	osd->DrawRectangle(logox, y5, x3 - 1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
	    	}
	    else {
	        osd->DrawRectangle(logox, y5, x3 - 1, y6 - 1, clrTransparent);
	        osd->DrawEllipse  (logox, y5, x3 - 1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 3);
	        }
	    }
	    osd->DrawRectangle(x10, y4, x11 - 1, y5 - 1, bg);
	    osd->DrawRectangle(x10, y5, x11 - 1, y6 - 1, clrTransparent);
	osd->DrawEllipse(x10, y4, x11 - 1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 5);
	osd->DrawText(x3, y4, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), font, x10 - x3, 0, taCenter);

	if (SkinElchiSetup.logo) {
	    osd->DrawRectangle(x0, y0, logox - 1, logoy - 1, Theme.Color(clrChannelNameBg));
	    osd->DrawRectangle(x1, logoy, logox-1, y5 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
    	    osd->DrawRectangle(logox, y0 + 2, logox + 2 - 1, logoy + 2 - 1, bg);
    	    osd->DrawRectangle(x1, logoy, logox - 1, logoy + 2 - 1, bg);
	    }
	osd->DrawRectangle(logox, y4, x3-1, y5 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));

	}
	message = true;
	free(text);
    }
    else {
	if (message) {
	    if (SkinElchiSetup.logo) {
    		osd->DrawRectangle(logox, y0 + 2, logox + 2 - 1, logoy + 2 - 1, bg);
    		osd->DrawRectangle(x1, logoy, logox - 1, logoy + 2 - 1, bg);
		}
	    osd->RestoreRegion();
	    message = false;
	    }
    }
}

void cSkinElchiDisplayChannel::Flush(void)
{
    if (!message) {
	const cFont *font = cFont::GetFont(fontOsd);
	const cFont *smallfont = cFont::GetFont(fontSml);
	int lineOffset = (font->Height() - smallfont->Height()) / 2;
	cString date = DayDateTime();
	osd->DrawText(x6 - smallfont->Width(date), y1 + lineOffset, date, Theme.Color(clrChannelDateFg), Theme.Color(clrChannelDateBg), smallfont);
	cDevice *Device = cDevice::PrimaryDevice();
	const tTrackId *Track = Device->GetTrack(Device->GetCurrentAudioTrack());
	int w = 0;
	if (Track) {
	    char *audiostring = NULL;
	    int numaudiotracks = Device->NumAudioTracks();
	    if (numaudiotracks) {
		asprintf(&audiostring, "%s", Track->description);
		w = smallfont->Width(audiostring);
		if (!w)
	    	    asprintf(&audiostring, "%s", Track->language);
		if (numaudiotracks > 1)
		    asprintf(&audiostring, "%s /%d", audiostring, numaudiotracks);
		}
	    else
		asprintf(&audiostring, "%s", tr("No audio available!"));
#ifdef VIDEOSTATUS
    	    if (fd_video >= 0) {
		int videoformat = -1;
		char *videoformatchar = NULL;
		char *resolution = NULL;
		if (ioctl(fd_video, VIDEO_GET_SIZE, &vs) == 0) {
		    switch (vs.aspect_ratio) {
			case VIDEO_FORMAT_4_3: 
			    videoformat = 0;
			    asprintf(&videoformatchar, "%s", "4:3");
			    break;
			case VIDEO_FORMAT_16_9: {
			    videoformat = 1;
			    asprintf(&videoformatchar, "%s", "16:9");
			    break;
			    }
			case VIDEO_FORMAT_221_1: {
			    videoformat = 2;
			    asprintf(&videoformatchar, "%s", "2.21:1");
			    break;
			    }
			}
		    asprintf(&resolution, "%d*%d", vs.w, vs.h);
		    }
		if (videoformatchar != NULL && videoformat != Setup.VideoFormat) {
	    	    if (ioctl(fd_video, VIDEO_GET_STATUS, &videostatus) == 0) {
			switch (videostatus.video_format) {
			    case VIDEO_FORMAT_4_3: {
				if (videoformat == 1)
				    asprintf(&audiostring, "%s (%s) %s", videoformatchar,  "Pan&Scan", audiostring);
				else
				    asprintf(&audiostring, "%s (%d) %s", videoformatchar,  (int)(videostatus.video_format), audiostring);
			        break;
				}
			    case VIDEO_FORMAT_16_9: {
				if (videoformat == 1) {
				    if (Setup.VideoDisplayFormat != 1)
					cDevice::PrimaryDevice()->SetVideoDisplayFormat(eVideoDisplayFormat(Setup.VideoDisplayFormat));
				    asprintf(&audiostring, "%s (%s) %s", videoformatchar, "Letterbox", audiostring);
				    }
				else
				    asprintf(&audiostring, "%s (%d) %s", videoformatchar,  (int)(videostatus.video_format), audiostring);
			        break;
				}
			    case VIDEO_FORMAT_221_1: {
				if (videoformat == 1)
				    asprintf(&audiostring, "%s (%s) %s", videoformatchar,  "Center Cut Out", audiostring);
				else
				    asprintf(&audiostring, "%s (%d) %s", videoformatchar,  (int)(videostatus.video_format), audiostring);
				}
			    }
	    		}
		    }
		else {
		    asprintf(&audiostring, "%s %s", videoformatchar, audiostring);
		    }
		if (videoformatchar!=NULL)
		    free(videoformatchar);
		if (resolution != NULL) {
		    asprintf(&audiostring, "%s %s", resolution, audiostring);
		    free (resolution);
		    }
	        }
#endif
	    w = smallfont->Width(audiostring);
	    osd->DrawText(x10 - w, y1 + lineOffset, audiostring, Theme.Color(clrChannelSymbolOn), bg, smallfont, w);
	    free(audiostring);
	    }
	osd->DrawRectangle(x7, y1, x10 - w - 1, y3 - 1, bg);
	}
    if (present) {
    	time_t presentstarttime = present->StartTime();
	if (withInfo) {
    	    time_t t = time(NULL);
	    if (t > presentstarttime) {
    		int seen = min(y9 - y6 - 1, int((y9 - y6) * double(t - presentstarttime) / present->Duration()));
		if (seen < 0)
		    seen = 0;
		if (seen != lastSeen) {
		    osd->DrawRectangle(x4 + Gap, y6 + seen, x4 + Gap + ScrollWidth - 1, y9 - 1, Theme.Color(clrChannelTimebarRest));
    		    osd->DrawRectangle(x4 + Gap, y6, x4 + Gap + ScrollWidth - 1, y6 + seen - 1, Theme.Color(clrChannelTimebarSeen));
    		    lastSeen = seen;
		    }
		}
	    }
	time_t presentvps = present->Vps();
    	if((presentvps)!=oldpresentvps) {
	    if (presentvps != 0) {
		int d = 3;
		int x = setchannel_x;
		x -= bmVps.Width() + d;
		osd->DrawBitmap(x, y4 + (y6 - y4 - bmVps.Height()) / 2, bmVps, Theme.Color(presentvps != presentstarttime ? clrChannelSymbolOn:clrChannelSymbolOff), bg);
    		}
	    }
    	    oldpresentvps = presentvps;
	}
    else {
	oldpresentvps = -1;
	if (withInfo)
	    osd->DrawRectangle(x4 + Gap, y6, x4 + Gap + ScrollWidth - 1, y9 - 1, Theme.Color(clrChannelEpgTitleBg));
	}
  osd->Flush();
}
