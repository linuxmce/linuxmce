import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs

Rectangle {
    height: style.orbiterH
    width: style.orbiterW
    color: "transparent"
    id: advancedscreen

    HomeButton{}

    Flow {
        id: flow1
        anchors.centerIn: parent
        height: 400
        width: 400
        spacing: 10

        ButtonSq{
            id:regenorbiter
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Regen Orbiter"
            radius: 5

            MouseArea{
                anchors.fill:parent
                z:5
                onClicked: regenOrbiter(35)
            }
        }

        ButtonSq{
            id:regenorbiters
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Regen All Orbiters"
            radius: 5
        }

        ButtonSq{
            id:pending
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Pending Tasks"
            radius: 5
        }

        ButtonSq{
            id:networksettings
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Network Settings"
            radius: 5
        }

        ButtonSq{
            id:avwizard
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Av wizard"
            radius: 5
        }



        ButtonSq{
            id:quickreload
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Quick Reload"
            radius: 5
            MouseArea{
                anchors.fill: parent
                z:10
                onClicked: quickReload()
            }
        }

        ButtonSq{
            id:remoteassistance
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Remote Assistance"
            radius: 5
            MouseArea{
                anchors.fill: parent
                z:10
                onClicked: quickReload()
            }
        }

        ButtonSq{
            id:addsoftware
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Add Software"
            radius: 5
        }

        ButtonSq{
            id:changeStyle
            height: scaleY(style.buttonH)
            width: scaleX(style.buttonW)
            buttontext: "Change Styles"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked: MyJs.createComponentObjects("SkinSelector.qml")
            }
        }



    }

}
