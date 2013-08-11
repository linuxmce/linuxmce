import QtQuick 1.0
import QtWebKit 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs

Item {
    height: manager.appHeight
    width: manager.appWidth
    id: advancedscreen

    HomeButton{}


    Item{
        height: scaleY(75)
        width: scaleX(95)
        anchors.centerIn: parent
        Rectangle{
            id:phil
            color: "darkblue"
            radius: 5

            anchors.fill: parent
            border.color: style.highlight1
            border.width: 2
            anchors.centerIn: parent
            opacity: .45

        }

        Flow {
            id: flow1
            anchors.centerIn: parent
            height: scaleY(50)
            width: scaleX(60)
            spacing: 10

            AdvancedButton{
                id:regenorbiter
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Regen This Orbiter")
                buttonsqradius: 5
                MouseArea{
                    anchors.fill:parent
                    z:5
                    onClicked: manager.regenOrbiter(35)
                }
            }

            AdvancedButton{
                id:regenorbiters
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Regen All Orbiters")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:reloadandregen
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Reload and Regen")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:quickreload
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Quick Reload")
                buttonsqradius: 5
                MouseArea{
                    anchors.fill: parent
                    z:10
                    onClicked: manager.quickReload()
                }
            }

            AdvancedButton{
                id:networksettings
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Network Settings")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:avwizard
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Av wizard")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:pendingtasks
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Pending Tasks")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:remoteassistance
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Remote Assistance")
                buttonsqradius: 5
                MouseArea{
                    anchors.fill: parent
                    z:10
                }
            }

            AdvancedButton{
                id:addsoftware
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Add Software")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:changeStyle
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Change Styles")
                buttonsqradius: 5
                MouseArea{
                    anchors.fill: parent
                    onClicked: MyJs.createStageComponent("SkinSelector.qml", advancedscreen)
                }
            }

            AdvancedButton{
                id:gc100
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Add GC100")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:pingtest
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Ping Test")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:ircodes
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("IR Codes")
                buttonsqradius: 5
            }

            AdvancedButton{
                id:qorbiterConfig
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("Local Config")
                buttonsqradius: 5
                MouseArea{
                    id:showConfig
                    anchors.fill: parent
                    onClicked: localConfigPanel.state="showing"
                }
            }

            AdvancedButton{
                id:html5
               height: style.stdbuttonh
               width: style.stdbuttonw
                buttontext: qsTr("HTML 5 video test")
                buttonsqradius: 5
                MouseArea{
                    id:html5test
                    anchors.fill: parent
                    onClicked: htmltestsurface.visible = !htmltestsurface.visible
                }
            }
        }

    }
    //    ConfigPanel{
    //        id:localConfigPanel
    //    }

    WebView{
        id:htmltestsurface
        anchors.top: parent.top
        height: 300
        width: parent.width
        url: "http://www.w3schools.com/html/tryit.asp?filename=tryhtml5_video_all"
        visible: false

    }
}

