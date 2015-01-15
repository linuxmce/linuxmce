import QtQuick 2.2

Item{
    id:styled_button
    height: button_label.paintedHeight < style.buttonHeight ? style.buttonHeight : button_label.paintedHeight+(button_label.paintedHeight*.02)
    width:opacity ? button_label.paintedWidth < style.buttonWidth ? style.buttonWidth : (button_label.paintedWidth) + (button_label.paintedWidth*.02) :0

    //    anchors{
    //        left: button_label.left
    //        right: button_label.right
    //        top:button_label.top
    //        bottom:button_label.bottom
    //    }

    Component.onCompleted: {
        if(useHandler ){
            fly_trap.anchors.fill = undefined
            fly_trap.height=0
            fly_trap.width=0
        }
    }
    smooth: true

    property string phil:useHandler ? dceHandler.item.pressed ? "yellow" : "black" :  fly_trap.pressed ? "green": "black"
    property int textSize:manager.isProfile ? scaleY(2) : scaleY(4)
    property alias buttonText:button_label.text
    property alias txtObj:button_label
    property alias hitArea:fly_trap
    property alias handler:dceHandler.sourceComponent
    property bool useHandler:false
    property alias label:button_label.text
    property alias buttonRadius:bg_fill.radius


    signal activated()
    Behavior on opacity{
        PropertyAnimation{
            duration: 500
        }
    }
    clip:true

    Rectangle{
        id:bg_fill
        anchors.fill: parent
        color:phil
        border.color: style.buttonBorderColor
        border.width: buttonRadius == height ? 0 :1
        radius: 5
        opacity:useHandler ? dceHandler.item.pressed ? 1 : .65 :  fly_trap.pressed ? 1 : .65

    }

    StyledText{
        id:button_label
        text:"ipsum"
        isBold: false
        font.weight: Font.Light
        fontSize: textSize
        anchors.centerIn: parent
        anchors.margins: 5
        color:"antiquewhite"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

    MouseArea{
        id:fly_trap
        anchors.fill: styled_button
        onReleased: activated()
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
                height:scaleY(12)
                width:scaleX(18)
            }

            PropertyChanges{
                target: button_label
                wrapMode:Text.WrapAtWordBoundaryOrAnywhere
            }
        },
        State {
            name: "numberpad"
            PropertyChanges {
                target: styled_button
                height:manager.isProfile ? scaleY(12) : scaleY(18)
                width: manager.isProfile ? scaleX(18) : scaleX(12)
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
                height:manager.isProfile ? scaleY(9) : scaleY(15)
                width: manager.isProfile ? scaleX(15) : scaleX(9)
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
                height:manager.isProfile ? scaleY(15) : scaleY(20)
                width:manager.isProfile ? scaleX(28) : scaleX(25)
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
        }
    ]
}
