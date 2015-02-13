import QtQuick 2.2

import "../components"

StyledScreen {
    id:files_view_screen
    screen: "Media Files View"
    property int current_view_type:1
    property string currentSeekLetter:""
    property bool showDetails: false
    keepHeader: false


    Panel{
        id:container
        anchors.fill: parent
        headerTitle: ""

        Flickable{
            height:style.rowH
            width: media_goback.x - x
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.rightMargin: scaleX(1)
            parent:container.headerRect
            contentWidth: btnRow.width
            clip:true

            Row {
                id:btnRow
                width: childrenRect.width
                height: scaleY(7)
                spacing: scaleX(1)

                StyledButton{
                    buttonText: "Attribute"
                    onActivated: {attributeSelector.currentModel=attribfilter; }
                }

                StyledButton{
                    buttonText: "Genre"
                    onActivated: {attributeSelector.currentModel=genrefilter; }
                }

                StyledButton{
                    buttonText: "MediaType"
                    onActivated: {attributeSelector.currentModel=mediatypefilter; }
                }

                StyledButton{
                    buttonText: "Resolution"
                    onActivated: {attributeSelector.currentModel=fileformatmodel; }
                }
                StyledButton{
                    buttonText: "Sources"
                    // onActivated: {attributeSelector.currentModel=undefined; }
                }

                StyledButton{
                    buttonText: "Users"
                    onActivated: {attributeSelector.currentModel=userList; attributeSelector.selectingUser=true; }
                }

                StyledButton{
                    buttonText: "Play All"
                    onActivated:  manager.playMedia("!G"+iPK_Device)
                }
            }

        }

        StyledButton {
            id: home_label
            buttonText: qsTr("Home")
            parent:container.headerRect
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            hitArea.onReleased: manager.setCurrentScreen("Screen_1.qml")
        }

        StyledButton{
            id:media_goback
            buttonText: "Back"
            parent:container.headerRect
            anchors.right: home_label.left
            anchors.rightMargin: scaleX(1)
            anchors.verticalCenter: parent.verticalCenter
            hitArea.onReleased:{
                manager.goBackGrid();
            }
        }


        Alphabetlist {
            id: alphabetlist
        }

        Component.onCompleted: {
            //setNavigation("MedialistNav.qml")
            hideInfoPanel()
            manager.setStringParam(0, manager.q_mediaType)
            forceActiveFocus()
        }
        Item{
            id:content
            anchors{
                left:parent.left
                right:parent.right
                top:container.headerRect.bottom
                bottom:parent.bottom
            }

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

            StyledButton{
                id:all_media
                anchors{
                    bottom: typeSelection.top
                    bottomMargin: scaleY(8)
                    horizontalCenter: typeSelection.horizontalCenter
                }
                width: typeSelection.width/2

                visible: typeSelection.visible
                height: scaleY(8)
                buttonText: "All Videos"
                onActivated: {
                   content.state="viewing"
                }
            }


            GridView{
                id:typeSelection
                anchors.centerIn: parent
                height:parent.height / 2
                width: parent.width *.85
                model:mediatypefilter
                cellHeight:manager.isProfile ? scaleY(15) : scaleY(20)
                cellWidth:manager.isProfile ? scaleX(28) : scaleX(25)
                delegate:
                    StyledButton{
                    height:typeSelection.cellHeight -5
                    width: typeSelection.cellWidth -5
                    buttonText: name
                   state:"large-fixed"
                    hitArea.onReleased:{
                        mediatypefilter.setSelectionStatus(name);
                        content.state="viewing"
                    }
                }

            }


            MediaListProgressBar{
                id:progress_bar
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }

            FileDetailsGeneric{
                id:fileDetails
            }

            GenericAttributeSelector{
                id:attributeSelector

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

                    PropertyChanges {
                        target: fileDetails
                        state:"closed"
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
                    PropertyChanges {
                        target: fileDetails
                        state:"closed"
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
                    PropertyChanges {
                        target: fileDetails
                        state:"open"
                    }
                }
            ]

        }
    }



    Keys.onReleased: {

        switch(event.key){
        case Qt.Key_Back:
            manager.goBackGrid();
            event.accepted=true
            break;
        case Qt.Key_MediaPrevious:
            if (!manager.goBackGrid())
                event.accepted=false
            else
                event.accepted=true
            break;
        default:
            console.log(event.key)
            break
        }
    }

}
