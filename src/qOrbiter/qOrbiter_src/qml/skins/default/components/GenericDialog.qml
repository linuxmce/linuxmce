import QtQuick 2.2
import "../"
Item {
    id:dlg
    width: appStyle.scaleX(65)
    height: appStyle.scaleY(65)
    property string title:"Ipsum"
    property Component dialogContent:undefined
    focus:true
    Component.onCompleted:{ forceActiveFocus(); console.log(title+" is loaded.")}
    onActiveFocusChanged: if(activeFocus)contentLoader.item.forceActiveFocus();

    Item{
        anchors.centerIn: parent
        width: appStyle.dialog_large_width
        height: appStyle.dialog_large_height
        Rectangle{
            id:hdr
            anchors{
                top:parent.top
                left: parent.left
                right: parent.right
            }
            height: appStyle.scaleY(8)
            color: appStyle.appcolor_background_medium
        }
        StyledText{
            anchors.centerIn: hdr
           fontSize: appStyle.fontSize_listTitle
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
            color:appStyle.appcolor_background_list
        }
        Loader{
            id:contentLoader
            anchors.fill: contentfill
            sourceComponent:dialogContent
        }

    }

}
