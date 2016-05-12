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
            // onStatusChanged: imdb.status == Image.Ready ? filedetailrect.height = scaleY(100) : ""
        }

        Image {
            id: filedetailsimage
            property bool profile : filedetailsimage.sourceSize.height > filedetailsimage.sourceSize.width ? true : false
            width:profile ? Style.scaleX(25) : Style.scaleX(45)
            height:profile ? Style.scaleY(65) : Style.scaleY(58)
            source:filedetailsclass.screenshot !=="" ? "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
            smooth: true

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
            StyledText{

                width: parent.width
                id:title
                text:qsTr("Title: ")+filedetailsclass.mediatitle
                fontSize: largeFontSize
            }
            StyledText{
                width: parent.width
                id:album
                text:qsTr("Album: ")+filedetailsclass.album
            }

            StyledText{
                width: parent.width
                id:director
                text:qsTr("Director: ")+filedetailsclass.director
            }

            StyledText{
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
                delegate: StyledText{
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

            StyledText{
                width: parent.width
                id:program
                text:qsTr("Program: ")+filedetailsclass.program
            }

            StyledText{
                width: parent.width
                id:episode
                text:qsTr("Episode: ")+filedetailsclass.episode
            }

            StyledText{
                width: parent.width
                id:description
                text:qsTr("Synopsis:\n")+filedetailsclass.synop
            }

            StyledText{
                width: parent.width
                id:studio
                text:qsTr("Studio: ")+filedetailsclass.studio
            }

            StyledText{
                width: parent.width
                id:release
                text:qsTr("Filename: ")+filedetailsclass.filename
            }

        }

        Item{
            anchors{
                left:parent.left
                right:parent.right
                bottom:media_options.top
            }
            height:parent.height *.35 + synop2.height

            Rectangle{ color: "black"; anchors.fill: parent; opacity: .45}

            StyledText{
                anchors{
                    top:parent.top
                    left:parent.left
                }
                id:title2
                text:qsTr("Title: ")+filedetailsclass.mediatitle
                fontSize: largeFontSize
            }

            StyledText{
                anchors{
                    top:parent.top
                    left:parent.left
                }
                width: parent.width
                id:album2
                text:qsTr("Album: ")+filedetailsclass.album
            }

            StyledText{
                id:director2
                text:qsTr("Director: ")+filedetailsclass.director
                anchors{
                    left:parent.left
                    top:title2.bottom
                }
            }

            ListView{

                anchors{
                    left:parent.left
                    right:studio2.left
                    bottom:synop2.top
                    top:director2.bottom
                }

                orientation: ListView.Horizontal
                clip:true

                spacing:10
                model:filedetailsclass.performersList
                delegate: Item{
                    height: 240
                    width: 160
                    Rectangle{
                        anchors.fill: parent
                        color:"grey"
                        opacity: .45
                    }
                    StyledText{
                        width:parent.width
                        height:parent.height
                        text:modelData.attribute
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

            ListView{
                width: parent.width*.35
                height: 200
                id:studio2
               model:filedetailsclass.studioList
                anchors{
                    right:parent.right
                    top:parent.top
                }
                delegate:StyledText{
                    text:modelData.attribute
                }
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
                onActivated: {manager.playMedia(filedetailsclass.file); fileDetails.close()}
            }
            LargeStyledButton{
                buttonText: qsTr("Move", "Move Media Selection")
                height: parent.height
                width: parent.width/4
                arrow: activeFocus
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
