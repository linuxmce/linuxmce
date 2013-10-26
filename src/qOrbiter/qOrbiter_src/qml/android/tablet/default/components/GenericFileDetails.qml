import QtQuick 1.0
Item {
    id: filedetailrect
    height: parent.height
    width: parent.width
    anchors.top: parent.top
    // property int bgImageProp:manager.q_subType ===("1"||"13") ? 43 : manager.q_attributetype_sort===53 ? 43 :36
    MouseArea{
        anchors.fill: filedetailrect
        hoverEnabled: true
    }

    Rectangle{
        id:bg_mask
        anchors.fill: parent
        color:"black"
        opacity:.85
    }

    Rectangle{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width
        color:"darkgreen"
        radius:2.5
        StyledText {
            id: text2
            anchors.horizontalCenter: parent.horizontalCenter
            text: filedetailsclass.filename
            font.pixelSize: 14
            font.bold: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }


    Rectangle {
        id: rectangle1
        anchors.verticalCenter: parent.verticalCenter

        width:  parent.width *.40
        height: childrenRect.height
        radius: 2.5
        clip:  true
        color: "transparent"

        anchors.left: imageholder.right
        anchors.leftMargin: scaleX(1)

        Column
        {
            spacing:5
            anchors.margins: scaleY(1)
            width: parent.width
            height: childrenRect.height

            StyledText {
                id: fnametext
                text: "File: " + filedetailsclass.file
                font.pixelSize: scaleY(4)
                color:"white"
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: rectangle1.width *.95
            }

        }
    }


    Row{
        id:options
        anchors.bottom: parent.bottom
        height: childrenRect.height
        width: parent.width

        StyledButton{
            buttonText: "Play"
            hitArea.onReleased: manager.playMedia(filedetailsclass.file)
        }

        StyledButton{
            buttonText: "Close"
            hitArea.onReleased: {fileviewscreen.state="gridbrowsing"; filedetailsclass.clear(); }
        }
    }
}






