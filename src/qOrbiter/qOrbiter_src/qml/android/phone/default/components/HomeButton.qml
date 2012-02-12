import QtQuick 1.0

Item {
    id: homebutton
    height: 45
    width: 45

    ButtonSq{
        buttonsqradius: 2.5
        buttontext: "Home"
        buttontextfontsize: 14
        height: 45
        width:  45
        Image {
            id: homeimage
            source: "../img/homebig.png"
            anchors.centerIn: parent
            height: 45
            width: 45
        }
        MouseArea{
            anchors.fill: parent
            onClicked:gotoQScreen("Screen_1.qml")
        }
    }
}
