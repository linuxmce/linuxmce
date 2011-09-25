import QtQuick 1.0
import "../components"

    Rectangle{
    id:securitystatus
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        Image {
            id: bg
            source: "../../../img/icons/orbiterbg.png"
        }

        ShadowRect{
            id:mainrect
            rectheight: scaleY(55)
            rectwidth: scaleX(55)
            rectcolor: style.lighthighlight
            anchors.centerIn: securitystatus
            Text {
                id: statussec
               anchors.centerIn: mainrect
                text: "Mode Changed"
                font.family: "Droid Sans"
                font.bold: false
                font.pointSize: 18
            }
            HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
        }


    }


