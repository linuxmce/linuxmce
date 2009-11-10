#ifndef VIDEODLG_H_
#define VIDEODLG_H_

#include <QPointer>
#include <QStringList>

#include <mythtv/libmythui/mythscreentype.h>

#include "parentalcontrols.h"
#include "quicksp.h"

class MythUIText;
class MythUIButtonList;
class MythUIButtonTree;
class MythUIButtonListItem;
class MythUIBusyDialog;
class MythUIImage;
class MythUIStateType;
class MythDialogBox;
class MythGenericTree;

class Metadata;
class VideoScanner;

class QUrl;

enum ImageDownloadErrorState { esOK, esError, esTimeout };

class VideoDialog : public MythScreenType
{
    Q_OBJECT

  public:
    enum DialogType { DLG_DEFAULT = 0, DLG_BROWSER = 0x1, DLG_GALLERY = 0x2,
                      DLG_TREE = 0x4, DLG_MANAGER = 0x8, dtLast };

    enum BrowseType { BRS_FOLDER = 0, BRS_GENRE = 0x1, BRS_CATEGORY = 0x2,
                      BRS_YEAR = 0x4, BRS_DIRECTOR = 0x8, BRS_CAST = 0x10,
                      BRS_USERRATING = 0x20, BRS_INSERTDATE = 0x40,
                      BRS_TVMOVIE = 0x80, btLast };

    typedef simple_ref_ptr<class VideoList> VideoListPtr;

    typedef QPointer<class VideoListDeathDelay> VideoListDeathDelayPtr;

    static VideoListDeathDelayPtr &GetSavedVideoList();

  public:
    VideoDialog(MythScreenStack *lparent, QString lname,
            VideoListPtr video_list, DialogType type,
            BrowseType browse);
    ~VideoDialog();

    bool Create();
    bool keyPressEvent(QKeyEvent *levent);

  private:
    void searchStart();

  public slots:
    void searchComplete(QString string);

  protected slots:
    void Init(); /// Called after the screen is created by MythScreenStack

  private slots:
    void UpdatePosition();
    void UpdateText(MythUIButtonListItem *);
    void handleSelect(MythUIButtonListItem *);
    void SetCurrentNode(MythGenericTree *);

    void playVideo();
    void playVideoAlt();
    void playFolder();
    void playVideoWithTrailers();
    void playTrailer();

    void SwitchTree();
    void SwitchGallery();
    void SwitchBrowse();
    void SwitchManager();
    void SwitchVideoFolderGroup();
    void SwitchVideoGenreGroup();
    void SwitchVideoCategoryGroup();
    void SwitchVideoYearGroup();
    void SwitchVideoDirectorGroup();
    void SwitchVideoCastGroup();
    void SwitchVideoUserRatingGroup();
    void SwitchVideoInsertDateGroup();
    void SwitchVideoTVMovieGroup();

    void EditMetadata();
    void VideoSearch();
    void TitleSubtitleSearch();
    void ImageOnlyDownload();
    void ManualVideoUID();
    void ManualVideoTitle();
    void ResetMetadata();
    void ToggleBrowseable();
    void ToggleWatched();
    void RemoveVideo();
    void OnRemoveVideo(bool);

    void VideoMenu();
    void InfoMenu();
    void VideoOptionMenu();
    void ManageMenu();
    void PlayMenu();
    void DisplayMenu();
    void ViewMenu();
    void MetadataBrowseMenu();

    void ChangeFilter();

    void ToggleBrowseMode();
    void ToggleFlatView();

    void ViewPlot();
    void ShowCastDialog();
    bool DoItemDetailShow();

    void OnParentalChange(int amount);

    // Called when the underlying data for an item changes
    void OnVideoSearchListSelection(QString video_uid);
    void OnVideoImgSearchListSelection(QString video_uid);

    void OnManualVideoUID(QString video_uid);
    void OnManualVideoTitle(QString title);

    void doVideoScan();

  protected slots:
    void reloadAllData(bool);
    void reloadData();
    void refreshData();
    void UpdateItem(MythUIButtonListItem *item);

  protected:
    void customEvent(QEvent *levent);

