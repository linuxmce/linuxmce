import QtQuick 2.2

Rectangle {
    id:rect
    width: scaleX(55)
    height: scaleY(55)
    color: "transparent"
    anchors.centerIn: parent
    border.color: "silver"
    border.width: 1
    clip: true
    Rectangle{
        id:exit_button
        height: scaleY(8)
        width: parent.width
        anchors.top: rect.top
        color: "transparent"
        Image {
            id: headerbg
            source: "../img/widegreyshape.png"
            anchors.fill: exit_button
        }

        Text {
            id: exit
            text: qsTr("Exit")
            font.pixelSize: scaleY(3)
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            onClicked: loadComponent("NullComponent.qml")
        }
    }

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
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    anchors.centerIn: formatrect
                    onFocusChanged: {loadComponent("NullComponent.qml")}

                }

                MouseArea{
                    anchors.fill: parent
                    onPressed: parent.color = "silver"
                    onReleased: {
                        formatrect.opacity = 0
                        attribfilter.setSelectionStatus(name)
                        formatrect.color = status ? "silver" : "orange"
                        formatrect.opacity = 1
                        parent.color = "orange"
                        loadComponent("NullComponent.qml")
                    }

                }
            }
        }
    }

    ListView{
        id:genrelist
        height: scaleY(45)
        width: scaleX(55)
        model: attribfilter
        delegate: attributedelegate

    }

}


