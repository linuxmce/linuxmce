#include "avcommand.h"

AvCommand::AvCommand(int inc_command, QString command_name, bool state, QObject *parent) :
    i_commandnum(inc_command), qs_command(command_name), learning(false), QObject(parent)
{
}
