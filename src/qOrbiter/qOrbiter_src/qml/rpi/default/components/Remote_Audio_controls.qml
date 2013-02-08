import QtQuick 2.0

AvOptionButton {
    width: 60
    height: 200
    color: style.advanced_bg

    ButtonSq{
        height:50
        width: 50
        anchors.centerIn: parent
        buttontext: "Mute"
        buttontextzindex: 20
        buttontextcolor: "red"
        buttontextbold: true
        buttontextfontsize: 12
        Image {
            id: muteimg
           anchors.centerIn: parent
            width: 50
            height: 50

            source: "../img/icons/kmix.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: manager.mute()
        }
    }

    Image {
        id: image1
        x: 6
        y: 11
        width: 50
        height: 50

        rotation: -90
        smooth: true
        source: "../img/icons/player_fwd.png"
        MouseArea{
            anchors.fill: parent
            onClicked: manager.adjustVolume(1)
        }

    }

    Image {
        id: image2
        x: 6
        y: 142
        width: 50
        height: 50
        rotation: 90
        smooth: true
        source: "../img/icons/player_fwd.png"
        MouseArea{
            anchors.fill: parent
            onClicked: manager.adjustVolume(-1)
        }
    }



}
