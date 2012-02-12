// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Component{

    Rectangle{
        height: childrenRect.height
        width: childrenRect.width

        Rectangle{
            Image {
                id: lighting
                source: "../img/lights.png"
            }
        }
        MouseArea{
            anchors.fill: parent
        }

    }

    Rectangle{
        height: childrenRect.height
        width: childrenRect.width

        Rectangle{
            Image {
                id: media
                source: "../img/media.png"
            }
        }
        MouseArea{
            anchors.fill: parent
        }
    }

    Rectangle{
        height: childrenRect.height
        width: childrenRect.width

        Rectangle{
            Image {
                id: climate
                source: "../img/climate.png"
            }
        }
        MouseArea{
            anchors.fill: parent
        }
    }



}
