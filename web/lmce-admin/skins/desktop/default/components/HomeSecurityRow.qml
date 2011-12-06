import QtQuick 1.0

Item {
    height: scaleY(17)
    width: scaleX(100)
    clip:true
    Rectangle{
        height: scaleY(17)
        width: scaleX(100)
        clip:true
color:"transparent"
        radius: 20

        HomeButtonDelegate{id:securityDelegate}

        Row {
            id: guide
            spacing:2
            x: scaleX(2)
            ButtonSq {
                id: rowheader
                height: scaleY(13)
                width: scaleX(8)
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                color: style.homescreenfloorplanbuttoncolor
                radius: style.but_smooth
                buttontext: ""

                Image {
                    id: buttonbg2
                    source: "../../../img/icons/buttonoverlay.png"
                    height: parent.height
                    width: parent.width
                }

                Image {
                    id: buttonbg
                    source: "../../../img/icons/Security.png"
                    height: parent.height
                    width: parent.width
                }

                MouseArea{
                    id: mousearea1
                    onClicked:showfloorplan(5)
                }
            }

        Flickable
        {
            id:securityflick
            height: scaleY(16)
            width: scaleX(95)
            flickableDirection: "HorizontalFlick"
            contentHeight: childrenRect.height
            contentWidth: ((style.buttonW + 5) * (securityScenarios.count + 1)) - 5
            clip: true

            ListView{
                id: securityScenarios
                height: scaleY(style.buttonH)
                width: stage.width
                model: currentRoomSecurity
                orientation:ListView.Horizontal
                spacing: 5

                delegate: securityDelegate
                interactive: false
            }
        }
    }
}
}
