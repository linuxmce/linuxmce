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

    }
    clip: true
    focus: true


    Image {
        id: image1
        x: 5
        y: 5
       scale: .75
        source: "image://filedetailsprovider/"+filedetailsclass.screenshot
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
            radius: 25
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#bbb4b4"
                }

                GradientStop {
                    position: 0.57
                    color: "#466ec6"
                }

                GradientStop {
                    position: 1
                    color: "#352b7a"
                }
            }







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

    Text {
        id: fnametext
        x: 104
        y: 355
        text: filedetailsclass.filename
        font.pixelSize: 12
    }

    Text {
                id: fnamelabel
                x: 31
                y: 355
                text: "Filename"
                font.pixelSize: 12
    }

    Text {
        id: synopsistext
        x: 104
        y: 383
        width: 500
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: filedetailsclass.synop
        font.pixelSize: 12
    }

    Text {
        id: locationtext
        x: 104
        y: 335
                text: "text"
                font.pixelSize: 12
    }

    Text {
        id: titletext
        x: 104
        y: 315
        text: filedetailsclass.objecttitle
        font.pixelSize: 12
            }

Text {
    id: text4
    x: 31
    y: 383
    text: "Synopsis"
    font.pixelSize: 12
}

Text {
    id: text3
    x: 31
            y: 335
            text: "Location"
            font.pixelSize: 12
        }

Text {
    id: text1
    x: 30
    y: 315
    text: "Title"
    font.pixelSize: 12
}

Text {
    id: text2
    x: 231
    y: 16
    text: filedetailsclass.objecttitle
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}









}






