import QtQuick 1.0

Rectangle {
    width: style.orbiterH
    height: style.orbiterW
    color: "transparent"

    Image {
        id: ssimg
        fillMode: Image.PreserveAspectCrop
        source: screensaver.current_image

    }

    Connections{
        target: screensaver
        onImageChanged:flash.complete
    }

    Rectangle{
        id:flashbox
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"

        SequentialAnimation on color {

            id:flash
         //   running: true
            //loops: Animation.Infinite
          // PauseAnimation { duration: 900 }
            PropertyAnimation{ target:flashbox; property: "color"; to: "black"; duration: 2000}
            PropertyAnimation{ target:flashbox; property: "color"; to: "transparent"; duration: 2000}
        }
    }
}
