import QtQuick 2.2
import "../."

Item {
    id:panelRoot
    height: appStyle.scaleY(99)
    width: appStyle.scaleX(99)
    property bool centered:true
    property alias headerRect:panelHeader
    property int panelHeaderHeight: useHeader ? appStyle.appNavigation_panelHeight : 0
    property string headerTitle:"Lorem Ipsum"
    property bool useHeader:true
    property alias headerRow:bttnLoader.item
    property alias contentItem:panelContent.item
    property alias fillColor:bgfill.color
    property string headerFillColor:appStyle.appcolor_background_list
    property Component content
    property Component buttonControls
    property alias buttonContent:bttnLoader.item

    clip:true
    anchors.centerIn: centered ? parent : undefined

    Rectangle{
        id:bgfill
        anchors.fill: parent
        color:appStyle.appcolor_background_list
        opacity: .65
        border.color: "white"
        border.width: 1
    }
    Loader{
        id:panelContent
        sourceComponent: content
        anchors{
            top: useHeader ? panelHeader.bottom : parent.top
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            margins: 5
        }
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

    ScrollRow{
        id:bttncntrl
        height: panelHeader.height *.95
        contentWidth:parent.width
        contentHeight: height
        clip: false

        anchors{
            right:panelHeader.right
            left:headerText.right
            leftMargin: 5
            verticalCenter: panelHeader.verticalCenter
        }

        Loader{
            id:bttnLoader
            height: panelHeader.height

            sourceComponent:buttonControls
            onLoaded: {
                bttncntrl.contentWidth = bttnLoader.item.width
            }
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
        anchors.leftMargin: appStyle.scaleX(2)
        text:useHeader ? headerTitle : ""
        height: panelHeader.height
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.weight: Font.DemiBold
        fontSize:appStyle.fontSize_medium
        color:appStyle.appbutton_color
    }
}
