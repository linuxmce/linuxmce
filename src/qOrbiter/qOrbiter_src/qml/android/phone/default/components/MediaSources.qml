import QtQuick 1.0

Rectangle {
    id:rect
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"
    anchors.centerIn: parent
    border.color: "silver"
    border.width: 1
    clip: true

    Component{
        id:attributedelegate
        Item {
            id: fileformatitem
            height: scaleY(10)
            width: scaleX(55)

            Rectangle{
                id: formatrect
                height: scaleY(10)
                width: scaleX(55)
                border.color: "black"
                border.width: 1
                color: status ? "transparent": "silver"

                Text {
                    width: parent.width
                    height: parent.height
                    id: fileformatcell
                    text: name
                    font.pixelSize: scaleY(2)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    onFocusChanged: {rect.destroy()}

                }

                MouseArea{
                    anchors.fill: parent
                    onPressAndHold: {
                        formatrect.opacity = 0
                        mediasourcelist.setSelectionStatus(name)
                        formatrect.color = status ? "green" : "red"
                        formatrect.opacity = 1
                        parent.color = "orange"
                        rect.destroy()
                    }
                    onReleased: parent.color = "silver"

                }
            }
        }
    }

    ListView{
        id:genrelist
        height: scaleY(45)
        width: scaleX(55)
        model: mediasourcelist

        delegate: attributedelegate

    }

}


