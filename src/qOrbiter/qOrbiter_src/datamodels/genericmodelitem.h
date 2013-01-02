#ifndef GENERICMODELITEM_H
#define GENERICMODELITEM_H

#include <QObject>
#include <QHash>
#include <QVariant>

class GenericModelItem : public QObject
{
    Q_OBJECT
public:
    GenericModelItem(){}
    explicit GenericModelItem(QObject *parent = 0);
    QVariant data(int role) const;
    inline QString id() const {  return ident; }
    QHash<int, QByteArray> roleNames() const;

    QString ident;
    
signals:
    void imageChanged();
    void dataChanged();
    
public slots:
    
};

#endif // GENERICMODELITEM_H
