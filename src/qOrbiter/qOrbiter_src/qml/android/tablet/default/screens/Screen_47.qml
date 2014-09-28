import QtQuick 1.1

import "../components"

StyledScreen {
    id:files_view_screen
    screen: "Media Files View"
    property int current_view_type:1

    Component.onCompleted: {
        setNavigation("MedialistNav.qml")
        hideInfoPanel()
    }
    Item{
        id:content
        anchors.fill: parent

        state: manager.q_mediaType =="5" ? "selection" : "viewing"
        Connections{
            target: filedetailsclass
            onShowDetailsChanged:{
                content.state="detail"
            }
        }
        MultiViewMediaList{
            id:media_view

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
                height:scaleY(8)
                width: parent.width

                StyledButton{
                    buttonText: name
                    textSize: 48
                    hitArea.onReleased:{
                         mediatypefilter.setSelectionStatus(name);
                        content.state="viewing"

                    }
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
            source:"../components/FileDetails_"+manager.q_mediaType+".qml"
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
            }
        ]

    }


}
