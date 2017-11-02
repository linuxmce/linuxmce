import QtQuick 2.1
import "../components"
StyledScreen {
    id:screen_forty_seven_of_nine
    description: "Browse media library"
    screen: "Media Browser"
    property int depth:0
    focusTarget: content

    Item{
        id:content
        anchors.fill: parent
        state: manager.q_mediaType =="5" ? "selection" : "viewing"
        focus:true
        onActiveFocusChanged: {
            if(activeFocus){
                console.log("content gained active focus")
                if(manager.q_mediaType =="5"){
                    typeSelection.forceActiveFocus()
                } else{
                    mediaList.forceActiveFocus()
                }
            } else {
                console.log("content lost focus")
            }


        }

//        Component.onCompleted: {
//            if(manager.q_mediaType =="5"){
//               content.state="selection"
//            } else{
//               content.state="viewing"
//            }
//        }



        //    onActiveFocusChanged: {
        //        console.log(mediaList.parent.objectName+ " recieved focus, passing to mediaList grid")
        //        mediaList.forceActiveFocus()
        //    }

        Connections{
            target: filedetailsclass
            onShowDetailsChanged:{
                content.state = "detail"
                depth--
            }
        }

        MediaNavHeader{
            id:navHeader

        }

        MultiViewMediaList{
            id:mediaList
            anchors{
                top:navHeader.bottom
                left:parent.left
                right:parent.right
                bottom:filtering.top
            }
        }

        MediaFilters{
            id:filtering
            height:scaleY(0)
        }

        Item{
            id:filter_view
            height: scaleY(8)
            width: childrenRect.width
            visible:false
            signal select()
            anchors.bottom: filtering.top
            x:filtering.currentFilterX
            property variant currentFilterModel: ""
            onCurrentFilterModelChanged: {
                if(currentFilterModel !==""){
                    visible=true
                    filterView.visible = true
                }else{
                    visible = false
                    filterView.visible = false
                }
            }
            function selectItem(){
                select()
                mediaList.currentIndex=0
                depth++
            }
            ListView{
                id:filterView
                height: scaleY(65)
                width: scaleX(20)
                model:parent.currentFilterModel
                currentIndex: -1
                clip:true
                visible: false
                delegate: StyledButton{
                    buttonText: name
                    Connections{
                        target: filter_view
                        onSelect:if(filterView.currentIndex === index) filter_view.currentFilterModel.setSelectionStatus(name)
                    }
                }
            }
        }
        MediaListProgressBar{
            id:progress_bar
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }

        GridView{
            id:typeSelection
            anchors.centerIn: content
            height:content.height / 2
            width: content.width *.85
            focus:true
            model:mediatypefilter
            cellWidth:scaleX(25)
            cellHeight:scaleY(15)
            visible:false
            onActiveFocusChanged: {
                console.log("type selection list active focus is "+activeFocus)
                if(!visible){
                    mediaList.forceActiveFocus()
                } else {

                }
            }

            delegate:
                StyledButton{
                buttonText: name
                textSize: 48
                useLocalKeys: true
                idxMatch: typeSelection.currentIndex===index ? true : false
                state:"fixed"
                hitArea.onReleased:{
                    content.state="viewing"
                    mediatypefilter.setSelectionStatus(name);
                    mediaList.forceActiveFocus()
                }
                onActivated: {
                    content.state="viewing"
                    mediatypefilter.setSelectionStatus(name);
                    mediaList.forceActiveFocus()
                }
            }

        }

        Item{
            id:infoPanel
            height: content.height
            width: 0
            focus:true
            anchors.right: content.right
            onActiveFocusChanged: if(activeFocus) {file_details.forceActiveFocus()}
            Rectangle{
                anchors.fill: parent
                color:"black"
                opacity:.85
                radius:5
            }
            FileDetails{
                id:file_details
            }

            Behavior on width {
                PropertyAnimation{
                    duration: appstyle.globalAnimationSpeed
                }
            }
            Keys.onEscapePressed: {
                console.log("Setting grid back in focus")
                console.log("Changing focus from details")
                mediaList.forceActiveFocus()
                content.state = "viewing"
            }
        }
        states: [
            State {
                name: "selection"
                extend:""
                PropertyChanges {
                    target: mediaList
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
                    target: infoPanel
                    width:0
                }

//                StateChangeScript{
//                    script: {
//                        manager.resetModelAttributes;
//                        mediatypefilter.resetStates();
//                        attribfilter.resetStates();
//                    }
//                }
                //("5", "", "", "", "1,2", "", "13", "", "2", "")
                //("5", "1", "", "", "1,2", "", "12", "", "2", "")
                //("5", "1", "", "", "1,2", "", "52", "", "2", "7691")
                //("5", "1", "", "", "1,2", "", "12", "", "2", "")
            },
            State {
                name: "viewing"
                extend:""
                PropertyChanges {
                    target: progress_bar
                    visible:true
                }
                PropertyChanges {
                    target: mediaList
                    visible:true
                }
                PropertyChanges{
                    target:typeSelection
                    visible:false
                }
                PropertyChanges {
                    target: infoPanel
                    width:0
                }

            },
            State {
                name: "detail"
                extend:""
                PropertyChanges {
                    target: progress_bar
                    visible:false
                }
                PropertyChanges {
                    target: mediaList
                    visible:false
                }
                PropertyChanges{
                    target:typeSelection
                    visible:false
                }
                PropertyChanges {
                    target: infoPanel
                    width:content.width
                }

            },
            State {
                name: "filtering"
                extend:""
                PropertyChanges {
                    target: filtering
                    height:scaleY(8)
                }
                PropertyChanges {
                    target: filterView
                    visible:true
                }




            }
        ]

    }
    ListModel{
        id:indexStack
    }

}
