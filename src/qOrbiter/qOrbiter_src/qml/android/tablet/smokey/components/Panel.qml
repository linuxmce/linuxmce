import QtQuick 1.1

Item {
    id:panelRoot
    height: scaleY(85)
    width: scaleX(90)
    anchors.centerIn: parent
    property alias headerRect:panelHeader
    property int panelHeaderHeight:height*.10
    property string headerTitle:"Lorem Ipsum"
    clip:true

    Rectangle{
        id:bgfill
        anchors.fill: parent
        color:style.contentBgColor
        opacity: .65
        border.color: "white"
        border.width: 2
    }

    Rectangle{
        id:phil
        anchors.fill: parent
        gradient:style.buttonGradient
        opacity: .55
    }

    Rectangle{
        id:panelHeader
        height: panelHeaderHeight
        color:style.buttonColor
        anchors.margins: 5
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
    }

    StyledText{
        id:headerText
        anchors.left: panelHeader.left
        anchors.verticalCenter: panelHeader.verticalCenter
        anchors.leftMargin: scaleX(2)
        text:headerTitle
        height: panelHeader.height
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.weight: Font.DemiBold
        isBold: true
        font.pixelSize: scaleY(5)
        color:appStyle.tile_color
    }

}
