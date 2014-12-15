#include "gridimageprovider.h"
#include "qorbitermanager.h"


GridIndexProvider::GridIndexProvider(DceListModel *model  , int pathRole, int pixmapRole) :
    #if (QT5)
    QQuickImageProvider(QQuickImageProvider::Image), mModel(*model),  mPathRole(pathRole), mPixmapRole(pixmapRole)
  #else
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image), mModel(*model),  mPathRole(pathRole), mPixmapRole(pixmapRole)
  #endif
{
    // For each pixmap already in the model, get a mapping between the name and the index
    qRegisterMetaType<QModelIndex>("QModelIndex");

    for(int row = 0; row < mModel.rowCount(); row++) {

        QModelIndex index = mModel.index(row, 0);

        QString path = mModel.data(index, mPathRole).value<QString>();


        mPixmapIndex[path] = index;
    }

    QObject::connect(&mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex, int)), this, SLOT(dataUpdated(QModelIndex,QModelIndex, int)),Qt::DirectConnection);
    QObject::connect(&mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(dataDeleted(QModelIndex,int,int)) );
    QObject::connect(&mModel, SIGNAL(modelAboutToBeReset()), this, SLOT(dataReset()), Qt::DirectConnection);

}


QImage GridIndexProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage image;    
    return image;

}

void GridIndexProvider::dataUpdated(const QModelIndex& topLeft, const QModelIndex& bottomRight, const int &cRow)
{

    if(mModel.clearing)
    {
        this->dataReset();
    }
    else
    {
        // For each pixmap already in the model, get a mapping between the name and the index
        for(int row = 0; row < mModel.rowCount(); row++) {
            QModelIndex index = mModel.index(row, 0);
            QString path = QVariant(mModel.data(index, mPathRole)).toString();
            mPixmapIndex.insert(path, index);
        }

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
}
