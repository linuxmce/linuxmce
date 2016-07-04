#ifndef ROUTERHELPER_H
#define ROUTERHELPER_H

#include <QObject>
#include "qmap.h"
/*!
 * \brief The RouterHelper class. Purely for holding references to dceROUTER related information, cached on the GUI side for expediency.
 * most of this is set once, but it should not be hardcoded.
 */


class RouterHelper : public QObject
{
    Q_OBJECT
public:
    explicit RouterHelper(QObject* qorbiter_ptr, QObject *parent = 0);

    int generalInfoPluginId() const;
    void setGeneralInfoPluginId(int generalInfoPluginId);

    int climatePluginId() const;
    void setClimatePluginId(int climatePluginId);

    int dataGridPluginId() const;
    void setDataGridPluginId(int dataGridPluginId);

    int infraredPluginId() const;
    void setInfraredPluginId(int infraredPluginId);

    int lightingPluginId() const;
    void setLightingPluginId(int lightingPluginId);

    int orbiterPluginId() const;
    void setOrbiterPluginId(int orbiterPluginId);

    int mediaPluginId() const;
    void setMediaPluginId(int mediaPluginId);

    int mythTvPluginId() const;
    void setMythTvPluginId(int mythTvPluginId);

    int dceRouterId() const;
    void setDceRouterId(int dceRouterId);

    int securityPluginId() const;
    void setSecurityPluginId(int securityPluginId);

    int eventPluginId() const;
    void setEventPluginId(int eventPluginId);

    int telecomPluginId() const;
    void setTelecomPluginId(int telecomPluginId);

signals:

    void generalInfoPluginIdChanged();
    void climatePluginIdChanged();
    void dataGridPluginIdChanged();
    void infraredPluginIdChanged();
    void lightingPluginIdChanged();
    void securityPluginIdChanged();
    void eventPluginIdChanged();
    void orbiterPluginIdChanged();
    void mediaPluginIdChanged();
    void dceRouterIdChanged();
    void telecomPluginIdChanged();
public slots:

    void handleCoreDevicesChanged(QMap<long, long> cd);

private:

    int m_generalInfoPluginId;
    int m_climatePluginId;
    int m_dataGridPluginId;
    int m_infraredPluginId;
    int m_lightingPluginId;
    int m_securityPluginId;
    int m_eventPluginId;
    int m_telecomPluginId;
    int m_orbiterPluginId;
    int m_mediaPluginId;
    int m_mythTvPluginId;
    int m_dceRouterId;

};

#endif // ROUTERHELPER_H
