#include "routerhelper.h"
#include <qOrbiter/qOrbiter.h>
#include "qmap.h"
#include "QMapIterator"
#include "qdebug.h"
#include "pluto_main/Define_DeviceTemplate.h"

RouterHelper::RouterHelper(QObject *qorbiter_ptr, QObject *parent) : QObject(parent)
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
}
int RouterHelper::climatePluginId() const
{
    return m_climatePluginId;
}

void RouterHelper::setClimatePluginId(int climatePluginId)
{
    m_climatePluginId = climatePluginId;
}
int RouterHelper::dataGridPluginId() const
{
    return m_dataGridPluginId;
}

void RouterHelper::setDataGridPluginId(int dataGridPluginId)
{
    m_dataGridPluginId = dataGridPluginId;
}
int RouterHelper::infraredPluginId() const
{
    return m_infraredPluginId;
}

void RouterHelper::setInfraredPluginId(int infraredPluginId)
{
    m_infraredPluginId = infraredPluginId;
}
int RouterHelper::lightingPluginId() const
{
    return m_lightingPluginId;
}

void RouterHelper::setLightingPluginId(int lightingPluginId)
{
    m_lightingPluginId = lightingPluginId;
}
int RouterHelper::orbiterPluginId() const
{
    return m_orbiterPluginId;
}

void RouterHelper::setOrbiterPluginId(int orbiterPluginId)
{
    m_orbiterPluginId = orbiterPluginId;
}
int RouterHelper::mediaPluginId() const
{
    return m_mediaPluginId;
}

void RouterHelper::setMediaPluginId(int mediaPluginId)
{
    m_mediaPluginId = mediaPluginId;
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
}

void RouterHelper::handleCoreDevicesChanged(QMap<long, long> cd)
{
    qDebug() << Q_FUNC_INFO;

    QMapIterator<long, long> it(cd);
    while(it.hasNext()){

        switch (it.key()) {
        case DEVICETEMPLATE_Media_Plugin_CONST:         setMediaPluginId(it.value()); break;
        case DEVICETEMPLATE_Lighting_Plugin_CONST:      setLightingPluginId(it.value()); break;
        case DEVICETEMPLATE_General_Info_Plugin_CONST:  setGeneralInfoPluginId(it.value()); break;
        case DEVICETEMPLATE_Climate_Plugin_CONST:       setClimatePluginId(it.value());  break;
        case DEVICETEMPLATE_Orbiter_Plugin_CONST:       setOrbiterPluginId(it.value());  break;
        case DEVICETEMPLATE_Datagrid_Plugin_CONST:      setDataGridPluginId(it.value());  break;
        case DEVICETEMPLATE_Infrared_Plugin_CONST:      setInfraredPluginId(it.value());  break;
        case DEVICETEMPLATE_DCERouter_CONST:            setDceRouterId(it.value());  break;
        default:
            qWarning() << QString("Failed to handle core device; DeviceTemplate:%1 and id:%2 ").arg(it.key()).arg(it.value());
            break;
        }
    }

}










