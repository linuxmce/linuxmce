/*!
 *\brief Manage playlist screen.
 *
 *\ingroup qml_desktop_default
 *\todo needs better description.
 *Manage playlist screen.
 */
import QtQuick 1.1
import "../components"
Item{
    id:manage_playlist
    anchors.fill: parent
    Rectangle{
        anchors.fill: parent
        color: "transparent"
        Text {
            id: statussec
            x: 74
            y: 101
            text: "Manage Playlist"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
