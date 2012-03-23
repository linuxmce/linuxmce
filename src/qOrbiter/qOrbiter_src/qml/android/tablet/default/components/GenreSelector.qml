import QtQuick 1.0

Rectangle {
    id:rect
    width: scaleX(90)
    height: scaleY(85)
    color: "transparent"
    anchors.centerIn: parent
    border.color: style.button_system_color
    border.width: 1
    clip: true


    Component{
        id:genredelegate
        Item {
            id: fileformatitem
            height: scaleY(8)
            width: scaleX(15)

            Rectangle{
                id: formatrect
                height: scaleY(8)
                width: scaleX(15)
                border.color: "black"
                border.width: 1
                color: status ? style.stageBG : "slategrey"
                clip:true
                Image {
                    id: overlay
                    source: "../img/icons/rowbg.png"
                    anchors.fill:parent
                    opacity: .75
                }
                MouseArea{
                    anchors.fill: parent                  
                    onClicked:
                    {
                        loadComponent("NullComponent.qml")
                    }
                }


                Row{
                    height: childrenRect.height
                    width: childrenRect.width

                    Text {

                        width: scaleX(10)
                        height: parent.height
                        id: fileformatcell
                        text: status ? name +" Is selected": name
                        font.pointSize: scaleY(2.5)
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"

                    }

                    Rectangle{
                        height: scaleY(8)
                        width: scaleX(5)
                        border.color: "black"
                        border.width: 1
                        color: status ? "green" : "red"
                        anchors.right: parent.right                       
                        MouseArea{
                            anchors.fill: parent
                            onClicked:{
                                genrefilter.setSelectionStatus(name)

                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle{
        id:exit_button
        height: scaleY(12.5)
        width: scaleX(90)
        color: "darkgrey"
        opacity: .65
        anchors.top: rect.top
        clip: true
        MouseArea{
            id: exit_mousearea
            anchors.fill: parent
            onPressed:loadComponent("NullComponent.qml")
        }
    }
    Text {
        id: exit_label
        text: qsTr("Exit")
        anchors.centerIn: exit_button
        font.pixelSize: scaleY(3.5)
    }


    GridView{
        id:genrelist
        height: scaleY(85)
        width: scaleX(95)
        model: genrefilter
        cellHeight: scaleY(8.5)
        cellWidth: scaleX(15.5)
        delegate: genredelegate
        anchors.top: exit_button.bottom
        clip:true
    }
}


