import Qt 4.7
Rectangle {
    property alias rowlabel: rowheader.buttontext
    height: 65
    width: parent.width
    color:style.rowbgColor

    Component
    {
        id:lightingdelegate

        Item {
            id: delegateitem
            height: 65
            width: childrenRect.width

        ButtonSq
        {
                height: style.buttonH
                width: style.buttonW
                color: style.button_action_color
                radius: style.but_smooth
                buttontext: label
                buttontextfontsize: 12
                buttontextbold: true
                buttontextzindex: 30

                 MouseArea{
                     anchors.fill: parent
                     onClicked: console.log ("I am button:" + index + ". My Name is:"+ label+". My Params:" + params +". And my Command:" + command)
                           }

                  Image {
                        id:buttonimage
                        source: "../../../img/icons/"+label+".png"
                        height: style.iconHeight
                        width: style.iconWidth
                         anchors.centerIn: parent
                         }
                }
            }
       }
    ListView.onAdd: SequentialAnimation {
          PropertyAction { target: lightingdelegate; property: "height"; value: 0 }
          NumberAnimation { target: lightingdelegate; property: "height"; to: 65; duration: 500; easing.type: Easing.InOutQuad }

      }

      ListView.onRemove: SequentialAnimation {
          PropertyAction { target: lightingdelegate; property: "ListView.delayRemove"; value: true }
          NumberAnimation { target: lightingdelegate; property: "height"; to: 0; duration: 500; easing.type: Easing.InOutQuad }

          // Make sure delayRemove is set back to false so that the item can be destroyed
          PropertyAction { target: lightingdelegate; property: "ListView.delayRemove"; value: false }

           }

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
            width: 300
            height: 50
            model: currentRoomLights
            orientation:ListView.Horizontal

            delegate:  lightingdelegate
                 }
        }
    }
}

