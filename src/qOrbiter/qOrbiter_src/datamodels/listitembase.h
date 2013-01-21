#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include <QObject>
#include <QVariant>
#include <QHash>

class ListItemBase : public QObject
{
    Q_OBJECT
public:

    explicit ListItemBase(QObject *parent = 0);
    
    virtual QVariant data(int role) const { return QVariant();}



    void emitPing();

signals:
    void ping();
    
public slots:
    
};

#endif // LISTITEMBASE_H
