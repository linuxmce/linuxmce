import QtQuick 1.1
import "../components"
/*!
 *
 *\brief Call from Favorites screen.
 *
 *\ingroup qml_desktop_default
 *Dial a call from the favorites list.
 */
Item{
    id:callfromfavorites

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        Text {
            id: favcall
            x: 74
            y: 101
            text: "Make Call From Favorites"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
