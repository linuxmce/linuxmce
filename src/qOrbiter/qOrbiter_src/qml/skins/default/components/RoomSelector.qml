import QtQuick 2.2
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import "../"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
GenericPopup{
    title:qsTr("Select Location")
    content:Item{
        anchors.fill: parent
        focus:true
        onActiveFocusChanged: {if(activeFocus)primaryModel.forceActiveFocus(); console.log("location focus")}
        ListView{
            id:primaryModel
            clip:true
            focus:true
            model:roomList
            anchors{
                left:parent.left
                bottom:parent.bottom
                top:parent.top
                margins: 5
            }
            width: Style.listViewWidth_large
            delegate: Item{
                id:generic_delegate
                height: Style.listViewItemHeight
                width: Style.listViewWidth_large
                property bool active:activeFocus
                property string activeColor:Style.appcolor_background_medium
                property string label:name
                function handleTriggered(){
                    if(ea_list[0].ea_number!==0){
                        if(ea_list.length > 1){
                            console.log(ea_list.length)
                            secondLevel.model = ea_list
                            console.log(generic_delegate.y)
                            secondLevel.y = primaryModel.height/3
                            secondLevel.forceActiveFocus()
                        }
                        else{
                            manager.setActiveRoom(intRoom,ea_list[0].ea_number);
                            manager.setBoundStatus(true)
                            close()

                        }
                    } else {
                        manager.setActiveRoom(intRoom,0);
                        manager.setBoundStatus(true)
                        close()
                    }
                }

                Rectangle{
                    anchors.fill: parent
                    color:generic_delegate.active ? generic_delegate.activeColor : Style.appcolor_background_list
                }
                StyledText{
                    anchors.fill: parent
                    anchors.margins: 5
                    horizontalAlignment:Text.AlignLeft
                    verticalAlignment:Text.AlignVCenter
                    text:generic_delegate.label
                }
                Image {
                    id: indicator
                    source: "../images/arrow.png"
                    height: parent.height /2
                    fillMode: Image.PreserveAspectFit
                    width: height
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    visible:ea_list.length > 1
                }
                Keys.onReturnPressed: generic_delegate.handleTriggered()
                Keys.onEnterPressed: generic_delegate.handleTriggered()
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                       generic_delegate.handleTriggered()
                    }
                }
            }
            
        }
        ListView{
            id:secondLevel
            
            height: Style.scaleY(55)
            width: Style.scaleX(25)

            anchors{
                left: primaryModel.right
            }
            
            delegate:Item{
                id:generic_subdelegate
                height: Style.listViewItemHeight
                width: Style.listViewWidth_medium
                property bool active:activeFocus
                property string activeColor:Style.appcolor_background_medium

                Rectangle{
                    anchors.fill: parent
                    color:generic_subdelegate.active ? generic_subdelegate.activeColor : Style.appcolor_background_list
                }
                StyledText{
                    anchors.centerIn: parent
                    width:parent.width
                    text: Number(secondLevel.model[index].ea_name) == manager.iPK_Device ? "This Device " : secondLevel.model[index].ea_name
                }
                Keys.onEnterPressed: {
                    manager.setActiveRoom(secondLevel.model[index].room, secondLevel.model[index].ea_number);
                    close()
                }
                Keys.onReturnPressed: {
                    manager.setActiveRoom(secondLevel.model[index].room, secondLevel.model[index].ea_number);
                    close()
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        manager.setActiveRoom(secondLevel.model[index].room, secondLevel.model[index].ea_number);
                        close()
                    }
                }
            }
        }
        
    }
}
