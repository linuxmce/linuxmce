/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: splashPage1
    Image {
        id: splashImage
        source: "qrc:/img/harmattan_splash_bkg.png"
        fillMode: Image.PreserveAspectCrop
        anchors.fill: parent
    }
    Image {
        id: splashLogo
        source: "qrc:/img/harmattan_splash.png"
        anchors.centerIn: splashImage
    }
}
