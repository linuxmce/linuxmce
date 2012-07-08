// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"

    Component{
        id:generic_delegate_component
        Item {
            id: generic_delegate_item
            height: scaleY(15)
            width: scaleX(35)

            Rectangle{
                id: generic_delegate_container
                height: scaleY(15)
                width: scaleX(35)
                border.color: "white"
                border.width: 1
                color: status ? "grey": "steelblue"

                Text {
                    id: generic_delegate_text
                    text: name
                    font.pointSize: scaleY(3)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    anchors.centerIn: parent
                    onFocusChanged: { loadComponent("NullComponent.qml")}
                }
            }
        }
    }
}
