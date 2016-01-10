#ifndef ATTRIBUTEOBJECT_H
#define ATTRIBUTEOBJECT_H

#include <QObject>

class AttributeObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int a_id READ getIdent WRITE setIdent NOTIFY identChanged )
public:
    explicit AttributeObject(int id, QString name, QObject *parent = 0);
    QString name;
    int a_id;
    
signals:
    void nameChanged();
    void identChanged();
    
public slots:
    void setName(QString n) { n = name; emit nameChanged();}
    QString getName() {return name;}

    void setIdent(int id){a_id = id; emit identChanged();}
    int getIdent() {return a_id;}
    
};

#endif // ATTRIBUTEOBJECT_H
