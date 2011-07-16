import QtQuick 1.0
Item {

    height: style.homescreenrowheight
    width: parent.width


    Component
    {
        id:lightingdelegate

        Item {
            id: delegateitem
            height: 80
            width: childrenRect.width

        ButtonSq
        {       id:delegateButton
                height: style.buttonH
                width: style.buttonW
                color: style.bgcolor
                radius: style.but_smooth
                buttontext: ""
                buttontextfontsize: 12
                buttontextbold: true
                buttontextzindex: 30
                opacity: .5
                 MouseArea{
                     anchors.fill: parent
                     onClicked: dceObject.execGrp(params);
                           }
                        }
                  Image {
                        id:buttonimage
                        source: "../../../img/icons/"+label+".png"
                        height: style.iconHeight
                        width: style.iconWidth
                         anchors.centerIn: delegateButton
                         }

                  Text {
                      id: daslabel
                      text: label
                      font.pointSize: 12
                      font.bold: true
                      wrapMode: "WrapAtWordBoundaryOrAnywhere"
                      anchors.centerIn: delegateButton
                      width: delegateButton.width
                      height: delegateButton.height
                      z:2
                      horizontalAlignment: Text.AlignHCenter
                      verticalAlignment: Text.AlignVCenter

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
        height: 80
        width: parent.width
        contentHeight: 80
        contentWidth: childrenRect.width
        clip: true

        ButtonSq {
            id: lightingfloorplan
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
            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked: dceObject.gotoQScreen("Screen_2.qml")
              }

        }
        Row {
        id: guide

        anchors.left: lightingfloorplan.right
        spacing:5



        ListView{
            id: lightingScenarios
            width: stage.width
            height: 50
            model: currentRoomLights
            spacing: 5
            orientation:ListView.Horizontal

            delegate:  lightingdelegate
                 }
        }
    }
}

