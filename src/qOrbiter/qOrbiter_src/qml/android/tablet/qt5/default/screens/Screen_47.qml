import QtQuick 2.0
import org.linuxmce.enums 1.0
import "../components"

StyledScreen {
    id:files_view_screen
    screen: "Media Files View"
    property int current_view_type:1
navigationComponent: "MedialistNav.qml"
    Component.onCompleted: {

        if(manager.q_mediaType === Mediatypes.STORED_VIDEO ) {state= "selection"}else{state="viewing"}
    }

    Item{
        id:content
        anchors.fill: parent
        Connections{
            target: filedetailsclass
            onShowDetailsChanged:{
                files_view_screen.state="detail"
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
                height: childrenRect.height
                width: childrenRect.width

                StyledButton{
                    buttonText.text: name
                    textSize: 48
                    hitArea.onReleased:{
                        mediatypefilter.setSelectionStatus(name);
                        files_view_screen.state="viewing"
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
        }

    }

    states: [

        State{
            name:"default"
            PropertyChanges {
                target: media_view
                visible:true
            }
            PropertyChanges {
                target: progress_bar
                visible:true
            }
            PropertyChanges {
                target: typeSelection
                visible:false
            }
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.right
            }
//            StateChangeScript{
//                script: {
//                    manager.resetModelAttributes;
//                    mediatypefilter.resetStates();
//                    attribfilter.resetStates();
//                }
//            }
        },

        State {
            name: "selection"
            extend:"default"
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
            StateChangeScript{
                script: {
                    manager.resetModelAttributes;
                    mediatypefilter.resetStates();
                    attribfilter.resetStates();
                }
            }
            //("5", "", "", "", "1,2", "", "13", "", "2", "")
            //("5", "1", "", "", "1,2", "", "12", "", "2", "")
            //("5", "1", "", "", "1,2", "", "52", "", "2", "7691")
            //("5", "1", "", "", "1,2", "", "12", "", "2", "")
        },
        State {
            name: "viewing"
            extend:"default"

        },
        State {
            name: "detail"
            extend:"selection"

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
                source:"../components/FileDetails_"+manager.q_mediaType+".qml"
            }
        }

    ]


}
