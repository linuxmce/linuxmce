import QtQuick 2.2
import "."

Item{
    id:genericListContainer

    property alias listView:view
    property alias currentItem:view.currentItem
    property alias model:view.model
    property alias label:itemlabel.text
    property alias delegate:view.delegate
    property int modelSpacing: 1
    property int modelCount:view.count
    property int dataGrid:-1
    property bool extended:true

    Rectangle{
        id:hdr
        anchors{
            left: parent.left
            right:parent.right
            top:parent.top
        }
        height: Style.appNavigation_panelHeight
        color:enabled ? Style.appcolor_background_medium :" grey"
        opacity: Style.appList_opacity

        Behavior on color {
            ColorAnimation {
                duration: Style.transition_animationTime
                easing.type: Easing.InOutCubic
            }
        }

        Behavior on opacity {
            PropertyAnimation {
                duration: Style.transition_animationTime
                easing.type: Easing.InOutCubic
            }
        }

    }
    Text{
        id:itemlabel
       anchors.fill: hdr
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text:genericListContainer.label
        font.pointSize: Style.appFontSize_title
        color:"white"
    }

    ListView{
        id:view
        clip:true
        onCurrentIndexChanged: console.log("currentIndex"+currentIndex)
        spacing: modelSpacing
        anchors{
            left:parent.left
            top:hdr.bottom
            right:parent.right
            bottom:genericListContainer.bottom
        }
    }


}
