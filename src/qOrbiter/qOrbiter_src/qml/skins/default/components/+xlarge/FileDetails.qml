import QtQuick 2.2
import org.linuxmce.enums 1.0
import FileAttribute 1.0
import "../components"
import "../../."
GenericPopup{
    id:fileDetails
    title: qsTr("Media Information", "File Details")
    content: Item{
        id:content_item
        property int bgImageProp:manager.q_subType ===("1"||"13") ? 43 : manager.q_attributetype_sort===53 ? 43 :36

        anchors.fill: parent
        focus:true
        onActiveFocusChanged: if(activeFocus)media_options.forceActiveFocus()
        Image{
            id:imdb_background
            anchors.fill: parent
            asynchronous: true
            // onStatusChanged: imdb.status == Image.Ready ? filedetailrect.height = scaleY(100) : ""
        }

        Image {
            id: filedetailsimage
            property bool profile : filedetailsimage.sourceSize.height > filedetailsimage.sourceSize.width ? true : false
            width:profile ? Style.scaleX(25) : Style.scaleX(45)
            height:profile ? Style.scaleY(65) : Style.scaleY(58)
            source:filedetailsclass.screenshot !=="" ? "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot: ""
            smooth: true
            asynchronous: true

        }
        Rectangle{
            id:metaBg
            width: metadata.width
            height: metadata.height
            color: "black"
            opacity: .65
            anchors.centerIn: metadata
            visible: metadata.visible
        }
        Column{
            id:metadata
            visible: false
            spacing: 5
            width:  parent.width /2 //Style.listViewWidth_large
            height: parent.height
            anchors.right: content_item.right
            Text{
                width: parent.width
                id:title
                text:qsTr("Title: ")+filedetailsclass.mediatitle
               // fontSize: largeFontSize
            }
            Text{
                width: parent.width
                id:album
                text:qsTr("Album: ")+filedetailsclass.album
            }

            Text{
                width: parent.width
                id:director
                text:qsTr("Director: ")+filedetailsclass.director
            }

            Text{
                width: parent.width
                id:compwriter
                text:qsTr("Composer/Writer: ")+filedetailsclass.composerlist
            }

            GridView{
                id:perfs
                height: 100
                width: parent.width
                clip:true

                model:filedetailsclass.performersList
                delegate: Text{
                    width: parent.width
                    text:modelData.attribute
                }
            }


            //            StyledText{
            //                width: parent.width
            //                //height:manager.q_mediaType == 5 ? 0 : parent.height *.15
            //                id:artists
            //                text:manager.q_mediaType == 5 ? qsTr("Starring: ")+filedetailsclass.performerlist : qsTr("Artist: ")+filedetailsclass.performerlist
            //            }

            Text{
                width: parent.width
                id:program
                text:qsTr("Program: ")+filedetailsclass.program
            }

            Text{
                width: parent.width
                id:episode
                text:qsTr("Episode: ")+filedetailsclass.episode
            }

            Text{
                width: parent.width
                id:description
                text:qsTr("Synopsis:\n")+filedetailsclass.synop
            }

            Text{
                width: parent.width
                id:studio
                text:qsTr("Studio: ")+filedetailsclass.studio
            }

            Text{
                width: parent.width
                id:release
                text:qsTr("Filename: ")+filedetailsclass.filename
            }

        }

        Item{
            id:lowerMetadata
            anchors{
                left:parent.left
                right:parent.right
                bottom:media_options.top
            }
            height:parent.height *.35 + synop2.height

            Rectangle{ color: "black"; anchors.fill: parent; opacity: .45}

            Column{
                id:focus_col
                focus: true
                onActiveFocusChanged: if(activeFocus) { currentIndex = 0 } else { focus_col.children[currentIndex].current = false }
                property int currentIndex:0
                spacing: 10
                property int max:focus_col.children.length
                Keys.onUpPressed: if(currentIndex === 0 ){ currentIndex= max;} else {currentIndex--}
                Keys.onDownPressed: if(currentIndex=== max) {  performer_list.forceActiveFocus()  } else {currentIndex++}
                onCurrentIndexChanged: {  if(focus_col.children[currentIndex].visible) {focus_col.children[currentIndex].current=true } else { currentIndex++ } }
                anchors{
                    top:parent.top
                    left:parent.left
                }

                Text{
                    id:title2
                    property bool current:false
                    text:qsTr("Title: ")+filedetailsclass.mediatitle
                   // fontSize: largeFontSize
                   font.bold:current
                }

                Text{
                    width: parent.width
                    property bool current:false
                    id:album2
                    text:qsTr("Album: ")+filedetailsclass.album
                    font.bold:current
                }

                Text{
                    id:director2
                    property bool current:false
                    text:qsTr("Director: ")+filedetailsclass.director
                    font.bold:current
                }
            }

            ListView{
                id:performer_list
                focus:true
                anchors{
                    left:parent.left
                    right:studio_list.left
                    bottom:synop2.top
                    top:focus_col.bottom
                }
                keyNavigationWraps: true
                Keys.onUpPressed: focus_col.forceActiveFocus()
                Keys.onDownPressed: media_options.forceActiveFocus()
                onActiveFocusChanged: if(!activeFocus) {currentIndex = -1} else {currentIndex = 0}
                orientation: ListView.Horizontal
                clip:true
                spacing:10
                model:filedetailsclass.performersList
                delegate: Item{
                    height: 240 * (current ? 1.5 : 1)
                    width: 160  * (current ? 1.5 : 1 )
                    PropertyAnimation{
                        properties: "height, width"
                        duration: 220
                    }

                    property bool current: performer_list.currentIndex === index
                    Rectangle{
                        anchors.fill: parent
                        color:"grey"
                        opacity: .45
                        border.color: "white"
                        border.width: current ? 1 : 0
                    }
                    Text{
                        width:parent.width
                        height:parent.height
                        text:modelData.attribute
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.jumpToAttributeGrid(modelData.attributeType, modelData.attributeValue)
                    }
                }
            }

            ListView{
                focus: true
                id:studio_list
                width: parent.width*.35
                height: 200
                model:filedetailsclass.studioList
                anchors{
                    right:parent.right
                    top:parent.top
                }
                delegate:Text{
                    text:modelData.attribute
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.jumpToAttributeGrid(modelData.attributeType, modelData.attributeValue)
                    }
                }
            }

            StyledText{
                id:synop2
                anchors{
                    bottom:parent.bottom
                    left:parent.left
                }
                width: parent.width
                text:filedetailsclass.synop
            }

        }
        Row{
            id:media_options
            focus:true
            property int currentIndex:-1
            spacing: 10
            property int max:media_options.children.length-1
            Keys.onLeftPressed: if(currentIndex === 0 ){ currentIndex= max;} else {currentIndex--}
            Keys.onRightPressed: if(currentIndex=== max) {  currentIndex=0   } else {currentIndex++}
            Keys.onUpPressed: performer_list.forceActiveFocus()
            onCurrentIndexChanged: {media_options.children[currentIndex].forceActiveFocus(); }
            onActiveFocusChanged: if(activeFocus)currentIndex=0
            anchors{
                left:parent.left
                right:parent.right
                bottom:parent.bottom
            }
            height: Style.appNavigation_panelHeight

            LargeStyledButton{
                buttonText: qsTr("Play", "Play Media Selection")
                height: parent.height
                width: parent.width/4
                arrow: activeFocus
                onActivated: {
                    manager.playMedia(filedetailsclass.file, false); fileDetails.close()}
            }
            LargeStyledButton{
                buttonText: qsTr("Queue", "Play Media Selection")
                height: parent.height
                width: parent.width/4
                arrow: activeFocus
                onActivated: {
                    manager.playMedia(filedetailsclass.file, true); fileDetails.close()
                }
            }
            LargeStyledButton{
                buttonText: qsTr("Move", "Move Media Selection")
                height: parent.height
                width: parent.width/4
                arrow: activeFocus
                onActivated: {
                    qmlRoot.createPopup(moveFiles)
                }
            }
            LargeStyledButton{
                buttonText: qsTr("Close", "Close window")
                height: parent.height
                width: parent.width/4
                arrow: activeFocus
                onActivated: fileDetails.close()
            }
            LargeStyledButton{
                buttonText: qsTr("Delete", "Delete Media Selection")
                height: parent.height
                width: parent.width/4
            }

        }

        states: [
            State {
                when:manager.q_mediaType==MediaTypes.LMCE_StoredAudio
                name: "audio"
                PropertyChanges{
                    target: imdb_background
                    source:""
                }
                PropertyChanges { target: episode; visible:false }
                PropertyChanges { target: program; visible:false }
                PropertyChanges { target: description; visible:false }
                PropertyChanges { target: studio; visible:false }
                PropertyChanges { target: director; visible:false }
                PropertyChanges { target: album2; visible:true }
                PropertyChanges { target: compwriter; visible:true }

            },
            State {
                when:manager.q_mediaType==MediaTypes.LMCE_StoredVideo && manager.q_subType == MediaSubtypes.MOVIES
                name: "video"
                PropertyChanges{
                    target: imdb_background
                    source:"http://"+manager.currentRouter+"/lmce-admin/imdbImage.php?type=imdb&file="+filedetailsclass.file+"&val="+content_item.bgImageProp
                }

                PropertyChanges { target: album; visible:false }
                PropertyChanges { target: compwriter; visible:false }
                PropertyChanges { target: album; visible:false }
                PropertyChanges { target: album2; visible:false }
                PropertyChanges { target: episode; visible:false }
                PropertyChanges { target: program; visible:false }

            },
            State{
                name:"photo"
            }

        ]
    }

}
