#ifdef VIDEOSTATUS
#include <sys/ioctl.h>
#endif
#include "DisplayReplay.h"
#include "setup.h"
#include "symbols/ffwd.xpm"
#include "symbols/ffwd1.xpm"
#include "symbols/ffwd2.xpm"
#include "symbols/ffwd3.xpm"
#include "symbols/ffwd4.xpm"
#include "symbols/ffwd5.xpm"
#include "symbols/ffwd6.xpm"
#include "symbols/ffwd7.xpm"
#include "symbols/ffwd8.xpm"
#include "symbols/frew.xpm"
#include "symbols/frew1.xpm"
#include "symbols/frew2.xpm"
#include "symbols/frew3.xpm"
#include "symbols/frew4.xpm"
#include "symbols/frew5.xpm"
#include "symbols/frew6.xpm"
#include "symbols/frew7.xpm"
#include "symbols/frew8.xpm"
#include "symbols/pause.xpm"
#include "symbols/play.xpm"
#include "symbols/sfwd.xpm"
#include "symbols/sfwd1.xpm"
#include "symbols/sfwd2.xpm"
#include "symbols/sfwd3.xpm"
#include "symbols/sfwd4.xpm"
#include "symbols/sfwd5.xpm"
#include "symbols/sfwd6.xpm"
#include "symbols/sfwd7.xpm"
#include "symbols/sfwd8.xpm"
#include "symbols/srew.xpm"
#include "symbols/srew1.xpm"
#include "symbols/srew2.xpm"
#include "symbols/srew3.xpm"
#include "symbols/srew4.xpm"
#include "symbols/srew5.xpm"
#include "symbols/srew6.xpm"
#include "symbols/srew7.xpm"
#include "symbols/srew8.xpm"

