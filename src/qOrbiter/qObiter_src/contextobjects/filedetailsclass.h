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
    Q_PROPERTY(QString object_title READ getTitle WRITE setTitle NOTIFY object_changed)

    QString object_title;

    QString file;
    inline QString getFile () {return file;}
    inline void setFile(QString incFile) { file = incFile; emit FileChanged(file);}

    bool showDetails;
    inline bool isVisible () { return showDetails; }
    inline void setVisible (bool state) { showDetails = state; emit VisibleChanged( showDetails);}

   // Q_INVOKABLE void setSelectionStatus(QString format);
   // Q_INVOKABLE bool getSelectionStatus();

signals:
    void object_changed();
    void FileChanged(QString f);
    void VisibleChanged(bool vis);

public slots:
    inline QString getTitle() {return object_title;}
    void setFileMediaType();
    void setTitle(QString t);

};

#endif // FILEDETAILSCLASS_H
