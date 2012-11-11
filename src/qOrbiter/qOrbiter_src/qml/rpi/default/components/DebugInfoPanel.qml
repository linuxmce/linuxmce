// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0

Rectangle {
    id:infoPanel
    width: 300
    height: 200
    color: "lightgrey"
    visible: manager.debugMode ? true : false
    property int titleColWidth:.25 * infoPanel.width
    property int valueColWidth:.65 *infoPanel.width
    property bool active:false
    onActiveChanged: active ? x = 0 : x = parent.x -(infoPanel.width)
    x: x = parent.x -(infoPanel.width)
    anchors.bottom: parent.bottom
    clip:false

    Behavior on x{
      PropertyAnimation{
          duration: 500
        }
    }

    Rectangle{
        id:activityTab
        height: 45
        width: 45
        anchors.left: infoPanel.right
        anchors.verticalCenter: infoPanel.verticalCenter
        color: "darkgrey"
        radius: 5
        StyledText {

            text: qsTr("debug")
        }
        MouseArea{
            anchors.fill: activityTab
            onClicked: active =!active
        }
    }

    ListModel{
        id:orbiterVars
    }

    Row{
        id:headerRow
        width:parent.width
        height:50
        anchors.top: parent.top
        anchors.left: parent.left
        StyledText {
            id: titleColHdr
            text: qsTr("Variable")
            width:titleColWidth
        }
        StyledText {
            id: valColWidth
            text: qsTr("Value")
            width:valueColWidth
        }
    }

    ListView{
        id:orbiterVarDisplay
        height: 150
        width:infoPanel.width
        model:orbiterVars
        delegate: Row{
            width: infoPanel.width
            height: 50
            StyledText {
                text: title
                width: titleColWidth
           }
            StyledText {
               text: value
               width: valueColWidth
            }
        }
    }

}
