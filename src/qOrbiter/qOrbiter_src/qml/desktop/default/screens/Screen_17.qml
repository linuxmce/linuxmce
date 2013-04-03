import QtQuick 1.1
import "../components"
/*!
 *\brief Camera Multiple View
 *
 *\ingroup qml_desktop_default
 *
 * Camera Multi-view. See Screen_15 for more details
 */
Item{
    id:quad_view_cameras
    anchors.fill: parent

    Rectangle{
      anchors.fill: parent
        color: "transparent"
        Text {
            id: climatelabel
            x: 74
            y: 101
            text: "Quad View Cameras"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
            color:style.titletextcolor
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
