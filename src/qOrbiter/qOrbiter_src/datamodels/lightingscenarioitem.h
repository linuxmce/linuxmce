#ifndef LIGHTINGSCENARIOITEM_H
#define LIGHTINGSCENARIOITEM_H

#include <datamodels/listModel.h>
#include <QImage>

/*!
 * \brief The LightingScenarioItem class
 * \ingroup data_model
 */
class LightingScenarioItem: public QObject
{
         Q_OBJECT
        // Q_PROPERTY(QImage buttonImage READ buttonImage NOTIFY imageChanged)

        public:
          enum Roles {
            NameRole = Qt::UserRole+1,
            LabelRole,
            ParamRole,
            CommandRole,
            GotoRole,
            ImageRole
          };

        public:

          LightingScenarioItem() {}
          explicit LightingScenarioItem( const QString &name,  const QString &title, const  QString &parameter, const QString &scommand, const  QString &gotoScreen , QImage img, QObject *parent=0);

          QVariant data(int role) const;
          QHash<int, QByteArray> roleNames() const;


          inline QString id() const { return m_name; }
          inline QString title() const { return m_title; }
          inline QString params() const { return m_param; }
          inline QString command() const { return m_command; }
          inline QString gotoCommand() const { return m_goto; }
          inline QImage buttonImage() const {return m_image; }


        private:
          QString m_name;
          QString m_title;
          QString m_param;
          QString m_command;
          QString m_goto;
          QImage m_image;

        signals:
          void imageChanged();

public slots:
          void destruct() {
              qDebug() << "calling destructor for item.";
             this->deleteLater();
          }

};

#endif // LIGHTINGSCENARIOITEM_H
