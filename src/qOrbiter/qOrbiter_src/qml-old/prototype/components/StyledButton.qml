import QtQuick 1.1

Item{
    id:styledButtonRoot
    width:sT.paintedWidth > scaleX(10) ? sT.paintedWidth+10 : scaleX(10)
    height:childrenRect.height
    enabled:true
    clip:true
    property string buttonText:"foo"
    property alias  textProp:sT
    property alias hitArea:ms

    signal activated()

    Rectangle{
        anchors.fill: parent
        color:ms.pressed ? "white" : "maroon"
        opacity: ms.pressed ? 1 : .15
        border.color: "white"
        border.width: 1
        radius:5

        Behavior on color{
            PropertyAnimation{
                duration: 100
            }
        }

        Behavior on opacity {
            PropertyAnimation{
                duration:250
            }
        }
    }

    StyledText{
        id:sT
        text:buttonText
        fontSize: scaleY(4.5)
        anchors.centerIn: parent
        isBold: true
        color:"white"
    }

    MouseArea{
        id:ms
        anchors.fill: parent
        onClicked: activated()
    }
}
