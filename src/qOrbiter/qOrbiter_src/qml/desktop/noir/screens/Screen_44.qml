import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs

Rectangle {
    height: appH
    width: appW
    color: "transparent"
    id: advancedscreen

    HomeButton{}

    Rectangle{
        height: scaleY(75)
        width: scaleX(75)
        color: style.darkhighlight
        radius: 5
        border.color: style.highlight1
        border.width: 2
        anchors.centerIn: parent
        opacity: .85

        Flow {
            id: flow1
            anchors.centerIn: parent
            height: scaleY(50)
            width: scaleX(60)
            spacing: 10

            AdvancedButton{
                id:regenorbiter
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Regen This Orbiter")
                radius: 5
                MouseArea{
                    anchors.fill:parent
                    z:5
                    onClicked: manager.regenOrbiter(35)
                }
            }

            AdvancedButton{
                id:regenorbiters
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Regen All Orbiters")
                radius: 5
            }

            AdvancedButton{
                id:reloadandregen
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Reload and Regen")
                radius: 5
            }

            AdvancedButton{
                id:quickreload
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Quick Reload")
                radius: 5
                MouseArea{
                    anchors.fill: parent
                    z:10
                    onClicked: manager.quickReload()
                }
            }

            AdvancedButton{
                id:networksettings
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Network Settings")
                radius: 5
            }

            AdvancedButton{
                id:avwizard
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Av wizard")
                radius: 5
            }

            AdvancedButton{
                id:pendingtasks
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Pending Tasks")
                radius: 5
            }

            AdvancedButton{
                id:remoteassistance
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Remote Assistance")
                radius: 5
                MouseArea{
                    anchors.fill: parent
                    z:10
                }
            }

            AdvancedButton{
                id:addsoftware
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Add Software")
                radius: 5
            }

            AdvancedButton{
                id:changeStyle
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Change Styles")
                radius: 5
                MouseArea{
                    anchors.fill: parent
                    onClicked: MyJs.createStageComponent("SkinSelector.qml", advancedscreen)
                }
            }

            AdvancedButton{
                id:gc100
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Add GC100")
                radius: 5
            }

            AdvancedButton{
                id:pingtest
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("Ping Test")
                radius: 5
            }

            AdvancedButton{
                id:ircodes
                height: style.buttonH
                width: style.buttonW
                buttontext: qsTr("IR Codes")
                radius: 5
            }
        }
    }
}

