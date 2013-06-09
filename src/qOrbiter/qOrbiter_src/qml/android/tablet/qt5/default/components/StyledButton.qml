import QtQuick 2.0

Item{
    id:styled_button
    height: scaleY(5)
    width: scaleX(8)
    property string phil:fly_trap.pressed ? "green": "transparent"
    property int textSize:28
    property alias buttonText:button_label
    Rectangle{
        id:bg_fill
        anchors.fill: parent
        color:phil
        border.color: "white"
        border.width: 1
        radius: 5
    }

    StyledText{
        id:button_label
        text:"ipsum"
        font.bold:false
        font.pixelSize: textSize
        anchors.centerIn: parent
        color:"antiquewhite"
    }

    MouseArea{
        id:fly_trap
        anchors.fill: styled_button

    }

}
