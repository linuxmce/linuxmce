#include "avcommand.h"
/*!
 *\class AvCommand
 *\brief This class represents the encapsulation for an individual av command.
 *
 *\ingroup qml_api
 *
 * The AvCommand represents the individual cells that appear in the AvCommand list
 *
 *\sa QObject
 *
 */

AvCommand::AvCommand(int inc_command, QString command_name, bool state, int iParent, QObject *parent) :
    i_commandnum(inc_command), qs_command(command_name), learning(false), i_parentDevice(iParent), QObject(parent)
{

}


QHash<int, QByteArray> AvCommand::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[ParentRole] = "parent";
  names[StatusRole] = "learningstate";
  names[CommandRole] = "commandnumber";
  return names;
}

QVariant AvCommand::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case ParentRole:
    return cmdParent();
  case StatusRole:
    return learnState();
  case CommandRole:
    return command();
  default:
    return QVariant();
  }
}
