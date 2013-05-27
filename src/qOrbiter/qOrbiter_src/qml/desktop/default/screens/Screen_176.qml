/*!
 *\brief LABEL ME
 *
 *\ingroup qml_desktop_default
 *\bug needs better description
 *
 */
import QtQuick 1.1
import "../components"
Rectangle {
    width: 100
    height: 62
    color: "transparent"

    StyledText{
        anchors.centerIn: parent
        text:"Video Playback Screen?"
    }

    HomeButton{}
}
