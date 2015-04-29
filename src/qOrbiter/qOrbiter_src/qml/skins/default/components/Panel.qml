import QtQuick 2.2
import "../."
Item {
    id:panelRoot
    height: Style.scaleY(99)
    width: Style.scaleX(99)

    property alias headerRect:panelHeader
    property int panelHeaderHeight: useHeader ? Style.appNavigation_panelHeight : 0
    property string headerTitle:"Lorem Ipsum"
    property bool useHeader:true
    property alias fillColor:bgfill.color
    property string headerFillColor:Style.appcolor_background_list
    property Component content
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

    Image {
        id: backBtn
        source: "../images/vertical_arrow.png"
        rotation: 90
        visible: useHeader
        width: height
        height:panelHeader.height
        fillMode: Image.PreserveAspectFit

        anchors{
            left:panelHeader.left
            verticalCenter: panelHeader.verticalCenter
        }
        MouseArea{
            anchors.fill: parent
            onClicked: manager.goBacktoQScreen()
        }
    }
    StyledText{
        id:headerText
        anchors.left: backBtn.right
        anchors.verticalCenter: panelHeader.verticalCenter
        anchors.leftMargin: Style.scaleX(2)
        text:useHeader ? headerTitle : ""
        height: panelHeader.height
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.weight: Font.DemiBold
        font.pixelSize:Style.fontSize_listTitle
        color:Style.appbutton_color
    }

    Loader{
        id:panelContent
        sourceComponent: content
        anchors{
            top:panelHeader.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            margins: 5
        }
    }

}
