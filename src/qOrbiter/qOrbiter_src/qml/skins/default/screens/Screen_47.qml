import QtQuick 2.3
import org.linuxmce.enums 1.0
import "../components"

StyledScreen {
    id:mediaFilesView
    screen:qsTr("Media Files View")
    onActiveFocusChanged:{
        if(activeFocus) innerContent.forceActiveFocus();
    }

    Item{
        id:innerContent
        anchors.fill: parent
        Component.onCompleted: {
            manager.setStringParam(0, manager.q_mediaType)


        }
        state:manager.q_mediaType =="5" ? "entry" : "selection"
        onActiveFocusChanged: {
            if(!activeFocus)
                return

            if(state=="entry")
                typeSelection.forceActiveFocus()
            else if(state=="selection")
                console.log("selection focus")
            else if(state=="filter")
                console.log("filter focus")
        }

        MultiMediaView {
            id: multi_view_list
        }

        GridView{
            id:typeSelection
            Component.onCompleted: innerContent.forceActiveFocus()
            onActiveFocusChanged: console.log("media focus")
            anchors.centerIn: parent
            height:parent.height / 2
            width: parent.width *.85
            model:mediatypefilter
            cellWidth:manager.isProfile ? Style.scaleX(30) : Style.scaleX(20)
            cellHeight:manager.isProfile ? Style.scaleY(15) : Style.scaleY(22)
            delegate:
                LargeStyledButton{
                height:typeSelection.cellHeight -5
                width: typeSelection.cellWidth -5
                arrow: true
                buttonText: name
                onActivated: {mediatypefilter.setSelectionStatus(name); innerContent.state="selection"}
//                textSize: txtObj.largeFontSize
//                hitArea.onReleased:{
//
//                }
            }

        }


        states: [
            State {
                name: "entry"
                PropertyChanges {
                    target: typeSelection
                    visible:true
                }
                PropertyChanges {
                    target: multi_view_list
                    visible:false
                }
                StateChangeScript{
                    script:typeSelection.forceActiveFocus()
                }
            },
            State {
                name: "selection"
                PropertyChanges {
                    target: typeSelection
                    visible:false
                }
                PropertyChanges {
                    target: multi_view_list
                    visible:true
                }
                StateChangeScript{
                    script:multi_view_list.forceActiveFocus()
                }
            },
            State {
                name: "filter"
                extend: "entry"
            },
            State {
                name: "aplha"
                extend: "selection"
            }
        ]
    }


}
