import QtQuick 2.1

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

   GradientFiller{

   }

    Item{
        id:roomSelectorHdr
        anchors{
            left:parent.left
            right:parent.right
            top:parent.top
        }
        height: appStyle.stdHdrHeight
        Rectangle{
            anchors.fill: parent
            color: appStyle.primaryLightColor
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

            GradientFiller{
                fillColor: index === room_list.currentIndex ? appStyle.complimentColorLight : appStyle.complimentColorDark
            }

            Keys.onPressed:{
           console.log("Pressed-"+manager.dumpKey(event.key)+" in outer model.")
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
                } else if (activeFocus){
                    console.log("Room List Item Parent index ==>"+index+ " gained focus, no children.")
                }
            }
            StyledText{
                id:rm_lbl
                height: scaleY(16)
                width:parent.width / 3
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
               anchors{
                   left:rm_lbl.right
                   right:parent.right
               }
                visible: parent.expanded
                model:ea_list
                onActiveFocusChanged: {
                    if(activeFocus){
                        console.log("Inner model item ==>"+innerList.model[index].ea_name+ " gained focus.")
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
                    onActiveFocusChanged:{
                        if(activeFocus){
                            console.log(innerList.model[index].ea_name+ " has focus.")
                        }
                    }

                    GradientFiller{
                        fillColor: appStyle.primaryLightColor
                    }

                    Rectangle{
                        anchors.fill: parent
                        color: index === innerList.currentIndex ? appStyle.mainColor : "grey"
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
                            event.accepted=true
                            manager.setActiveRoom(innerList.model[index].room, innerList.model[index].ea_number);
                            manager.setBoundStatus(true)
                            close()
                        }
                        else{
                            console.log("Key unhandled, bubbling up::"+manager.dumpKey(event.key))
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
