import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../."
Item{
    id:styled_button
    height: Style.appButtonHeight
    width:Style.appButtonWidth

    Component.onCompleted: {
        if(useHandler ){
            fly_trap.anchors.fill = undefined
            fly_trap.height=0
            fly_trap.width=0
        }
    }
    smooth: true

    property string phil: fly_trap.pressed ? Style.appbutton_confirm_color: Style.appcolor_background_list
    property int textSize:Style.fontSize_listItem
    property alias fontSize:button_label.fontSize
    property alias buttonText:button_label.text
    property alias txtObj:button_label
    property alias hitArea:fly_trap
    property alias handler:dceHandler.sourceComponent
    property bool useHandler:false
    property alias label:button_label.text
    property alias buttonRadius:bg_fill.radius
    property alias buttonColor: bg_fill.color

    signal confirm()
    signal activated()
    Behavior on opacity{
        PropertyAnimation{
            duration: 500
        }
    }
    clip:true
    Rectangle{
        anchors.fill: bg_fill
        color: "black"
        radius: bg_fill.radius
    }

    Rectangle{
        id:bg_fill
        anchors.fill: parent
        color:phil
        border.color: "white"
        border.width: buttonRadius == height ? 0 :1
        radius: 5
        opacity:useHandler ? dceHandler.item.pressed ? 1 : .35 :  fly_trap.pressed ? 1 : .35
        Behavior on color {
            ColorAnimation {
                duration: Style.transition_animationTime
            }
        }

    }

    StyledText{
        id:button_label
        text:"ipsum"    
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:parent.top
            margins: 5
        }

        height: parent.height
        fontSize: textSize
        font.weight: Font.Light
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment:Text.AlignHCenter       
        color:Style.apptext_color_active
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

    MouseArea{
        id:fly_trap
        anchors.fill: styled_button
        onReleased: activated()
        onPressAndHold: confirm()
    }

    Loader{
        id:dceHandler
        anchors.fill: parent
        sourceComponent: undefined
    }

    states: [
        State {
            name: "fixed"
            PropertyChanges {
                target: styled_button
                height:Style.scaleY(12)
                width:Style.scaleX(18)
            }

            PropertyChanges{
                target: button_label
                wrapMode:Text.WrapAtWordBoundaryOrAnywhere
            }
        },
        State {
            name: "scenario"
            PropertyChanges {
                target: styled_button
                height:Style.scaleY(12)
                width:Style.scaleX(18)
                textSize:Style.fontSize_listItem
            }
            PropertyChanges{
                target: button_label
                wrapMode:Text.WrapAtWordBoundaryOrAnywhere
            }
        }
        ,
        State {
            name: "numberpad"
            PropertyChanges {
                target: styled_button
                height:Style.appButtonNumberPadHeight
                width: Style.appButtonNumberPadWidth
            }

            PropertyChanges{
                target: button_label
                wrapMode:Text.WrapAtWordBoundaryOrAnywhere
            }
        },
        State {
            name: "numberpad-small"
            PropertyChanges {
                target: styled_button
                height:Style.appButtonNumberPadSmallHeight
                width: Style.appButtonNumberPadSmallWidth
            }

            PropertyChanges{
                target: button_label
                wrapMode:Text.WrapAtWordBoundaryOrAnywhere
            }
        },
        State {
            name: "large-fixed"
            PropertyChanges {
                target: styled_button
                height:Style.appButtonLargeHeight
                width:Style.appButtonLargeWidth
            }

            PropertyChanges{
                target: button_label
                state:"para"
                wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                width:styled_button.width
                horizontalAlignment:Text.AlignHCenter
            }
        },
        State {
            name: "round"
            PropertyChanges {
                target: styled_button
                width:height
                buttonRadius:height
            }

            PropertyChanges{
                target: button_label
                state:"para"
                wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                width:styled_button.width
                horizontalAlignment:Text.AlignHCenter
            }
        },
        State {
            name: "remote"
            PropertyChanges {
                target: styled_button
                height:manager.isProfile ? Style.scaleY(8) : Style.scaleY(10)
                width:manager.isProfile ? Style.scaleX(20) : Style.scaleX(10)

            }
        }
    ]
}
