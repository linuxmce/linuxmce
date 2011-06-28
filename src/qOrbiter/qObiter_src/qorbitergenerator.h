#ifndef QORBITERGENERATOR_H
#define QORBITERGENERATOR_H

#include <QWidget>
#include <QTimer>
#include <QDeclarativeView>
#include "pluto_main/Database_pluto_main.h"
#include "pluto_media/Database_pluto_media.h"
#include "Orbiter/OrbiterData.h"
#include "PlutoUtils/DBHelper.h"

/*
  this class is for the purposes of generating a blob of data that can be utilized by the qt based orbiter and qml
  */
class qOrbiterGenerator : public QWidget
{
    Q_OBJECT
public:
    explicit qOrbiterGenerator(int piDevice, int piRoom = 0, int piUser = 0, QWidget *parent = 0);

    QDeclarativeView *splashScreen;
    //device variables
    int m_iDevice;
    int m_iRoom;
    int m_iUser;


    //listmodel variables

    /*--------ListModel Objects------------*/

    //sql variables



signals:
    void regenFinished(int device);

public slots:
    bool initializeRegen();


private:
    void get_virtual_devices();
    void get_locations();
    void get_users();
    void get_entertain_areas();

};

#endif // QORBITERGENERATOR_H
