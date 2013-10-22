import QtQuick 1.1

Item{
    id:pinButton
    height: parent.height
    width: height

    Rectangle{
        radius: 5
        anchors.fill: parent
        color:"green"       
    }

    StyledText{
        text:modelData
        color:"white"
        font.pointSize: scaleY(4)
        anchors.centerIn: parent
    }

}
