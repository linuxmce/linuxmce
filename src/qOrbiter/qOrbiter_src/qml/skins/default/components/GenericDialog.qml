import QtQuick 2.2
import "../"
Item {
    id:dlg
    width: Style.scaleX(65)
    height: Style.scaleY(65)
    property string title:"Ipsum"
    property Component dialogContent:undefined
    focus:true
    Component.onCompleted:{ forceActiveFocus(); console.log(title+" is loaded.")}
    onActiveFocusChanged: if(activeFocus)contentLoader.item.forceActiveFocus();

    Item{
        anchors.centerIn: parent
        width: Style.dialog_large_width
        height: Style.dialog_large_height
        Rectangle{
            id:hdr
            anchors{
                top:parent.top
                left: parent.left
                right: parent.right
            }
            height: Style.scaleY(8)
            color: Style.appcolor_background_medium
        }
        StyledText{
            anchors.centerIn: hdr
           fontSize: Style.fontSize_listTitle
            text:dlg.title
        }

        Rectangle{
            id:contentfill
            anchors{
                top:hdr.bottom
                left:parent.left
                right:parent.right
                bottom:parent.bottom
            }
            color:Style.appcolor_background_list
        }
        Loader{
            id:contentLoader
            anchors.fill: contentfill
            sourceComponent:dialogContent
        }

    }

}
