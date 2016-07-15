#ifndef GENERICMODELITEM_H
#define GENERICMODELITEM_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include "qdebug.h"
class GenericModelItem : public QObject
{
    Q_OBJECT

private:
    QHash<int, QByteArray> m_roleNames;
protected:
    QHash<int, QVariant> m_data;
public:
    explicit GenericModelItem(QObject *parent = 0);
    virtual QVariant data(int role) const;
    virtual bool setData(int role, const QVariant &value);
    virtual bool updateData(int role, const QVariant &value);
    virtual void LoadData(QHash<int, QVariant>* data);

    inline QString id() const {  return ident; }
    QHash<int, QByteArray> roleNames() const { return m_roleNames; }
    void setRoleNames(QHash<int, QByteArray> roleNames) { m_roleNames = roleNames;/* qDebug() <<roleNames;*/ }

    QString ident;
    
signals:
    void imageChanged();
    void dataChanged();
    
public slots:
    
};

#endif // GENERICMODELITEM_H
