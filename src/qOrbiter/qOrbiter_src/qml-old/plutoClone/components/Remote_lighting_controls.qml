import QtQuick 2.0

AvOptionButton {
    width: 60
    height: 200
    color: style.advanced_bg

    ButtonSq{
        height:50
        width: 50
        anchors.centerIn: parent
        buttontext: "Floorplan"
        buttontextzindex: 20
        buttontextcolor: "red"
        buttontextbold: true
        buttontextfontsize: 10

        Image {
            id: lightimg
           anchors.centerIn: parent
            width: 50
            height: 50
            source: "../img/icons/jabber_protocol.png"
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
            onClicked: manager.adjustLights(10)
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
            onClicked: manager.adjustLights(-10)
        }
    }



}
