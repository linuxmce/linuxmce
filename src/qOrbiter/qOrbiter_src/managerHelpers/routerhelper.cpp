#include "routerhelper.h"
#include <qOrbiter/qOrbiter.h>
#include "qmap.h"
#include "QMapIterator"
#include "qdebug.h"
#include "pluto_main/Define_DeviceTemplate.h"

RouterHelper::RouterHelper(QObject *qorbiter_ptr, QObject *parent) : QObject(parent),
    m_generalInfoPluginId(-1), m_climatePluginId(-1), m_dataGridPluginId(-1), m_infraredPluginId(-1),
    m_lightingPluginId(-1), m_securityPluginId(-1), m_eventPluginId(-1), m_orbiterPluginId(-1), m_mediaPluginId(-1),
    m_mythTvPluginId(-1), m_dceRouterId(-1), m_telecomPluginId(-1)
{
    qOrbiter*ptr = qobject_cast<qOrbiter*>(qorbiter_ptr);
    connect(ptr, &qOrbiter::coreDevicesChanged, this, &RouterHelper::handleCoreDevicesChanged, Qt::QueuedConnection);
    //  m_manager = qobject_cast<qorbiterManager*>(parent);
}
int RouterHelper::generalInfoPluginId() const
{
    return m_generalInfoPluginId;
}

void RouterHelper::setGeneralInfoPluginId(int generalInfoPluginId)
{
    m_generalInfoPluginId = generalInfoPluginId;
    emit generalInfoPluginIdChanged();
}
int RouterHelper::climatePluginId() const
{
    return m_climatePluginId;
}

void RouterHelper::setClimatePluginId(int climatePluginId)
{
    m_climatePluginId = climatePluginId;
    emit climatePluginIdChanged();
}
int RouterHelper::dataGridPluginId() const
{
    return m_dataGridPluginId;
}

void RouterHelper::setDataGridPluginId(int dataGridPluginId)
{
    m_dataGridPluginId = dataGridPluginId;
    emit dataGridPluginIdChanged();
}
int RouterHelper::infraredPluginId() const
{
    return m_infraredPluginId;
}

void RouterHelper::setInfraredPluginId(int infraredPluginId)
{
    m_infraredPluginId = infraredPluginId;
    emit infraredPluginIdChanged();
}
int RouterHelper::lightingPluginId() const
{
    return m_lightingPluginId;
}

void RouterHelper::setLightingPluginId(int lightingPluginId)
{
    m_lightingPluginId = lightingPluginId;
    emit lightingPluginIdChanged();
}
int RouterHelper::orbiterPluginId() const
{
    return m_orbiterPluginId;
}

void RouterHelper::setOrbiterPluginId(int orbiterPluginId)
{
    m_orbiterPluginId = orbiterPluginId;
    emit orbiterPluginIdChanged();
}
int RouterHelper::mediaPluginId() const
{
    return m_mediaPluginId;
}

void RouterHelper::setMediaPluginId(int mediaPluginId)
{
    m_mediaPluginId = mediaPluginId;
    emit mediaPluginIdChanged();
}
int RouterHelper::mythTvPluginId() const
{
    return m_mythTvPluginId;
}

void RouterHelper::setMythTvPluginId(int mythTvPluginId)
{
    m_mythTvPluginId = mythTvPluginId;

}
int RouterHelper::dceRouterId() const
{
    return m_dceRouterId;
}

void RouterHelper::setDceRouterId(int dceRouterId)
{
    m_dceRouterId = dceRouterId;
    emit dceRouterIdChanged();
}

int RouterHelper::eventPluginId() const
{
    return m_eventPluginId;
}

void RouterHelper::setEventPluginId(int eventPluginId)
{
    m_eventPluginId = eventPluginId;
    emit eventPluginIdChanged();
}

int RouterHelper::securityPluginId() const
{
    return m_securityPluginId;
}

void RouterHelper::setSecurityPluginId(int securityPluginId)
{
    m_securityPluginId = securityPluginId;
    emit securityPluginIdChanged();
}

int RouterHelper::telecomPluginId() const
{
    return m_telecomPluginId;
}

void RouterHelper::setTelecomPluginId(int telecomPluginId)
{
    m_telecomPluginId = telecomPluginId;
    emit telecomPluginIdChanged();
}


void RouterHelper::handleCoreDevicesChanged(QMap<long, long> cd)
{
    QMapIterator<long, long> it(cd);
    while(it.hasNext()){
        it.next();
        switch (it.key()) {
        case DEVICETEMPLATE_Media_Plugin_CONST:         setMediaPluginId(it.value()); break;
        case DEVICETEMPLATE_Lighting_Plugin_CONST:      setLightingPluginId(it.value()); break;
        case DEVICETEMPLATE_General_Info_Plugin_CONST:  setGeneralInfoPluginId(it.value()); break;
        case DEVICETEMPLATE_Climate_Plugin_CONST:       setClimatePluginId(it.value());  break;
        case DEVICETEMPLATE_Orbiter_Plugin_CONST:       setOrbiterPluginId(it.value());  break;
        case DEVICETEMPLATE_Datagrid_Plugin_CONST:      setDataGridPluginId(it.value());  break;
        case DEVICETEMPLATE_Infrared_Plugin_CONST:      setInfraredPluginId(it.value());  break;
        case DEVICETEMPLATE_DCERouter_CONST:            setDceRouterId(it.value());  break;
        case DEVICETEMPLATE_Security_Plugin_CONST:      setSecurityPluginId(it.value());  break;
        case DEVICETEMPLATE_Event_Plugin_CONST:         setEventPluginId(it.value());  break;
        case DEVICETEMPLATE_Telecom_Plugin_CONST:       setTelecomPluginId(it.value());  break;
        default:
            qWarning() << QString("Failed to handle core device; DeviceTemplate:%1 and id:%2 ").arg(it.key()).arg(it.value());
            break;
        }
    }

}










