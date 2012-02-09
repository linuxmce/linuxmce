import QtQuick 1.0

Column {
    width: 45
    height: childrenRect.height
    spacing: 5

    Rectangle{
        height: 45
        width: 45

        Image {
            id: up_bg
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
        }
        Image {
            id: image2
            anchors.centerIn: parent
            smooth: true
            source: "../img/plus.png"

            MouseArea{
                anchors.fill: parent
                onClicked: dcerouter.adjustLighting(10)
            }
        }
    }



    ButtonSq{
        height:45
        width: 45
        anchors.centerIn: parent
        buttontext: "Floorplan"
        buttontextzindex: 20
        buttontextcolor: "red"
        buttontextbold: true
        buttontextfontsize: 10

        Image {
            id: lightimg
            anchors.centerIn: parent
            width: 45
            height: 45
            source: "../img/On.png"
        }
    }

    Rectangle{
        height: 45
        width: 45

        Image {
            id: dn_bg
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
        }
        Image {
            id: ltdn
            anchors.centerIn: parent
            smooth: true
            source: "../img/minus.png"

        }
        MouseArea{
            anchors.fill: parent
            onClicked: dcerouter.adjustLighting(-10)
        }

    }



}
