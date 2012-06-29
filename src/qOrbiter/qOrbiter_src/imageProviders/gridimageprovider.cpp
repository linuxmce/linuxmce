#include "gridimageprovider.h"
#include "qorbitermanager.h"


GridIndexProvider::GridIndexProvider(ListModel *model  , int pathRole, int pixmapRole) :
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QQmlImageProvider(QQmlImageProvider::Image), mModel(*model),  mPathRole(pathRole), mPixmapRole(pixmapRole)
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

    QString key = QString("image://datagridimg/%1").arg(id);
    QModelIndex index = mPixmapIndex.value(id);
    //  QImage image = mModel.data(index, mPixmapRole).value<QImage>();
    QImage image = mModel.data(index, 4).value<QImage>();

    if (image.isNull())
    {
        image.load(":/icons/icon.png");
    }
    else
    {
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
    }


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
    this->deleteLater();
}
