import QtQuick 2.0

Item{
    id:styled_button
    height:style.buttonHeight // button_label.paintedHeight < scaleY(5) ? scaleY(5) : button_label.paintedHeight+(button_label.paintedHeight*.02)
    width:  opacity ? button_label.paintedWidth < style.buttonWidth ? style.buttonWidth : (button_label.paintedWidth) + (button_label.paintedWidth*.1) :0
    property string phil:fly_trap.pressed ? style.buttonPressedColor :style.buttonColor
    property int textSize:style.buttonLabelSize
    property alias label:button_label.text
    property alias buttonText:button_label
    property alias hitArea:fly_trap
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
        opacity: fly_trap.pressed ? 1 : .65

        gradient: style.buttonGradient
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
        onClicked: activated()
    }

}
