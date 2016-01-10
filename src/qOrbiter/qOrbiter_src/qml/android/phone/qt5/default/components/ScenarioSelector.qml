import QtQuick 2.2
Item {
    id:generic_model_display
    width: scaleX(100)
    height: scaleY(100)
    property variant currentModel:dummy
    state:"inactive"
    onStateChanged:console.log("ScenarioView State Change to "+generic_model_display.state);

    Behavior on opacity{

        SequentialAnimation{
            PropertyAnimation{
                duration:350
            }
        }
    }

    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity:.75
    }

    onCurrentModelChanged:{ currentModel !==dummy ? state = "active" : state = "inactive" ; console.log("current model changed.")}
    ListModel{
        id:dummy
    }

    Item{
        id:listClose
        height: scaleY(8)
        width: scaleX(61)
       anchors.horizontalCenter: parent.horizontalCenter
       anchors.verticalCenter: parent.verticalCenter
       anchors.verticalCenterOffset: scaleY(-40)

        Rectangle{
            anchors.fill: parent
            color:androidSystem.orangeStandard
        }

        Rectangle{
            anchors.fill: parent
            opacity:.15
            gradient: Gradient{
                GradientStop{ position:0.0;color:"transparent"}
                GradientStop{ position:0.35;color:androidSystem.orangeHighlight}
                GradientStop{ position:0.65;color:"black"}
            }
        }

        StyledText {
            id: exit
            text: qsTr("Exit")
            font.pixelSize: scaleY(4)
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {currentModel=dummy  }
        }

        Rectangle{
            height: parent.height - 5
            width: height
            color: "black"
            opacity: .65
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            StyledText{
                anchors.centerIn: parent
                text: "F"
                font.weight: Font.DemiBold
                fontSize: scaleY(5)
                color: "white"
            }
        }
    }
    ListView{
        id: generic_view
        anchors.top: listClose.bottom
        anchors.left: listClose.left
        width: scaleX(75)
        height: scaleY(75)
        model:currentModel
        spacing:1
        orientation:ListView.Vertical
        delegate:  HomeButtonDelegate{}
        interactive: true
        clip:true
        contentHeight: childrenRect.height
    }

    states: [
        State {
            name: "active"
            PropertyChanges {
                target: generic_model_display
                opacity:1
                visible:true
            }
            AnchorChanges{
                anchors.right: undefined
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: generic_model_display
                opacity:0
                visible:false
            }
            AnchorChanges{
                anchors.right: parent.left
                anchors.verticalCenter: undefined
                anchors.horizontalCenter:  undefined
            }

        }
    ]
}
