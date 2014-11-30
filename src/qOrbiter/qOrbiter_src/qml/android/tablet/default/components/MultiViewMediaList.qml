import QtQuick 1.1
import org.linuxmce.enums 1.0
import "../components"

Item{
    id:multi_view_list
    anchors{
        left:parent.left
        right:parent.right
        top:parent.top
        bottom:parent.bottom
    }

    property int currentCellHeight:currentCellWidth
    property int currentCellWidth:scaleX(25)

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
    clip:true

    Component{
        id:audioItem
        AudioDelegate{}    }

    Component{
        id:videoItem
        VideoDelegate{}
    }

    property variant currentDelegate:manager.q_mediaType==5 ? videoItem :audioItem

    Connections{
        target:manager
        onModelChanged:{
            media_grid.positionViewAtIndex(manager.currentIndex, GridView.Beginning)
            manager.currentIndex=-1
        }
    }

    Component.onCompleted: {

        if(manager.q_mediaType=="4"){
            multi_view_list.state="audio"
        } else if(manager.q_mediaType=="5"){
            multi_view_list.state="video"
            manager.setGridStatus(false);
        } else {

        }
        console.log("Getting grid for "+manager.q_mediaType);

        media_grid.positionViewAtIndex(0, ListView.Beginning)
    }

    Connections {
        target: manager.getDataGridModel("MediaFile", 63)
        onScrollToItem: {
            console.log("scroll to item : " + item+ " of "+media_grid.count);
            media_grid.positionViewAtIndex(item, ListView.Beginning);
        }
    }
    ListView{
        id:media_list
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:parent.top
            margins: scaleX(2)
        }
        delegate:currentDelegate
        visible: current_view_type===2
        spacing:scaleY(2)
        clip:true
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

    PathView{
        id:media_path
        anchors.fill: parent
        // model:manager.getDataGridModel("MediaFile", 63)
        visible:current_view_type===3
    }
    states: [
        State {
            name: "audio"
            //  when:manager.q_mediaType == Mediatypes.STORED_AUDIO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight:manager.isProfile ? scaleX(18) : scaleX(14)
                currentCellWidth:currentCellHeight
            }

        },
        State {
            name: "video"
            //  when:manager.q_mediaType == Mediatypes.STORED_VIDEO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(19)
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
                currentCellHeight: scaleY(23)
                currentCellWidth:scaleX(17)
            }
        },
        State {
            name: "tv"
            when:manager.q_subType==MediaSubtypes.TVSHOWS && manager.q_pk_attribute==""
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(25)
            }
        },
        State {
            name: "movies"
            when: manager.q_subType==MediaSubtypes.MOVIES && manager.q_mediaType==MediaTypes.LMCE_StoredVideo
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: currentCellWidth*hdPosterRatio
                currentCellWidth:manager.b_orientation ? scaleX(10) : scaleX(14.25)
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
                currentCellWidth:scaleX(20)
            }
        },
        State {
            name: "episodes"
            when:manager.q_attributeType_sort==Attributes.Title && manager.q_subType==MediaSubtypes.TVSHOWS
            extend:"tv"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(33)
                currentCellWidth:scaleX(33)
            }
        }
        ,
        State {
            name: "radio"
            when:manager.q_mediaType == MediaTypes.NP_OTARadio
            extend:"audio"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(33)
                currentCellWidth:scaleX(33)
            }
        }
    ]

}
