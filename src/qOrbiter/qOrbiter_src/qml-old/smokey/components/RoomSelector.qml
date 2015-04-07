import QtQuick 1.0

Item{
    id:room_selection_component
    height: parent.height
    width: parent.width
    state:"roomselect"
    StyledText{
        id:select_room_label
        anchors.top: room_selection_component.top
        anchors.left: room_selection_component.left
        text:"Please Select Location"
        color: "White"
        fontSize: 42
    }


    GridView{
        id:entertainArea_list
        anchors.top:select_room_label.bottom
        width: parent.width
        height: parent.height-select_room_label.height
        cellHeight:scaleY(17)
        cellWidth:scaleX(19)

        delegate:Item{
            height: scaleY(16)
            width: scaleX(18)
            StyledButton{
                height: scaleY(16)
                width: scaleX(18)
                buttonText: Number(entertainArea_list.model[index].ea_name) == manager.iPK_Device ? "This Device " : entertainArea_list.model[index].ea_name
                hitArea.onReleased: {
                    manager.setActiveRoom(entertainArea_list.model[index].room, entertainArea_list.model[index].ea_number);
                    info_panel.state="retracted"
                    manager.setBoundStatus(true)
                    room_selection_component.state = "roomselect"
                }
            }
        }
    }

    GridView{
        id:room_list
        model:roomList
        anchors.top:select_room_label.bottom
        width: parent.width
        height: parent.height-select_room_label.height
        cellHeight:scaleY(17)
        cellWidth:scaleX(19)
        delegate: Item{
            height: scaleY(16)
            width: scaleX(18)

            StyledButton{
                height: scaleY(16)
                width: scaleX(18)
                buttonText: name

                hitArea.onReleased: {
                    console.log(ea_list[0].ea_number!==0)

                    if(ea_list[0].ea_number!==0){

                        if(ea_list.length > 1){
                            console.log(ea_list.length)
                            entertainArea_list.model = ea_list
                            room_selection_component.state="easelect"
                        }
                        else{
                            manager.setActiveRoom(intRoom,ea_list[0].ea_number);
                           info_panel.state="retracted"
                           manager.setBoundStatus(true)
                           room_selection_component.state = "roomselect"
                        }

                    }
                    else{
                         manager.setActiveRoom(intRoom,0);
                        info_panel.state="retracted"
                        manager.setBoundStatus(true)
                        room_selection_component.state = "roomselect"
                    }
                }
            }
        }
    }



    states: [
        State {
            name: "roomselect"
            PropertyChanges {
                target: room_list
                opacity:1
                scale:1
            }
            PropertyChanges {
                target: entertainArea_list
                opacity:0
                scale:3
            }
            PropertyChanges {
                target: select_room_label
                text: "Please Select Location"
            }
        },
        State {
            name: "easelect"
            PropertyChanges {
                target: room_list
                opacity:0
                scale:.01
            }
            PropertyChanges {
                target: entertainArea_list
                opacity:1
                scale:1
            }
            PropertyChanges {
                target: select_room_label
                text: "Select Entertain Area"
            }
        }
    ]

}
