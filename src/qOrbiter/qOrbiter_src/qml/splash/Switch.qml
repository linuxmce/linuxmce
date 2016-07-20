import QtQuick 2.2
import "."

Item{
    id:sw_root
    width: parent.width
    height: theme.listViewItemHeight
    property bool enabled:false
    property string switchLabel:qsTr("Use Network Skins?")
    signal triggered()
    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: .55
    }
    Text {
        id: skinsLabel
        text: switchLabel
        color:theme.apptext_color_active
        font.pixelSize: theme.appFontSize_list
        anchors{
            verticalCenter: parent.verticalCenter
            leftMargin: theme.listViewMargin
            left: parent.left
        }
        
    }

    Rectangle{
        id:outerRect
        radius: height
        color: "white"
        width: parent.width *.30
        height: parent.height/2

        anchors{
            right:parent.right
            rightMargin: theme.listViewMargin
            verticalCenter: parent.verticalCenter
        }
    }
    Rectangle{
        id:inner_rect
        height: outerRect.height-1
        width: height
        radius: width
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:outerRect.left
        anchors.leftMargin:scaleX(.15)
        anchors.rightMargin:scaleX(.15)
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
                color:theme.appbutton_confirm_color
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
                color:theme.appbutton_disabled_color
            }
        }

    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            ColorAnimation {  duration: theme.transitionFadeTime }
            AnchorAnimation{
                duration: theme.transition_animationTime
                easing.type: Easing.InCubic
            }
        }
    ]
}
