#include "genericmodelitem.h"
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


QHash<int, QByteArray> GenericModelItem::roleNames() const
{

  QHash<int, QByteArray> names;
  return names;
}

QVariant GenericModelItem::data(int role) const
{
  switch(role) {
   default:
    return QVariant();
  }
}
