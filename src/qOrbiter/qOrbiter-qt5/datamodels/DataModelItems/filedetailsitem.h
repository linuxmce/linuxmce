#ifndef FILEDETAILSITEM_H
#define FILEDETAILSITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

//for use with the fileformat table.
class FileDetailsItem: public QObject
{
    Q_OBJECT
    Q_PROPERTY (bool m_isSelected READ selectedStatus WRITE updateSelection NOTIFY filterChanged)

public:
    enum Roles {
        IndexRole = Qt::UserRole+1,
        NameRole =Qt::UserRole+2,
        DataRole= Qt::UserRole+3,
        ImageRole = Qt::DisplayRole+4,
        StateRole = Qt::DisplayRole+5
    };

public:
    FileDetailsItem() {}
    explicit FileDetailsItem( QString title,  QString data,  QImage img, bool selstat, QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    QString id() const {  return m_title; }
    QString title() const {  return m_attrib_data; }
    QString attribute_data() const { return m_attrib_data; }
    QImage cellImage() const {  return m_image; }
    bool selectedStatus() const { return m_isSelected; }
    bool setStatus(bool b) { m_isSelected = b; }
    void updateSelection (bool newBool) ;

private:

    bool m_isSelected;
    QString m_title;
    QString m_attrib_data;
    QImage m_image;

signals:
    void imageChanged();
    void dataChanged();
    void filterChanged();
};

#endif // FILEDETAILSITEM_H
