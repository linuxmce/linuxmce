import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import "."

/*! QML based screensaver optimized for qt5 */
Item{
    id:qmlPictureFrame

    property variant pictureList:[]
    property int switchTimer:15000
    readonly property int transitionTime:switchTimer-4500

    Timer{
        id:screenSaverTimer
        interval: transitionTime
        onTriggered: loadNextImage()
    }

    function loadImageList(){

    }

    function loadNextImage(){

    }

    function startScreenSaver(){

    }
    function stopScreenSaver(){

    }

    states: [
        State {
            name: "active"
        },
        State {
            name: "inactive"
        }
    ]

}
