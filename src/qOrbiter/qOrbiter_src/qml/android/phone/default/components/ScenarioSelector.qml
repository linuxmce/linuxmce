import QtQuick 1.1

Item {
    id:generic_model_display
    width: generic_view.width
    height: generic_view.height + listClose.height + 10
    property variant currentModel:dummy
    property int floorplanType:2
    state:"inactive"
    onStateChanged:console.log("ScenarioView State Change");

    Behavior on opacity{

        SequentialAnimation{
            PropertyAnimation{
                duration:350
            }
            ScriptAction{ script:
                    if(state==="inactive"){
                        console.log("destroying")
                        componentLoader.source=""
                    }
            }
        }
    }

    Rectangle{
        anchors.fill: parent
        color: "black"
        radius: 5
    }


   // onCurrentModelChanged: currentModel !==dummy ? state = "active" : state = "inactive"
    ListModel{
        id:dummy
    }

    Item{
        id:listClose
        height: scaleY(8)
        width: generic_view.width
        anchors.top: parent.top

        Rectangle{
            anchors.fill: parent
            color:androidSystem.orangeStandard
            radius:5
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
            width: parent.width *.35
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
            MouseArea{
                anchors.fill: parent
                onClicked:  manager.showfloorplan(floorplanType)
            }
        }
    }
    ListView{
        id: generic_view
        anchors.top: listClose.bottom
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

            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: generic_model_display
                opacity:0
            }
        }
    ]
}
