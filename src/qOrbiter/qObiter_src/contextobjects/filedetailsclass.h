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


#include <QObject>

class FileDetailsClass : public QObject
{
    Q_OBJECT
public:
    explicit FileDetailsClass(QObject *parent = 0);

signals:

public slots:

};

#endif // FILEDETAILSCLASS_H
