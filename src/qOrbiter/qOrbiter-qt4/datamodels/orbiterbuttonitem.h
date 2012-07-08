#ifndef ORBITERBUTTONITEM_H
#define ORBITERBUTTONITEM_H

#include  <datamodels/listModel.h>
#include <QImage>

class OrbiterButtonItem : public QObject {
    Q_OBJECT

    Q_PROPERTY(QImage cellImage READ cellImage NOTIFY imageChanged)

   public:
     enum Roles {
       NameRole = Qt::UserRole+1,
       SizeRole,
       GotoRole,
       TitleRole,
       ImageRole,
       DisplayRole,
       AttributeRole

     };

   public:
     OrbiterButtonItem(QObject *parent = 0) {}
     OrbiterButtonItem(const QString &name, const QString &size,  const QString &goto_screen, const QImage &img, QObject *parent = 0);
     QVariant data(int role) const;
     QHash<int, QByteArray> roleNames() const;
     void setPrice(qreal price);
     inline QString id() const { return m_name; }
     inline QString name() const { return m_name; }
     inline QString size() const { return m_size; }
     inline QImage cellImage() const {return m_image; }
     inline QString goto_data() const { return m_goto; }
     inline QString display() const { return m_display; }
     inline QString attribute() const { return m_attribribute; }

   private:
     QString m_name;
     QString m_size;
     QString m_goto;
     QImage m_image;
     QString m_display;
     QString m_attribribute;

   signals:
     void imageChanged();

};

#endif // ORBITERBUTTONITEM_H
