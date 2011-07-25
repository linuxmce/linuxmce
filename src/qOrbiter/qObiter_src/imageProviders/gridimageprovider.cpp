#include "gridimageprovider.h"
#include <QDebug>

GridIndexProvider::GridIndexProvider(ListModel *model  , int pathRole, int pixmapRole) :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image), mModel(*model),  mPathRole(pathRole),
    mPixmapRole(pixmapRole)
{
    // For each pixmap already in the model, get a mapping between the name and the index
 qRegisterMetaType<QModelIndex>("QModelIndex");

    for(int row = 0; row < mModel.rowCount(); row++) {

        QModelIndex index = mModel.index(row, 0);

        QString path = mModel.data(index, mPathRole).value<QString>();


        mPixmapIndex[path] = index;
    }

    connect(&mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(dataUpdated(QModelIndex,QModelIndex)));
    connect(&mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(dataDeleted(QModelIndex,int,int)));
    connect(&mModel, SIGNAL(modelReset()),
            this, SLOT(dataReset()));
}


QImage GridIndexProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{

        QString key = QString("image://datagridimg/%1").arg(id);
        QModelIndex index = mPixmapIndex.value(id);
        QImage image = mModel.data(index, mPixmapRole).value<QImage>();
        QImage result;

        if (requestedSize.isValid()) {
            result = image.scaled(requestedSize, Qt::KeepAspectRatio);
        } else {
            result = image;
        }
        *size = result.size();
        return result;

}

void GridIndexProvider::dataUpdated(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    // For each pixmap already in the model, get a mapping between the name and the index

    for(int row = 0; row < mModel.rowCount(); row++) {     
        QModelIndex index = mModel.index(row, 0);
        QString path = mModel.data(index, mPathRole).value<QString>();
        mPixmapIndex[path] = index;
    }

}

void GridIndexProvider::dataDeleted(const QModelIndex&, int start, int end)
{
    // For each pixmap already in the model, get a mapping between the name and the index
    for(int row = 0; row < mModel.rowCount(); row++) {
        QModelIndex index = mModel.index(row, 0);
        QString path = mModel.data(index, mPathRole).value<QString>();
        mPixmapIndex[path] = index;
    }

}

void GridIndexProvider::dataReset()
{
    mPixmapIndex.clear();
}

GridIndexProvider::~GridIndexProvider()
{
    this->deleteLater();
}
