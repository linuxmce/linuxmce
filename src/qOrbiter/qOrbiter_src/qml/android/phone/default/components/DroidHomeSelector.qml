// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs


Rectangle{
    id:selector
    height: childrenRect.height
    width: childrenRect.width
    anchors.horizontalCenter: parent.horizontalCenter
    property int imagebuttonsize: scaleY(15)
    color:"transparent"

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
                onClicked:loadComponent("LightingSelector.qml")
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
                onClicked:  loadComponent("MediaSelector.qml")
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
                onClicked:  loadComponent("ClimateSelector.qml")
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
                onClicked:  loadComponent("SecuritySelector.qml")
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
                onClicked:  loadComponent("TelecomSelector.qml")
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
                onClicked:  manager.gotoQScreen("Screen_44.qml")
            }
        }
    }
}





