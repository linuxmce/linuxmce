import QtQuick 2.1
import org.linuxmce.enums 1.0
import "../components"

Item{
    id:multi_view_list
    anchors{
        left:parent.left
        right:parent.right
        top:navHeader.bottom
        bottom:parent.bottom
    }
    focus:true
    onActiveFocusChanged: {
       console.log("Media grid active focus is "+activeFocus)
        if(content.state==="viewing"){
            if(activeView===null){
                activeView=media_grid
                media_grid.forceActiveFocus()
            } else {
                activeView.forceActiveFocus()
            }
        } else {

        }


    }

    property int currentCellHeight:scaleX(20)
    property int currentCellWidth:scaleX(20)

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
    property Item activeView:null
    property int current_view_type:1

    property int itemBuffer:25
    clip:true

    Component{
        id:audioItem
        AudioDelegate{}
    }

    Component{
        id:videoItem
        VideoDelegate{}
    }

    property variant currentDelegate:manager.q_mediaType=="5" ? videoItem :audioItem
    Component.onCompleted: {
        if(manager.q_mediaType=="4"){
            multi_view_list.state="audio"
        } else if(manager.q_mediaType=="5"){
            multi_view_list.state="video"
            manager.setGridStatus(false);
        } else {
            multi_view_list.state="default"
        }

        media_grid.model=manager.getDataGridModel("MediaFile", 63)
        media_grid.positionViewAtIndex(item, ListView.Beginning)
    }

    Connections {
        target: manager.getDataGridModel("MediaFile", 63)
        onScrollToItem: {
            console.log("scroll to item : " + item);
            activeView.currentIndex=item
            activeView.positionViewAtIndex(item, ListView.Beginning);
        }
    }
//    ListView{
//        id:media_list
//        focus:true
//        anchors{
//            left:parent.left
//            right:parent.right
//            bottom:parent.bottom
//            top:parent.top
//            margins: scaleX(2)
//        }
//        delegate:currentDelegate
//        visible: current_view_type===2
//        spacing:scaleY(2)
//        clip:true
//        model:visible ? manager.getDataGridModel("MediaFile", 63) : undefined
//        onVisibleChanged: {
//            if(visible){
//                activeView = media_list
//            }
//        }
//    }

    GridView{
        id:media_grid
        focus:true
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
        onVisibleChanged: {
            if(visible){
                activeView = media_grid
            }
        }
        contentHeight: mediaList.height
        contentWidth: mediaList.width
        clip:true
        onCurrentIndexChanged: console.log("Current index changed: ==> " +currentIndex)
        Keys.onPressed: { // enter and back keys
            if(event.key ===Qt.Key_Enter || (event.key === 16777220 || event.key ===16777221 || event.key === Qt.Key_Return)) {
                manager.setStringParam(4, activeView.model.get(activeView.currentIndex, "id"))
                console.log("Current Index ==>"+activeView.currentIndex)
                activeView.positionViewAtIndex(activeView.currentIndex, ListView.Visible)


                if(indexStack.count !==0 && indexStack.get(indexStack.count).idx !==activeView.currentIndex){
                    indexStack.append({"idx":activeView.currentIndex})
                }

                console.log("IndexStack length ==>" + indexStack.count)
                depth++
                return;
            } else if(event.key === Qt.Key_Shift){
                content.state="filtering"

            } else if(event.key !== Qt.Key_Escape && event.key !== Qt.Key_Tab&& event.key !== 16777237 && event.key !==16777236 && event.key !==16777234 && event.key !==16777235){
                manager.seekGrid("MediaFile", event.text)
            } else if(event.key=== Qt.Key_M){
                manager.gotoQScreen("Screen_1.qml")
            }

            console.log(event.key)
            console.log("Depth ==>" + depth)
            console.log("Stack Depth ==> "+ indexStack.count)
        }

        Keys.onEscapePressed: {
            if(content.state=== "detail")
            {
                content.state = "browsing"

            } else if (depth!==0){
                manager.goBackGrid()
                if(indexStack.count!==0){
                    activeView.currentIndex = indexStack.get(indexStack.count-1).val
                    activeView.positionViewAtIndex(activeView.currentItem)
                    indexStack.remove(indexStack.count)
                }
                depth--
            }
            else if(depth <= 0){
                manager.gotoQScreen("Screen_1.qml")
            }
        }
        highlightFollowsCurrentItem:true

        highlight: Rectangle{
            height:scaleY(23)
            width: scaleX(16)
            color: "black"
            border.color: "white"
            border.width: 2
        }
        Behavior on width {
            PropertyAnimation{
                duration: appStyle.globalAnimationSpeed
            }
        }

        Behavior on scale{
            PropertyAnimation{
                duration: appStyle.globalAnimationSpeed
            }
        }
    }

//    PathView{
//        id:media_path
//        focus:true
//        anchors.fill: parent
//        model:visible ? manager.getDataGridModel("MediaFile", 63) : undefined
//        visible:current_view_type===3
//        onVisibleChanged: {
//            if(visible){
//                activeView = media_path
//            }
//        }
//    }
    states: [
        State {
            name: "audio"
            //  when:manager.q_mediaType == Mediatypes.STORED_AUDIO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleX(25)
                currentCellWidth:scaleX(25)
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
            name: "default"
            when:manager.q_mediaType != Mediatypes.STORED_VIDEO && manager.q_mediaType != Mediatypes.STORED_AUDIO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(19)
            }
        },
        State {
            name: "video-default"
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(19)
            }
        },
        State {
            name: "tv"
            when:manager.q_subType==MediaSubtypes.TVSHOWS && manager.q_pk_attribute==""
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(20)
                currentCellWidth:scaleX(19)
            }
        },
        State {
            name: "movies"
            when: manager.q_subType==MediaSubtypes.MOVIES && manager.q_mediaType==Mediatypes.STORED_VIDEO
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: currentCellWidth*hdPosterRatio
                currentCellWidth:scaleX(20)
            }
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
                currentCellHeight: scaleY(20)
                currentCellWidth:scaleX(19)
            }
        }
    ]

}
