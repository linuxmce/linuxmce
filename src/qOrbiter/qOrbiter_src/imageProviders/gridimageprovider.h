#ifndef GRIDIMAGEPROVIDER_H
#define GRIDIMAGEPROVIDER_H
/*
  with the complexity of managing model indexes and other position data, this image provider is used for the purpose
  of keeping a link between the two, enabling datagrid images to be correlated to the right cells.
  */
#include <QObject>
#include <QtGlobal>
#if (QT5)
#include <QtCore/QObject>
#include <QtCore/QModelIndex>
#include <QtQuick/QQuickImageProvider>
#include <QtGui/QImage>
#else
#include <QObject>
#include <QtDeclarative/qdeclarative.h>
#include <QModelIndex>
#include <QtDeclarative/QDeclarativeImageProvider>
#include <QImage>
#endif

#include <datamodels/listModel.h>


class qorbiterManager;
class DceListModel;

#if (QT5)
class GridIndexProvider :public QObject , public QQuickImageProvider
#else
class GridIndexProvider :public QObject , public QDeclarativeImageProvider
#endif
{
    Q_OBJECT
public:
    GridIndexProvider( DceListModel* model, int pathRole, int pixmapRole);
   virtual ~GridIndexProvider();
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

public slots:
    void dataUpdated(const QModelIndex & topLeft, const QModelIndex & bottomRight, const int & cRow);
    void dataDeleted(const QModelIndex & parent, int start, int end);
    void dataReset();

private:

    DceListModel& mModel;
    int mPathRole;
    int mPixmapRole;
    QMap<QString, QModelIndex> mPixmapIndex;
};

#endif // GRIDIMAGEPROVIDER_H
