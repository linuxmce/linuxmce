import QtQuick 2.2

Item {
    id:panelRoot
    height: Style.scaleY(99)
    width: Style.scaleX(99)
    property bool centered:true
    property alias headerRect:panelHeader
    property int panelHeaderHeight: useHeader ? Style.appNavigation_panelHeight : 0
    property string headerTitle:"Lorem Ipsum"
    property bool useHeader:true
    property alias headerRow:control_row
    property alias contentItem:panelContent.item
    property alias fillColor:bgfill.color
    property string headerFillColor:Style.appcolor_background_list
    property Component content
    property alias buttonContent:control_row.children

    clip:true
    anchors.centerIn: centered ? parent : undefined

    Rectangle{
        id:bgfill
        anchors.fill: parent
        color:Style.appcolor_background_list
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
        height: panelHeader.height *.95
        contentWidth: control_row.width
        contentHeight: height

        anchors{
            right:panelHeader.right
            left:headerText.right
            leftMargin: 5
            verticalCenter: panelHeader.verticalCenter
        }

        Row{
            id:control_row
            spacing: 2
            width: children.length*Style.appButtonWidth
            height: panelHeader.height *95
            Component.onCompleted: console.log("ctrl row len"+children.length)
            onChildrenChanged:{
                if(children.length===0)
                    return

                var h = children[children.length-1].height
                children[children.length-1].height=25
               children[children.length-1].height=h
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
        anchors.leftMargin: Style.scaleX(2)
        text:useHeader ? headerTitle : ""
        height: panelHeader.height
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.weight: Font.DemiBold
        font.pixelSize:Style.fontSize_listTitle
        color:Style.appbutton_color
    }



}
