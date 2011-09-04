import QtQuick 1.0


Rectangle {
    id: filedetailrect
    width: 650
    height: 550
    anchors.centerIn: parent
    color: "slategrey"
    Image {
        id: bg
        anchors.centerIn: parent
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: "image://filedetailsprovider/"+filedetailsclass.program
        asynchronous: true

    }
    clip: true
    focus: true
    Text {
        id: text2
        x: 231
        y: 16
        text: filedetailsclass.objecttitle
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

    Image {
        id: image1
        x: -15
        y: -28
        width: 268
        height: 351
        fillMode: Image.PreserveAspectFit
        scale: .75
        source: "image://filedetailsprovider/"+filedetailsclass.screenshot
        asynchronous: true
    }
    MouseArea{
        x: 0
        y: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent

        Rectangle {
            id: rectangle1
            x: 17
            y: 291
            width: 622
            height: 173
            radius: 4
            clip:  true
            color: style.not_color

            Flickable {
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick
                contentWidth: parent.width
                contentHeight: childrenRect.height
                id: contentFlick
                Text {
                    id: fnametext
                    x: 104
                    y: 55
                    text: filedetailsclass.filename
                    font.pixelSize: 12
                }

                Text {
                    id: fnamelabel
                    x: 31
                    y: 55
                    text: "Filename"
                    font.pixelSize: 12
                }

                Text {
                    id: synopsistext
                    x: 104
                    y: 83
                    width: 500
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.synop
                    font.pixelSize: 12
                }

                Text {
                    id: locationtext
                    x: 104
                    y: 35
                    text: "text"
                    font.pixelSize: 12
                }

                Text {
                    id: titletext
                    x: 104
                    y: 15
                    text: filedetailsclass.objecttitle
                    font.pixelSize: 12
                }

                Text {
                    id: synopsislabel
                    x: 31
                    y: 83
                    text: "Synopsis"
                    font.pixelSize: 12
                }

                Text {
                    id: locationlabel
                    x: 31
                    y: 35
                    text: "Location"
                    font.pixelSize: 12
                }

                Text {
                    id: titlelabel
                    x: 30
                    y: 15
                    text: "Title"
                    font.pixelSize: 12
                }
            }
        }

        Text {
            id: txtmoredn
            x: 599
            y: 449
            text: "More..."
            anchors.horizontalCenterOffset: 294
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 12
            visible: !contentFlick.atYEnd
        }

        Text {
            id: txtmoreup
            x: 599
            y: 291
            text: "More..."
            anchors.horizontalCenterOffset: 294
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 12
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

    ListView {
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

    }



    ButtonSq {
        id: buttonsq1
        y: 485
        width: 140
        height: 52
        radius: 10
        x: (parent.width/3)-140
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
        //x: 227
        y: 485
        width: 140
        height: 52
        radius: 10
        anchors.horizontalCenter: parent.horizontalCenter
        buttontext: "Move"
    }

    ButtonSq {
        id: buttonsq3
        //x: 421
        y: 485
        width: 140
        height: 52
        radius: 10
        buttontext: "Close"
        x: ((parent.width/3)*2)
        MouseArea{
            anchors.fill:  parent
            onClicked: filedetailrect.destroy()
        }
    }
    Component.onCompleted: contentFlick.contentHeight=synopsistext.height+104

}






