import QtQuick 2.0
import "../components"
Rectangle {
    height: manager.appHeight
    width: manager.appWidth

    ButtonSq {
    height: 50
    width: 50
    buttontext: "home"
    Image {
        id: homeimage
        source: "../../../img/icons/kfm_home.png"
        height:50
        width: 50
    }
    MouseArea
    {
        anchors.fill:parent
        onClicked:setCurrentScreen("Screen_1.qml")
    }
    }

}
