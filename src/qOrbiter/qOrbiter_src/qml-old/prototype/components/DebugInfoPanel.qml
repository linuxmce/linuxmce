// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

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
        Text {

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
        Text {
            id: titleColHdr
            text: qsTr("Variable")
            width:titleColWidth
        }
        Text {
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
            Text {
                text: title
                width: titleColWidth
           }
            Text {
               text: value
               width: valueColWidth
            }
        }
    }

}
