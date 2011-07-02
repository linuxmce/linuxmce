#include "lightingscenarioitem.h"

LightingScenarioItem:: LightingScenarioItem( QString name,  QString label,  QString parameter,  QString scommand,  QString gotoScreen , QImage img, QObject *parent):
    ListItem(parent),m_name(name), m_label(label), m_param(parameter), m_command(scommand), m_goto(gotoScreen), m_image(img)
{

}

QHash<int, QByteArray> LightingScenarioItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "username";
  names[ParamRole] = "params";
  names[CommandRole] = "command";
  names[GotoRole] = "gotoscreen";
  names[ImageRole] = "image";
  names[LabelRole] = "label";
  return names;
}

/* NameRole = Qt::UserRole+1,
LabelRole,
ParamRole,
CommandRole,
GotoRole,
ImageRole */

QVariant LightingScenarioItem::data(int role) const
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