cBitmap cSkinElchiDisplayReplay::bmFfwd(ffwd_xpm);
cBitmap cSkinElchiDisplayReplay::bmFfwd1(ffwd1_xpm);
cBitmap cSkinElchiDisplayReplay::bmFfwd2(ffwd2_xpm);
cBitmap cSkinElchiDisplayReplay::bmFfwd3(ffwd3_xpm);
cBitmap cSkinElchiDisplayReplay::bmFfwd4(ffwd4_xpm);
cBitmap cSkinElchiDisplayReplay::bmFfwd5(ffwd5_xpm);
cBitmap cSkinElchiDisplayReplay::bmFfwd6(ffwd6_xpm);
cBitmap cSkinElchiDisplayReplay::bmFfwd7(ffwd7_xpm);
cBitmap cSkinElchiDisplayReplay::bmFfwd8(ffwd8_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew(frew_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew1(frew1_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew2(frew2_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew3(frew3_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew4(frew4_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew5(frew5_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew6(frew6_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew7(frew7_xpm);
cBitmap cSkinElchiDisplayReplay::bmFrew8(frew8_xpm);
cBitmap cSkinElchiDisplayReplay::bmPause(pause_xpm);
cBitmap cSkinElchiDisplayReplay::bmPlay(play_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd(sfwd_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd1(sfwd1_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd2(sfwd2_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd3(sfwd3_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd4(sfwd4_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd5(sfwd5_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd6(sfwd6_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd7(sfwd7_xpm);
cBitmap cSkinElchiDisplayReplay::bmSfwd8(sfwd8_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew(srew_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew1(srew1_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew2(srew2_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew3(srew3_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew4(srew4_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew5(srew5_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew6(srew6_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew7(srew7_xpm);
cBitmap cSkinElchiDisplayReplay::bmSrew8(srew8_xpm);

int pos, dir, delay;

#ifdef VIDEOSTATUS
#include <linux/dvb/video.h>
extern video_status videostatus;
extern int fd_video;
extern video_size_t vs;
bool forward;
#define DEV_DVB_VIDEO     "/dev/dvb/adapter%d/video%d"
#endif

// --- cSkinElchiDisplayReplay ---------------------------------------------

cSkinElchiDisplayReplay::cSkinElchiDisplayReplay(bool ModeOnly)
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
  modeonly = ModeOnly;
  title = NULL;
  oldtitle = NULL;
  const cFont *font = cFont::GetFont(fontOsd);
  const cFont *smallfont = cFont::GetFont(fontSml);
  int lineHeight = font->Height();
  int lineHeighthalf = lineHeight / 2;
  lastCurrentWidth = 0;
  message = false;
  cBitmap bm(play_xpm);
  x0 = 0;
  x1 = x0 + Setup.OSDWidth;
  x2 = x1 - lineHeight - smallfont->Width("00:00:00 / 00:00:00");
  x3 = x1 - lineHeight - bmFrew.Width() - 3 - bmSrew.Width() - 3 - bmPlay.Width() - 3 - bmSfwd.Width() - 3 - bmFfwd.Width();
  x4 = x2 - 2 * lineHeight;
  x5 = x3 - 2 * lineHeight;
  y0 = 0;
  y1 = lineHeight;
  y2 = 2 * lineHeight;
  y3 = 3 * lineHeight;
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - y3);
  tArea Areas[] = { { x0, y0, x1 - 1, y3 - 1, 4 } };
  osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  if (ModeOnly) {
    osd->DrawRectangle(x0, y0, x1 - 1, y2 - 1, clrTransparent);
    osd->DrawRectangle(x3, y2, x3 + lineHeighthalf - 1, y3 - 1, clrTransparent);
    osd->DrawEllipse(x3, y2, x3 + lineHeighthalf - 1, y3 - 1, Theme.Color(clrBackground), 7);
    osd->DrawRectangle(x3 + lineHeighthalf, y2, x1 - lineHeighthalf - 1, y3 - 1, Theme.Color(clrBackground));
    osd->DrawRectangle(x1 - lineHeighthalf, y2, x1 - 1, y3 - 1, clrTransparent);
    osd->DrawEllipse(x1 - lineHeighthalf, y2, x1 - 1, y3 - 1, Theme.Color(clrBackground), 5);
    }
  else {
    osd->DrawRectangle(x0, y0, x2 - 1, y1 - 1, clrTransparent);
    osd->DrawSlope(x4, y0, x2 - 1, y1 - 1, Theme.Color(clrBackground), 0);

    osd->DrawRectangle(x2, y0, x1 - lineHeighthalf - 1, y1 - 1, Theme.Color(clrBackground));

    osd->DrawRectangle(x1 - lineHeighthalf, y0, x1 - 1, y1 - lineHeighthalf - 1, clrTransparent);
    osd->DrawEllipse(x1 - lineHeighthalf, y0, x1 - 1, y1 - lineHeighthalf - 1, Theme.Color(clrBackground), 1);

    osd->DrawRectangle(x0 + lineHeighthalf, y1, x0 + lineHeight - 1, y2 - 1, clrTransparent);
    osd->DrawEllipse(x0 + lineHeighthalf, y1, x0 + lineHeight - 1, y2 - 1, Theme.Color(clrBackground), 7);
    osd->DrawRectangle(x0 + lineHeight, y1, x1 - lineHeighthalf - 1, y2 - 1, Theme.Color(clrBackground));

    osd->DrawRectangle(x0, y2, x3 - 1, y3 - 1, clrTransparent);
    osd->DrawSlope(x5, y2, x3 - 1, y3 - 1, Theme.Color(clrBackground), 3);

    osd->DrawRectangle(x1 - lineHeighthalf, y1 - lineHeighthalf, x1 - 1, y2 + lineHeighthalf, Theme.Color(clrBackground));

    osd->DrawRectangle(x3, y0, x1 - lineHeighthalf - 1, y3 - 1, Theme.Color(clrBackground));

    osd->DrawRectangle(x1 - lineHeighthalf, y3 - lineHeighthalf, x1 - 1, y3 - 1, clrTransparent);
    osd->DrawEllipse(x1 - lineHeighthalf, y3 - lineHeighthalf, x1 - 1, y3 - 1, Theme.Color(clrBackground), 4);
    }
}

cSkinElchiDisplayReplay::~cSkinElchiDisplayReplay()
{
#ifdef VIDEOSTATUS
  if (fd_video != -1) {
    close(fd_video);
    fd_video = -1;
    }
#endif
  if (title)
    free(title);
  if (oldtitle)
    free(oldtitle);
  delete osd;
}

void cSkinElchiDisplayReplay::SetTitle(const char *Title)
{
  if (title) {
    if (oldtitle) {
	if (!strcasecmp(oldtitle, Title))
	    return;
	else {
    	    free(oldtitle);
	    asprintf(&oldtitle,"%s", Title);
	    }
	}
    else
        asprintf(&oldtitle,"%s", Title);
    free(title);
    asprintf(&title," %s ", oldtitle);
    }
  asprintf(&title," %s ", Title);
  const cFont *font = cFont::GetFont(fontOsd);
  const cFont *smallfont = cFont::GetFont(fontSml);
  int w = smallfont->Width(title);
  int x = x1 - (x1 - x4);
  int h = font->Height() - smallfont->Height();
  if (w > x) {
    w = x;
    dir = -1;
    pos = 0;
    delay = 20;
    DelayTime.Set();
    }
  else
    dir = 0;
  osd->DrawText(x0, y0, title, Theme.Color(clrReplayTitleFg), Theme.Color(clrReplayTitleBg), smallfont, w);
  osd->DrawRectangle(w, y0, w + 3 - 1, y0 + 3 - 1, Theme.Color(Transparent));
  osd->DrawRectangle(w + 3, y0, x4 - 1, y1 - h - 1, Theme.Color(Transparent));
  osd->DrawRectangle(x4, y0 + 3, x4 + 3 - 1, y1 - h - 1, Theme.Color(Transparent));
  if (h)
    osd->DrawRectangle(w + 3, y1 - h, x4 + 3 - 1, y1 - h + 3 - 1, Theme.Color(Transparent));
  osd->DrawRectangle(w, y0 + 3, w + 3 - 1, y1 - h + 3 - 1, Theme.Color(clrBackground));
  osd->DrawRectangle(x0 + 3, y1 - h, w - 1, y1 - h + 3 - 1, Theme.Color(clrBackground));
}

void cSkinElchiDisplayReplay::SetMode(bool Play, bool Forward, int Speed)
{
#ifdef VIDEOSTATUS
  forward = Forward;
#endif
  if (Setup.ShowReplayMode) {
    tColor bg = Theme.Color(clrBackground);
    tColor off = Theme.Color(clrReplaySymbolOff);
    int x = x3 + y1 / 2;
    int y = y3 - y2;
    if (Speed < -1)
	Speed = -1;
    if (Speed > 9)
        Speed = 9;
    if ((Speed > -1) && Play && !Forward) {
        switch (Speed) {
            case 0:
	    case 1: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew, Theme.Color(clrReplaySymbolOn), bg); break;
	    case 2: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew1, Theme.Color(clrReplaySymbolOn), bg); break;
	    case 3: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew2, Theme.Color(clrReplaySymbolOn), bg); break;
	    case 4: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew3, Theme.Color(clrReplaySymbolOn), bg); break;
	    case 5: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew4, Theme.Color(clrReplaySymbolOn), bg); break;
	    case 6: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew5, Theme.Color(clrReplaySymbolOn), bg); break;
	    case 7: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew6, Theme.Color(clrReplaySymbolOn), bg); break;
	    case 8: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew7, Theme.Color(clrReplaySymbolOn), bg); break;
	    case 9: osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew8, Theme.Color(clrReplaySymbolOn), bg); break;
	    }
	}
    else
        osd->DrawBitmap(x, y2 + (y - bmFrew.Height()) / 2, bmFrew, off, bg);
    x += bmFrew.Width() + 3;

    if ((Speed > -1) && !Play && !Forward) {
        switch (Speed) {
            case 0:
            case 1: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew, Theme.Color(clrReplaySymbolOn), bg); break;
            case 2: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew1, Theme.Color(clrReplaySymbolOn), bg); break;
            case 3: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew2, Theme.Color(clrReplaySymbolOn), bg); break;
            case 4: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew3, Theme.Color(clrReplaySymbolOn), bg); break;
            case 5: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew4, Theme.Color(clrReplaySymbolOn), bg); break;
            case 6: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew5, Theme.Color(clrReplaySymbolOn), bg); break;
            case 7: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew6, Theme.Color(clrReplaySymbolOn), bg); break;
            case 8: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew7, Theme.Color(clrReplaySymbolOn), bg); break;
            case 9: osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew8, Theme.Color(clrReplaySymbolOn), bg); break;
	    }
	}
    else
        osd->DrawBitmap(x, y2 + (y - bmSrew.Height()) / 2, bmSrew, off, bg);
    x += bmSrew.Width() + 3;

    if (Speed == -1)
	osd->DrawBitmap(x, y2 + (y - bmPlay.Height()) / 2, Play ? bmPlay : bmPause, Theme.Color(clrReplaySymbolOn), bg);
    else
        osd->DrawBitmap(x, y2 + (y - bmPlay.Height()) / 2, bmPlay, off, bg);
    x += bmPlay.Width() + 3;
     
    if ((Speed > -1) && !Play && Forward) {
        switch (Speed) {
            case 0:
            case 1: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd, Theme.Color(clrReplaySymbolOn), bg); break;
            case 2: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd1, Theme.Color(clrReplaySymbolOn), bg); break;
            case 3: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd2, Theme.Color(clrReplaySymbolOn), bg); break;
            case 4: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd3, Theme.Color(clrReplaySymbolOn), bg); break;
            case 5: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd4, Theme.Color(clrReplaySymbolOn), bg); break;
            case 6: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd5, Theme.Color(clrReplaySymbolOn), bg); break;
            case 7: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd6, Theme.Color(clrReplaySymbolOn), bg); break;
            case 8: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd7, Theme.Color(clrReplaySymbolOn), bg); break;
            case 9: osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd8, Theme.Color(clrReplaySymbolOn), bg); break;
	    }
	}
    else
	osd->DrawBitmap(x, y2 + (y - bmSfwd.Height()) / 2, bmSfwd, off, bg);
    x += bmSfwd.Width() + 3;

    if ((Speed > -1) && Play && Forward) {
        switch (Speed) {
            case 0:
	    case 1: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd, Theme.Color(clrReplaySymbolOn), bg); break;
            case 2: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd1, Theme.Color(clrReplaySymbolOn), bg); break;
            case 3: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd2, Theme.Color(clrReplaySymbolOn), bg); break;
            case 4: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd3, Theme.Color(clrReplaySymbolOn), bg); break;
            case 5: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd4, Theme.Color(clrReplaySymbolOn), bg); break;
            case 6: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd5, Theme.Color(clrReplaySymbolOn), bg); break;
            case 7: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd6, Theme.Color(clrReplaySymbolOn), bg); break;
            case 8: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd7, Theme.Color(clrReplaySymbolOn), bg); break;
            case 9: osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd8, Theme.Color(clrReplaySymbolOn), bg); break;
	    }
	}
    else
        osd->DrawBitmap(x, y2 + (y - bmFfwd.Height()) / 2, bmFfwd, off, bg);
    }
}

