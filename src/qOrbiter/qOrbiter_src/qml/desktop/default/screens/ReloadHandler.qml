// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:router_reloading
    width: appW
    height: appH
    color: "transparent"


    Text {
        id: reload_text
        text: qsTr("Router is Reloading")
        font.pixelSize: scaleY(4)
        color: "black"
        font.bold: true
    }
}
