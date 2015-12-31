#include "genericmodelitem.h"
#include "DCE/Logger.h"
#include <QDebug>
using namespace DCE;
/*!
 *\class GenericModelItem
 *\brief This class is a generic flat C++ listmodel item for use with qml
 *
 *\ingroup qt_controllers
 *
 *This class is an abstracted item class for use of with the \ref GenericFlatListModel.
 *
 *\sa QObject
 *
 */
GenericModelItem::GenericModelItem(QObject *parent) :
    QObject(parent)
{
}

QVariant GenericModelItem::data(int role) const
{

    if (m_data.contains(role)) {
        return m_data[role];
    } else {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "GenericModelItem.data : no such data role");
        return QVariant();
    }
}

bool GenericModelItem::setData(int role, const QVariant &value)
{
  //    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericModelItem.setData");
    if(!m_roleNames.contains(role)){
        qDebug() << "Data Role ";
        return false;
    }


    m_data.insert(role, value);
    emit dataChanged();
    return true;
}

bool GenericModelItem::updateData(int role, const QVariant &value)
{
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericModelItem.updateData");
    m_data.insert(role, value);
    emit dataChanged();
    return true;
}

void GenericModelItem::LoadData(QHash<int, QVariant>* pData)
{
  foreach (int i, pData->keys()) {
    m_data.insert(i, pData->value(i));
  }
  delete pData;
}