void cSkinElchiDisplayReplay::SetProgress(int Current, int Total)
{
  cProgressBar pb(x1 - x0 - y1 - y1 / 2, y2 - y1 - 8, Current, Total, marks, Theme.Color(clrReplayProgressSeen), Theme.Color(clrReplayProgressRest), Theme.Color(clrReplayProgressSelected), Theme.Color(clrReplayProgressMark), Theme.Color(clrReplayProgressCurrent));
  osd->DrawBitmap(x0 + y1, y1 + 4, pb);
}

void cSkinElchiDisplayReplay::SetCurrent(const char *Current)
{
  const cFont *font = cFont::GetFont(fontSml);
  int v = font->Width("00:00:00");
  char *current;
  asprintf(&current,"%s %s ", Current, "/");
  int w = font->Width(current);
  osd->DrawText(x1 - w - v - y1 / 2, y0, current, Theme.Color(clrReplayCurrent), Theme.Color(clrBackground), font, lastCurrentWidth > w ? lastCurrentWidth : 0);
  free(current);
  lastCurrentWidth = w;
}

void cSkinElchiDisplayReplay::SetTotal(const char *Total)
{
  const cFont *font = cFont::GetFont(fontSml);
  osd->DrawText(x1 - font->Width(Total) - y1 / 2, y0, Total, Theme.Color(clrReplayTotal), Theme.Color(clrBackground), font);
}

