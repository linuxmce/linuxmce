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

        MouseArea{
            width: stage.width
            height: stage.height

            onEntered: {
                // HIDE SUBS
                subClimate.visible=false
                subLightning.visible=false
                subMedia.visible=false
                subSecurity.visible=false
                subTelecom.visible=false

                // HIDE ARROWS
                arrowClimate.visible=false
                arrowLights.visible=false
                arrowMedia.visible=false
                arrowSecurity.visible=false
                arrowTelecom.visible=false
            }
        }

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


        // SUBMENU ITEMS
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

                    Image {
                        id: arrowMedia
                        visible: false
                        source: "../images/arrowDown.png"
                        height: Image.height
                        width: Image.width
                        anchors.bottom: btMedia.top
                        anchors.horizontalCenter: btMedia.horizontalCenter
                    }

                    MouseArea{
                        id: mediaMouse
                        anchors.fill: parent

                        onPressed: {

                            // HIDE SUBS
                            subClimate.visible=false
                            subLightning.visible=false
                            subSecurity.visible=false
                            subTelecom.visible=false

                            // HIDE ARROWS
                            arrowClimate.visible=false
                            arrowLights.visible=false
                            arrowSecurity.visible=false
                            arrowTelecom.visible=false

                            arrowMedia.visible=true
                            subMedia.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id:btLights
                    currentImage: "../images/btLights.png"
                    currentText: "Lights"

                    Image {
                        id: arrowLights
                        visible: false
                        source: "../images/arrowDown.png"
                        height: Image.height
                        width: Image.width
                        anchors.bottom: btLights.top
                        anchors.horizontalCenter: btLights.horizontalCenter
                    }

                    MouseArea{
                        id: mouseLightning
                        anchors.fill: parent

                        onPressed: {

                            // HIDE SUBS
                            subClimate.visible=false
                            subMedia.visible=false
                            subSecurity.visible=false
                            subTelecom.visible=false

                            // HIDE ARROWS
                            arrowClimate.visible=false
                            arrowMedia.visible=false
                            arrowSecurity.visible=false
                            arrowTelecom.visible=false

                            arrowLights.visible=true
                            subLightning.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id: btClimate
                    currentImage: "../images/btClimate.png"
                    currentText: "Climate"

                    Image {
                        id: arrowClimate
                        visible: false
                        source: "../images/arrowDown.png"
                        height: Image.height
                        width: Image.width
                        anchors.bottom: btClimate.top
                        anchors.horizontalCenter: btClimate.horizontalCenter
                    }

                    MouseArea{
                        id: mouseClimate
                        anchors.fill: parent

                        onPressed: {

                            // HIDE SUBS
                            subLightning.visible=false
                            subMedia.visible=false
                            subSecurity.visible=false
                            subTelecom.visible=false

                            // HIDE ARROWS
                            arrowLights.visible=false
                            arrowMedia.visible=false
                            arrowSecurity.visible=false
                            arrowTelecom.visible=false

                            arrowClimate.visible=true
                            subClimate.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id: btTelecom
                    currentImage: "../images/btTelecom.png"
                    currentText: "Telecom"

                    Image {
                        id: arrowTelecom
                        visible: false
                        source: "../images/arrowDown.png"
                        height: Image.height
                        width: Image.width
                        anchors.bottom: btTelecom.top
                        anchors.horizontalCenter: btTelecom.horizontalCenter
                    }

                    MouseArea{
                        id: mouseTelecom
                        anchors.fill: parent

                        onPressed: {

                            // HIDE SUBS
                            subClimate.visible=false
                            subLightning.visible=false
                            subMedia.visible=false
                            subSecurity.visible=false

                            // HIDE ARROWS
                            arrowClimate.visible=false
                            arrowLights.visible=false
                            arrowMedia.visible=false
                            arrowSecurity.visible=false

                            arrowTelecom.visible=true
                            subTelecom.visible=true
                        }
                    }
                }
                HomeBtDefault {
                    id: btSecurity
                    currentImage: "../images/btSecurity.png"
                    currentText: "Security"

                    Image {
                        id: arrowSecurity
                        visible: false
                        source: "../images/arrowDown.png"
                        height: Image.height
                        width: Image.width
                        anchors.bottom: btSecurity.top
                        anchors.horizontalCenter: btSecurity.horizontalCenter
                    }

                    MouseArea{
                        id: mouseSecurity
                        anchors.fill: parent

                        onPressed: {

                            // HIDE SUBS
                            subClimate.visible=false
                            subLightning.visible=false
                            subMedia.visible=false
                            subTelecom.visible=false

                            // HIDE ARROWS
                            arrowClimate.visible=false
                            arrowLights.visible=false
                            arrowMedia.visible=false
                            arrowTelecom.visible=false

                            arrowSecurity.visible=true
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
                        }
                    }
                }
            }
        }







        BottomPanel{id: advanced; anchors.bottom: maindisplay.bottom}
    }
}
