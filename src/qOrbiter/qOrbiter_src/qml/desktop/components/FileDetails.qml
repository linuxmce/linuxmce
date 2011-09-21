import QtQuick 1.0

Rectangle {
    id: filedetailrect
    width: scaleX(50)
    height: scaleY(80)
    anchors.top: parent.top
    anchors.topMargin: scaleY(5)
    anchors.horizontalCenter: parent.horizontalCenter
    color: style.highlight2
    clip: true
    radius: 5

    Timer{
        id:singleshot
        repeat: false
        interval: 250
        triggeredOnStart: false
        onTriggered: image1.source = "image://filedetailsprovider/"+securityvideo.timestamp
        running: true
    }

    Rectangle{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width
        color:style.highlight1
        radius:5

        Text {
            id: text2
            anchors.horizontalCenter: parent.horizontalCenter
            text: filedetailsclass.objecttitle
            font.pixelSize: 14
            font.bold: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }

    Image {
        id: image1
        anchors.top: titlerect.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: scaleX(40)
        height: scaleY(40)
        fillMode: Image.PreserveAspectFit
        source: "image://filedetailsprovider/"+filedetailsclass.screenshot
        asynchronous: true
    }

    Rectangle {
        id: rectangle1
        anchors.top: image1.bottom
        anchors.topMargin: scaleY(1)
        width: parent.width *.95
        height: scaleY(20)
        radius: 4
        clip:  true
        color: style.darkhighlight
        border.color: style.highlight1
        anchors.horizontalCenter: parent.horizontalCenter

        Flickable {
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            contentWidth: parent.width
            contentHeight: childrenRect.height
            id: contentFlick
            Column
            {
                spacing:5
                anchors.horizontalCenter: parent.horizontalCenter
                width: childrenRect.width
                Text {
                    id: fnametext
                    text: "Filename: " + filedetailsclass.filename
                    font.pixelSize: scaleY(2)
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                }

                Text {
                    id:  titletext
                    text:"Title:" + filedetailsclass.objecttitle
                    font.pixelSize: scaleY(2)
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                }

                Text {
                    id: synopsistext
                    width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.synop
                    font.pixelSize: scaleY(2)
                    color:"aliceblue"
                }
            }
        }

        Text {
            id: txtmoredn
            text: "More..."
            anchors.horizontalCenterOffset: 294
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: scaleY(2)
            color:"aliceblue"
            visible: !contentFlick.atYEnd
        }

        Text {
            id: txtmoreup
            x: scaleX(60)
            y: scaleY(20)
            text: "More..."
            anchors.horizontalCenterOffset: 294
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: scaleY(2)
            color:"aliceblue"
            visible: !contentFlick.atYBeginning
        }
    }

    Component{
        id:performersheader

        Rectangle {
            width: container.width
            height: childrenRect.height
            color: style.button_system_color

            Text {
                text: section
                font.bold: true
            }
        }

    }

    /* ListView {
        id: list_view1
        x: 380
        y: 16
        width: 256
        height: 154
        model: filedetails.performers

        delegate: Item {
            width: list_view1.width;
            Text {
            id: titleof
            text:pkattribute
            height:  20
            }
        }

    } */

    Row{
        id:controlrow
        anchors.top: rectangle1.bottom
        anchors.topMargin: scaleY(1)
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: scaleY(.5)
        ButtonSq {
            id: buttonsq1
            width: style.stdbuttonw
            height: style.stdbuttonh
            radius: 10

            //anchors.leftMargin: 18
            buttontext: "Play"
            MouseArea
            {
                anchors.fill: parent
                onClicked: playMedia(filedetailsclass.file)  //dce function
            }
        }

        ButtonSq {
            id: buttonsq2
            width: style.stdbuttonw
            height: style.stdbuttonh
            radius: 10
            buttontext: "Move"
        }

        ButtonSq {
            id: buttonsq3
            width: style.stdbuttonw
            height: style.stdbuttonh
            radius: 10
            buttontext: "Close"
            x: ((parent.width/3)*2)
            MouseArea{
                anchors.fill:  parent
                onClicked: filedetailrect.destroy()
            }
        }
    }
    Component.onCompleted: contentFlick.contentHeight=synopsistext.height+104

}






