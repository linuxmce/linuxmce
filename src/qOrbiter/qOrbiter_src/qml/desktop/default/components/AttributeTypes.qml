import QtQuick 1.1
import "../../../skins-common/lib/handlers"
Rectangle {
    id:rect
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"
    anchors.centerIn: parent
    border.color: style.button_system_color
    border.width: 1
    clip: true

    Component{
        id:attributedelegate
        Item {
            id: fileformatitem
            height: 50
            width: 150

            Rectangle{
                id: formatrect
                height: 50
                width: 130
                border.color: "black"
                border.width: 1
                color: status ? style.bgcolor : "cornflowerblue"
                MouseArea{
                    anchors.fill: parent
                    z:0
                    onClicked:
                    {
                        rect.destroy()
                    }
                }


                Row{
                    height: childrenRect.height
                    width: childrenRect.width
                    spacing: 10

                    StyledText {
                        width: 75
                        height: parent.height
                        id: fileformatcell
                        textLabel: name
                        fontSize:12
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        onFocusChanged: {rect.destroy()}

                    }

                    Rectangle{
                        height: 25
                        width: 25
                        border.color: "black"
                        border.width: 1
                        color: status ? "green" : "red"
                        z:10
                        AttributeFilterHandler {
                            onClicked: {
                                formatrect.opacity = 0
                                formatrect.color = status ? "green" : "red"
                                formatrect.opacity = 1
                            }


                        }
                    }

                }


            }
        }
    }


    GridView{
        id:genrelist
        height: 400
        width: 450
        model: attribfilter
        cellHeight: 150
        cellWidth: 150
        delegate: attributedelegate

    }

}


