import QtQuick 2.2

Item {
    id: homebutton
    height: style.stdbuttonh
    width: style.stdbuttonw

    ButtonSq{
        buttonsqradius: 2.5
        buttontext: "Home"
        buttontextfontsize: scaleY(2)
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
            onReleased:manager.setCurrentScreen("Screen_1.qml")
        }
    }
}
