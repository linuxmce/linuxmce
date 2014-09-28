import QtQuick 1.0

Item{
    id:styled_button
    height: button_label.paintedHeight < scaleY(5) ? scaleY(5) : button_label.paintedHeight+(button_label.paintedHeight*.02)
    width:opacity ? button_label.paintedWidth < scaleX(12) ? scaleX(12) : (button_label.paintedWidth) + (button_label.paintedWidth*.02) :0

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

    property string phil:useHandler ? dceHandler.item.pressed ? "yellow" : "black" :  fly_trap.pressed ? "green": "black"
    property int textSize:28
    property alias buttonText:button_label.text
    property alias txtObj:button_label
    property alias hitArea:fly_trap
    property alias handler:dceHandler.sourceComponent
    property bool useHandler:false
    property alias label:button_label.text

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
        border.width: 1
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


}
