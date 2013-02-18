import QtQuick 1.0

Item {
    id: homebutton
    height: scaleY(13)
    width:  scaleX(8)

    ButtonSq{
        buttonsqradius: 2.5
        buttontext: ""
        buttontextfontsize: 14
        anchors.fill: parent
        Image {
            id: homeimage
            source: "../img/ui3/homebig.png"
         anchors.fill: parent
        }
        MouseArea{
            anchors.fill: parent
            onClicked:gotoQScreen("Screen_1.qml")
        }
    }
}
