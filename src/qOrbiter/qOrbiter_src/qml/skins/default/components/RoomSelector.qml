import QtQuick 2.3
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import "../"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
GenericPopup{
    title:qsTr("Select Location")
    content:Item{
        anchors.fill: parent
        ListView{
            id:primaryModel
            clip:true
            model:roomList
            anchors{
                left:parent.left
                bottom:parent.bottom
                top:parent.top
            }
            width: Style.listViewWidth_large
            delegate: Item{
                id:generic_delegate
                height: Style.listViewItemHeight
                width: Style.listViewWidth_large
                property string label:name
                Rectangle{
                    anchors.fill: parent
                    color:Style.appcolor_background_list
                }
                StyledText{
                    anchors.fill: parent
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
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        if(ea_list[0].ea_number!==0){
                            if(ea_list.length > 1){
                                console.log(ea_list.length)
                                secondLevel.model = ea_list
                                secondLevel.y = mouse.y
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
                
                Rectangle{
                    anchors.fill: parent
                    color:Style.appcolor_background_list
                }
                StyledText{
                    anchors.centerIn: parent
                    width:parent.width
                    text: Number(secondLevel.model[index].ea_name) == manager.iPK_Device ? "This Device " : secondLevel.model[index].ea_name
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
