// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2

Rectangle {
    id:genericlist
    height: childrenRect.height
    width: genericview.width
    border.color: "orange"
    border.width: 1
    clip: true
    color:"black"
    anchors.centerIn: parent
    property bool subMenu:false
    property variant subModel
    onSubMenuChanged: console.log("submenu changed")

    Rectangle{
        id:exit_button
        height: scaleY(8)
        width: scaleX(61)
        anchors.top: genericlist.top
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
                    text:subMenu ? subModel[index].ea_name : name
                    color: "white"
                    font.pixelSize: scaleY(5)
                    anchors.centerIn: container
                }


            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("click")
                    if(subMenu){
                        manager.setActiveRoom(genericview.model[index].room, genericview.model[index].ea_number);
                        manager.setBoundStatus(true)
                        loadComponent("NullComponent.qml")
                    }
                    else
                    {
                        console.log("setting sublist active")
                         genericlist.subModel= ea_list
                        genericlist.subMenu = true

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
        model:subMenu ?  subModel :roomList
        spacing:1
        orientation:ListView.Vertical
        delegate:  delegatemenu
        interactive: true
        clip:true
        // contentHeight: (roomList.count +1) * scaleY(5)

    }
}
