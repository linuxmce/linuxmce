import QtQuick 1.0
import "../components"
Item{
    id:reloadrouter

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: style.maincolor
        Rectangle{
            id:reloadroutercontainer
            height: scaleY(50)
            width:scaleY(25)
            anchors.centerIn: parent

            Column{
                id:buttoncolumn
                anchors.fill: parent

                Text {
                    id: reloadrouterlabel
                    x: 74
                    y: 101
                    text: "Reload Router"
                    font.family: "Droid Sans"
                    font.bold: false
                    font.pointSize: 15
                    color:style.titletextcolor
                }
            }

        }


        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
