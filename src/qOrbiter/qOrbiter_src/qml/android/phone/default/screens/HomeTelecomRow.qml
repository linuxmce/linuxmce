import QtQuick 1.0

Item {
    property alias rowlabel: rowheader.buttontext
    height: style.homescreenrowheight
    width: parent.width


    HomeButtonDelegate{id:telecomDelegate}

    Flickable{
        id:securityflick

        height: style.rowH
        width: parent.width
        contentHeight: style.buttonH + 10
        contentWidth: ((style.buttonW + 5) * (telecomScenarios.count + 1)) - 5
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
            color: style.homescreenfloorplanbuttoncolor
            radius: style.but_smooth
            buttontext: "Telecom"
            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked: gotoQScreen("Screen_6.qml")
              }

            }
        ListView{
            id: telecomScenarios
            width: stage.width
            height: style.rowH
            model: currentRoomTelecom
            spacing: 5
            interactive: false
            orientation:ListView.Horizontal
            delegate: telecomDelegate
                 }

          }
    }
}
