import QtQuick 1.0

Item {
    id: homebutton
    height: style.stdbuttonh
    width: style.stdbuttonw

    ButtonSq{
        buttonsqradius: 2.5
        buttontext: "Home"
        buttontextfontsize: 14
        height: scaleY(13)
        width:  scaleX(8)
        Image {
            id: homeimage
            source: "../images/homebig.png"
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
