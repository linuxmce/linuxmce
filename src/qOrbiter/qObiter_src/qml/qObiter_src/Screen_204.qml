import Qt 4.7
import "components"
Rectangle {
    width: 100
    height: 62

    ButtonSq {
    height: 50
    width: 50
    buttontext: "home"
    MouseArea
    {
        anchors.fill:parent
        onClicked: gotoQScreen("Screen_1.qml")
    }
    }

}
