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
                }
                HomeBtDefault {
                    id:btLights
                    currentImage: "../images/btLights.png"
                    currentText: "Lights"
                }
                HomeBtDefault {
                    id: btClimate
                    currentImage: "../images/btClimate.png"
                    currentText: "Climate"
                }
                HomeBtDefault {
                    id: btTelecom
                    currentImage: "../images/btTelecom.png"
                    currentText: "Telecom"
                }
                HomeBtDefault {
                    id: btSecurity
                    currentImage: "../images/btSecurity.png"
                    currentText: "Security"
                }
                HomeBtDefault {
                    id: btSettings
                    currentImage: "../images/btSettings.png"
                    currentText: "Settings"
                }
            }
        }

        BottomPanel{id: advanced; anchors.bottom: maindisplay.bottom}
    }
}
