import QtQuick 2.0
import "../components"

Item {
    id:files_view_screen
    anchors.fill: parent
    property int current_view_type:1
    Component.onCompleted: current_header_model=media_filters
    state: manager.i_current_mediaType === 5 ? "selection" : "viewing"

    MultiViewMediaList{
        id:media_view
        anchors.centerIn: parent
    }

    GridView{
        id:typeSelection
        anchors.centerIn: parent
        height:parent.height / 2
        width: parent.width *.85
        model:mediatypefilter
        cellWidth:scaleX(25)
        cellHeight:scaleY(15)
        delegate:
            Item{
            height: childrenRect.height
            width: childrenRect.width

            StyledButton{
              buttonText.text: name
             textSize: 48
              hitArea.onReleased:{ mediatypefilter.setSelectionStatus(name); files_view_screen.state="viewing"}
            }
        }
    }

    MediaListProgressBar{
        id:progress_bar
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    states: [
        State {
            name: "selection"
            PropertyChanges {
                target: media_view
                visible:false
            }
            PropertyChanges {
                target: progress_bar
                visible:false
            }
            PropertyChanges {
                target: typeSelection
                visible:true
            }
        },
        State {
            name: "viewing"
            PropertyChanges {
                target: progress_bar
                visible:true
            }
            PropertyChanges {
                target: media_view
                visible:true
            }
            PropertyChanges{
                target:typeSelection
                visible:false
            }
        }

    ]

}
