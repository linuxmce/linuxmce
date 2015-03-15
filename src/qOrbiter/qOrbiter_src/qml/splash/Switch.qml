import QtQuick 2.3
import "."

Item{
    id:sw_root
    width: parent.width
    height: Style.listViewItemHeight
    property bool enabled:false
    property string switchLabel:qsTr("Use Network Skins?")
    signal triggered()

    Text {
        id: skinsLabel
        text: switchLabel
        color:Style.apptext_color_active
        font.pointSize: Style.appFontSize_list
        anchors{
            verticalCenter: parent.verticalCenter
            leftMargin: Style.listViewMargin
            left: parent.left
        }
        
    }

    Rectangle{
        id:outerRect
        radius: 5
        color: "white"
        width: scaleX(7)
        height: scaleY(3.5)
        anchors{
            right:parent.right
            rightMargin: Style.listViewMargin
            verticalCenter: parent.verticalCenter
        }
    }
    Rectangle{
        id:inner_rect
        height: outerRect.height-1
        width: height
        radius: width
        anchors.verticalCenter: outerRect.verticalCenter
        anchors.left:outerRect.left
        anchors.leftMargin:scaleX(.25)
         anchors.rightMargin:scaleX(.25)
    }
    MouseArea{
        anchors.fill: parent
        onClicked: triggered()
    }

    states: [
        State {
            name: "on"
            when:enabled
            AnchorChanges{
                target: inner_rect
                anchors.right: undefined
                anchors.left: outerRect.left
            }
            PropertyChanges {
                target: inner_rect
                color:Style.appbutton_confirm_color
            }
        },
        State{
            name:"off"
            when:!enabled
            AnchorChanges{
                target: inner_rect
                anchors.left: undefined
                anchors.right: outerRect.right
            }
            PropertyChanges {
                target: inner_rect
                color:Style.appbutton_disabled_color
            }
        }

    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            ColorAnimation {  duration: Style.transitionFadeTime }
            AnchorAnimation{
                duration: Style.transition_animationTime
                easing.type: Easing.InCubic
            }
        }
    ]
}
