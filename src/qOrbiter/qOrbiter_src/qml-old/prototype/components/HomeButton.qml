import QtQuick 1.1

Item {
    id: homebutton
    height: scaleY(13)
    width:  scaleX(8)

    ButtonSq{
        buttonsqradius: 2.5
        buttontext: "Home"
        buttontextfontsize: 14
        height: scaleY(13)
        width:  scaleX(8)
        Image {
            id: homeimage
            source: "../img/ui3/homebig.png"
            anchors.centerIn: parent
            height: scaleY(13)
            width:  scaleX(8)
        }
        MouseArea{
            anchors.fill: parent
            onClicked:setCurrentScreen("Screen_1.qml")
        }
    }
}
