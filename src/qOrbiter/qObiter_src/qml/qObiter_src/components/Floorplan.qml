import QtQuick 1.0

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

        Rectangle {
            id: rectangle1
            x: 28
            y: 28
            width: 0
            height: 0
            color: "#ffffff"
            visible: false
            opacity: 0

            HomeButton {
                id: homebutton1
                x: 200
                y: 30
                opacity: 0
            }
        }
            }
    MouseArea{
        id: mousearea1
        anchors.fill: parent
        onClicked: rowheader.state = "State1"
      }
      states: [
          State {
              name: "State1"

              PropertyChanges {
                  target: mousearea1
                  visible: false
              }

              PropertyChanges {
                  target: onimg
                  visible: false
              }

              PropertyChanges {
                  target: rowheader
                  visible: false
              }

              PropertyChanges {
                  target: rectangle1
                  anchors.centerIn: maindisplay
                  width: 400
                  height: 300
                  clip: true
                  visible: true
                  opacity: 1
                  x:50
                  y:50
                  z:20
              }

              PropertyChanges {
                  target: homebutton1
                  x: 325
                  y: 0
                  width: 75
                  height: 75
                  visible: true
                  opacity: 1
              }
          }
      ]
}
