// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../../../skins-common/lib/handlers"

Item {
    id:filterView
    width: scaleX(35)
    height: scaleY(15)  
    state: "inactive"
    property bool selectingUser;

    property alias currentFilterModel:filterList.model
    onCurrentFilterModelChanged: filterList.count===0 ? state = "inactive" : state = "active"

    ListView{
        id:filterList
        orientation: ListView.Horizontal

        height: filterView.height
        width: filterView.width
        clip: true
        spacing:scaleX(2)
        delegate: Rectangle{
            height: scaleY(10)
            width: scaleX(10)
            color: "transparent"
            property bool currentState:selectingUser ? false : status
            onCurrentStateChanged: console.log("Qml sort item status change"+status)
            Image {
                source: currentState ? manager.imagePath+"ui3/green_button.png" :  manager.imagePath+"ui3/red_button.png"
                anchors.fill: parent
            }

            StyledText{
                text:selectingUser ? username : name
                anchors.centerIn: parent
                font.bold: true
                font.pointSize: scaleY(3)
            }

            /* need special handler here to insert pass before sending request to change */

            AttributeFilterHandler{
                onClicked:{
                    filterView.state = "inactive";
                    filterList.model="";
                    selectingUser = false
                }
            }
        }
    }

    states: [
        State {
            name: "active"
            PropertyChanges {
                target: filterView
                visible:true
            }
            AnchorChanges{
                anchors.bottom: undefined
                anchors.top: parent.top
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: filterView
                visible:false
            }
            AnchorChanges{
                anchors.top: undefined
                anchors.bottom: parent.top
            }
        }
    ]

}
