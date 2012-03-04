// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs


Rectangle{
    id:selector
    height: childrenRect.height
    width: childrenRect.width
    anchors.horizontalCenter: parent.horizontalCenter
property int imagebuttonsize: scaleY(15)

    Row{
        id:toprow
        height: childrenRect.height
        width: childrenRect.width

        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: lighting
                source: "../img/lights.png"
                height: imagebuttonsize
                width: imagebuttonsize
                smooth:true

            }
            MouseArea{
                anchors.fill: parent
               onClicked:  MyJs.createListComponent("LightingSelector.qml", selector)
            }
        }


        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: media
                source: "../img/media.png"
                height:imagebuttonsize
                width: imagebuttonsize
                smooth:true
            }

            MouseArea{
                anchors.fill: parent
                onClicked:  MyJs.createListComponent("MediaSelector.qml", selector)
            }
        }



        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: climate
                source: "../img/climate.png"
                height: imagebuttonsize
                width: imagebuttonsize
                smooth:true
            }

            MouseArea{
                anchors.fill: parent
                onClicked:  MyJs.createListComponent("ClimateSelector.qml", selector)
            }

        }
    }

    //---bottom row
    Row{
        id:bottomrow
        anchors.top: toprow.bottom
        height: childrenRect.height
        width: childrenRect.width

        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: security
                source: "../img/security.png"
                height: imagebuttonsize
                width: imagebuttonsize
                smooth:true
            }
            MouseArea{
                anchors.fill: parent
                onClicked:  MyJs.createListComponent("SecuritySelector.qml", selector)
            }
        }


        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: telecom
                source: "../img/telecom.png"
                height: imagebuttonsize
                width: imagebuttonsize
                smooth:true
            }

            MouseArea{
                anchors.fill: parent
                onClicked:  MyJs.createListComponent("TelecomSelector.qml", selector)
            }
        }



        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: adv
                source: "../img/advanced.png"
                height: imagebuttonsize
                width:imagebuttonsize
                smooth:true
            }

            MouseArea{
                anchors.fill: parent
                onClicked:  manager.gotoQScreen("Screen_44.qml")
            }

        }
    }
}





