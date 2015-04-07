import QtQuick 1.0

Item {
    property alias rowlabel: rowheader.buttontext
    height: style.homescreenrowheight
    width: parent.width

   HomeButtonDelegate{id:climateDelegate}
    Flickable{

        id:climateRow
        height:style.buttonH +5
        width: parent.width
        contentHeight: style.buttonH
        contentWidth: childrenRect.width * 2
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
            color: style.homescreenfloorplanbuttoncolor
            radius: style.but_smooth
            buttontext: "Climate"

            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked:setCurrentScreen("Screen_4.qml")
             z:5 }
            }


        ListView{
            id: climateScenarios
            width: 300
            height: style.buttonH
            model: currentRoomClimate
            spacing: 5
            orientation:ListView.Horizontal
            interactive:false
            delegate: climateDelegate
                 }

    }
}
}
