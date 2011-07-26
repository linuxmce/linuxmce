import QtQuick 1.0

Item {
    id: homebutton

    ButtonSq{
        Image {
            id: homeimage
            source: "../../../../img/icons/kfm_home.png"
            anchors.centerIn: parent
            height: 75
            width: 75
        }
        MouseArea{
            anchors.fill: parent
            onClicked:gotoQScreen("Screen_1.qml")
        }
    }
}
