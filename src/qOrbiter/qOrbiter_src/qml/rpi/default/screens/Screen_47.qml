import QtQuick 2.2
import "../components"
Item {
    id:screen_forty_seven_of_nine
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
        bottom:parent.bottom
    }

    state: "browsing"
    property int depth:0

    Component.onCompleted: screen_forty_seven_of_nine.forceActiveFocus()

    ListModel{
        id:indexStack
    }

    onActiveFocusChanged: {
        console.log(mediaList.parent.objectName+ " recieved focus, passing to mediaList grid")
        mediaList.forceActiveFocus()
    }

    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            screen_forty_seven_of_nine.state = "detail"
            depth--
        }
    }

    MediaListGridDelagate {
        id: contactDelegate
        visible: false
    }

    GridView{
        id:mediaList
        height: parent.height - scaleY(15)
        width: parent.width
        cellHeight: contactDelegate.height
        cellWidth: contactDelegate.width
        contentHeight: scaleY(24)
        contentWidth: scaleX(25)
        anchors.top: parent.top
        model: manager.getDataGridModel("MediaFile", 63)
       flickableDirection:GridView.HorizontalFlick
        focus:true
        clip:true
        onCurrentIndexChanged: console.log("Current index changed: ==> " +currentIndex)
        Keys.onPressed: { // enter and back keys
            if(event.key ===Qt.Key_Enter || (event.key === 16777220 || event.key ===16777221 || event.key === Qt.Key_Return)) {
                manager.setStringParam(4, manager.getDataGridModel("MediaFile", 63).get(mediaList.currentIndex, "id"))
                console.log("Current Index ==>"+mediaList.currentIndex)
                mediaList.positionViewAtIndex(mediaList.currentIndex, ListView.Visible)


                if(indexStack.count !==0 && indexStack.get(indexStack.count).idx !==mediaList.currentIndex){
                    indexStack.append({"idx":mediaList.currentIndex})
                }

                console.log("IndexStack length ==>" + indexStack.count)
                depth++
                return;
            } else if(event.key === Qt.Key_Shift){
                filtering.forceActiveFocus()
            } else if(event.key !== Qt.Key_Escape && event.key !== Qt.Key_Tab&& event.key !== 16777237 && event.key !==16777236 && event.key !==16777234 && event.key !==16777235){
		manager.seekGrid("MediaFile", event.key)
            } else if(event.key=== Qt.Key_M){
                manager.setCurrentScreen("Screen_1.qml")
            }

            console.log(event.key)
            console.log("Depth ==>" + depth)
            console.log("Stack Depth ==> "+ indexStack.count)
        }
	Connections {  
            target: manager.getDataGridModel("MediaFile", 63)  
	    onScrollToItem: { mediaList.currentIndex = item; grid_view1.maingrid.positionViewAtIndex(item, ListView.Beginning); } 
	}  
        Keys.onEscapePressed: {
            if(screen_forty_seven_of_nine.state=== "detail")
            {
                screen_forty_seven_of_nine.state = "browsing"

            } else if (depth!==0){
                manager.goBackGrid()
                if(indexStack.count!==0){
                    mediaList.currentIndex = indexStack.get(indexStack.count-1).val
                    mediaList.positionViewAtIndex(mediaList.currentItem)
                    indexStack.remove(indexStack.count)
                }
                depth--
            }
            else if(depth <= 0){
                manager.setCurrentScreen("Screen_1.qml")
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
        delegate:MediaListGridDelagate{}


        Behavior on width {
            PropertyAnimation{
                duration: skinStyle.globalAnimationSpeed
            }
        }

        Behavior on scale{
            PropertyAnimation{
                duration: skinStyle.globalAnimationSpeed
            }
        }
    }

    Item{
        id:infoPanel
        height: parent.height
        width: 0
        focus:false
        anchors.right: screen_forty_seven_of_nine.right
        onActiveFocusChanged: file_details.forceActiveFocus()
        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity:.85
            radius:5
        }
        FileDetails{
            id:file_details
        }

        Behavior on width {
            PropertyAnimation{
                duration: skinStyle.globalAnimationSpeed
            }
        }
        Keys.onEscapePressed: {
            console.log("Setting grid back in focus")
            console.log("Changing focus from details")
            mediaList.forceActiveFocus()
            screen_forty_seven_of_nine.state = "browsing"
        }

    }

    Row{
        id:filtering
        height: childrenRect.height
        width: parent.width
        anchors.bottom: parent.bottom
        focus:false
        spacing:scaleX(2)
        property int childCount:5
        Keys.onPressed: if(event.key === Qt.Key_Shift) {mediaList.forceActiveFocus(); filter_view.currentFilterModel = ""; filter_view.visible = false }
        onActiveFocusChanged: {
            sorting_filter.forceActiveFocus()
        }
        Behavior on opacity{
            PropertyAnimation{
                duration: 500
            }
        }
        function moveFocus(idx){
            if(idx !==-1 && idx !==childCount ){
                for (var i = 0; i < filtering.children.length; i++){
                    if(filtering.children[i].rowindex===(idx)){
                        filtering.children[i].forceActiveFocus()
                        filter_view.x = filtering.children[i].x
                    }
                }
            }
        }
        function moveSort(idx){

        }

        /*
          Sort, Genre, Type, Source

          */

        FilterButton {
            id: sorting_filter
            text: qsTr("Sort")
            rowindex: 0
            onActiveFocusChanged: if(activeFocus) {filter_view.currentFilterModel = attribfilter; }
        }
        FilterButton{
            id:genre_filter
            text:qsTr("Genres")
            rowindex: 1
            onActiveFocusChanged: if(activeFocus) filter_view.currentFilterModel = genrefilter
        }
        FilterButton{
            id:type_filter
            text:qsTr("MediaTypes")
            rowindex: 2
            onActiveFocusChanged: if(activeFocus) filter_view.currentFilterModel = mediatypefilter
        }
        FilterButton{
            id:source_filter
            text:qsTr("File Format")
            rowindex: 3
            onActiveFocusChanged:  if(activeFocus) filter_view.currentFilterModel = fileformatmodel
        }
        FilterButton{
            id:playAllbutton
            text:"Play All"
            rowindex: 4
            onActiveFocusChanged: if(activeFocus) {filter_view.currentFilterModel = "" }
            Keys.onEnterPressed: {
                  manager.playMedia("!G"+iPK_Device)
            }
            Keys.onReturnPressed: {
                  manager.playMedia("!G"+iPK_Device)
            }

            Keys.onPressed: {
                if(event.key === Qt.Key_Enter|| event.key ===Qt.Key_Return){
                    manager.playMedia("!G"+iPK_Device)
                }
                console.log(event.key)
            }
        }
    }

    Item{
        id:filter_view
        height: childrenRect.height
        width: childrenRect.width
        visible:false
        signal select()
        anchors.bottom: filtering.top
        x:sorting_filter.x
        property variant currentFilterModel: ""
        onCurrentFilterModelChanged: {
            if(currentFilterModel !==""){
                visible=true
                filterView.visible = true
            }else{
                visible = false
                filterView.visible = false
            }
        }
        function selectItem(){
            select()
            mediaList.currentIndex=0
            depth++
        }



        ListView{
            id:filterView
            height: scaleY(65)
            width: scaleX(20)
            model:parent.currentFilterModel
            currentIndex: -1
            clip:true
            visible: false
            delegate: Item{
                height: scaleY(12)
                width: scaleX(18)

                Connections{
                    target: filter_view
                    onSelect:if(filterView.currentIndex === index) filter_view.currentFilterModel.setSelectionStatus(name)
                }

                Rectangle{
                    anchors.fill: parent
                    color: filterView.currentIndex === index  ? skinStyle.lightHighlightColor : skinStyle.mainColor
                    opacity:.85
                }
                StyledText{
                    id:lbl
                    text:name
                    color:"white"
                    fontSize: headerText
                }
            }
        }
    }

    states: [
        State {
            name: "browsing"
            PropertyChanges {
                target: mediaList
                width:screen_forty_seven_of_nine.width
                visible:true
                scale:1
                opacity:1
            }

            PropertyChanges{
                target: infoPanel
                width:0
            }
            PropertyChanges {
                target: filtering
                opacity:1
            }
            StateChangeScript{
                script:{
                    mediaList.forceActiveFocus()
                    console.log("Setting Media list focus to active.")
                }
            }
        },
        State {
            name: "detail"
            PropertyChanges {
                target: mediaList
                scale:.25
                opacity:.25
            }
            PropertyChanges {
                target: infoPanel
                width:scaleX(100)
                opacity:.90
            }
            PropertyChanges {
                target: filtering
                opacity:0
            }
            StateChangeScript{
                script:{
                    infoPanel.forceActiveFocus()
                    console.log("Setting info panel focus to active.")
                }
            }
        }
    ]


}
