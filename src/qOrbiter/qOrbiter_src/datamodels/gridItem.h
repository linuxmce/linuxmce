#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>
#include <QDebug>
/*!
 * \brief The gridItem class is the item class associated with ListModel class.
 * Its purpose is to serve as a representation of the data contaied in a single object.
 */
class gridItem: public QObject
{
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::DisplayRole+1,
        IndexRole =Qt::DisplayRole+2,
        PathRole= Qt::DisplayRole+3,
        ImageRole = Qt::DisplayRole+4,
        AttributeRole = Qt::DisplayRole+5,
        FKRole = Qt::DisplayRole+6,
        AspectRole= Qt::DisplayRole+7,
        TrackerRole=Qt::DisplayRole+8
    };

    gridItem() {}
    explicit gridItem( QString ident, QString name,  QString path,  int index, QObject *parent=0 );


    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const {  return m_fk_file; }
    inline QString name() const { return m_name; }
    inline int index() const { return m_index; }
    inline QString path() const { return m_path; }
    inline QMap <QString*, int> attributes() const {return m_mapAttrib;}
    inline QString imgAspect () const {return m_aspect;}
    inline bool pinged () const {return sorted;}
    void setAspect (QString qs_aspect) {m_aspect = qs_aspect;}
    void setPing (bool ping) {sorted = ping; emit dataChanged();}
    void emitPing(){emit ping();}


public slots:
    void destruct() {
    //  qDebug() << "calling destructor for GridItem in " << this->thread();
       this->deleteLater();
    }

private:
    QString m_name;
    int m_index;
    QString m_path;
    QString m_fk_file;
    QString m_aspect;
    QMap <QString*, int> m_mapAttrib;
    bool sorted;

signals:
    void imageChanged();
    void dataChanged();
    void ping();
};
#endif // GRIDITEM_H
