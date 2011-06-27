import QtQuick 1.0

Rectangle {
    property alias rowlabel: rowheader.buttontext
    height: 80
    width: parent.width
color:style.rowbgColor
  Flickable{
      height: parent.height
      width: parent.width -5
      id:lightflick
      contentHeight: 80
      contentWidth: 800
      interactive: true
      clip: true
      anchors.fill: parent

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
                source: "../../img/icons/jabber_protocol.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }

            }

        ButtonSq {
            id: lt_on
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "On"


            }

        ButtonSq {
            id: lt_off
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Off"

            }
    }
  }
}
