import QtQuick 1.0
Rectangle{
    id: btHomeMedia
    clip:false
    color:"#000000"
    opacity: 0.800
    visible: false

    width: scaleX(97)
    height: style.btHomeDefaultH
    anchors.bottom: parent.top
    anchors.bottomMargin: parent.buttonbg.height + mainButtonContainer.height + scaleY(1)
    anchors.horizontalCenter: parent.stage.horizontalCenter

    HomeButtonDelegate{id:mediaDelegate}
    Flickable{
            id:mediaflick
            height: scaleY(16)
            width: parent.width
            contentHeight: style.buttonH
            contentWidth: (style.buttonW + 5) * (mediaflick.children.width)
            clip: true
            flickableDirection: "HorizontalFlick"
            anchors.verticalCenter: parent.verticalCenter

            ListView{
                id: mediaScenarios
                height: scaleY(style.buttonH)
                width: stage.width

                model: currentRoomMedia
                orientation:ListView.Horizontal
                spacing:5
                delegate: mediaDelegate
                interactive: false

            }
        }
}
