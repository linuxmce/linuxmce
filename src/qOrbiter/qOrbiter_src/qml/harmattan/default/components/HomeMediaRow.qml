import QtQuick 1.0

Item {
    signal showDatagrid( string msg)
    signal gotoScreen( string screen, string type)
    property alias nowplayingtext:now_playing.buttontext
    property alias rowlabel: rowheader.buttontext


    height: style.homescreenrowheight
    width: parent.width

HomeButtonDelegate{id:mediaDelegate}


Flickable{
    id:mediaflick

    height: style.buttonH
    width: parent.width
    contentHeight: style.buttonH
    contentWidth: childrenRect.width * 2
    clip: true
    flickableDirection: "HorizontalFlick"

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
            buttontext: ""

            Image {
                id: mHeaderImg
                source: "../../../img/icons/kmix.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent
                }
            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked:setCurrentScreen("Screen_3.qml")
              }
            }

        ButtonSq {
            id: now_playing
            height: style.buttonH
            width: style.buttonW
            color: nowPlayingColor
            radius: style.but_smooth
            buttontext: ""

            MouseArea{
                anchors.fill: parent
                onClicked:setCurrentScreen("Screen_48.qml")
            }
            }
        ListView{
            id: listView
            width: stage.width
            height: style.buttonH
            model: currentRoomMedia

            orientation:ListView.Horizontal
            spacing: 5
            delegate: mediaDelegate
            interactive: false

              }
         }
    }
}
