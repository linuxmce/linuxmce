import QtQuick 1.1

Item {
    height: scaleY(75)
    width: scaleX(65)

    property int panelHeaderHeight:parent.height(.25)
    property string headerTitle:"Lorem Ipsum"

    Rectangle{
        anchors.fill: parent
        color:skinStyle.toolbarBgColor
    }

    Rectangle{
        id:phil
        anchors.fill: parent
       gradient:skinStyle.buttonGradient
    }

    Rectangle{
        id:panelHeader

        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
    }

    StyledText{
        id:headerText
        anchors.centerIn: panelHeader
        text:headerTitle
        isBold: true
        font.pixelSize: scaleY(7)
    }

    Item{
        id:content

        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:panelHeader.bottom
        }


    }
}
