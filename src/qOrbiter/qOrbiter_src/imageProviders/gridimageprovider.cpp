#include "gridimageprovider.h"
#include "qorbitermanager.h"
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

    connect(&mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex, int)), this, SLOT(dataUpdated(QModelIndex,QModelIndex, int)), Qt::DirectConnection);
    connect(&mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(dataDeleted(QModelIndex,int,int)));
    connect(&mModel, SIGNAL(modelReset()), this, SLOT(dataReset()));
}


QImage GridIndexProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{

        QString key = QString("image://datagridimg/%1").arg(id);
        QModelIndex index = mPixmapIndex.value(id);
      //  QImage image = mModel.data(index, mPixmapRole).value<QImage>();
        QImage image = mModel.data(index, 4).value<QImage>();

        if (image.isNull())
        {
             image.load(":/icons/mediacell.png");
        }


        QImage result;
        if (image.height() < image.width())
        {
           mModel.find(id)->setAspect("poster");
        }
        else
        {
             mModel.find(id)->setAspect("wide");
        }

        if (requestedSize.isValid()) {
            result = image.scaled(requestedSize);
        } else {
            result = image;
        }


       // *size = result.size();
        return result;

}

void GridIndexProvider::dataUpdated(const QModelIndex& topLeft, const QModelIndex& bottomRight, const int &cRow)
{

    // For each pixmap already in the model, get a mapping between the name and the index
    for(int row = 0; row < mModel.rowCount(); row++) {     
        QModelIndex index = mModel.index(row, 0);
        QString path = QVariant(mModel.data(index, mPathRole)).toString();
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
