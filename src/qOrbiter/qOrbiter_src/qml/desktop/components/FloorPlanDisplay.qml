import QtQuick 1.0

Rectangle {
    width: scaleX(95)
    height: scaleY(95)
    color: style.darkhighlight
    clip: true

    Row{
        id:mainrow
        height: scaleY(50)
        width: parent.width
        spacing: 5
        clip: true
        anchors.verticalCenter: parent.verticalCenter

        Rectangle{
            id:imgdummy
            color:style.lighthighlight
            border.color: "white"
            border.width: 2
            height: scaleY(55)
            width: scaleX(55)
            clip: true

            Image {
                id: floorplanimage
                source: "image://listprovider/"+"floorplan"
                asynchronous: true
                anchors.fill: parent
            }
        }

        Rectangle
        {
            height: childrenRect.height + 5
            width: childrenRect.width + 5
            color: style.lighthighlight
            Text{
                id:fplabel
                text: qsTr("Floorplans")
                height: scaleY(5)
            }

        ListView{
            id:floorplanlist
            height: scaleY(10)
            width: scaleX(18)
            model:floorplan
            clip:true
            anchors.top: fplabel.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            delegate:Rectangle{
                height: scaleY(9)
                width: scaleX(16)
                color: style.accentcolor
                border.color: style.darkhighlight
                Text {
                    id: desc
                    text: description
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: floorplanimage.source = "image://listprovider/floorplan"
                }
            }
        }
        }

        Rectangle
        {
            height: childrenRect.height + 5
            width: childrenRect.width
            color: style.lighthighlight
            Text{
                id:devicelabel
                text: qsTr("Devices")
                height: scaleY(5)
            }

        ListView{
            id:devicelist
            height: scaleY(10)
            width: scaleX(18)
            model:floorplan
             clip:true
            anchors.top: devicelabel.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            delegate:Rectangle{
                height: scaleY(9)
                width: scaleX(16)
                color: style.accentcolor
                border.color: style.darkhighlight
                Text {
                    id: devicedesc
                    text: ident
                }
            }
        }
        }


    }
}
