import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item {
    id:mediaPlaybackBase

    anchors.fill: parent
    focus:true
    state:"controls"

    property double conversionRate:metadataContainer.x/manager.appWidth

    Component.onCompleted:{
        manager.setBoundStatus(true)
        metadataContainer.forceActiveFocus()     
        setNavigation("MediaControl.qml")
    }


    TemplatePlaylist {
        id: templatePlaylist
        scale:conversionRate
        opacity: scale
    }

    MetadataContainer {
        id: metadataContainer
        height: parent.height
        width: parent.width
    }

    AdvancedMediaOptions{
        id:advanced_options
    }

    Item{
        id:buttonList
    }

    states: [


        State {
            name: "controls"
            PropertyChanges {
                target: metadataContainer
                visible:true
                focus:true              
            }

            PropertyChanges{
                target:templatePlaylist
                scale:conversionRate
                opacity: scale
                enabled:false
            }

            PropertyChanges{
                target:volIndicator
                visible:false
            }

            AnchorChanges{
                target: metadataContainer
                anchors{
                    top:parent.top
                    left:parent.left

                }
            }
        },
        State{
            name:"playlist"
            PropertyChanges{
                target:templatePlaylist
                spacetracker:1
                scale:conversionRate
                opacity: scale
                psPos:0

            }

            AnchorChanges{
                target:metadataContainer
                anchors{
                    left:parent.right
                }
            }
        },
        State{
            name:"advanced"
            AnchorChanges{
                target: metadataContainer
                anchors{
                    left:undefined
                    right:parent.left
                }
            }
        },
        State {
            name: "lighting"
        },
        State {
            name: "volume"
            PropertyChanges{
                target:volIndicator
                height: parent.height
                width: parent.width
                visible:true
                opacity:1
            }
        }
    ]




    Item{
        id:volIndicator
        anchors.centerIn: parent
        focus:true
        clip:true
        property bool tripped: false
        Keys.onReleased: {
            switch(event.key){
            case Qt.Key_VolumeDown:
                manager.adjustVolume(-2)
                tripped=true
                event.accepted = true
                break;

            case Qt.Key_VolumeUp:
                manager.adjustVolume(+2)
                tripped=true
                event.accepted = true
                break;

            default:
                console.log("Unknown Key ==> "+ event.key)
            }

            closeTimer.restart()
        }

        MouseArea{
            anchors.fill: parent
            onClicked: mediaPlaybackBase.state = "controls"
        }
        Timer{
            id:closeTimer
            running:parent.activeFocus
            interval: 5000
            onTriggered: mediaPlaybackBase.state = "controls"
        }

        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity: .85
        }

        Rectangle{
            id:indicator
            height: scaleY(35)
            width: height
            anchors.centerIn: parent
            color:volIndicator.tripped ? androidSystem.blueStandard: androidSystem.orangeStandard
            border.color: "white"
            border.width: 2
            radius:15

            Timer{
                id:untrip
                interval:500
                running:volIndicator.tripped
                onTriggered: volIndicator.tripped = false
            }
        }
    }
}
