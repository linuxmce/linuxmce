import Qt 4.7

Rectangle {

    height: 65
    width: parent.width
color:style.rowbgColor

Component
{
    id:securityDelegate

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

Flickable
{
    id:securityflick
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


        ListView{
            id: securityScenarios
            width: 300
            height: 50
            model: currentRoomSecurity
            orientation:ListView.Horizontal

            delegate: securityDelegate
                 }

    }
}
}
