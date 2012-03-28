import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    height: appH
    width: appW
    color: "transparent"
    id: advancedscreen

    HomeButton{}

    Flow {
        id: flow1
        anchors.centerIn: parent
        height: scaleY(85)
        width: scaleX(85)
        spacing: 10

        ButtonSq{
            id:regenorbiter
            height: style.buttonH
            width: style.buttonW
            buttontext: "Regen Orbiter"
            radius: 5

            MouseArea{
                anchors.fill:parent
                z:5
                onClicked: regenOrbiter(35)
            }
        }

        ButtonSq{
            id:pending
            height: style.buttonH
            width: style.buttonW
            buttontext: "Pending Tasks"
            radius: 5
        }

        ButtonSq{
            id:networksettings
            height: style.buttonH
            width: style.buttonW
            buttontext: "Network Settings"
            radius: 5
        }

        ButtonSq{
            id:avwizard
            height: style.buttonH
            width: style.buttonW
            buttontext: "Av wizard"
            radius: 5
        }

        ButtonSq{
            id:regenorbiters
            height: style.buttonH
            width: style.buttonW
            buttontext: "Regen All Orbiters"
            radius: 5
        }

        ButtonSq{
            id:quickreload
            height: style.buttonH
            width: style.buttonW
            buttontext: "Quick Reload"
            radius: 5
            MouseArea{
                anchors.fill: parent
                z:10
                onClicked: dcerouter.quickReload()
            }
        }

        ButtonSq{
            id:changeStyle
            height: style.buttonH
            width: style.buttonW
            buttontext: "Change Styles"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked: MyJs.createComponentObjects("SkinSelector.qml")
            }
        }



    }

}
