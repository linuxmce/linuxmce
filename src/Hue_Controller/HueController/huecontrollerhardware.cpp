#include "huecontrollerhardware.h"

HueControllerHardware::HueControllerHardware(QObject *parent) :
    QObject(parent)
{
    m_ipAddress= "";
    m_macAddress= "";
    m_name="";
    m_phillips_id="";
    m_userName="";
    linkButtonState = false;
    portalEnabled= false;
}
