import QtQuick 1.0
import com.nokia.android 1.1
//import "../androidComponents"
import "../components"


Item
{
    anchors.centerIn: parent

    Rectangle {
        id:stage

        signal swapStyle()
        height: appH
        width: appW
        color:"transparent"

        Rectangle {
            id: headerbg
            color: "#000000"
            anchors.top: stage.top
            height: scaleY(7)
            width: scaleX(100)
            opacity: 0.800
        }

        Text{
            id:connectstatus
            text: "Orbiter ID: "+ deviceid
            color: "aliceblue"
            font.pointSize: 14;
            anchors.left: parent.left
            anchors.leftMargin: scaleX(2)
            height: headerbg.height
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        Date{
            id:screen1date
            anchors.right: parent.right
            anchors.rightMargin: scaleX(2)
        }
        Clock{
            id:screen1time
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Rectangle{
            id:subMenu
        //    transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 15 }
            clip:true
            color:"transparent"

            width: scaleX(97)
            height: 100
            anchors.bottom: mainButtonContainer.top
            anchors.bottomMargin:  16
            anchors.horizontalCenter: parent.horizontalCenter

            HomeClimateRow {
                id: subClimate
            }
            HomeLightingRow {
                id: subLightning
            }
            HomeMediaRow {
                id: subMedia
            }
            HomeSecurityRow {
                id: subSecurity
            }
            HomeTelecomRow {
                id: subTelecom
            }
        }

        Rectangle{
            id: mainButtonContainer
            width: scaleX(97)
            height: style.btHomeDefaultH
            color: "transparent"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: advanced.height + scaleY(1)
            anchors.horizontalCenter: parent.horizontalCenter

            Flow {
                id: btFloatMain
                anchors.fill: parent
                anchors.margins: 0
                spacing: style.btHomeDefaultSpace                

                HomeBtDefault {
                    id: btMedia
                    currentImage: "../images/btMedia.png"
                    currentText: "Media"
                    MouseArea{
                        id: mediaMouse
                        anchors.fill: parent

                        onPressed: {
                            for (var i = 0; i < subMenu.children.length; ++i)
                            subMenu.children[i].visible = false;
                            subMedia.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id:btLights
                    currentImage: "../images/btLights.png"
                    currentText: "Lights"
                    MouseArea{
                        id: mouseLightning
                        anchors.fill: parent

                        onPressed: {
                            for (var i = 0; i < subMenu.children.length; ++i)
                            subMenu.children[i].visible = false;
                            subLightning.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id: btClimate
                    currentImage: "../images/btClimate.png"
                    currentText: "Climate"
                    MouseArea{
                        id: mouseCLimate
                        anchors.fill: parent

                        onPressed: {
                            for (var i = 0; i < subMenu.children.length; ++i)
                            subMenu.children[i].visible = false;
                            subClimate.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id: btTelecom
                    currentImage: "../images/btTelecom.png"
                    currentText: "Telecom"
                    MouseArea{
                        id: mouseTelecom
                        anchors.fill: parent

                        onPressed: {
                            for (var i = 0; i < subMenu.children.length; ++i)
                            subMenu.children[i].visible = false;
                            subTelecom.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id: btSecurity
                    currentImage: "../images/btSecurity.png"
                    currentText: "Security"
                    MouseArea{
                        id: mouseSecurity
                        anchors.fill: parent

                        onPressed: {
                            for (var i = 0; i < subMenu.children.length; ++i)
                            subMenu.children[i].visible = false;
                            subSecurity.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id: btSettings
                    currentImage: "../images/btSettings.png"
                    currentText: "Settings"
                    MouseArea{
                        id: mouseSettings
                        anchors.fill: parent

                        onPressed: {
                            for (var i = 0; i < subMenu.children.length; ++i)
                            subMenu.children[i].visible = false;
                        }
                    }
                }
            }
        }







        BottomPanel{id: advanced; anchors.bottom: maindisplay.bottom}
    }
}
