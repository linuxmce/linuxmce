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
            height: scaleY(12.5)
            width: scaleX(30)

            Rectangle{
                id: formatrect
                height: scaleY(12.5)
                width: scaleX(30)
                border.color: "white"
                border.width: 1
                color: status ? "grey": "steelblue"

                Text {
                    id: fileformatcell
                    text: name
                    font.pointSize: scaleY(3)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    anchors.centerIn: parent
                    onFocusChanged: {loadComponent("NullComponent.qml")}
                }

                MouseArea{
                    id:generic_filter_mouseArea
                    anchors.fill: parent
                    onReleased: {
                        attribfilter.setSelectionStatus(name)
                        formatrect.color = status ? "grey": "steelblue"
                        loadComponent("NullComponent.qml")
                    }
                    onPressed: status ? "grey": "steelblue"
                }
            }
        }
    }

    ListView{
        id:genrelist
        height: scaleY(55)
        width: scaleX(30)
        model: attribfilter
        delegate: attributedelegate
        anchors.top: exit_button.bottom
        clip:true
    }
    Rectangle{
        id:exit_button
        height: scaleY(12.5)
        width: scaleX(30)
        color: "darkgrey"
        opacity: .45
        anchors.top: rect.top
        clip: true
        MouseArea{
            id: exit_mousearea
            anchors.fill: parent
            onPressed: loadComponent("NullComponent.qml")
        }
    }
    Text {
        id: exit_label
        text: qsTr("Exit")
        anchors.centerIn: exit_button
        font.pixelSize: scaleY(3.5)
    }


}


