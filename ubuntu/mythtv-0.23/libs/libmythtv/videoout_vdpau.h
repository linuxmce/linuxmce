#ifndef VIDEOOUT_VDPAU_H
#define VIDEOOUT_VDPAU_H

// MythTV headers
#include "videooutbase.h"
#include "mythrender_vdpau.h"

struct vdpauPIP
{
    QSize videoSize;
    uint  videoSurface;
    uint  videoMixer;
};

class VideoOutputVDPAU : public VideoOutput
{
  public:
    static void GetRenderOptions(render_opts &opts);
    VideoOutputVDPAU(MythCodecID codec_id);
    ~VideoOutputVDPAU();
    bool Init(int width, int height, float aspect, WId winid,
              int winx, int winy, int winw, int winh, WId embedid = 0);
    bool SetDeinterlacingEnabled(bool interlaced);
    bool SetupDeinterlace(bool interlaced, const QString& ovrf="");
    bool ApproveDeintFilter(const QString& filtername) const;
    void ProcessFrame(VideoFrame *frame, OSD *osd,
                      FilterChain *filterList,
                      const PIPMap &pipPlayers,
                      FrameScanType scan);
    void PrepareFrame(VideoFrame*, FrameScanType);
    void DrawSlice(VideoFrame*, int x, int y, int w, int h);
    void Show(FrameScanType);
    void ClearAfterSeek(void);
    bool InputChanged(const QSize &input_size,
                      float        aspect,
                      MythCodecID  av_codec_id,
                      void        *codec_private,
                      bool        &aspect_only);
    void Zoom(ZoomDirection direction);
    void VideoAspectRatioChanged(float aspect);
    void EmbedInWidget(int x, int y, int w, int h);
    void StopEmbedding(void);
    void MoveResizeWindow(QRect new_rect);
    void DrawUnusedRects(bool sync = true);
    void UpdatePauseFrame(void);
    int  SetPictureAttribute(PictureAttribute attribute, int newValue);
    void InitPictureAttributes(void);
    static QStringList GetAllowedRenderers(MythCodecID myth_codec_id,
                                    const QSize &video_dim);
    static MythCodecID GetBestSupportedCodec(uint width, uint height,
                                             uint stream_type,
                                             bool no_acceleration);
    DisplayInfo  GetDisplayInfo(void);
    virtual bool IsPIPSupported(void) const { return true; }
    virtual bool IsPBPSupported(void) const { return false; }
    virtual bool NeedExtraAudioDecode(void) const
        { return codec_is_vdpau(m_codec_id); }
    virtual bool hasHWAcceleration(void) const
        { return codec_is_vdpau(m_codec_id); }
    virtual bool IsSyncLocked(void) const { return true; }
    void SetNextFrameDisplayTimeOffset(int delayus) { m_frame_delay = delayus; }

  private:
    virtual bool hasFullScreenOSD(void) const { return true; }
    void TearDown(void);
    bool InitRender(void);
    void DeleteRender(void);
    bool InitBuffers(void);
    bool CreateVideoSurfaces(uint num);
    void ClaimVideoSurfaces(void);
    void DeleteVideoSurfaces(void);
    void DeleteBuffers(void);
    void RestoreDisplay(void);
    void UpdateReferenceFrames(VideoFrame *frame);
    bool FrameIsInUse(VideoFrame *frame);
    void ClearReferenceFrames(void);
    void DiscardFrame(VideoFrame*);
    void DiscardFrames(bool next_frame_keyframe);
    void DoneDisplayingFrame(VideoFrame *frame);
    void CheckFrameStates(void);
    virtual int DisplayOSD(VideoFrame *frame, OSD *osd,
                           int stride = -1, int revision = -1);
    virtual void ShowPIP(VideoFrame        *frame,
                         NuppelVideoPlayer *pipplayer,
                         PIPLocation        loc);
    virtual void RemovePIP(NuppelVideoPlayer *pipplayer);
    bool InitPIPLayer(QSize size);
    void DeinitPIPS(void);
    void DeinitPIPLayer(void);
    void ParseOptions(void);
    void InitOSD(QSize size);
    void DeinitOSD(void);

    MythCodecID          m_codec_id;
    Window               m_win;
    MythRenderVDPAU     *m_render;

    uint                 m_buffer_size;
    QVector<uint>        m_video_surfaces;
    QVector<uint>        m_reference_frames;
    uint                 m_pause_surface;
    bool                 m_need_deintrefs;
    uint                 m_video_mixer;
    uint                 m_mixer_features;
    bool                 m_checked_surface_ownership;
    bool                 m_checked_output_surfaces;

    uint                 m_decoder;
    int                  m_pix_fmt;

    int                  m_frame_delay;
    QMutex               m_lock;

    uint                 m_pip_layer;
    uint                 m_pip_surface;
    bool                 m_pip_ready;
    QMap<NuppelVideoPlayer*,vdpauPIP> m_pips;

    uint                 m_osd_layer;
    uint                 m_osd_surface;
    uint                 m_osd_output_surface;
    uint                 m_osd_alpha_surface;
    uint                 m_osd_mixer;
    bool                 m_osd_ready;
    bool                 m_osd_avail;
    QSize                m_osd_size;

    bool                 m_using_piccontrols;
    bool                 m_skip_chroma;
    float                m_denoise;
    float                m_sharpen;
    bool                 m_studio;
    int                  m_colorspace;
};

#endif // VIDEOOUT_VDPAU_H


