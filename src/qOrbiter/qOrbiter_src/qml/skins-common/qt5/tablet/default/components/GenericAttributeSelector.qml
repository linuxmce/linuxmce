import QtQuick 2.2

Panel {
    id:attributeSelector
    property variant currentModel:undefined
    property int modelCount: filterlist.count
    property bool selectingUser:false
    state:"inactive"
    headerTitle: "Filter"
    anchors{
        top:content.top
        left: parent.left
        right:parent.right
    }

    onCurrentModelChanged: {
        console.log("AttributeSelector::ModelChanged")
        if(currentModel!==undefined){
            selectingUser=false;
            console.log("AttributeSelector::Opening")
            open()
        }
        else {
            console.log("AttributeSelector::Closing")

            close()
        }
    }

    function open(){
        attributeSelector.state = "active"
    }

    function close(){
        attributeSelector.state = "inactive"
    }


    MouseArea{
        anchors.fill: parent
        onClicked:currentModel=undefined
    }
    ListView{
        id:filterlist
        model:currentModel
        clip:true
        anchors{
            top:headerRect.bottom
            left:parent.left
            right: parent.right
            bottom:parent.bottom
        }
        spacing:scaleY(2)
        opacity: attributeSelector.state === "active" ? 1 :0
        delegate:Item{
            height:scaleY(18)
            width: parent.width

            Rectangle{
                anchors.fill: parent
                color: "black"
                border.color: "white"
                border.width: 1
                radius: 5
            }

            Row{
                height: parent.height
                width: parent.width

                StyledText{
                    text:selectingUser? username: name
                    color: "white"
                    elide: Text.ElideRight
                    width:parent.width *.75
                    font.pointSize:scaleY(4)
                    font.bold: false
                    font.weight: Font.Light
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle{
                    height: scaleY(14)
                    width: height
                    radius:5
                    color: ms.itemActive ? androidSystem.greenHighlight : androidSystem.redHighlight
                    anchors.verticalCenter: parent.verticalCenter
                }
                MouseArea{
                    id:ms
                    anchors.fill: parent
                    property bool itemActive:model.status
                    onClicked:{
                        filterlist.model.setSelectionStatus(name);
                        itemActive =  filterlist.model.getSelectionStatus(name)
                    }

                }
            }
        }
    }


    states: [
        State {
            name: "inactive"
            PropertyChanges{
                target:phil
                opacity:0
            }

            AnchorChanges{
                target:attributeSelector
                anchors{
                    bottom:content.top
                }
            }


        },
        State {
            name: "active"

            PropertyChanges{
                target:phil
                opacity:.65
            }
            AnchorChanges{
                target:attributeSelector
                anchors{
                    bottom:content.bottom
                }
            }

        }
    ]

}
