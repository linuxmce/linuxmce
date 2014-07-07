import QtQuick 2.1
import "../components"
Row{
    id:filtering
    height: scaleY(0)
    width: parent.width
    anchors.bottom: progress_bar.top
    clip:true
    focus:true
    spacing:scaleX(2)
    property int childCount:5
    property alias currentFilterX:sorting_filter.x
    Keys.onPressed: if(event.key === Qt.Key_Shift) {
                        content.state="viewing";
                        filter_view.currentFilterModel = "";
                        filter_view.visible = false
                    }

    onActiveFocusChanged: {
        if(content.state==="filtering"){
        sorting_filter.forceActiveFocus()
        } else {
           content.forceActiveFocus()
        }
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
//    FilterButton{
//        id:genre_filter
//        text:qsTr("Genres")
//        rowindex: 1
//        onActiveFocusChanged: if(activeFocus) filter_view.currentFilterModel = genrefilter
//    }
    FilterButton{
        id:type_filter
        text:qsTr("MediaTypes")
        rowindex: 1
        onActiveFocusChanged: if(activeFocus) filter_view.currentFilterModel = mediatypefilter
    }
    FilterButton{
        id:source_filter
        text:qsTr("File Format")
        rowindex: 2
        onActiveFocusChanged:  if(activeFocus) filter_view.currentFilterModel = fileformatmodel
    }
    FilterButton{
        id:users_filter
        text:qsTr("users")
        rowindex: 3
        onActiveFocusChanged: if(activeFocus) filter_view.currentFilterModel = userList
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
        }
    }
}
