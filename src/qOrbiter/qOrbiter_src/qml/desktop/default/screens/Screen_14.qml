import QtQuick 1.1
import "../components"
/*!
 *\brief LABEL ME
 *
 *\ingroup qml_desktop_default
 *\bug needs better description
 *This screen pops up with the selected security status after a mode change.
 */
Item{
    id:securitystatus

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        Text {
            id: statussec
            x: 74
            y: 101
            text: "Security Status"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
