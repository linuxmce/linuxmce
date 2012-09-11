#include "skindatamodel.h"
#include "qorbitermanager.h"
#if (QT5)
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>

#else
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeEngine>
#endif

#include <QtCore/QUrl>

SkinDataModel::SkinDataModel(QUrl &baseUrl, SkinDataItem* prototype, qorbiterManager *uiRef, QObject* parent): QAbstractListModel(parent), m_prototype(prototype)
{
    m_baseUrl = baseUrl ;
    uiRef->setDceResponse("Setting skin source: " + m_baseUrl.toString());
    setRoleNames(m_prototype->roleNames());
    qRegisterMetaType<QModelIndex>("QModelIndex");
    ui_reference = uiRef;
    SkinLoader* load = new SkinLoader(m_baseUrl, ui_reference, this);
    m_skin_loader = load;

    QObject::connect(m_skin_loader, SIGNAL(finishedList()), this, SLOT(checkStatus()));
}


int SkinDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant SkinDataModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

SkinDataModel::~SkinDataModel() {
    delete m_prototype;
    clear();
}

void SkinDataModel::appendRow(SkinDataItem *item)
{
    appendRows(QList<SkinDataItem*>() << item);
}

void SkinDataModel::appendRows(const QList<SkinDataItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+items.size()-1);
    foreach(SkinDataItem *item, items) {
        connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
        m_list.append(item);
    }
    endInsertRows();
}

void SkinDataModel::insertRow(int row, SkinDataItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.insert(row, item);
    endInsertRows();
}

void SkinDataModel::handleItemChange()
{
    SkinDataItem* item = static_cast<SkinDataItem*>(sender());
    QModelIndex index = indexFromItem(item);
    if(index.isValid())
        emit dataChanged(index, index);
}

SkinDataItem * SkinDataModel::find(const QString &id) const
{
    //qDebug()<< "Looking for skin..." << id;
    //qDebug() << m_list.size();
    foreach(SkinDataItem* item, m_list)
    {
        //qDebug() << "Skin Item" << item->id();
        if(item->id().toLower() == id.toLower())
        {
             //qDebug()<< "Found:" << item->id() << "of" << m_list.size();
            return item;
        }       
    }
         ui_reference->setDceResponse("SKIN ERROR");

         return 0;
}

QModelIndex SkinDataModel::indexFromItem(const SkinDataItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {
        if(m_list.at(row) == item) return index(row);
    }
    return QModelIndex();
}

void SkinDataModel::sortModel(int column, Qt::SortOrder order)
{
}

void SkinDataModel::clear()
{
    qDeleteAll(m_list.begin(), m_list.end());
    m_list.clear();
}

bool SkinDataModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool SkinDataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || (row+count) >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i) {
        delete m_list.takeAt(row);
    }
    endRemoveRows();
    return true;
}

SkinDataItem * SkinDataModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    SkinDataItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

void SkinDataModel::addSkin(QString name) {


    /*QImage skinPic(":/icons/Skin-Data.png");

    // Init the skin loader at this URL and then load the SkinDataItems from that SkinLoader


    qDebug() << "Loading Skin at " + m_baseUrl.toString() + "/" + name;
    QUrl skinBase(m_baseUrl.toString()  + "/" + name);
    //skinBase.setPath(m_baseUrl.toString());
   QUrl style;
   style.setUrl(m_baseUrl.toString() + "/" + name + "/Style.qml");

   if (!style.isValid()) {
       qDebug() << "Invalid URL!";
   }*/
    //qDebug() << "Loading skin : " << name;
    m_skin_loader->prepSkinsToLoad(name);

}

void SkinDataModel::setActiveSkin(QString name)
{
    ui_reference->setDceResponse("Loading Skin at " + m_baseUrl.toString() + "/" + name);

    QString skinURL = find(name)->path();
   //dir -l qDebug() << skinURL;
#if (QT5)
    QQmlComponent skinData(ui_reference->qorbiterUIwin->engine(), skinURL);
#else
    QDeclarativeComponent skinData(ui_reference->qorbiterUIwin->engine(), skinURL);
#endif
    QObject *styleObject = skinData.create(ui_reference->qorbiterUIwin->rootContext());
    if (skinData.isError()) {
        // this dir does not contain a Style.qml; ignore it
        ui_reference->setDceResponse(skinData.errors().last().toString());
        return;
    }
    else
    {
        //turning it into a qObject - this part actually loads it - the error should connect to a slot and not an exit

        //wait for it to load up
        while (!skinData.isReady())
        {
            if(skinData.isError())
            {
                ui_reference->setDceResponse(skinData.errors().last().toString());

                break;
            }


        }

    }
}

void SkinDataModel::checkStatus()
{
    qDebug("Finished loading skins") ;
        emit skinsFinished(true);
}
