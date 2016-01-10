import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"

Item{
    id:multi_view_list

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
    property alias currentView:media_grid
    property int bug:0

    signal options()

    Connections {
        target: manager.getDataGridModel("MediaFile", 63)
        onScrollToItem: {
            manager.currentIndex=item
            console.log("scroll to item : " + item+ " of "+media_grid.count);
        }
    }
    Connections{
        target:manager

        onCurrentIndexChanged:{
            media_grid.currentIndex=manager.currentIndex
            media_grid.positionViewAtIndex(manager.currentIndex, ListView.Beginning)
        }

        onDgRequestFinished:{
            media_grid.currentIndex=manager.currentIndex
            media_grid.positionViewAtIndex(manager.currentIndex, ListView.Beginning)
        }
    }

    function seek(seekToken) {
        manager.seekGrid("MediaFile", seekToken);
    }

    function load(){
        currentView.model= manager.getDataGridModel("MediaFile", 63)
    }

    Component{
        id:videoItem
        VideoDelegate {
            id: videoDelegate

        }
    }

    Connections {
        target: manager
        onScrollToItem: {
            console.log("scroll to item : " + item+ " of "+media_grid.count);
            media_grid.positionViewAtIndex(item, ListView.Beginning);
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
            clip:true
            anchors{
                left:parent.left
                right:parent.right
                bottom:parent.bottom
                top:parent.top
            }


            cellHeight: currentCellHeight
            cellWidth:currentCellWidth
            visible:true //current_view_type===1
            delegate:currentDelegate
            Keys.onDigit1Pressed: options()
            Keys.onMenuPressed: options()
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
                    currentCellWidth:Style.scaleY(25) //manager.isProfile ? Style.scaleX(10) : Style.scaleX(14.25)
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

