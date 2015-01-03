import QtQuick 2.2

Item {
    width: manager.appWidth *55
    height:scaleY(8)

    Row{
        id:vol_row
        spacing:5
        anchors.left: parent.left
        height: scaleY(8)

        Image {
            id: volUp
            source: "../images/volup_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.adjustVolume(1)
            }
        }
        Image {
            id: volmute
            source: "../images/mute_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height
             smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.mute()
            }
        }
        Image {
            id: voldn
            source: "../images/voldown_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height
             smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.adjustVolume(-5)
            }
        }
    }

    Row{
        height: scaleY(8)
        anchors.left: vol_row.right
        anchors.leftMargin: scaleX(10)
        anchors.top: parent.top
        width: childrenRect.width
        Image {
            id: lightUp
            source: "../images/lightsup_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height

             smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.adjustLights("+10")
            }
        }
        Image {
            id: lightdn
            source: "../images/lightsdown_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height
             smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.adjustLights("-10")
            }
        }

    }

}
