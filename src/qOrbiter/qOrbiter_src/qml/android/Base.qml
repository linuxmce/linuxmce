import QtQuick 1.1
import com.nokia.android 1.1

Item {
    anchors.fill: parent

    Loader{
        anchors.fill: parent
        source:"http://"+dcerouter.dceIP+"lmce-admin/skins/android/Splash.qml"
    }

}
