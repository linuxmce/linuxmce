// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../components"
Rectangle {
    id:myth_pvr_recordings
    width: manager.appWidth
    height: manager.appHeight


    HomeButton{id:home}

    ButtonSq{
        buttontext: "Back to live screen"
        MouseArea{
            anchors.fill: parent
            onReleased: manager.setCurrentScreen("Screen_48.qml")
        }
    }


}
