import QtQuick 1.0

Item {
    id: homebutton
    height: style.stdbuttonh
    width: style.stdbuttonw

    ButtonSq{
        buttonsqradius: 2.5
        buttontext: "Home"
        buttontextfontsize: 14
        height: style.stdbuttonh
        width: style.stdbuttonw
        Image {
            id: homeimage
            source: "../img/homebig.png"
            anchors.centerIn: parent
            height: style.stdbuttonh
            width: style.stdbuttonw
        }
        MouseArea{
            anchors.fill: parent
            onClicked:gotoQScreen("Screen_1.qml")
        }
    }
}
