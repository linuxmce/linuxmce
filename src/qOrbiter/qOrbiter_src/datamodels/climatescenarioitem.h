/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

    This class is another specific model/item pair with the parent being climatescenariomodel It is implemented as a specific class
    as opposed to abstracted because each home automation category is unique, and it seemed more logical to provide custom classes
    for the QAbstractlistmodel base than further abstracting that model interface.

*/
#ifndef CLIMATESCENARIOITEM_H
#define CLIMATESCENARIOITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>

/*!
 * \brief The ClimateScenarioItem class
 * \ingroup data_model
 */
class ClimateScenarioItem: public QObject{
         Q_OBJECT
        // Q_PROPERTY(QImage buttonImage READ buttonImage NOTIFY imageChanged)

        public:
    /*!
           * \brief The Climate Roles enum defines the qml properties for listmodels.
           * \ingroup data_model
           */
          enum Roles {
            NameRole = Qt::UserRole+1, /*!< 'username' - String username */
            LabelRole,/*!< 'title' - String name of climate scenario item.  */
            ParamRole, /*!< 'params' - String of parameters associated with this climate item.  */
            CommandRole,/*!< 'command' - The command or command group associated with this item. */
            GotoRole, /*!< 'gotoscree' - The screen to goto - depreciated.  */
            ImageRole /*!< image - String of the image name.  */
          };

        public:

          ClimateScenarioItem() {}
          explicit ClimateScenarioItem( const QString &name,  const QString &title, const  QString &parameter, const QString &scommand, const  QString &gotoScreen , QImage img, QObject *parent=0);

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

};

#endif // CLIMATESCENARIOITEM_H
