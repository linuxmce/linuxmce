import QtQuick 1.0

Rectangle {
    id:buttonBase
     property alias buttontext: buttonLabel.text

    Text {
        id: buttonLabel
        text:""
        anchors.centerIn: parent

         }
    states: [
      State {
            name: "hovering"
            PropertyChanges {target: buttonBase; color: style.button_action_color_hover}
            PropertyChanges {target: buttonLabel; text: "hovering!"}
             },

        State {
        name: "regular"
        PropertyChanges {target: buttonBase; color: style.button_action_color}
        PropertyChanges {target: buttonLabel; text: "I have become unset!"}
            }
    ]

    MouseArea
    {   anchors.fill: parent
        hoverEnabled: true
        onEntered: {buttonBase.state= "hovering"}
        onExited: {buttonBase.state="regular"}

    }
}
