#ifndef CLIMATESCENARIOITEM_H
#define CLIMATESCENARIOITEM_H
#include <datamodels/listModel.h>
#include <QImage>

class ClimateScenarioItem: public ListItem
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

          ClimateScenarioItem(QObject *parent = 0): ListItem(parent) {}
          explicit ClimateScenarioItem( const QString &name,  const QString &label, const  QString &parameter, const QString &scommand, const  QString &gotoScreen , QImage img, QObject *parent=0);

          QVariant data(int role) const;
          QHash<int, QByteArray> roleNames() const;


          inline QString id() const { return m_name; }
          inline QString label() const { return m_label; }
          inline QString params() const { return m_param; }
          inline QString command() const { return m_command; }
          inline QString gotoCommand() const { return m_goto; }
          inline QImage buttonImage() const {return m_image; }


        private:
          QString m_name;
          QString m_label;
          QString m_param;
          QString m_command;
          QString m_goto;
          QImage m_image;

        signals:
          void imageChanged();

};

#endif // CLIMATESCENARIOITEM_H
