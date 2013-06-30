import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../../lib/components"
Item{
    id:ftr
    anchors.bottom: parent.bottom
    width: qmlroot.width
    height:scaleY(10)
    onFocusChanged: if(activeFocus) scenarioList.forceActiveFocus()
    Rectangle{
        anchors.fill: parent
        opacity:ftr.activeFocus ? 1 : .65
        color:appStyle.mainColor
        border.width: ftr.activeFocus ? 2 : 0
        border.color:  "red"
    }

    ListView{
        id:scenarioList
        height: parent.height / 2
        width: parent.width
        spacing:scaleX(1)
        orientation: ListView.Horizontal
        onActiveFocusChanged: console.log("Active focus on Scenario listmodel is " + activeFocus)
        clip:false
        focus:true
        model:scenarios
        delegate: Item{
            property bool showSublist:activeFocus
            height: parent.height
            width: scaleX(18)
            onActiveFocusChanged: {console.log(name + " listview active focus is " + activeFocus); if(!activeFocus) {showSublist=false} else showSublist = true }
            Rectangle{
                anchors.fill: parent
                color: parent.activeFocus ? appStyle.lightHighlightColor : appStyle.darkHighlightColor
            }
            Text{
                anchors.centerIn: parent
                text:name
                font.pixelSize: 32
            }
            Keys.onEnterPressed:{ console.log("Clicked "+ name);showSublist = true }
            Keys.onTabPressed: swapFocus()
            ListView{
                height: parent.showSublist ? scaleY(65) : 0
                width: parent.width
                onHeightChanged: positionViewAtEnd()
                model:currentRoomLights
                orientation:ListView.Vertical
                clip:true
                anchors.bottom: parent.top
                spacing:scaleY(1)
                delegate: Rectangle{
                    color:"black"
                    height:scaleY(10)
                    width: parent.width
                    opacity: .65
                    Text {
                        color:"white"
                        text: title
                        anchors.centerIn: parent
                        font.pointSize: 18
                    }
                }
                Behavior on height {
                    PropertyAnimation{
                        duration: 750

                    }
                }
            }
        }

    }

    Keys.onTabPressed:swapFocus()
    onActiveFocusChanged: {
        if(activeFocus)
            console.log("Footer gained focus")
        else
            console.log("Footer lost focus")
    }
}
