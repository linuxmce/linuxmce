import QtQuick 1.0


/*
local configuration for qOrbiter to retain variables at runtime

  */
    QtObject{
        id:orbiterdata
        objectName: "local_conf"
        //skin description
        property string routerip: "192.168.0.1"
        property string deviceid: "35"
        property string defaultskin: "noir"

}
