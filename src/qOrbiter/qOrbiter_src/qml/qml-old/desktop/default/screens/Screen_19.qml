/*!
 *\brief File Save screen
 *
 *\ingroup qml_desktop_default
 *\bug needs better description.
 *Save file screen, need more description
 */
import QtQuick 1.1
import "../components"
Item{
    id:file_save
    anchors.fill: parent
    Rectangle{
       anchors.fill: parent
        color: "transparent"
        Text {
            id: statussec
            x: 74
            y: 101
            text: "File Save"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
