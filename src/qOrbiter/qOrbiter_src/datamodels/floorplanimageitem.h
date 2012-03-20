#ifndef FLOORPLANITEM_H
#define FLOORPLANITEM_H

/*
  this class is intended to manage the various aspects of floorplan images. it does not handle floorplan devices,
  but just the floorplan images themselves.
  */

#include <QObject>
#include <QVariant>
#include <QImage>
#include <QMap>

class FloorPlanItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int m_page READ page NOTIFY pageChanged)
    Q_PROPERTY(QString m_description READ description NOTIFY descChanged)
    enum Roles {
        InstallationRole = Qt::UserRole+1,
        DescriptionRole =Qt::UserRole+2,
        PageRole= Qt::UserRole+3,
        PathRole = Qt::UserRole+4,
        FloorPlanImageRole = Qt::UserRole+5,
        IconImageRole = Qt::UserRole+6,
        IdRole = Qt::UserRole+7
    };

public:
    FloorPlanItem() {}
    explicit FloorPlanItem( QString &installation, QString &description,  int &page, QString imgPath,  QObject *parent);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const {  return m_ident; }
    inline QString installation() const { return m_installation; }
    inline QString description() const { return m_description; }
    inline int page() const { return m_page; }
    inline QString icon() const { return m_iconPath; }
    inline QImage floorplanImage() const {  return m_floorplanImage; }
    inline QImage iconImg() const {  return m_iconImage; }
    inline QString imgPath() const {return m_imgPath;}

private:
    QString m_ident;
    QString m_installation;
    int m_page;
    QString m_description;
    QImage m_floorplanImage;
    QString m_iconPath;
    QImage m_iconImage;
    QString m_imgPath;

    QMap <int , QObject> devicelist ;

signals:
    void imageChanged();
    void dataChanged();
    void pageChanged();
    void descChanged();

};

#endif // FLOORPLANITEM_H
