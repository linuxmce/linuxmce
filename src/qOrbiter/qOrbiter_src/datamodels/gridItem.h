#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

class gridItem: public QObject
{
    Q_OBJECT
    enum Roles {
        NameRole = Qt::UserRole+1,
        IndexRole =Qt::UserRole+2,
        PathRole= Qt::DisplayRole+3,
        ImageRole = Qt::DisplayRole+4,
        AttributeRole = Qt::UserRole+5,
        FKRole = Qt::DisplayRole+6,
        AspectRole= Qt::DisplayRole+7
    };

public:
    gridItem() {}
    explicit gridItem( QString &ident, QString &name,  QString &path,  int &index,  QImage &img);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const {  return m_fk_file; }
    inline QString name() const { return m_name; }
    inline int index() const { return m_index; }
    inline QString path() const { return m_path; }
    inline QImage cellImage() const {  return m_image; }
    inline QMap <QString*, int> attributes() const {return m_mapAttrib;}
    inline QString imgAspect () const {return m_aspect;}
    void setAspect (QString qs_aspect) {m_aspect = qs_aspect;}


private:
    QString m_name;
    int m_index;
    QString m_path;
    QImage m_image;
    QString m_fk_file;
    QString m_aspect;
    QMap <QString*, int> m_mapAttrib;

signals:
    void imageChanged();
    void dataChanged();
};
#endif // GRIDITEM_H
