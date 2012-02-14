import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs



Rectangle {
    id:fileviewscreen

    width: appW
    height: appH
    color: "slateblue"

    Text {
        id: label
        text: qsTr("File View Screen")
    }

}
