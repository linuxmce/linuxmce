import QtQuick 1.1

Item{
    id:styled_button
    height: button_label.paintedHeight < scaleY(5) ? scaleY(5) : button_label.paintedHeight+(button_label.paintedHeight*.05)
    width:opacity ? button_label.paintedWidth < scaleX(10) ? scaleX(10) : (button_label.paintedWidth) + (button_label.paintedWidth*.05) :0

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

    property string phil:useHandler ? dceHandler.item.pressed ? "yellow" : "black" :  fly_trap.pressed ? appStyle.button_system_color_hover: appStyle.button_system_color
    property int textSize:28
    property alias buttonText:button_label.text
    property alias hitArea:fly_trap
    property alias handler:dceHandler.sourceComponent
    property bool useHandler:false
    property bool useBorder:true
    signal activated()
    signal held()
    Behavior on opacity{
        PropertyAnimation{
            duration: 500
        }
    }

    Rectangle{
        id:bg_fill
        anchors.fill: parent
        color:phil
        border.color: "white"
        border.width: useBorder ? 1 : 0
        radius: 5
        opacity:useHandler ? dceHandler.item.pressed ? .65 : .65 :  fly_trap.pressed ? .65 : .65
    }

   Rectangle{
       anchors.fill: parent
       gradient:appStyle.buttonGradient
       opacity: .50
       radius: bg_fill.radius
   }


    StyledText{
        id:button_label
        text:"ipsum"
        font.weight: Font.Light
        state:"para"
        anchors.centerIn: parent
        color:"antiquewhite"
        wrapMode:button_label.paintedWidth < scaleX(10) ? Text.NoWrap : Text.WrapAtWordBoundaryOrAnywhere
       // width: button_label.paintedWidth < styled_button.width ? button_label.paintedWidth :styled_button.width
    }

    MouseArea{
        id:fly_trap
        anchors.fill: styled_button
        onClicked: activated()
        onPressAndHold: held()
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
                state:"reg"
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
        }
    ]

}
