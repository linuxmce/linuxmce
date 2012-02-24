import QtQuick 1.0

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
                color: status ? "transparent": "silver"



                Text {
                    width: 75
                    height: parent.height
                    id: fileformatcell
                    text: name
                    font.pointSize: scaleY(3)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    onFocusChanged: {rect.destroy()}

                }

                MouseArea{
                    anchors.fill: parent
                    onPressed: {
                        formatrect.opacity = 0
                        attribfilter.setSelectionStatus(name)
                        formatrect.color = status ? "green" : "red"
                        formatrect.opacity = 1
                        parent.color = "orange"
                    }
                    onReleased: parent.color = "silver"

                }
            }
        }
    }

    ListView{
        id:genrelist
        height: 400
        width: 450
        model: attribfilter

        delegate: attributedelegate

    }

}


