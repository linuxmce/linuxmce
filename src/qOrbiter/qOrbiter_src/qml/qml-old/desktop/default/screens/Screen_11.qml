import QtQuick 1.1
import "../components"
/*!
 *\brief Make call / Dial Number
 *
 *\ingroup qml_desktop_default
 *Make a call, dial a number from on screen pad or other hardware input.
 */
Item{
    id:calldialnumber

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        Text {
            id: dialcall
            x: 74
            y: 101
            text: "Make Call Dial Number"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
