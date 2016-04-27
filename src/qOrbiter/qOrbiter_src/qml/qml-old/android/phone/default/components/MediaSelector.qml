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
        color: "transparent"
        Image {
            id: headerbg
            source: "../img/widegreyshape.png"
            anchors.fill: exit_button
        }

        Text {
            id: exit
            text: qsTr("Exit")
            font.pixelSize: scaleY(3)
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            onClicked: loadComponent("NullComponent.qml")
        }
    }
    HomeButtonDelegate{id:generic_delegate}

    ListView{
        id: generic_view
        anchors.top: exit_button.bottom
        width: scaleX(61)
        height: scaleY(50)
        model: currentRoomMedia
        spacing:1
        orientation:ListView.Vertical
        delegate:  generic_delegate
        interactive: true
        clip:true
        contentHeight: childrenRect.height

    }
}
