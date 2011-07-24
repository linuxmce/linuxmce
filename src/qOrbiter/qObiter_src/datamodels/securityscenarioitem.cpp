#include "securityscenarioitem.h"


SecurityScenarioItem:: SecurityScenarioItem( const QString &name, const QString &label, const QString &parameter, const QString &scommand,  const QString &gotoScreen , QImage img, QObject *parent):
   m_name(name), m_label(label), m_param(parameter), m_command(scommand), m_goto(gotoScreen), m_image(img)
{

}

QHash<int, QByteArray> SecurityScenarioItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "username";
  names[LabelRole] = "label";
  names[ParamRole] = "params";
  names[CommandRole] = "command";
  names[GotoRole] = "gotoscreen";
  names[ImageRole] = "image";

  return names;
}


QVariant SecurityScenarioItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return id();
  case LabelRole:
    return label();
  case ParamRole:
    return params();
  case CommandRole:
    return command();
  case GotoRole:
    return gotoCommand();
  case ImageRole:
    return buttonImage();
  default:
    return QVariant();
  }
}
