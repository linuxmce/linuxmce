import QtQuick 1.0

Rectangle {
    property alias rnd_but_label: rnd_but_text.text
    id: rnd_base
    width: 25
    height: 25
    radius: 31

    Text {
        id: rnd_but_text
        text: ""
        anchors.centerIn: parent
    }

}
