import QtQuick 1.0
import "../components"

Item {
    id:files_view_screen
    height: pageLoader.height
    width:manager.appWidth
    property int current_view_type:1
    Component.onCompleted: {current_header_model=media_filters; }
    state: manager.i_current_mediaType === 5 ? "selection" : "viewing"
    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            files_view_screen.state="detail"
        }
    }
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

    Loader{
        id:file_details_loader
        height: parent.height
        width: parent.width
        anchors.left: files_view_screen.right
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
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.right
            }
            StateChangeScript{
                script: {manager.resetModelAttributes; mediatypefilter.resetStates(); attribfilter.resetStates(); }
            }
        //("5", "", "", "", "1,2", "", "13", "", "2", "")
        //("5", "1", "", "", "1,2", "", "12", "", "2", "")
        //("5", "1", "", "", "1,2", "", "52", "", "2", "7691")
        //("5", "1", "", "", "1,2", "", "12", "", "2", "")
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
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.right
            }
        },
        State {
            name: "detail"
            PropertyChanges {
                target: progress_bar
                visible:false
            }
            PropertyChanges {
                target: media_view
                visible:false
            }
            PropertyChanges{
                target:typeSelection
                visible:false
            }
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.left
            }
            PropertyChanges {
                target: file_details_loader
                source:"../components/FileDetails_"+manager.i_current_mediaType+".qml"
            }
        }

    ]

}
