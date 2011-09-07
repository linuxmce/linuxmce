import QtQuick 1.0

Item {
    property alias rowlabel: rowheader.buttontext
    height: scaleY(17)
    width: scaleX(parent.width)

    Rectangle{
        height: scaleY(17)
        width: scaleX(100)
        clip:true
        color:"midnightblue"
        radius: 20

   HomeButtonDelegate{id:climateDelegate}
    Flickable{

        id:climateRow
        height: scaleY(16)
        width: scaleX(100)
        anchors.centerIn: parent
        contentHeight: style.buttonH
        contentWidth: ((style.buttonW + 5) * (climateScenarios.count + 1)) - 5
        clip: true
        flickableDirection: "HorizontalFlick"



    Row {
        id: guide
        spacing:5

        ButtonSq {
            id: rowheader
            height: scaleY(16)
            width: scaleX(9)
            color: style.homescreenfloorplanbuttoncolor
            radius: style.but_smooth
            buttontext: "Climate"

            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_4.qml")
             z:5 }
            }


        ListView{
            id: climateScenarios
            height: scaleY(style.buttonH)
            width: stage.width
            model: currentRoomClimate
            spacing: 5
            orientation:ListView.Horizontal
            interactive:false
            delegate: climateDelegate
                 }

    }
}
}
}
