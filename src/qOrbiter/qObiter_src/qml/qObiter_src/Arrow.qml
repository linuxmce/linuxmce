import QtQuick 1.0

Rectangle {
    property alias arrow_label: arrow_type.text
    width: 50
    height: 25
    Text {
        id: arrow_type
        text: ""
    }
}
