import QtQuick 1.0

Item {
    id: homebutton
    height: scaleY(style.buttonH)
    width:  scaleX(style.buttonW)

    ButtonSq{
        buttonsqradius: 7
        buttontext: "Home"
        buttontextfontsize: 14
        height: scaleY(style.buttonH)
        width:  scaleX(style.buttonW)
        Image {
            id: homeimage
            source: "/src/1004/src/qOrbiter/qObiter_src/img/icons/kfm_home.png"
            anchors.centerIn: parent
            height: scaleY(style.buttonH)
            width:  scaleX(style.buttonW)
        }
        MouseArea{
            anchors.fill: parent
            onClicked:gotoQScreen("Screen_1.qml")
        }
    }
}
