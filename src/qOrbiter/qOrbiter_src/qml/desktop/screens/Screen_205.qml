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

               ButtonSq{
                   id:confirm
                   height: style.stdbuttonh
                   width:style.stdbuttonw
                   buttontext: "Reload Router Now"

               }

               ButtonSq{
                   id:deny
                   height: style.stdbuttonh
                   width:style.stdbuttonw
                   buttontext: "Reload Later"

               }
            }

        }


        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
