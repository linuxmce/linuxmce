import QtQuick 1.1
import com.nokia.android 1.1
Rectangle {
    id:generic_model_display
    width: generic_view.width
    height: generic_view.height + listClose.height
    property variant currentModel
    state: "inactive"
    visible: false
    color: "black"


    onCurrentModelChanged: currentModel !=="" ? state = "active" : state = "inactive"
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
            onClicked: generic_model_display.state = "inactive"
        }
    }
    ListView{
        id: generic_view
        anchors.top: listClose.bottom
        width: scaleX(61)
        height: scaleY(50)
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
                visible:true

            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: generic_model_display
                visible:false
            }
            PropertyChanges{
                target: generic_view
                model:""
            }
        }

    ]
}
