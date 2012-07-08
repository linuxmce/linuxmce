import QtQuick 1.0

Rectangle {
    width: style.orbiterW
    height: style.orbiterH
    color: style.highlight2
    Connections{
        target: floorplan
        onPageChanged: {
            floorplanimage.source = "image://listprovider/floorplan/"+floorplan.currentPage
        }
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


            Image {
                id: floorplanimage
                source: ""
                anchors.fill: parent
                height:scaleY(45)
                width: scaleX(45)
                fillMode: Image.PreserveAspectFit
                anchors.left: parent.left
                anchors.leftMargin: scaleY(2)

            }



            Rectangle
            {
                height: childrenRect.height + 5
                width: childrenRect.width + 5
                color: style.lighthighlight
                anchors.right: parent.right
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
    }
}

