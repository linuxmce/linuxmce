#include "avcommand.h"

AvCommand::AvCommand(int inc_command, QString command_name, bool state, int iParent, QObject *parent) :
    i_commandnum(inc_command), qs_command(command_name), learning(false), i_parentDevice(iParent), QObject(parent)
{
}
