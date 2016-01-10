import QtQuick 1.0
import "../components"

Rectangle {
    id:myth_recordings
    width: appW
    height: appH

    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: parent
    }

    HomeButton{
        id:home
        anchors.left: parent.left
    }

    Text {
        id: temp_label
        text: qsTr("Guide Goes here")
        font.pixelSize: scaleY(4)
        anchors.bottom: recordings_control.top
    }

    Column{
        id:mythVerticalButtons
        height: childrenRect.height
        width: childrenRect.width

        ButtonSq{
            id:recordings_button
            buttontext: "Recordings"

        }
        ButtonSq{
            id:schedule_button
            buttontext: "Schedule"

        }
        ButtonSq{
            id:live_tv
            buttontext: "Recordings"

        }
        ButtonSq{
            id:guide_button
            buttontext: "Recordings"

        }

    }

    Row{
        id:bottom_row
        height: childrenRect.height
        width: childrenRect.width
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        ArrowBar{}

        ButtonSq{
            id:button_ok
            buttontext: "Ok"
        }
        ButtonSq{
            id:button_delete
            buttontext: "Delete"
        }
        ButtonSq{
            id:button_info
            buttontext: "Info"

        }
    }



}
