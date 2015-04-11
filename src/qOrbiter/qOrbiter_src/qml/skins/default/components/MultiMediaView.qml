import QtQuick 2.3
import org.linuxmce.enums 1.0
import "../components"

Item{
    id:multi_view_list
    anchors.fill: parent
    focus:true
    onActiveFocusChanged: media_grid.forceActiveFocus()
     property variant currentDelegate:manager.q_mediaType==5 ? videoItem :audioItem
    property int currentCellHeight:currentCellWidth
    property int currentCellWidth:Style.scaleX(25)
    
    property variant dvdPosterDimensions:{"w":755, "h":1080 }
    property variant hdPosterDimensions:{"w":955, "h":1080 }
    property variant cdDimensions:{"w":320, "h":230}
    property variant mameArtDimensions:{"w":320, "h":230}
    
    property double dvdPosterRatio:1080/955
    property double hdPosterRatio:1080/755
    property double cdCoverRatioFront:1080/1080
    property double cdCoverRatioBack:1080/1264
    property double vcdRatio:1080/1080
    property double vhsRatio:1280/620
    property int itemBuffer:25

    Component{
        id:videoItem
        VideoDelegate {
            id: videoDelegate
        }
    }

    Component{
        id:audioItem
        AudioDelegate {
            id: audioDelegate
        }
    }
    
    GridView{
        id:media_grid
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:parent.top
        }
        cellHeight: currentCellHeight
        cellWidth:currentCellWidth
        model:manager.getDataGridModel("MediaFile", 63)
        visible:true //current_view_type===1
        delegate:currentDelegate
    }
    
    states: [
        State {
            name: "audio"
            //  when:manager.q_mediaType == Mediatypes.STORED_AUDIO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight:manager.isProfile ? Style.scaleX(18) : Style.scaleX(14)
                currentCellWidth:currentCellHeight
            }
            
        },
        State {
            name: "video"
            //  when:manager.q_mediaType == Mediatypes.STORED_VIDEO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: Style.scaleY(24)
                currentCellWidth:Style.scaleX(19)
            }
            StateChangeScript{
                script: manager.setGridStatus(false)
            }
        },
        State {
            name: "video-default"
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: Style.scaleY(23)
                currentCellWidth:Style.scaleX(17)
            }
        },
        State {
            name: "tv"
            when:manager.q_subType==MediaSubtypes.TVSHOWS && manager.q_pk_attribute==""
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: Style.scaleY(24)
                currentCellWidth:Style.scaleX(33)
            }
        },
        State {
            name: "movies"
            when: manager.q_subType==MediaSubtypes.MOVIES && manager.q_mediaType==MediaTypes.LMCE_StoredVideo
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: currentCellWidth*hdPosterRatio
                currentCellWidth:manager.b_orientation ? Style.scaleX(10) : Style.scaleX(14.25)
            }
        },
        State {
            name: "music-video"
            when: manager.q_subType==MediaSubtypes.MUSICVIDEOS && manager.q_mediaType==MediaTypes.LMCE_StoredVideo
            extend:"tv"
            
        },
        State {
            name: "seasons"
            when:manager.q_attributeType_sort==Attributes.TV_Season_ID && manager.q_subType==MediaSubtypes.TVSHOWS
            extend:"tv"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: currentCellWidth*hdPosterRatio
                currentCellWidth:Style.scaleX(20)
            }
        },
        State {
            name: "episodes"
            when:manager.q_attributeType_sort==Attributes.Title && manager.q_subType==MediaSubtypes.TVSHOWS
            extend:"tv"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: Style.scaleY(33)
                currentCellWidth:Style.scaleX(33)
            }
        }
        ,
        State {
            name: "radio"
            when:manager.q_mediaType == MediaTypes.NP_OTARadio
            extend:"audio"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: Style.scaleY(33)
                currentCellWidth:Style.scaleX(33)
            }
        }
    ]
}
