import QtQuick 1.0

Rectangle {
    property alias bottomPanelTextColor:adv_but_text.color

    width: parent.width
    height: 50
    id:advanced_panel
            Text {
            id: adv_but_text
            text: "Advanced"
            font.pointSize: 14
        }


}
