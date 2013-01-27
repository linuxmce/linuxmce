import QtQuick 1.1
import com.nokia.android 1.1
Rectangle {
    id:generic_model_display
    width: generic_view.width
    height: generic_view.height + listClose.height
    property alias currentModel:generic_view.model
    state:"inactive"
    color: "black"

    onCurrentModelChanged: currentModel !==undefined ? state = "active" : state = "inactive"
    ListModel{
        id:dummy
    }

    Rectangle{
        id:listClose
        height: scaleY(8)
        width: scaleX(61)
        anchors.top: parent.top
        color: "transparent"
        Image {
            id: headerbg
            source: "../img/widegreyshape.png"
            anchors.fill: listClose
        }

        Text {
            id: exit
            text: qsTr("Exit")
            font.pixelSize: scaleY(3)
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {currentModel=undefined;  }
        }
    }
    ListView{
        id: generic_view
        anchors.top: listClose.bottom
        width: scaleX(61)
        height: scaleY(50)
        model:undefined
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
                visible:true

            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: generic_model_display
                visible:false
            }


        }

    ]
}
