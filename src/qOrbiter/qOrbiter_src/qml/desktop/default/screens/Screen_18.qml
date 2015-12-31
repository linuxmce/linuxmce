/*!
 *\brief Video Call in progress
 *
 *\ingroup qml_desktop_default
 *\bug needs better description
 * Screen for video call in progress, needs more definition.
 */
import QtQuick 1.1
import "../components"
Item{
    id:climate

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        Text {
            id: climatelabel
            x: 74
            y: 101
            text: "Video Call in Progress"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
