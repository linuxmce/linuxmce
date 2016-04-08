import QtQuick 2.4
import org.linuxmce.enums 1.0
import "../components"
import "../"
GenericPopup{
    id:fileDetails
    title: qsTr("Media Information", "File Details")
    content: Item{
        id:content_item
        property int bgImageProp:manager.q_subType ===("1"||"13") ? 43 : manager.q_attributetype_sort===53 ? 43 :36
        property double moviePosterRatio: 1080/755
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
            width:Style.scaleX(55)
            height:width* content_item.moviePosterRatio
            anchors.verticalCenter: parent.verticalCenter
            source:filedetailsclass.screenshot !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
            smooth: true
        }

        Column{
            id:metadata
            spacing: 5
            width: manager.isProfile ? Style.scaleX(35) : Style.scaleX(25)
            height: content_item.height
            anchors.top: content_item.top
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
            StyledText{
                width: parent.width
                id:synop
                text: filedetailsclass.synop
            }
            StyledText{
                width: parent.width
                id:director
                text: filedetailsclass.director
            }
            StyledText{
                width: parent.width
                id:studio
                text:filedetailsclass.studio
            }
            StyledText{
                width: parent.width
                id:rating
                text:filedetailsclass.rating
            }

            StyledText{
                width: parent.width
                id:album
                text:filedetailsclass.album
            }
            StyledText{
                width: parent.width
                id:track
                text:filedetailsclass.track
            }

            StyledText{
                width: parent.width
                id:performers
                text:content_item.state
            }
            StyledText{
                width: parent.width
                id:path
                text:filedetailsclass.path+filedetailsclass.filename
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

            StyledButton{
                buttonText: qsTr("Play", "Play Media Selection")
                height: parent.height

                onActivated: {manager.playMedia(filedetailsclass.file); fileDetails.close()}
            }
            StyledButton{
                buttonText: qsTr("Move", "Move Media Selection")
                height: parent.height

            }
            StyledButton{
                buttonText: qsTr("Delete", "Delete Media Selection")
                height: parent.height

            }
            StyledButton{
                buttonText: qsTr("Close", "Close orbiterWindow")
                height: parent.height

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
                name: "movies"
                when: manager.q_subType==MediaSubtypes.MOVIES && manager.q_mediaType==MediaTypes.LMCE_StoredVideo
                extend: "video"
                PropertyChanges {
                    target: filedetailsimage
                    width:manager.isProfile ? Style.scaleX(100) : Style.scaleX(35)
                }
                PropertyChanges {
                    target: title
                    visible:false
                }
                PropertyChanges{
                    target: imdb_background
                    source:!manager.isProfile ? "http://"+manager.currentRouter+"/lmce-admin/imdbImage.php?type=imdb&file="+filedetailsclass.file+"&val="+content_item.bgImageProp : ""
                }
            },
            State{
                name:"photo"
            }

        ]
    }

}
