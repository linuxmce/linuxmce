import QtQuick 1.0

Item {
    property alias rowlabel: rowheader.buttontext
    height: style.homescreenrowheight
    width: parent.width


    Component
    {
        id:telecomDelegate

        Item {
            id: delegateitem
            height: 75
            width: 75

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
                anchors.centerIn: parent

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

    Flickable{
        id:securityflick
        interactive: true
        height: 80
        width: parent.width
        contentHeight: 80
        contentWidth: childrenRect * 1
        clip: true
    Row {
        id: telecomRow
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
            buttontext: "Telecom"

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

            }

            }
        ListView{
            id: climateScenarios
            width: stage.width
            height: 50
            model: currentRoomTelecom
            spacing: 5
            orientation:ListView.Horizontal
            delegate: telecomDelegate
                 }

          }
    }
}
