import QtQuick 1.0


Item {
    id:generic_filter_view
    width: manager.appWidth
    height: manager.appHeight
    clip: true
    property variant currentModel
    property int modelCount: filterlist.count
    property int delegateHeight:scaleY(10)
    state:"inactive"
    onCurrentModelChanged: currentModel !== "" ? state="active": state="inactive"

    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .65
        MouseArea{
            anchors.fill: parent
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
    Item{
        id:heading
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: filterlist.width
        height: scaleY(8)
        Rectangle{
            anchors.fill: parent
            color:androidSystem.orangeStandard
        }

        StyledText{
            text: "Click to Close"
            color:"black"
            font.bold: false
            anchors.verticalCenter: parent.verticalCenter
            fontSize: scaleY(3)
        }
        MouseArea{
            anchors.fill: parent
            onClicked: generic_filter_view.currentModel=""
        }
    }

    states: [
        State {
            name: "active"
            PropertyChanges {
                target: generic_filter_view
                visible:true

            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: generic_filter_view
                visible:false
            }

        }
    ]
}


