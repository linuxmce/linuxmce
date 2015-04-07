import QtQuick 2.0
import "../components"
import "../js/ComponentLoader.js" as MyJs

Rectangle {
    id: storedaudioremote
    Component.onCompleted: manager.setBoundStatus(true)
    height: manager.appHeight
    width: manager.appWidth
    radius: 0
    opacity: 1
    color: "transparent"
    focus:false
    Keys.onTabPressed: {shiftFocus(); console.log("Tab to next back to scenarios?") }
    onActiveFocusChanged: console.log(dcenowplaying.qs_screen+ " focus::"+focus)
    FocusScope{
        width: parent.width
        height: parent.height
        anchors.centerIn: parent

        Connections{
            target:dcenowplaying
            onImageChanged:nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        }
        //main 'now playing rect containing all the other items


        Column{
            anchors.right: storedaudioremote.right
            anchors.rightMargin: scaleX(1)
            Remote_lighting_controls{ id: remote_lighting_controls1; }
            Remote_Audio_controls{ id: remote1; }
        }

        Rectangle{
            id:contentDisplay
            width: parent.width
            height: scaleY(35)
            anchors.verticalCenter: parent.verticalCenter
            color: "transparent"

            Rectangle{
                id:opacityMask
                anchors.fill: parent
                color: "black"
                opacity: .25
            }

            Image {
                id: nowplayingimage
                width: dcenowplaying.aspect=="wide"? scaleX(18) : scaleX(18)
                height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(30)
                source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
                anchors.horizontalCenter: parent.horizontalCenter
                transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 30 }
                anchors.verticalCenter: parent.verticalCenter

                smooth: true
                Image {
                    id: npmask
                    source: "../img/external/icons/transparencymask.png"
                    anchors.fill: nowplayingimage
                    opacity: .5
                }
            }

            AudioMetadataDisplay {
                id:textMetadata
                anchors.left: nowplayingimage.right
                anchors.rightMargin: scaleX(5)
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // MediaScrollBar{id:media_transit; anchors.bottom: controlrow.top; anchors.horizontalCenter: controlrow.horizontalCenter; anchors.bottomMargin: scaleY(2)}
        AudioButtonControls {
            id: controlrow
            focus: true

        }
    }


}

