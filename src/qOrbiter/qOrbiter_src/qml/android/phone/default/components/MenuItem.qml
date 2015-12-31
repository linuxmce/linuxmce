import QtQuick 1.1

Item{
    id:styled_button
    
    height: button_label.paintedHeight < scaleY(5) ? scaleY(5) : button_label.paintedHeight+(button_label.paintedHeight*.02)
    width:opacity ? button_label.paintedWidth < scaleX(10) ? scaleX(10) : (button_label.paintedWidth) + (button_label.paintedWidth*.02) :0

    property string phil: fly_trap.pressed ? appStyle.button_system_color_hover: appStyle.button_system_color
    property int textSize:28
    property alias buttonText:button_label.text
    signal activated()
    signal held()

    Rectangle{
        id:bg_fill
        anchors.fill: parent
        color:phil
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
        onClicked: activated()
        onPressAndHold: held()
    }

}
