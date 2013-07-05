import QtQuick 1.1
import "../components"
Item {
    id:screen_forty_seven_of_nine
    height: scaleY(100)
    width: scaleX(100)
    state: "browsing"
    property int depth:0
        Component.onCompleted: screen_forty_seven_of_nine.forceActiveFocus()
    onActiveFocusChanged: {
        console.log(mediaList.parent.objectName+ " recieved focus, passing to mediaList grid")
        mediaList.forceActiveFocus()
    }

    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            screen_forty_seven_of_nine.state = "detail"
            depth--
        }
    }

    MediaListGridDelagate {
        id: contactDelegate
       visible: false
    }

    GridView{
        id:mediaList
        height: parent.height
        width: parent.width
        cellHeight: contactDelegate.height
        cellWidth: contactDelegate.width
        contentHeight: scaleY(24)
        contentWidth: scaleX(18)
        model: dataModel
        Keys.onPressed: { // enter and back keys
            if(event.key ===Qt.Key_Enter || event.key === 16777220 ) {

                manager.setStringParam(4, dataModel.get(currentIndex, "id"))
                mediaList.positionViewAtIndex(0, ListView.Beginning)
                depth++
            }
            else{
                if(event.key !== Qt.Key_Escape && event.key !== Qt.Key_Tab&& event.key !== 16777237 && event.key !==16777236 && event.key !==16777234 && event.key !==16777235){
                    mediaList.currentIndex = dataModel.setSection(event.key)
                    mediaList.positionViewAtIndex(currentIndex,ListView.Beginning)
                }
            }
            console.log(event.key)
            console.log("Depth ==>" + depth)
        }



        Keys.onEscapePressed: {
            if(screen_forty_seven_of_nine.state=== "detail")
            {
                screen_forty_seven_of_nine.state = "browsing"
            } else if (depth!==0){
                manager.goBackGrid()
                depth--
            }
            else if(depth === 0){
                manager.gotoQScreen("Screen_1.qml")
            }
        }
        highlightFollowsCurrentItem:true

        highlight: Rectangle{
            height:scaleY(23)
            width: scaleX(16)
            color: "black"
            border.color: "white"
            border.width: 2
        }
        delegate:MediaListGridDelagate{}

    }

    Item{
        id:infoPanel
        height: parent.height
        width: 0
        anchors.right: screen_forty_seven_of_nine.right
        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity:.85
        }
    }

    states: [
        State {
            name: "browsing"
            PropertyChanges {
                target: mediaList
                width:screen_forty_seven_of_nine.width
            }

            PropertyChanges{
                target: infoPanel
                width:0
            }
        },
        State {
            name: "detail"
            PropertyChanges {
                target: mediaList
                width:scaleX(65)
            }
            PropertyChanges {
                target: infoPanel
                width:scaleX(35)
            }
        }

    ]

}
