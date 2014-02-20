#include "genericmodelitem.h"
#include "DCE/Logger.h"
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
