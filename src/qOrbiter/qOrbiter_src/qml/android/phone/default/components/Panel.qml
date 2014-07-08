import QtQuick 1.1

Item {
    id:panelRoot
    height: scaleY(85)
    width: scaleX(85)

    property int panelHeaderHeight:height*.25
    property string headerTitle:"Lorem Ipsum"
    property alias headerAnchor:panelHeader
    clip:true

    Rectangle{
        anchors.fill: parent
        color:appStyle.toolbarBgColor
    }

    Rectangle{
        id:phil
        anchors.fill: parent
        gradient:appStyle.buttonGradient
        opacity: .55
    }

    Rectangle{
        id:panelHeader
        height: panelRoot.height*.20
        color:appStyle.toolbarBgColor
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
        color:appStyle.tile_color
    }

}
