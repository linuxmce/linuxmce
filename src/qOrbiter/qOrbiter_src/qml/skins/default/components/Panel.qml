import QtQuick 2.2
import "../."
Item {
    id:panelRoot
    height: Style.scaleY(85)
    width: Style.scaleX(90)
    anchors.centerIn: parent

    property alias headerRect:panelHeader
    property int panelHeaderHeight: useHeader ? height*.10 : 0
    property string headerTitle:"Lorem Ipsum"
    property bool useHeader:true
    property alias fillColor:bgfill.color
    property string headerFillColor:Style.appcolor_background_list
    clip:true

    Rectangle{
        id:bgfill
        anchors.fill: parent
        color:Style.appcolor_background_list
        opacity: .65
        border.color: "white"
        border.width: 1
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
        anchors.leftMargin: Style.scaleX(2)
        text:useHeader ? headerTitle : ""
        height: panelHeader.height
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.weight: Font.DemiBold
        font.pixelSize:Style.fontSize_listTitle
        color:Style.appbutton_color
    }

}
