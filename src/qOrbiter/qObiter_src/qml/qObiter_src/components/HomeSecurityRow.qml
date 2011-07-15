import QtQuick 1.0

Item {

    height: style.homescreenrowheight
    width: parent.width


Component
{
    id:securityDelegate

    Item {
        id: delegateitem
        height: 80
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

Flickable
{
    id:securityflick
    interactive: true
    height: 80
    width: parent.width
    contentHeight: 80
    contentWidth: childrenRect * 1
    clip: true

        ButtonSq {
            id: rowheader

            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Security Floorplan"

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

            }
        }

        ListView{
            id: securityScenarios
            width: stage.width
            height: 50
            model: currentRoomSecurity
            orientation:ListView.Horizontal
            spacing: 5
            anchors.left: rowheader.right
            anchors.margins: 5
            delegate: securityDelegate
                 }
           }
       }

