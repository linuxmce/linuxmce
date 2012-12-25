import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs
//FocusScope{
//    height: scenarios.height
//    width: scenarios.width


//    onFocusChanged: console.log("Focus for Scenario Container is now "+focus)
//    onActiveFocusChanged: console.log("ActiveFocus for Scenario Container is now " +activeFocus)

Row{
    id:presenceRow
    height: parent.height /2
    width: childrenRect.width
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 5
    Rectangle{
        id:roomRect
        height: 50
        width: 85
        color: "transparent"
        StyledText {
            id: roomLabel
            text: qsTr("room")
            anchors.left: parent.left
            color: "white"
            
        }
        
        StyledText {
            id: roomTitle
            text: manager.getCurrentRoom()
            font.pixelSize: 18
            anchors.left: roomLabel.right
            color: "white"
            
        }
    }
    
    Rectangle{
        id:userRect
        height: 50
        width: 85
        color: "transparent"
        
        StyledText {
            id: userLabel
            text: qsTr("user:")
            font.pixelSize: 12
            anchors.left: userRect.left
            color: "white"
            
        }
        StyledText {
            id: userName
            text: manager.getCurrentUser()
            font.pixelSize: 18
            anchors.left: userLabel.right
            color: "white"
        }
    }
}
