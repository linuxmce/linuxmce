import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"
import "../"
GenericPopup{
    id:fileDetails
    title: qsTr("Media Information for %1", "File Details").arg(filedetailsclass.filename)
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
           // source:filedetailsclass.screenshot !=="" ? "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
            smooth: true
            anchors{
                verticalCenter: parent.verticalCenter
                left:parent.left
            }
        }

        StyledText{
            id:title
            text:qsTr("Title: ")+filedetailsclass.mediatitle
            fontSize: Style.appFontSize_title
            anchors{
                left:parent.left
                top:parent.top
            }
        }

        Column{
           id:metadata
           spacing: 5
           width: Style.listViewWidth_medium
           height: parent.height
           anchors.right: content_item.right

           StyledText{
                width: parent.width
               id:album
               text:qsTr("Album: ")+filedetailsclass.album
           }
           GenericListModel{
               id:directors
                height: extended ? parent.height*.30 : parent.height*.10
                 extended: false
               width: parent.width
               clip:true
               label:"Director"
               model:filedetailsclass.directorList
               delegate: StyledText{
                   color:"white"
                   width: parent.width
                   text:modelData.attribute
                   MouseArea{
                       anchors.fill: parent
                       onClicked:{
                           manager.jumpToAttributeGrid(modelData.attributeType, modelData.attributeValue)
                           fileDetails.close()
                       }
                   }
               }
           }
           GenericListModel{
               id:comps
                height: extended ? parent.height*.30 : parent.height*.10
                 extended: false
               width: parent.width
               clip:true
               label:"Writers"
               model:filedetailsclass.writersList
               delegate: StyledText{
                   color:"white"
                   width: parent.width
                   text:modelData.attribute
                   MouseArea{
                       anchors.fill: parent
                       onClicked:{
                           manager.jumpToAttributeGrid(modelData.attributeType, modelData.attributeValue)
                           fileDetails.close()
                       }
                   }
               }
           }
           GenericListModel{
               id:perfs
              height: extended ? parent.height*.30 : parent.height*.10
               width: parent.width
               clip:true
               label:"Performers"
               extended: true
               model:filedetailsclass.performersList
               delegate: StyledText{
                   color:"white"
                   width: parent.width
                   text:modelData.attribute

                   MouseArea{
                       anchors.fill: parent
                       onClicked:{
                           manager.jumpToAttributeGrid(modelData.attributeType, modelData.attributeValue)
                           fileDetails.close()
                       }
                   }
               }
           }
           StyledText{
               width: parent.width
               id:program
               text:qsTr("Program: ")+filedetailsclass.program
               visible: filedetailsclass.program !== ""
           }
           StyledText{
                width: parent.width
               id:episode
               text:qsTr("Episode: ")+filedetailsclass.episode
                 visible: filedetailsclass.episode !== ""
           }

           GenericListModel{
               id:studios
               height: extended ? parent.height*.30 : parent.height*.10
               width: parent.width
               clip:true
                extended: true
               label:qsTr("Studios")
               model:filedetailsclass.studioList
               delegate: StyledText{
                   color:"white"
                   width: parent.width
                   text:modelData.attribute
                   MouseArea{
                       anchors.fill: parent
                       onClicked:{
                           manager.jumpToAttributeGrid(modelData.attributeType, modelData.attributeValue)
                           fileDetails.close()
                       }
                   }
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
           text:qsTr("Synopsis:\n")+filedetailsclass.synop
           anchors{
               bottom:media_options.top
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
                }

                PropertyChanges{ target: directors; height:0; visible:false }
                PropertyChanges{ target: album; visible:true }
                PropertyChanges{ target: comps; height:comps.extended ? parent.height*.30 : parent.height*.10; visible:true }
                PropertyChanges{ target: perfs; height:perfs.extended ? parent.height*.30 : parent.height*.10; visible:true }
                PropertyChanges{ target: studios; height:0; visible:false }
                PropertyChanges{ target: program; visible: false; height:0}
                PropertyChanges{ target: episode; visible: false; height:0}
                PropertyChanges{ target: description; visible:false; height:0}


            },
            State {
                when:manager.q_mediaType==MediaTypes.LMCE_StoredVideo
                name: "video"
                PropertyChanges{
                    target: imdb_background
                    source:"http://"+manager.currentRouter+"/lmce-admin/imdbImage.php?type=imdb&file="+filedetailsclass.file+"&val="+content_item.bgImageProp
                }

                PropertyChanges{ target:filedetailsimage; source:""     }
                PropertyChanges{ target: album; height:0; visible:false }
                PropertyChanges{ target: comps; height:0; visible:false }
                PropertyChanges{ target: perfs; height:perfs.extended ? parent.height*.30 : parent.height*.10; visible:true }
                 PropertyChanges{ target: directors; height:directors.extended ? parent.height*.20 : parent.height*.10; visible:true }
                PropertyChanges{ target: studios; height:studios.extended ? parent.height*.30 : parent.height*.10; visible:true }
                PropertyChanges{ target: program; visible: false; }
                PropertyChanges{ target: episode; visible: false; }
                PropertyChanges{ target: description; visible:true; }

            },

            State{
                name:"tv"
                extend: "video"
                when:manager.q_subType == (MediaSubtypes.TVSHOWS || MediaSubtypes.TVSHOWS )
                PropertyChanges{ target: program; visible: true; }
                PropertyChanges{ target: episode; visible: true; }

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
