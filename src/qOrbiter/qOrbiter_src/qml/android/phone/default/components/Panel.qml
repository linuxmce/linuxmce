import QtQuick 1.1

Item {
    id:panelRoot
    height: scaleY(85)
    width: scaleX(85)

    property int panelHeaderHeight:height*.25
    property string headerTitle:"Lorem Ipsum"
    clip:true

    Rectangle{
        anchors.fill: parent
        color:skinStyle.toolbarBgColor
    }

    Rectangle{
        id:phil
        anchors.fill: parent
        gradient:skinStyle.buttonGradient
        opacity: .55
    }

    Rectangle{
        id:panelHeader
        height: panelRoot.height*.20
        color:skinStyle.toolbarBgColor
        anchors.margins: 5
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
        height: panelHeader.height
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        width: panelHeader.width
        isBold: true
        font.pixelSize: scaleY(4)
        color:skinStyle.tile_color
    }

}
