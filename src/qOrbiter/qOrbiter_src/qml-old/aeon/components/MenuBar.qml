import QtQuick 1.1

Item {
    id: menuBar
    property alias items: repeater.model
    property variant origin: 'top-left' // also: top-right, bottom-right, bottom-left

    Row {
        Image {
            id: imgLeftEnd
            source: "../img/home/button_back_end.png"
            width: scaleX(1.941) // 25
            height: scaleY(3.75) // 27
            visible: (origin=="top-right"||origin=="bottom-right")?true:false
            transform: Rotation {
                origin.x: imgLeftEnd.width/2;
                origin.y: imgLeftEnd.height/2;
                axis {
                    x: 0;
                    y: menuBar.origin=="top-right"?1:0;
                    z: menuBar.origin=="bottom-right"?1:0
                }
                angle: 180
            }
        }
        Repeater {

            id: repeater
            Rectangle{
                width: scaleX(8.226) // 105
                height:  scaleY(3.75) // 27
                color: "transparent"
                Image {
                    id: imgBg
                    anchors.fill:  parent
                    source: "../img/home/button_back.png"
                    transform: Rotation {
                        origin.x: imgBg.width/2;
                        origin.y: imgBg.height/2;
                        axis {
                            x: 0;
                            y: (menuBar.origin=="bottom-right"||menuBar.origin=="top-right")?1:0
                            z: 0
                        }
                        angle: 180
                    }

                }
                Text {
                    text: model.text
                    color: "white"
                    font.family: aeonRss.name;
                    font.pixelSize: parent.height*.7;
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: if (model.action) eval(model.action) // Hate having to use eval(), but there seems to be no other choice
                }
            }
        }
        Image {
            id: imgRightEnd
            source: "../img/home/button_back_end.png"
            width: scaleX(1.941) // 25
            height: scaleY(3.75) // 27
            visible: (origin=="top-left"||origin=="bottom-left")?true:false
            transform: Rotation {
                origin.x: imgRightEnd.width/2;
                origin.y: imgRightEnd.height/2;
                axis {
                    x: (menuBar.origin=="bottom-left")?1:0;
                    y: 0
                    z: 0
                }
                angle: 180
            }
        }

    }

}
