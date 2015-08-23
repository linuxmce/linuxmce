import QtQuick 2.4
import org.linuxmce.enums 1.0
import "../components"
import "../../."

Item{
    id:multi_view_list

    focus:true
    onActiveFocusChanged: media_grid_container.forceActiveFocus()
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
    property int currentView: manager.isProfile ? 1 : 0
    property alias currentGrid:media_grid_container.item
    property int lastViewIndex: manager.currentIndex

    signal options()

    onCurrentViewChanged: {
        console.log("handle change view.")
        switch(currentView){
        case 0:media_grid_container.sourceComponent=grid_view; break;
        case 1:media_grid_container.sourceComponent=list_view; break;
        default:media_grid_container.sourceComponent=list_view;
        }
    }

    function load(){
        currentGrid.model= manager.getDataGridModel("MediaFile", 63);
        return;
    }

    function seek(seekToken) {
        manager.seekGrid("MediaFile", seekToken);
    }

    Component{
        id:videoItem
        VideoDelegate {
            id: videoDelegate
        }
    }

    Connections{
        target:manager

        onCurrentIndexChanged:{
            lastViewIndex=manager.currentIndex
            currentGrid.currentIndex=manager.currentIndex
            currentGrid.positionViewAtIndex(manager.currentIndex, ListView.Beginning)
        }

        onDgRequestFinished:{
           lastViewIndex=manager.currentIndex
            currentGrid.currentIndex=manager.currentIndex
            currentGrid.positionViewAtIndex(manager.currentIndex, ListView.Beginning)
        }
    }

    Connections {
        target: manager.getDataGridModel("MediaFile", 63)
        onScrollToItem: {
            manager.currentIndex=item
            console.log("scroll to item : " + item+ " of "+media_grid.count);
        }
    }

    Component{
        id:audioItem
        AudioDelegate {
            id: audioDelegate
        }
    }

    Loader{
        id:media_grid_container
        onSourceChanged: {
            if(media_grid_container.status==Loader.Ready){
                console.log("!!onSourceChanged!!!")
              //  item.model = manager.getDataGridModel("MediaFile", 63)

        }

        onLoaded: {
            if(media_grid_container.status==Loader.Ready){
                console.log("!!!on loaded !!!")
                item.model = manager.getDataGridModel("MediaFile", 63)
                item.positionViewAtIndex(lastViewIndex, ListView.Beginning)
            }
            }
        }

        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:parent.top
        }

    }

    Component{
        id:grid_view
        GridView{
            id:media_grid
            anchors.fill: parent
            cellHeight: currentCellHeight
            cellWidth:currentCellWidth
            //current_view_type===1
            delegate:currentDelegate
            Keys.onDigit1Pressed: options()
            Keys.onMenuPressed: options()
        }
    }

    Component{
        id:list_view
        ListView{
            id:media_list
            clip:true
            spacing: 5
            anchors.fill: parent
            delegate: Item {
                id: rowDelegate
                height: Style.listViewItemHeight
                clip:true
                Image{
                    id:imdbImg
                    fillMode: Image.PreserveAspectCrop

                    source:path !=="" ? "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
                    anchors.fill: parent
                    anchors.margins: 10
                    smooth: true
                    asynchronous: true
                }

                Rectangle{
                    anchors.fill: parent
                    color:trap.pressed ? Style.appbutton_confirm_color: Style.appcolor_background_list
                    opacity: .85
                }

                width: parent.width
                StyledText{
                    id:titleBlock
                    text: name
                    wrapMode: Text.WrapAnywhere
                    width:parent.width
                    height:parent.height
                    verticalAlignment:Text.AlignVCenter
                    fontSize:Style.appFontSize_description
                    visible:true //path==="" ? true : false
                    color: "White"
                }

                MouseArea{
                    id:trap
                    anchors.fill: parent
                    onPressed: lastViewIndex=index
                    onReleased: {
                        if(name==="back (..)"){
                            manager.goBackGrid()
                        } else {
                            manager.addRestoreIndex(model.index);
                            manager.setStringParam(4, id);
                        }
                    }
                }
            }
        }
    }




    states: [
        State {
            name: "audio"
            //  when:manager.q_mediaType == Mediatypes.STORED_AUDIO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight:manager.isProfile ? Style.scaleX(28) : Style.scaleX(20)
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
                currentCellWidth:manager.isProfile ? Style.scaleX(33) : Style.scaleX(24.25)
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
