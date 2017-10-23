import QtQuick 1.1
import org.linuxmce.enums 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs

Rectangle {
    height: manager.appHeight
    width: manager.appWidth
    color: "transparent"
    id: advancedscreen

    HomeButton{}


    Rectangle{
        height: scaleY(75)
        width: scaleX(75)
        color: "slategrey"
        radius: 5
        border.color: skinstyle.highlight1
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
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Regen This Orbiter")
                radius: 5
                MouseArea{
                    anchors.fill:parent
                    z:5
                    onClicked: manager.setCurrentScreen("Regenerating.qml")
                }
            }

            AdvancedButton{
                id:regenorbiters
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Regen All Orbiters")
                radius: 5
            }

            AdvancedButton{
                id:reloadandregen
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Reload and Regen")
                radius: 5
            }

            AdvancedButton{
                id:quickreload
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
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
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Network Settings")
                radius: 5
            }

            AdvancedButton{
                id:avwizard
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Av wizard")
                radius: 5
            }

            AdvancedButton{
                id:pendingtasks
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Pending Tasks")
                radius: 5
            }

            AdvancedButton{
                id:remoteassistance
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Remote Assistance")
                radius: 5
                MouseArea{
                    anchors.fill: parent
                    z:10
                }
            }

            AdvancedButton{
                id:addsoftware
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Add Software")
                radius: 5
            }

            AdvancedButton{
                id:changeskinStyle
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Change skinStyles")
                radius: 5
                MouseArea{
                    anchors.fill: parent
                    onClicked: MyJs.createStageComponent("SkinSelector.qml", advancedscreen)
                }
            }

            AdvancedButton{
                id:gc100
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Add GC100")
                radius: 5
            }

            AdvancedButton{
                id:pingtest
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Ping Test")
                radius: 5
            }

            AdvancedButton{
                id:ircodes
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("IR Codes")
                radius: 5
            }

            AdvancedButton{
                id:qorbiterConfig
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Local Config for this \n"+manager.getHostDeviceName())
                radius: 5
                MouseArea{
                    id:showConfig
                    anchors.fill: parent
                    onClicked: localConfigPanel.state="showing"
                }
            }

            AdvancedButton{
                id:componentDisplay
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("Clear Component \nCache")
                radius: 5
                MouseArea{
                    id:showComponents
                    anchors.fill: parent
                    onClicked: manager.clearSkinCache()
                }
            }

            AdvancedButton{
                id:html5
                height: scaleY(skinstyle.buttonH)
                width: scaleX(skinstyle.buttonW)
                buttontext: qsTr("HTML 5 video test")
                radius: 5
                MouseArea{
                    id:html5test
                    anchors.fill: parent
                    onClicked: htmltestsurface.visible = !htmltestsurface.visible
                }
            }
        }
    }
    ConfigPanel{
        id:localConfigPanel
    }

}

