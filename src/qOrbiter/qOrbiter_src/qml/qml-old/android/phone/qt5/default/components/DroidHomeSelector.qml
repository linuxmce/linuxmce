// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2

Item{
    id:selector
    height: childrenRect.height
    width: childrenRect.width
    anchors.horizontalCenter: parent.horizontalCenter
    property int imagebuttonsize: scaleY(15)


    Row{
        id:toprow
        height: childrenRect.height
        width: childrenRect.width

        Image {
            id: lighting
            source: "../img/lights.png"
            height: style.iconHeight
            width: style.iconWidth
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked:scenarioPopup.currentModel =currentRoomLights
            }
        }

        Image {
            id: media
            source: "../img/media.png"
            height: style.iconHeight
            width: style.iconWidth
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked:  scenarioPopup.currentModel = currentRoomMedia
            }
        }

        Image {
            id: climate
            source: "../img/climate.png"
            height: style.iconHeight
            width: style.iconWidth
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked:  scenarioPopup.currentModel = currentRoomClimate
            }
        }
    }

    //---bottom row
    Row{
        id:bottomrow
        anchors.top: toprow.bottom
        height: childrenRect.height
        width: childrenRect.width

        Image {
            id: security
            source: "../img/security.png"
            height: style.iconHeight
            width: style.iconWidth
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked:  scenarioPopup.currentModel = currentRoomSecurity
            }
        }

        Image {
            id: telecom
            source: "../img/telecom.png"
            height: style.iconHeight
            width: style.iconWidth
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked:  scenarioPopup.currentModel = currentRoomTelecom
            }
        }

        Image {
            id: adv
            source: "../img/advanced.png"
            height: style.iconHeight
            width: style.iconWidth
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked:  manager.setCurrentScreen("Screen_44.qml")
            }
        }
    }
}





