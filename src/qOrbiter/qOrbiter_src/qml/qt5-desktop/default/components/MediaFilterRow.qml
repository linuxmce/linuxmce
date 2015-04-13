import QtQuick 2.2
import "../components"
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
