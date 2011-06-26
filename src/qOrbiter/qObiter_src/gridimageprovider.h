#ifndef GRIDIMAGEPROVIDER_H
#define GRIDIMAGEPROVIDER_H

#include <QObject>
#include <qdeclarative.h>
#include <QModelIndex>
#include <QDeclarativeImageProvider>
#include <listModel.h>



class GridIndexProvider : public QObject, public QDeclarativeImageProvider
{
    Q_OBJECT
public:
    GridIndexProvider(ListModel model, int pathRole, int pixmapRole,
        QObject* parent = 0);
   virtual ~GridIndexProvider();
    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);

public slots:
    void dataUpdated(const QModelIndex & topLeft, const QModelIndex & bottomRight);
    void dataDeleted(const QModelIndex & parent, int start, int end);
    void dataReset();

private:
    ListModel& mModel;
    int mPathRole;
    int mPixmapRole;
    QMap<QString, QModelIndex> mPixmapIndex;
};

#endif // GRIDIMAGEPROVIDER_H
