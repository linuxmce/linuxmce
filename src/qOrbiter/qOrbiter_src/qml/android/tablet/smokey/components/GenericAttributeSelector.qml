import QtQuick 1.1

Item {
    id:attributeSelector
    width: manager.appWidth
    height:manager.appHeight
    property variant currentModel
    property int modelCount: filterlist.count
    state:"opening"

    function close(){

    }

    Rectangle{
        id:phil
        color:"black"
        anchors.fill: parent

        MouseArea{
            anchors.fill: parent
            onClicked:attributeSelector.state = "closing"
        }
    }

    ListView{
        id:filterlist
        height: scaleY(75)
        width:scaleX(95)
        model:currentModel
        anchors.top: heading.bottom
        anchors.left: heading.left
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
                    text: name
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
                    onClicked:{filterlist.model.setSelectionStatus(name); itemActive =  filterlist.model.getSelectionStatus(name)}

                }
            }
        }
    }


    states: [
        State {
            name: "opening"
            PropertyChanges{
                target:phil
                opacity:0
            }

        },
        State {
            name: "active"
            PropertyChanges{
                target:phil
                opacity:.65
            }

        },
        State {
            name: "closing"
            PropertyChanges{
                target:phil
                opacity:0
            }
        }
    ]

}
