#include "attributeobject.h"

AttributeObject::AttributeObject(int id, QString name, QObject *parent) :
    QObject(parent) , name(name), a_id(id)
{
}
