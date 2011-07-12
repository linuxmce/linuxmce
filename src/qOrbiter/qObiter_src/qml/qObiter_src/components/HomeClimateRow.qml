import Qt 4.7

Rectangle {
    property alias rowlabel: rowheader.buttontext
    height: 65
    width: parent.width
    color:style.rowbgColor
    Flickable{
        id:climateRow
        interactive: true
        height:65
        width: parent.width
        contentHeight: 65
        contentWidth: childrenRect * 1
        clip: true
    }

    Row {
        id: guide
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        spacing:5


        ButtonSq {
            id: rowheader
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Climate"

            }

    }
}