    virtual MythUIButtonListItem *GetItemCurrent();

    virtual void loadData();
    void fetchVideos();
    QString RemoteImageCheck(QString host, QString filename);
    QString GetCoverImage(MythGenericTree *node);
    QString GetFirstImage(MythGenericTree *node, QString type,
                          QString gpnode = NULL, int levels = 0);
    QString GetImageFromFolder(Metadata *metadata);
    QString GetScreenshot(MythGenericTree *node);
    QString GetBanner(MythGenericTree *node);
    QString GetFanart(MythGenericTree *node);

    Metadata *GetMetadata(MythUIButtonListItem *item);

    void handleDirSelect(MythGenericTree *node);
    bool goBack();
    void setParentalLevel(const ParentalLevel::Level &level);
    void shiftParental(int amount);
    bool createPopup();
    void createBusyDialog(QString title);
    void createOkDialog(QString title);

    void SwitchLayout(DialogType type, BrowseType browse);

// Start asynchronous functions.

// These are the start points, separated for sanity.
    // StartVideoPosterSet() start wait background
    //   OnPosterURL()
    //     OnPosterCopyFinished()
    //       OnVideoPosterSetDone()
    //       OnPosterCopyFinished()
    // OnVideoPosterSetDone() stop wait background
    void StartVideoImageSet(Metadata *metadata);

    // StartVideoSearchByUID() start wait background
    //   OnVideoSearchByUIDDone() stop wait background
    //     StartVideoPosterSet()
    void StartVideoSearchByUID(QString video_uid, Metadata *metadata);

    // StartVideoSearchByTitle()
    //   OnVideoSearchByTitleDone()
    void StartVideoSearchByTitle(QString video_uid, QString title,
            Metadata *metadata);
    void StartVideoSearchByTitleSubtitle(QString title,
            QString subtitle, Metadata *metadata);

  private slots:
    // called during StartVideoPosterSet
    void OnImageURL(QString uri, Metadata *metadata, QString type);
    void OnImageCopyFinished(ImageDownloadErrorState error, QString errorMsg,
                              Metadata *metadata, const QString &imagePath);

    // called during StartVideoSearchByTitle
    void OnVideoSearchByTitleDone(bool normal_exit,
                                  const QStringList &results,
                                  Metadata *metadata);
    void OnVideoSearchByTitleSubtitleDone(bool normal_exit,
                                  QStringList result,
                                  Metadata *metadata);
    void OnVideoImageOnlyDone(bool normal_exit,
                                  const QStringList &results,
                                  Metadata *metadata);

// and now the end points

    // StartVideoPosterSet end
    void OnVideoImageSetDone(Metadata *metadata);

    // StartVideoSearchByUID end
    void OnVideoSearchByUIDDone(bool normal_exit,
                                QStringList output,
                                Metadata *metadata, QString video_uid);

// End asynchronous functions.

  private:
    MythDialogBox    *m_menuPopup;
    MythUIBusyDialog *m_busyPopup;
    MythScreenStack  *m_popupStack;

    MythUIButtonList *m_videoButtonList;
    MythUIButtonTree *m_videoButtonTree;

    MythUIText       *m_titleText;
    MythUIText       *m_novideoText;

    MythUIText       *m_positionText;
    MythUIText       *m_crumbText;

    MythUIImage      *m_coverImage;
    MythUIImage      *m_screenshot;
    MythUIImage      *m_banner;
    MythUIImage      *m_fanart;

    MythUIStateType  *m_trailerState;
    MythUIStateType  *m_parentalLevelState;
    MythUIStateType  *m_videoLevelState;
    MythUIStateType  *m_userRatingState;
    MythUIStateType  *m_watchedState;

    class VideoDialogPrivate *m_d;
};

class VideoListDeathDelay : public QObject
{
    Q_OBJECT

  public:
    VideoListDeathDelay(VideoDialog::VideoListPtr toSave);
    ~VideoListDeathDelay();

    VideoDialog::VideoListPtr GetSaved();

  private slots:
    void OnTimeUp();

  private:
    class VideoListDeathDelayPrivate *m_d;
};

#endif
