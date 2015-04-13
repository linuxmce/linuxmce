// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2

Rectangle {
    id:genericlist
    height: childrenRect.height
    width: genericview.width
    border.color: "orange"
    border.width: 1
    clip: true
    color:"transparent"
    anchors.centerIn: parent

    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: genericview
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
            onClicked:loadComponent("NullComponent.qml")
        }
    }

    Component{
        id:delegatemenu


        Item{
            id:generic_item
            //important!! these need to be set on an imported component otherwise its appears all wrong!
            height:  container.height
            width: container.width

            Rectangle{
                id:container
                width: scaleX(61)
                height: scaleY(15)
                border.color: "silver"
                border.width: 1
                color:"transparent"
                Text {
                    id: generic_label
                    text:  username
                    color: "white"
                    font.pixelSize: scaleY(3)
                    anchors.centerIn: container
                    font.family: "Droid Sans"
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        setCurrentUser(index+1)
                        loadComponent("NullComponent.qml")
                    }
                }
            }
        }
    }

    ListView{
        id: genericview
        width: scaleX(61)
        height: scaleY(50)
        anchors.top: exit_button.bottom
        model: userList
        spacing:1
        orientation:ListView.Vertical
        delegate:  delegatemenu
        interactive: true
        clip:true
        // contentHeight: (roomList.count +1) * scaleY(5)

    }
}
