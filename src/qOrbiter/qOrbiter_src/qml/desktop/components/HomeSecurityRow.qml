import QtQuick 1.0

Item {
    height: scaleY(17)
    width: scaleX(100)
    clip:true
    Rectangle{
        height: scaleY(17)
        width: scaleX(100)
        clip:true
        color:"midnightblue"
        radius: 20
        Image {
            id: rowbgimg

            source: "../../../img/icons/rowbg.png"
            width: parent.width
            height: parent.height

        }
        HomeButtonDelegate{id:securityDelegate}

        Row {
            id: guide
            //anchors.left: lightingfloorplan.right
            spacing:2
            anchors.left: parent.left
            anchors.leftMargin: scaleX(2)
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
                    source: "../../../img/icons/buttonoverlay3.png"
                    anchors.fill: lightingfloorplan
                    height: parent.height
                    width: parent.width
                }

                Image {
                    id: buttonbg
                    source: "../../../img/icons/Security.png"
                    anchors.fill: rowheader
                    height: parent.height
                    width: parent.width
                }

                MouseArea{
                    id: mousearea1
                    anchors.fill: parent
                    onClicked:showfloorplan(5)
                }
            }

        Flickable
        {
            id:securityflick
            height: scaleY(16)
            width: scaleX(95)
            anchors.left: rowheader.right
            anchors.leftMargin: 4
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
