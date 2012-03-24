// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:genericlist
    height: scaleY(50)
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
            width: scaleX(35)
            height: scaleY(8)
            border.color: "silver"
            border.width: 1
            color:"transparent"
            Text {
                id: generic_label
                text:  title
                color: "white"
                font.pixelSize: scaleY(3)
                anchors.centerIn: container
                font.family: "Droid Sans"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    currentroom = title

                   setActiveRoom(intRoom, entertain_area)
                    loadComponent("NullComponent.qml")
                }
            }
        }
    }
}

    ListView{
        id: genericview
        width: scaleX(45)
        height: scaleY(50)
        model: roomList
        spacing:1
        orientation:ListView.Vertical
        delegate:  delegatemenu
        interactive: true
        clip:true
       // contentHeight: (roomList.count +1) * scaleY(5)

    }
}
