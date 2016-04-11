// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:room_selector
    height: scaleY(80)
    width: manager.appWidth-10

    Component.onCompleted: {
        console.log("My active focus is " + activeFocus)
        forceActiveFocus()

    }
    Keys.onPressed: {
        switch(event.key){
        case Qt.Key_MediaPrevious:
            if(subMenu){
            subMenu = false;
            subModel=[]
            } else {
                loadComponent("NullComponent.qml")
            }

            break;


        }
    }

    property int itemH:manager.appHeight /3 - exit_button.height
    property int itemW:width / 2

    Rectangle{
        id:exit_button
        height: scaleY(12)

        anchors{
            left:parent.left
            right:parent.right
            top:parent.top
        }
        color: "darkgreen"

        StyledText {
            id: exit
            text: subMenu ? qsTr("Back") : qsTr("Exit")
            font.pixelSize: manager.isProfile ? scaleY(3) :scaleY(5)
            anchors.centerIn: parent
            color: "white"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: subMenu ? subMenu = false : loadComponent("NullComponent.qml")
        }
    }
    clip: true

    anchors.centerIn: parent
    property bool subMenu:false
    property variant subModel:[]

    GridView{
        id: genericview
        visible: !subMenu
        anchors{
            top:exit_button.bottom
            bottom: parent.bottom
            left:parent.left
            right:parent.right
        }
        model: roomList
        cellHeight:itemH
        cellWidth: itemW
        interactive: true
        clip:true
        delegate:
            Item{
            id:generic_item
            height:  itemH- 15
            width: itemW -15

            StyledButton{
                anchors.fill: parent
                buttonText:name
                onActivated: {
                    if(ea_list.length===1){
                        room_selector.subModel = ea_list
                        manager.setActiveRoom(ea_list[0].room, ea_list[0].ea_number);
                        manager.setBoundStatus(true)
                        loadComponent("NullComponent.qml")
                    }
                    else{
                        room_selector.subModel = ea_list
                        room_selector.subMenu = true
                    }
                }
            }
        }
    }

    GridView{
        id: ea_display
        anchors{
            top:exit_button.bottom
            bottom: parent.bottom
            left:parent.left
            right:parent.right
        }
        visible:subMenu
        model: subModel
        cellHeight:itemH
        cellWidth: itemW
        interactive: true
        clip:true
        delegate:
            Item{
            id:generic_room_item
            height:  itemH- 15
            width: itemW -15

            StyledButton{
                anchors.fill: parent
                buttonText:Number(subModel[index].ea_name) == manager.iPK_Device ? qsTr("This Device") : subModel[index].ea_name
                onActivated: {
                    if(subMenu){
                        manager.setActiveRoom(subModel[index].room, subModel[index].ea_number);
                        manager.setBoundStatus(true)
                        loadComponent("NullComponent.qml")
                    }

                }
            }
        }
    }
}
