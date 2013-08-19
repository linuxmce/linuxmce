// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:generic_list
    height: childrenRect.height
    width: generic_view.width
    border.color: "orange"
    border.width: 1
    anchors.centerIn: parent
    clip:true
    property variant scenarioModel

    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: generic_list
    }
    Rectangle{
        id:exit_button
        height: scaleY(8)
        width: scaleX(61)
        anchors.top: generic_list.top
        color: "white"

        Text {
            id: exit
            text: qsTr("Exit")
            font.pixelSize: scaleY(3)
            anchors.left: exit_button.left
        }
        Image {
            id: radio
            source: componentExit.pressed ? "../img/radio_btn_active.png" : "../img/radio_btn_inactive.png"
        }
        MouseArea{
            id:componentExit
            anchors.fill: parent
            onClicked: scenarioLoader.source="NullComponent.qml"
        }
    }
    HomeButtonDelegate{id:generic_delegate}

    ListView{
        id: generic_view
        anchors.top: exit_button.bottom
        width: scaleX(61)
        height:  scaleY(50)
        model: scenarioModel
        spacing:1
        orientation:ListView.Vertical
        delegate:  generic_delegate
        interactive: true
        clip:true
        contentHeight: childrenRect.height

    }
}
