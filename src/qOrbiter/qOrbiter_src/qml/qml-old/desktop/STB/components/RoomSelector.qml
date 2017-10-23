import QtQuick 1.1

Item {
    id:roomSelector
    anchors{
        top:overLay.top
        left:overLay.left
        right:overLay.right
        bottom:overLay.bottom
    }

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

    Item{
        id:roomSelectorHdr
        anchors{
            left:parent.left
            right:parent.right
            top:parent.top
        }
        height: appstyle.stdHdrHeight
        Rectangle{
            anchors.fill: parent
            color: appstyle.primaryLightColor
        }
        StyledText{
            anchors.centerIn: parent
            color: "white"
            text:"Please Select a new room"
            fontSize: headerText
        }
    }

    ListView{
        id:room_list
        anchors{
            top:roomSelectorHdr.bottom
            left: parent.left
            right:parent.right
            bottom:parent.bottom
        }
        clip:true
        model: roomList
        delegate: Item{
            id:itm
            height: scaleY(25)
            width:parent.width
            property bool expanded:ea_list.length > 1 ? true : false
            Rectangle{
                anchors.fill: parent
                gradient: appstyle.buttonGradient
                color: "grey"
                border.color: "white"
                border.width: index === room_list.currentIndex ? 2 : 0
            }
            Keys.onPressed:{

                if(event.key=== Qt.Key_Enter || event.key=== Qt.Key_Return){
                    manager.setActiveRoom(innerList.model[0].room, innerList.model[0].ea_number);
                    manager.setBoundStatus(true)
                    event.accepted=true
                    overLay.source=""
                }
            }

            onActiveFocusChanged: {
                if(activeFocus && expanded){
                    innerList.forceActiveFocus()
                    console.log("Room List Item Parent index ==>"+index+ " gained focus, setting to child model view.")
                }
            }
            StyledText{
                id:rm_lbl
                height: scaleY(16)
                width:scaleX(18)
                anchors.left:itm.expanded ?  parent.left : parent.horizontalCenter
                font.bold:index === room_list.currentIndex
                text: name
                color:"white"
                fontSize: headerText
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if(!parent.expanded){
                        manager.setActiveRoom(innerList.model[0].room, innerList.model[0].ea_number);
                        manager.setBoundStatus(true)
                        close()
                    }
                }
            }

            ListView{
                id:innerList
                height: scaleY(25)
                width:parent.width
                anchors.left: rm_lbl.right
                visible: parent.expanded
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
                        gradient: appstyle.buttonGradient
                    }

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

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(!parent.expanded){
                                manager.setActiveRoom(innerList.model[index].room, innerList.model[index].ea_number);
                                manager.setBoundStatus(true)
                                close()
                            }
                        }
                    }
                }
            }
        }
    }

}
