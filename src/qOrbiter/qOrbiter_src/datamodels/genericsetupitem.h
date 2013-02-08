#ifndef GenericSetupItem_H
#define GenericSetupItem_H

#include <QAbstractItemModel>
#include <QImage>

class GenericSetupItem : public QObject
{
    Q_OBJECT

   public:
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
     GenericSetupItem(QObject *parent = 0) {}
     explicit GenericSetupItem( QString &ident, QString &name,  QString &path,  int &index,  QImage &img, QObject *parent = 0);
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



};

#endif // GenericSetupItem_H
