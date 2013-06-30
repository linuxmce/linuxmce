import QtQuick 1.1

Item {
    id:screen1
    height: manager.appHeight
    width: manager.appWidth



    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: parent.activeFocus ? .65 : .25
    }

    Text{
        anchors.centerIn: parent
        text:"Hello Middle Earth \n Active focus is "+screen1.activeFocus
        font.pixelSize: 32
        color:"white"
    }

}
