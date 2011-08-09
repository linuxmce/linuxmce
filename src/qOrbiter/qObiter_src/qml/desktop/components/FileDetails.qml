import QtQuick 1.0


Rectangle {
    id: filedetailrect
    width: 650
    height: 550
    anchors.centerIn: parent
    color: "slategrey"
    clip: true
    focus: true
    Component.onCompleted: showFileInfo(filedetailsclass.file)

    Image {
        id: image1
        x: 18
        y: 16


        source: filedetailsclass.screenshot
    }
    MouseArea{
        x: 0
        y: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent

        Text {
            id: text2
            x: 234
            y: 21
            text: filedetailsclass.objecttitle
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Text {
            id: text1
            x: 33
            y: 320
            text: "Title"
            font.pixelSize: 12
        }

        Text {
            id: text3
            x: 34
            y: 340
            text: "Location"
            font.pixelSize: 12
        }

        Text {
            id: text4
            x: 34
            y: 388
            text: "Synopsis"
            font.pixelSize: 12
        }

        Text {
            id: titletext
            x: 107
            y: 320
            text: filedetailsclass.objecttitle
            font.pixelSize: 12
        }

        Text {
            id: locationtext
            x: 107
            y: 340
            text: "text"
            font.pixelSize: 12
        }

        Text {
            id: synopsistext
            x: 107
            y: 388
            width: 500
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: filedetailsclass.synop
            font.pixelSize: 12
        }

        Text {
            id: fnamelabel
            x: 34
            y: 360
            text: "Filename"
            font.pixelSize: 12
        }

        Text {
            id: fnametext
            x: 107
            y: 360
            text: filedetailsclass.filename
            font.pixelSize: 12
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
        model: filedetails

        delegate: Text {
                    id: titleof
                    text:pkattribute
                }

            }



            ButtonSq {
                id: buttonsq1
                x: 19
                y: 483
                width: 143
                height: 54
                radius: 10
                buttontext: "Play"
            }

            ButtonSq {
                id: buttonsq2
                x: 227
                y: 485
                width: 149
                height: 52
                radius: 10
                buttontext: "Move"
            }

            ButtonSq {
                id: buttonsq3
                x: 421
                y: 485
                width: 135
                height: 52
                radius: 10
                buttontext: "Close"
                MouseArea{
                    anchors.fill: parent
                    onClicked: filedetailrect.destroy()
                }
            }

        }






