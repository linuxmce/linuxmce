import QtQuick 2.2

Item {
    id:panelRoot
    height: scaleY(85)
    width: scaleX(90)


    property alias headerRect:panelHeader
    property int panelHeaderHeight: useHeader ? height*.10 : 0
    property string headerTitle:"Lorem Ipsum"
    property bool useHeader:true
    property alias fillColor:bgfill.color
    property string headerFillColor:style.buttonColor
    clip:true

    Rectangle{
        id:bgfill
        anchors.fill: parent
        color:style.contentBgColor
        opacity: .65
        border.color: "white"
        border.width: 1
    }

    Rectangle{
        id:phil
        anchors.fill: parent
        gradient:style.buttonGradient
        opacity: .1
    }

    Rectangle{
        id:panelHeader
        height: panelHeaderHeight
        color:headerFillColor
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
        text:useHeader ? headerTitle : ""
        height: panelHeader.height
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.weight: Font.DemiBold
        isBold: true
        font.pixelSize:scaleY(parent.height*.010)
        color:skinStyle.listItemActiveBgColor
    }

}
