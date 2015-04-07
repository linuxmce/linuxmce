import QtQuick 1.0

Item {
    id: homebutton
    height: style.buttonH
    width: style.buttonW

    ButtonSq{
        buttonsqradius: 7
        buttontext: "Home"
        buttontextfontsize: 14
        height:style.buttonH
        width: style.buttonW
        Image {
            id: homeimage
            source: "img/icons/kfm_home.png"
            anchors.centerIn: parent
            height: style.buttonH
            width: style.buttonW
        }
        MouseArea{
            anchors.fill: parent
            onClicked:setCurrentScreen("Screen_1.qml")
        }
    }
}
