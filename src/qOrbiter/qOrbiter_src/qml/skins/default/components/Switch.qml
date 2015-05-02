import QtQuick 2.2
import org.linuxmce.settings 1.0
import "../"
Item{
    id:sw_root
    width: manager.isProfile ? Style.scaleX(24) :  Style.scaleX(12)
    height: Style.scaleY(6)
    property bool enabled:false
    property string switchLabel:"ipsum"
    signal triggered()

    
    Rectangle{
        id:outerRect
        radius: height
        color: "white"
        width: parent.width
        height: parent.height
        
        anchors{
            right:parent.right
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
        anchors.leftMargin:Style.scaleX(.15)
        anchors.rightMargin:Style.scaleX(.15)
    }

    StyledText{
        id:label
        anchors{
            verticalCenter: inner_rect.verticalCenter
        }
        text:sw_root.state==="on" ? qsTr("Enabled") : qsTr("Disabled")
        color:sw_root.state==="on" ? "green" : "red"
       anchors.margins: 5
       fontSize: Style.appFontSize_description
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
            AnchorChanges{
                target: label
                 anchors.left: undefined
                anchors.right: outerRect.right

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
            AnchorChanges{
                target: label
                anchors.right: undefined
                anchors.left: outerRect.left
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
