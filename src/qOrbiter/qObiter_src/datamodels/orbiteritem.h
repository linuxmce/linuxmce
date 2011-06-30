#ifndef ORBITERITEM_H
#define ORBITERITEM_H

#include <datamodels/listModel.h>
#include <QImage>
#include <QVariant>


class OrbiterItem : public ListItem
{
   Q_OBJECT
   Q_PROPERTY(QImage image READ image NOTIFY imageChanged)

  public:
    enum Roles {
      NameRole = Qt::UserRole+1, //id
      SizeRole,
      CommandRole,
      TitleRole,
      ImageRole

    };

public:
    OrbiterItem();
  public:
    OrbiterItem(QObject *parent = 0): ListItem(parent) {}
    explicit OrbiterItem(const QString &name, const QString &size, const QImage &img, QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;
    void setPrice(qreal price);
    inline QString id() const { return m_ident; }
    inline QString name() const { return m_name; }
    inline QString size() const { return m_size; }
    inline QImage image() const {return m_image; }
    inline QString title() const {return m_title; }
    inline QString command() const { return m_command; }
    void setCommand(QString);

  private:
    QString m_ident;
    QString m_name;
    QString m_title;
    QString m_size;
    QString m_command;
    QImage m_image;

  signals:
    void imageChanged();
  };


#endif // ORBITERITEM_H
