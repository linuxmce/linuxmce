#ifndef FILEDETAILSCLASS_H
#define FILEDETAILSCLASS_H

/*
  The purpose of this class is to make sure the current file details are always availble to the file details object in qml
  we are using a qobject because its construction lends itself to multiple qml types, such as list models, images, etc.
  By implementing the file details in this manner, we can provide a more complex object to the file details QML implementation,
  making it more flexible for the user interface designers

  Currently, the architecture of attributes in the pluto_media table / the linuxmce ecosystem lends itself to a many to one
  relationship. This present a unique challenge in that for any given attribute, there can be multiple connected attributes
  the file has. So, our Qt object must be allowed to grow and shrink as needed.
  */


#include <QDeclarativeItem>

class FileDetailsClass : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit FileDetailsClass(QDeclarativeItem *parent = 0);
    Q_PROPERTY (bool showDetails READ isVisible WRITE setVisible NOTIFY VisibleChanged)
    Q_PROPERTY(QString file READ getFile WRITE setFile NOTIFY FileChanged)
    Q_PROPERTY(QString objecttitle READ getTitle WRITE setTitle NOTIFY object_changed)
    Q_PROPERTY(QString synop READ getSynop WRITE setSynop NOTIFY object_changed)
    Q_PROPERTY(QUrl screenshot READ getScreenShot WRITE setScreenshot NOTIFY ImageChanged)
    Q_PROPERTY(QString filename READ getFilename WRITE setFilename NOTIFY object_changed)

    Q_PROPERTY( QImage title READ getTitleImage WRITE setTitleImage NOTIFY titleImageChanged)
    Q_PROPERTY(QImage program READ getProgramImage WRITE setProgramImage NOTIFY object_changed)

    QString objecttitle;

    QString file;
    inline QString getFile () {return file;}
    inline void setFile(QString incFile) { file = incFile; emit FileChanged(file);}

    QImage title;
    inline void setTitleImage (QImage inc_t) {title = inc_t; emit object_changed();}
    inline QImage getTitleImage () {return title;}

    QImage program;
    inline void setProgramImage (QImage inc_p) {program = inc_p; emit object_changed();}
    inline QImage getProgramImage () {return program;}
    QImage Season;
    QImage imdb;
    QImage episode;


    bool showDetails;
    inline bool isVisible () { return showDetails; }
    inline void setVisible (bool state) { showDetails = state; emit VisibleChanged( showDetails);}

    // Q_INVOKABLE void setSelectionStatus(QString format);
    // Q_INVOKABLE bool getSelectionStatus();

    QString synop;
    inline QString getSynop() {return synop;}
    inline void setSynop(QString s) { synop = s; emit object_changed(); }

    QUrl screenshot;
    QUrl bgImage;
    inline void setScreenshot(QUrl u) {screenshot = u; emit ImageChanged(screenshot);}
    inline QUrl getScreenShot() {return screenshot;}

    QString filename;
    inline QString getFilename() {return filename;}
    inline void setFilename (QString f) {filename = f; emit object_changed();}

signals:
    void object_changed();
    void FileChanged(QString f);
    void VisibleChanged(bool vis);
    void ImageChanged(QUrl);
    void titleImageChanged();

public slots:
    inline QString getTitle() {return objecttitle;}
    void setFileMediaType();
    void setTitle(QString t);

};

#endif // FILEDETAILSCLASS_H
