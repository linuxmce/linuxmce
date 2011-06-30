import QtQuick 1.0

Rectangle {
    property alias rowlabel: rowheader.buttontext
    height: 65
    width: parent.width
    color:style.rowbgColor

    Flickable{
        interactive: true
        height: 65
        width: parent.width
        contentHeight: 65
        contentWidth: childrenRect * 1
        clip: true

Row {
        id: guide
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        spacing:5

        ButtonSq {
            id: rowheader
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""
            Image {
                id: onimg
                source: "../../../img/icons/jabber_protocol.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent
                    }
              }

        ListView{
            id: lightingScenarios
            width: parent.width
            height: parent.height
            model: lightingModel
            delegate: ButtonSq {
                id: but
                height: style.buttonH
                width: style.buttonW
                color: style.button_action_color
                radius: style.but_smooth
                buttontext: "on"
                Image {
                    source: "../../../img/icons/jabber_protocol.png"
                    height: style.iconHeight
                    width: style.iconWidth
                    anchors.centerIn: parent
                        }
                    }
                }
            }
        }
}
