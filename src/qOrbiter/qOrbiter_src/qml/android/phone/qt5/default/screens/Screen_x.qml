import QtQuick 2.2
import "../components"
Item{
    id:no_screen

    Rectangle{
        width: manager.appWidth
        height: manager.appHeight
        color: "transparent"
        Text {            
            id: screen_not_found
            anchors.centerIn: parent
            width: scaleX(75)
            text: pageLoader.errorString()+"\n " + screenfile
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.family: "Droid Sans"
            font.bold: false
            font.pixelSize: scaleY(3)
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
