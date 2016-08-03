import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../."
GenericPopup{
    id:fileDetails
    title: qsTr("Media: %1", "File Details").arg(filedetailsclass.filename)
    content: Item{
        id:content_item
        property int bgImageProp:manager.q_subType ===("1"||"13") ? 43 : manager.q_attributetype_sort===53 ? 43 :36

        anchors.fill: parent
        focus:true
        onActiveFocusChanged: if(activeFocus)media_options.forceActiveFocus()

        MouseArea{
            anchors.fill: parent
        }

        Component{
            id:attribSelectButton

            Item{
                width: parent.width
                height: Style.appButtonHeight /2
                Rectangle{
                    color: index % 2 ? "black" : "darkgrey"
                    opacity: .45
                    anchors.fill: parent
                }

                StyledText{
                    color: "white"
                    fontSize: Style.appFontSize_title
                    text:modelData.attribute
                    anchors.centerIn: parent
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked:{
                        manager.jumpToAttributeGrid(modelData.attributeType, modelData.attributeValue)
                        fileDetails.close()
                    }
                }
            }
        }

        Image{
            id:imdb_background
            fillMode: Image.PreserveAspectCrop
            anchors.fill: parent
            // onStatusChanged: imdb.status == Image.Ready ? filedetailrect.height = scaleY(100) : ""
        }

        Image {
            id: filedetailsimage
            property bool profile : filedetailsimage.sourceSize.height > filedetailsimage.sourceSize.width ? true : false
            width:profile ? Style.scaleX(25) : Style.scaleX(45)
            height:profile ? Style.scaleY(65) : Style.scaleY(58)
            fillMode: Image.PreserveAspectCrop
            // source:filedetailsclass.screenshot !=="" ? "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
            smooth: true
            anchors{
                verticalCenter: parent.verticalCenter
                left:parent.left
            }
        }


        StyledText{
            id:title
            text:qsTr("Title: %1").arg(filedetailsclass.mediatitle)
            fontSize: Style.appFontSize_title
            anchors{
                left:parent.left
                top:parent.top
                right:parent.right
            }
        }

        StyledText{
            id:album
            text:qsTr("Album: %1, Released: %2").arg(filedetailsclass.album).arg(filedetailsclass.releaseDate)
            anchors{
                top:title.bottom
                left: title.left
                right:parent.right
            }
        }

        StyledText{
            id:track
            text:qsTr("Track: %1").arg(filedetailsclass.track)
            anchors{
                top:album.bottom
                left: album.left
                right:parent.right
            }
            visible:album.visible
        }

        Item{
            id:tv_meta
            visible:false
            anchors{
                top:parent.top
                left: parent.left
            }
            width: parent.width/2
            height: Style.scaleY(20)

            Column{
                anchors.fill: parent
                StyledText{
                    id:program
                    fontSize: Style.appFontSize_title
                    text:qsTr("Program: %1").arg(filedetailsclass.program)
                    visible: tv_meta.visible
                }

                StyledText{
                    id:episode
                    fontSize: Style.appFontSize_title
                    text:qsTr("Episode: %1").arg(filedetailsclass.episode)
                    visible:tv_meta.visible
                }

                StyledText{
                    id:episode_season
                    fontSize: Style.appFontSize_title
                    text:qsTr("Season: %1, Episode Number %2").arg(filedetailsclass.season).arg(filedetailsclass.episodeNumber)
                    visible:tv_meta.visible
                }
            }

        }



        Flickable{
            width: Style.scaleX(50)
            height: parent.height
            anchors.right: content_item.right
            contentWidth: metadata.width
            contentHeight: metadata.height

            Column{
                id:metadata
                spacing: 5
                width: Style.scaleX(50)
                height: Style.scaleY(65)
                anchors.centerIn: parent
                property int containerHeight:Style.scaleY(25)
                Rectangle{
                    id:spacer
                    color:"transparent"
                    height: 0
                }

                GenericListModel{
                    id:directors
                    height: extended ? metadata.containerHeight : headerHeight
                    extended: false
                    width: parent.width
                    clip:true
                    label:qsTr("%n Director(s)", "", filedetailsclass.directorList.length)
                    model:filedetailsclass.directorList
                    delegate: attribSelectButton
                }

                GenericListModel{
                    id:comps
                    height:extended ? metadata.containerHeight : headerHeight
                    extended: false
                    width: parent.width
                    clip:true
                    label:qsTr("%n Writer(s)", "", filedetailsclass.writerList.length)
                    model:filedetailsclass.writersList
                    delegate:attribSelectButton
                }

                GenericListModel{
                    id:perfs
                    height: extended ? metadata.containerHeight : headerHeight
                    width: parent.width
                    clip:true
                    label:qsTr("%n Performer(s)", "0", filedetailsclass.performersList.length)
                    extended: false
                    model:filedetailsclass.performersList
                    delegate: attribSelectButton
                }

                GenericListModel{
                    id:genres
                    height: extended ? metadata.containerHeight : headerHeight
                    width: parent.width
                    clip:true
                    label:qsTr("%n Genres(s)", "0", filedetailsclass.genreList.length)
                    extended: false
                    model:filedetailsclass.genreList
                    delegate: attribSelectButton
                }


                GenericListModel{
                    id:studios
                    height:extended ? metadata.containerHeight : headerHeight
                    width: parent.width
                    clip:true
                    extended: false
                    label:qsTr("%n Studio(s)", "0", filedetailsclass.studioList.length)
                    model:filedetailsclass.studioList
                    delegate: attribSelectButton
                }
            }
        }

        Rectangle{
            anchors.fill: description
            color: "black"
            opacity: .65
        }

        StyledText{
            width: parent.width
            id:description
            text:qsTr("Synopsis:\n %1").arg(filedetailsclass.synop)
            anchors{ bottom:media_options.top }
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
                buttonText: qsTr("Close", "Close orbiterWindow")
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
                PropertyChanges{target: imdb_background; source:""  }

                PropertyChanges{
                    target:filedetailsimage
                    source:filedetailsclass.screenshot !=="" ? "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
                    height: manager.isProfile? Style.scaleX(50) : Style.scaleY(50) ; width: filedetailsimage.height
                }
                PropertyChanges{ target: spacer; height:Style.scaleY(30); width:height }
                PropertyChanges{ target: directors; height:0; visible:false }
                PropertyChanges{ target: album; visible:true }
                PropertyChanges{ target: comps; visible:true; extended:true }
                PropertyChanges{ target: perfs; extended:true; visible:true }
                PropertyChanges{ target: studios; height:0; visible:false }
                PropertyChanges{ target: description; visible:false; height:0}
            },

            State{
                name:"tv"
                extend: "video"
                when:manager.q_attributeType_sort==Attributes.Title && manager.q_subType==MediaSubtypes.TVSHOWS
                PropertyChanges{
                    target:filedetailsimage
                    source:filedetailsclass.screenshot !=="" ? "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
                    //  height: manager.isProfile? Style.scaleX(50) : Style.scaleY(50) ; width: filedetailsimage.height
                }
                PropertyChanges{ target: spacer; height:Style.scaleY(8); width:height }
                PropertyChanges{ target: tv_meta; visible: true; }
                PropertyChanges{ target: title; visible: false; }
            },

            State {
                when:manager.q_mediaType==MediaTypes.LMCE_StoredVideo
                name: "video"
                PropertyChanges{
                    target: imdb_background
                    source:"http://"+manager.currentRouter+"/lmce-admin/imdbImage.php?type=imdb&file="+filedetailsclass.file+"&val="+content_item.bgImageProp
                }
                PropertyChanges{ target: spacer; height:Style.scaleY(6); width:height }
                PropertyChanges{ target:filedetailsimage; source:""     }
                PropertyChanges{ target: album; visible:false }
                PropertyChanges{ target: comps; height:0; visible:false }
                PropertyChanges{ target: perfs;  visible:true }
                PropertyChanges{ target: directors;  visible:true }
                PropertyChanges{ target: studios;  visible:true }
                PropertyChanges{ target: tv_meta; visible: false; }
                PropertyChanges{ target: description; visible:true; }
            },

            State{
                name:"photo"
            },

            State{
                name:""
                PropertyChanges{
                    target: imdb_background
                    source:""
                }
            }
        ]
    }

}
