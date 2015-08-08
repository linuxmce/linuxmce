import QtQuick 2.2
import "../components"
StyledScreen{
    id:securitystatus
    screen:qsTr("Mode Changed")
    screen_params: [
        165 /* PK_HouseMode */,
        166 /* HouseModeTime */,
        167 /* ExitDelay */,
        168 /* Alerts */,
    ]

    Panel{
        headerTitle: "Mode Changed"
        anchors.centerIn: parent
    }

}
