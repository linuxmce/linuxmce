import QtQuick 1.0

Component{
        Item {
    id: skindelegate
    anchors.centerIn: parent
    height: 350
    width: 350
    Row{

        height: 75
        width: skindelegate.width

        Rectangle{
            id:rectDelegate
            height: 100
            width: skindelegate.width
            color: style.button_system_color

        Text {
            anchors.top: rectDelegate.top
            id: nameskin
            text: "Name:" + name + ". Version:" + version
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            }
        Text {
            anchors.top: nameskin.bottom
            id: skinDescription
            text: description
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            }

        Rectangle
        {
            anchors.right: rectDelegate.right
            width: 150
            height: rectDelegate.height
            Text {
                anchors.centerIn: parent
                id: skincreator
                text: "Created By:" + creator
            }
            Text {
                anchors.top: skincreator.bottom
                id: skinvariant
                text: "Variation for: " + variation
            }
          }
        }

    }

  }
}
