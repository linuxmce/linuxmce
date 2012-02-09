// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs

Rectangle{
    id:selector
    height: childrenRect.height
    width: childrenRect.width

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
            }
            MouseArea{
                anchors.fill: parent
               onClicked:  MyJs.createListComponent("GenericSelector.qml", selector)
            }
        }


        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: media
                source: "../img/media.png"
            }

            MouseArea{
                anchors.fill: parent
            }
        }



        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: climate
                source: "../img/climate.png"
            }

            MouseArea{
                anchors.fill: parent
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
            }
            MouseArea{
                anchors.fill: parent
            }
        }


        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: telecom
                source: "../img/telecom.png"
            }

            MouseArea{
                anchors.fill: parent
            }
        }



        Rectangle{
            height: childrenRect.height
            width: childrenRect.width
            Image {
                id: adv
                source: "../img/advanced.png"
            }

            MouseArea{
                anchors.fill: parent
            }

        }
    }
}





