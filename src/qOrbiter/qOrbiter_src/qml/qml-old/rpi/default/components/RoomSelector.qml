import QtQuick 2.2

Item {
    id:roomSelector
    width: manager.appWidth
    height: manager.appHeight
    onActiveFocusChanged: if(activeFocus) room_list.forceActiveFocus()
    focus:true
    Keys.onEscapePressed: close()
    function close(){
        opacity = 0
        while(opacity !==0){

        }
        pageLoader.forceActiveFocus();
        overLay.source = ""
    }

    Component.onCompleted: {
        westWing.opacity = .85
    }

    Rectangle{
        id:westWing
        height: roomSelector.height
        width: roomSelector.width
        color:"black"
        opacity: 0

        Behavior on opacity {
            PropertyAnimation{
                duration: 750
            }
        }
    }

    ListView{
        id:room_list
        height: scaleY(85)
        width: parent.width
        model: roomList
        delegate: Item{
            height: scaleY(25)
            width:parent.width
            onActiveFocusChanged: {
                if(activeFocus){
                    innerList.forceActiveFocus()
                    console.log("Room List Item Parent index ==>"+index+ " gained focus, setting to child model view.")
                   }
            }

            StyledText{
                id:rm_lbl
                height: scaleY(16)
                width: scaleX(18)
                anchors.left: parent.left
                font.bold:index === room_list.currentIndex
                text: name
                color:"white"
            }

            ListView{
                id:innerList
                height: scaleY(25)
                width:parent.width
                anchors.left: rm_lbl.right

                model:ea_list
                onActiveFocusChanged: {
                    if(activeFocus){
                        console.log("Inner model item ==>"+innerList.model[index].ea_name+ "gained focus.")
                    }
                    else{
                        currentIndex = -1
                    }
                }

                orientation:ListView.Horizontal
                spacing:scaleX(2)
                Keys.onDownPressed: room_list.incrementCurrentIndex()
                Keys.onUpPressed:  room_list.decrementCurrentIndex()

                delegate:Item{
                    height: scaleY(16)
                    width: scaleX(18)
                    Rectangle{
                        anchors.fill: parent
                        color: index === innerList.currentIndex ? skinstyle.mainColor : "grey"
                        radius: 5
                        opacity:index === innerList.currentIndex ? .65 : .50
                    }

                    StyledText{
                        text: innerList.model[index].ea_name
                        color:"white"
                        font.bold: index === innerList.currentIndex
                        fontSize: headerText
                        anchors.centerIn: parent

                    }
                    Keys.onPressed: {
                        if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return){
                            manager.setActiveRoom(innerList.model[index].room, innerList.model[index].ea_number);
                            manager.setBoundStatus(true)

                            close()
                        }
                        else{
                            console.log(event.key)
                        }
                    }
                }
            }
        }




    }



}
