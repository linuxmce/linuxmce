#include "huebridge.h"

HueBridge::HueBridge(Hue_Controller* dceObject, QObject *parent) :
    QObject(parent), mp_dceController(dceObject)
{
}
