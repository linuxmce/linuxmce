#ifndef ATTRIBUTESORTITEM_H
#define ATTRIBUTESORTITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

//for use with the fileformat table.
class AttributeSortItem: public QObject
{
    Q_OBJECT

public:
    enum Roles {
        IndexRole = Qt::UserRole+1,
        NameRole =Qt::UserRole+2,
        PKRole= Qt::UserRole+3,
        ImageRole = Qt::DisplayRole+4,
        StateRole = Qt::DisplayRole+5
    };

public:
    AttributeSortItem() {}
    explicit AttributeSortItem( QString name,  QString pk_fileformat,  QImage img, bool selstat, QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    QString id() const {  return m_description; }
    QString name() const {  return m_description; }
    QString fileformat() const { return m_fk_fileformat; }
    QImage cellImage() const {  return m_image; }
    bool selectedStatus() const { return m_isSelected; }
    bool m_isSelected;
    QString m_fk_fileformat;
    QString m_description;
    QImage m_image;

signals:
    void imageChanged();
    void dataChanged();


public slots:
    bool setStatus(bool b) { m_isSelected = b; emit dataChanged();}
    bool updateSelection (bool newBool) ;
};
#endif // ATTRIBUTESORTITEM_H