void cSkinElchiDisplayReplay::SetJump(const char *Jump)
{
  osd->DrawText(x0 + (x3 - x0) / 4, y2, Jump, Theme.Color(clrReplayModeJump), Theme.Color(clrBackground), cFont::GetFont(fontOsd), (x3 - x0) / 2, 0, taCenter);
}

void cSkinElchiDisplayReplay::SetMessage(eMessageType Type, const char *Text)
{
  if (Text) {
     const cFont *font = cFont::GetFont(fontOsd);
     tColor bg = Theme.Color(clrMessageStatusBg + 2 * Type);
     int lineHeight = font->Height();
     osd->SaveRegion(x5, y2, x1 - 1, y3 - 1);
     osd->DrawEllipse(x0, y2, lineHeight / 2 - 1, y3 - 1, bg, 7);
     osd->DrawText(x0 + lineHeight / 2, y2, Text, Theme.Color(clrMessageStatusFg + 2 * Type), bg, font, x1 - x0 - lineHeight, y3 - y2, taCenter);
     osd->DrawEllipse(x1 - lineHeight / 2, y2, x1 - 1, y3 - 1, bg, 5);
     message = true;
     }
  else {
     osd->RestoreRegion();
     message = false;
     }
}

void cSkinElchiDisplayReplay::Flush(void)
{
  osd->Flush();

  if (!modeonly && dir != 0) {
    int elapsed = ((int)DelayTime.Elapsed()) / 7;
    if (delay) {
	if ( elapsed >= delay) {
	    DelayTime.Set();
	    delay -= elapsed;
	    if (delay < 0)
		delay = 0;
	    }
	}
    else {
	if (elapsed) {
	    DelayTime.Set();
	    const cFont *smallfont = cFont::GetFont(fontSml);
#ifdef VIDEOSTATUS
	    char *testtitle;
	    asprintf(&testtitle, "%s ", title);
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
				    asprintf(&testtitle, "%s (%s) %s", videoformatchar,  "Pan&Scan", testtitle);
				else
				    asprintf(&testtitle, "%s (%d) %s", videoformatchar,  (int)(videostatus.video_format), testtitle);
			        break;
				}
			    case VIDEO_FORMAT_16_9: {
				if (videoformat == 1) {
				    if (Setup.VideoDisplayFormat != 1 && forward && SkinElchiSetup.switchvideodisplayformat)
					cDevice::PrimaryDevice()->SetVideoDisplayFormat(eVideoDisplayFormat(Setup.VideoDisplayFormat));
				    asprintf(&testtitle, "%s (%s) %s", videoformatchar, "Letterbox", testtitle);
				    }
				else
				    asprintf(&testtitle, "%s (%d) %s", videoformatchar,  (int)(videostatus.video_format), testtitle);
			        break;
				}
			    case VIDEO_FORMAT_221_1: {
				if (videoformat == 1)
				    asprintf(&testtitle, "%s (%s) %s", videoformatchar,  "Center Cut Out", testtitle);
				else
				    asprintf(&testtitle, "%s (%d) %s", videoformatchar,  (int)(videostatus.video_format), testtitle);
				}
			    }
	    		}
		    }
		else {
		    asprintf(&testtitle, "%s %s", videoformatchar, testtitle);
		    }
		if (videoformatchar!=NULL)
		    free(videoformatchar);
		if (resolution != NULL) {
		    asprintf(&testtitle, "%s %s", resolution, testtitle);
		    free (resolution);
		    }
	        }
	    int w = smallfont->Width(testtitle);
#else
	    int w = smallfont->Width(title);
#endif
	    int x = x1 - (x1 - x4);
	    pos = pos - (dir * elapsed);
	    if (pos < 0) {
		pos = 0;
	        dir = -1;
		}
	    else {
		if ((pos + x) > w) {
	    	    pos = w - x;
	    	    dir = 1;
	    	    }
		}
	    if (pos < 20)
		delay = 20 - pos;
	    else {
		if ((w - x - pos) < 20)
		    delay = 20 - (w - x - pos);
		}
#ifdef VIDEOSTATUS
	    asprintf(&testtitle, " %s", testtitle);
	    osd->DrawText(x0 - pos, y0, testtitle, Theme.Color(clrReplayTitleFg), Theme.Color(clrReplayTitleBg), smallfont, x + pos);
	    free(testtitle);
#else
	    osd->DrawText(x0 - pos, y0, title, Theme.Color(clrReplayTitleFg), Theme.Color(clrReplayTitleBg), smallfont, x + pos);
#endif
	    }
	}
    }
}

