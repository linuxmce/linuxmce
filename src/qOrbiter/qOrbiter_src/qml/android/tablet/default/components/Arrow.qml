import QtQuick 1.0

Rectangle {
    Style{id:style}
    property alias arrow_label: arrow_type.text
    width: 50
    height: 25
    color: style.arrow_color_nav
    Text {
        id: arrow_type
        text: ""
        color:style.arrow_text_nav
    }
}
