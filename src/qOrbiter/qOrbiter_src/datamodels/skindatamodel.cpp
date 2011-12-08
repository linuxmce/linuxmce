#include "skindatamodel.h"
#include <QDebug>
#include <QDeclarativeComponent>
#include <QUrl>

SkinDataModel::SkinDataModel(QUrl &baseUrl, SkinDataItem* prototype, qorbiterManager *uiRef, QObject* parent): QAbstractListModel(parent), m_prototype(prototype)
{
    m_baseUrl = baseUrl ;

    setRoleNames(m_prototype->roleNames());
    qRegisterMetaType<QModelIndex>("QModelIndex");
    ui_reference = uiRef;
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
    foreach(SkinDataItem* item, m_list) {
        qDebug() << item->id();
        if(item->id().toLower() == id.toLower())
        {
            return item;
        }

    }
    return m_list.first();
}

QModelIndex SkinDataModel::indexFromItem(const SkinDataItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {
        if(m_list.at(row) == item) return index(row);
    }
    return QModelIndex();
}

void SkinDataModel::clear()
{
    qDeleteAll(m_list);
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

    QImage skinPic(":/icons/Skin-Data.png");

    qDebug() << "Loading Skin at " + m_baseUrl.toString() + "/" + name;
    QUrl skinBase(m_baseUrl.toString()  + "/" + name);

    //skinBase.setPath(m_baseUrl.toString());

    QUrl style(m_baseUrl);
    style.setPath(m_baseUrl.toString()+ "/" + name + "/Style.qml");
    QObject *styleObject = new QObject();
    QDeclarativeComponent skinData(ui_reference->qorbiterUIwin->engine(), style);

    if (!skinData.isError())
    {
        //wait for it to load up
        while (!skinData.isReady())
        {
            if(skinData.isError())
            {
                qDebug() << skinData.errors();
                qDebug("Error!");
                break;
            }
qDebug() << skinData.status();

        }

        styleObject = skinData.create(ui_reference->qorbiterUIwin->rootContext());
    }
    else
    {            // this dir does not contain a Style.qml; ignore it
        qDebug() << skinData.errors();
        return;
    }

    //Importing the document data for the skins themselves
    QString s_title = styleObject->property("skinname").toString();
    QString s_creator = styleObject->property("skincreator").toString();
    QString s_description = styleObject->property("skindescription").toString();
    QString s_version = styleObject->property("skinversion").toString();
    QString s_target = styleObject->property("skinvariation").toString();
    QString s_path = styleObject->property("skindir").toString();
    QString s_mainc = styleObject->property("maincolor").toString();
    QString s_accentc = styleObject->property("accentcolor").toString();
    // qDebug() << "Adding skin to list" << s_title;
    appendRow(new SkinDataItem(skinBase, s_title, s_creator, s_description, s_version, s_target, skinPic, s_path, s_mainc, s_accentc, this));
}


void SkinDataModel::setActiveSkin(QString name)
{
    qDebug() << "Loading Skin at " + m_baseUrl.toString() + "/" + name;

    QString skinURL = find(name)->path();
    qDebug() << skinURL;

    QDeclarativeComponent skinData(ui_reference->qorbiterUIwin->engine(), skinURL);
    QObject *styleObject = skinData.create(ui_reference->qorbiterUIwin->rootContext());
    if (skinData.isError()) {
        // this dir does not contain a Style.qml; ignore it
        qDebug() << skinData.errors();
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
                qDebug() << skinData.errors();
                qDebug("Error!");
                break;
            }

            qDebug() << " loading";
        }

    }
}
