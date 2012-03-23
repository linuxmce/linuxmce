import QtQuick 1.0

Rectangle {
    id:rect
    width: scaleX(55)
    height: scaleY(55)
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

                    id: fileformatcell
                    text: name
                    font.pixelSize: scaleY(2.5)
                    anchors.centerIn: formatrect
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    onFocusChanged: {loadComponent("NullComponent.qml")}

                }

                MouseArea{
                    anchors.fill: parent
                    onPressed: {
                        formatrect.opacity = 0
                        genrefilter.setSelectionStatus(name)
                        formatrect.color = status ? "green" : "red"
                        formatrect.opacity = 1
                        parent.color = "orange"
                       loadComponent("NullComponent.qml")
                    }
                    onReleased: parent.color = "silver"

                }
            }
        }
    }

    ListView{
        id:genrelist
        width: scaleX(55)
        height: scaleY(55)
        model: genrefilter
        delegate: attributedelegate

    }

}


