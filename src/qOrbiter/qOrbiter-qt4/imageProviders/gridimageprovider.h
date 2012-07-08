#ifndef GRIDIMAGEPROVIDER_H
#define GRIDIMAGEPROVIDER_H
/*
  with the complexity of managing model indexes and other position data, this image provider is used for the purpose
  of keeping a link between the two, enabling datagrid images to be correlated to the right cells.
  */
#include <QObject>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtCore/QObject>
#include <QtCore/QModelIndex>
#include <QtQml/QQmlImageProvider>
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
class ListModel;

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
class GridIndexProvider :public QObject , public QQmlImageProvider
#else
class GridIndexProvider :public QObject , public QDeclarativeImageProvider
#endif
{
    Q_OBJECT
public:
    GridIndexProvider( ListModel* model, int pathRole, int pixmapRole);
   virtual ~GridIndexProvider();
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

public slots:
    void dataUpdated(const QModelIndex & topLeft, const QModelIndex & bottomRight, const int & cRow);
    void dataDeleted(const QModelIndex & parent, int start, int end);
    void dataReset();

private:

    ListModel& mModel;
    int mPathRole;
    int mPixmapRole;
    QMap<QString, QModelIndex> mPixmapIndex;
};

#endif // GRIDIMAGEPROVIDER_H
