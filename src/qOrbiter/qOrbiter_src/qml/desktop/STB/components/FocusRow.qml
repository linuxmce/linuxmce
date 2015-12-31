import QtQuick 1.1

Row {
    id:focus_row
    width: childrenRect.width
    height:childrenRect.height
    property int childCount: focus_row.children.length
    spacing:scaleX(2)
    onActiveFocusChanged: {
        if(activeFocus){
            focus_row.children[0].forceActiveFocus()

        }
    }

    Component.onCompleted: init()

    function moveFocus(idx){
        console.log("Moving focus")
        if(idx !==-1 && idx !==childCount ){
            for (var i = 0; i < focus_row.children.length; i++){
                if(focus_row.children[i].rowIndex===(idx)){
                    focus_row.children[i].forceActiveFocus()
                }
            }
        }
    }

    function init(){
        for (var i = 0; i < focus_row.children.length; i++){
               focus_row.children[i].rowIndex = i
        }
    }


}
