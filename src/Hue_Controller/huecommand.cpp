#include "huecommand.h"
#include "qdebug.h"
HueCommand::HueCommand(QObject *parent) : QObject(parent)
{

}

HueCommand::HueCommand(QUrl target_url, QVariant target_params, QObject *parent) :
QObject(parent),
target(target_url),
parameters(target_params)
{
qDebug() << Q_FUNC_INFO <<" Created new Hue Command for dispatch\n" << target.toString() <<"\n"<<parameters;
}

