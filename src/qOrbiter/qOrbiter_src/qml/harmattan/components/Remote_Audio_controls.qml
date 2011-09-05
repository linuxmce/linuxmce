import QtQuick 1.0

Rectangle {
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

            source: "../../../img/icons/kmix.png"
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
        source: "../../../img/icons/player_fwd.png"
    }

    Image {
        id: image2
        x: 6
        y: 142
        width: 50
        height: 50
        rotation: 90
        smooth: true
        source: "../../../img/icons/player_fwd.png"
    }



}
