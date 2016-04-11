import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item {
    id:mediaPlaybackBase

    anchors.fill: parent
    focus:true


    Component.onCompleted:{
        state="metadata"
        manager.setBoundStatus(true)
        metadataContainer.forceActiveFocus()
        setNavigation("MediaControl.qml")
    }

    TemplatePlaylist {
        id: templatePlaylist       
        width: parent.width
        anchors{
            left:mediaPlaybackBase.right
            bottom:parent.bottom
            top:parent.top
        }
        opacity: 0
        scale:0
    }

    MetadataContainer {
        id: metadataContainer
        height: parent.height
        width: parent.width
        anchors{
            left:mediaPlaybackBase.right
            bottom:parent.bottom
        }
        scale:0
        opacity:0
    }

    AdvancedMediaOptions{
        id:advanced_options
        anchors{
            left:mediaPlaybackBase.right
            bottom:parent.bottom
        }
        scale:0
        opacity:0
    }

    Item{
        id:buttonList
    }

    states: [


        State {
            name: "metadata"
            extend: ""
            PropertyChanges {
                target: metadataContainer
                opacity:1
                scale:1
            }
            AnchorChanges{
                target: metadataContainer
                anchors.left: mediaPlaybackBase.left
            }
        },
        State{
            name:"playlist"
            extend:""

            PropertyChanges{
                target:templatePlaylist
                scale:1
                opacity: 1
            }

            AnchorChanges{
                target:templatePlaylist
                anchors.left: mediaPlaybackBase.left
            }
        },
        State{
            name:"advanced"
            extend: ""
            AnchorChanges{
                target: advanced_options
                anchors{
                    left:mediaPlaybackBase.left
                }
            }
            PropertyChanges{
                target: advanced_options
                opacity:1
                scale:1
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
        visible:false
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
