import QtQuick 1.0

Rectangle {
    width: style.orbiterW
    height: style.orbiterH
    color: style.highlight2
    Connections{
        target: floorplan
        onPageChanged: floorplanimage.source = "image://listprovider/floorplan/"+securityvideo.timestamp
    }

    Rectangle{
        id:mainRect
        height: scaleY(80)
        width:scaleX(75)
        border.color: style.highlight1
        border.width: scaleX(1) *.25
        radius: 10
        anchors.centerIn: parent
        color:style.lighthighlight

        Column
        {
            id:layoutcolumn
            height: scaleY(65)
            width: childrenRect.width
            anchors.centerIn: parent
            spacing: scaleY(1)

            Rectangle{
                id:imgdummy
                color:style.darkhighlight
                border.color: "white"
                border.width: 2
                height: scaleY(50)
                width: scaleX(50)
                clip: true

                Image {
                    id: floorplanimage
                    source: ""
                    anchors.fill: parent
                    height:parent.height
                    width: parent.width
                }
            }

            Row{
                id:mainrow
                height: childrenRect.height
                width: childrenRect.width
                spacing: 5
                clip: true

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
                                onPressed:{
                                    floorplan.setCurrentPage(ident)

                                        }
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
    }
}
