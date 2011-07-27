import QtQuick 1.0
import "../components"

Rectangle {
    height: style.orbiterH
    width: style.orbiterW
    color: style.advanced_bg

    HomeButton{}

    Flow {
        id: flow1
        anchors.centerIn: parent
        height: parent.height -50
        width: parent.width -50

        ButtonSq{
            id:regenorbiter
            height: 60
            width: 60
            buttontext: "Regen Orbiter"

            MouseArea{
                anchors.fill:parent
                z:5
                onClicked: regenOrbiter(35)
            }
        }

        ButtonSq{
            id:pending
            height: 60
            width: 60
            buttontext: "Pending Tasks"
        }

        ButtonSq{
            id:networksettings
            height: 60
            width: 60
            buttontext: "Network Settings"
        }

        ButtonSq{
            id:avwizard
            height: 60
            width: 60
            buttontext: "Av wizard"
        }

        ButtonSq{
            id:regenorbiters
            height: 60
            width: 60
            buttontext: "Regen All Orbiters"
        }

        ButtonSq{
            id:quickreload
            height: 60
            width: 60
            buttontext: "Quick Reload"
        }



    }

}
