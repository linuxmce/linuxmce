import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"
import "../"
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
            source:filedetailsclass.screenshot !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
            smooth: true
        }

        Column{
           id:metadata
           spacing: 5
           width: Style.listViewWidth_medium
           height: parent.height
           anchors.right: content_item.right
           StyledText{
               width: parent.width
               id:title
               text:filedetailsclass.mediatitle
           }
           StyledText{
               width: parent.width
               id:program
               text:filedetailsclass.program
           }
           StyledText{
                width: parent.width
               id:episode
               text:filedetailsclass.episode
           }
        }

        Row{
            id:media_options
            focus:true
            property int currentIndex:-1
            spacing: Style.listViewMargin
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
                arrow: activeFocus
                onActivated: {manager.playMedia(filedetailsclass.file); fileDetails.close()}
            }
            LargeStyledButton{
                buttonText: qsTr("Move", "Move Media Selection")
                height: parent.height
                arrow: activeFocus
            }
            LargeStyledButton{
                buttonText: qsTr("Delete", "Delete Media Selection")
                height: parent.height

            }
            LargeStyledButton{
                buttonText: qsTr("Close", "Close Window")
                height: parent.height
                arrow: activeFocus
                onActivated: fileDetails.close()
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
            },
            State {
                when:manager.q_mediaType==MediaTypes.LMCE_StoredVideo
                name: "video"
                PropertyChanges{
                    target: imdb_background
                    source:"http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=imdb&file="+filedetailsclass.file+"&val="+content_item.bgImageProp
                }

            },
            State{
                name:"photo"
            }

        ]
    }

}
