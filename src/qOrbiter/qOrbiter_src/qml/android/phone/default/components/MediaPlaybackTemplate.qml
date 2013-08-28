import QtQuick 1.1
import "../../lib/handlers"
Item {
    id:mediaPlaybackBase
    width: manager.appWidth
    height: manager.appHeight
    focus:false
    state:"controls"
    property Item controlSourceItem:AudioRemote{id:remote}
    property double conversionRate:metadataContainer.x/manager.appWidth
    onConversionRateChanged: console.log(conversionRate)

    Component.onCompleted:{
        manager.setBoundStatus(true)
        metadataContainer.forceActiveFocus()
    }


    Connections    {
        target: dcenowplaying
        onImageChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    TemplatePlaylist {
        id: templatePlaylist
        scale:conversionRate
        opacity: scale
    }

    Item{
        id:metadataContainer
        height: parent.height
        width: parent.width
        focus:true
        property int trigger: x+width/2

        Keys.onReleased: {
            switch(event.key){
            case Qt.Key_VolumeDown:
                manager.adjustVolume(-2)
                mediaPlaybackBase.state = "volume"
                break;

            case Qt.Key_VolumeUp:
                manager.adjustVolume(+2)
                mediaPlaybackBase.state = "volume"
                break;

            default:
                console.log("Unknown Key ==> "+ event.key)
            }
            event.accepted = true
        }

        onXChanged:{
            if (x >  width*.90 ){
                console.log(trigger)
                console.log("Playlist Mode")
                mediaPlaybackBase.state = "playlist"
            }else if ( x+width < ( width *.20) ){
                console.log("Advanced Mode")
            }
        }

        Column{
            id:md_layout
            height: scaleY(65)
            width: parent.width
            spacing:scaleY(4)

            NowPlayingRemoteImage {
                id: nowplayingimage
            }

            Loader{
                id:mediaMetadata
                source:"Metadata_"+manager.i_current_mediaType+".qml"
            }
        }

        MouseArea{
            id:md_drag
            anchors.fill: parent
            drag.target: metadataContainer
            drag.axis: Drag.XAxis
            onReleased: if(enabled){
                            mediaPlaybackBase.state="controls"
                        }

        }

        MediaScrollBar{id:media_scroller;
            anchors.top: metadataContainer.bottom
            anchors.horizontalCenter: metadataContainer.horizontalCenter
            anchors.topMargin: scaleY(2)
        }

        Loader{
            id:controlsTarget
            anchors.bottom: parent.bottom
            sourceComponent:AudioRemote{}
        }
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
                opacity:1
                scale:1
            }
            PropertyChanges {
                target: metadataContainer
                visible:false
                enabled:false
                focus:false
                 x:parent.x
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
            from: "dragging"
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
    HomeButton{
        id:home
        anchors.left:parent.left;
        anchors.top:parent.top
    }

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
                break;

            case Qt.Key_VolumeUp:
                manager.adjustVolume(+2)
                tripped=true
                break;

            default:
                console.log("Unknown Key ==> "+ event.key)
            }
            event.accepted = true
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
