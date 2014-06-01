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
        //    anchors{
        //        top:parent.top
        //        left:parent.left
        //        right:parent.right
        //        bottom:parent.Bottom
        //    }
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
                x:0
            }

            PropertyChanges{
                target:templatePlaylist
                scale:conversionRate
                opacity: scale
                enabled:false
            }

            PropertyChanges{
                target:volIndicator
                height: 0
                width: 0
                opacity:0
                visible:false
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
            PropertyChanges {
                target: metadataContainer
                //                visible:false
                enabled:false
                focus:false
                x:templatePlaylist.psPos+manager.appWidth
            }
        },
        State{
            name:"advanced"
            PropertyChanges {
                target: imgContainer
                visible:false
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
        },
        State{
            name:"dragging"
            when: md_drag.drag.active
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "controls"
            PropertyAnimation{
                target:metadataContainer
                property: "x"
                to:0
                duration: 350
                easing.type: Easing.InCurve
            }
        },
        Transition {
            from: "*"
            to: "controls"
            ScriptAction{
                script: metadataContainer.forceActiveFocus()
            }
        },
        Transition {
            from: "*"
            to: "volume"
            ScriptAction{
                script:volIndicator.forceActiveFocus()
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
