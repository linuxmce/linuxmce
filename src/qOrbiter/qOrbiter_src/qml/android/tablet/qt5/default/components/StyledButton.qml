import QtQuick 2.0

Item{
    id:styled_button
    height: button_label.paintedHeight < scaleY(5) ? scaleY(5) : button_label.paintedHeight+(button_label.paintedHeight*.02)
    width:opacity ? button_label.paintedWidth < scaleX(10) ? scaleX(10) : (button_label.paintedWidth) + (button_label.paintedWidth*.02) :0
    property string phil:fly_trap.pressed ? "green": "black"
    property int textSize:28
    property alias buttonText:button_label
    property alias hitArea:fly_trap
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
        border.color: "white"
        border.width: 1
        radius: 5
        opacity: fly_trap.pressed ? 1 : .65

    }

    StyledText{
        id:button_label
        text:"ipsum"
        isBold: false       
        fontSize: textSize
        anchors.centerIn: parent
        color:"antiquewhite"

        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        //elide:Text.ElideRight

    }

    MouseArea{
        id:fly_trap
        anchors.fill: styled_button
    }

}
