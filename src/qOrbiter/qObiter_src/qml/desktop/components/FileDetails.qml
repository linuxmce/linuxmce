import QtQuick 1.0



Rectangle {
    id: filedetailrect
    width: 650
    height: 550
    anchors.centerIn: parent
    color: "slategrey"
    clip: true
    focus: true
    MouseArea{
        anchors.fill: parent
    }

    Component{
        id:performersheader
        Rectangle {
                    width: container.width
                    height: childrenRect.height
                    color: "lightsteelblue"

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

            Image {
                id: image1
                x: 18
                y: 16
                width: 199
                height: 239
                source: "qrc:/qtquickplugin/images/template_image.png"
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

            Rectangle {
                id: rectangle1
                x: 9
                y: 266
                width: 618
                height: 178
                color: "#ffffff"
                radius: 56
                clip: true

                Text {
                    id: text1
                    x: 168
                    y: 24
                    width: 500
                    text: "Synopsis"
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: 12
                }
            }

        }        






