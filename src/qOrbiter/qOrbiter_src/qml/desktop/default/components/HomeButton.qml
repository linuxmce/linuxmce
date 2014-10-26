import QtQuick 1.1

Item {
    id: homebutton
height:25
width: 25
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
            onClicked:manager.setCurrentScreen("Screen_1.qml")
        }
    }
}
