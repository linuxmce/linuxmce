import QtQuick 1.1
//phone - default
Item{
    id:hdr
    anchors{
        left:parent.left
        top:parent.top
        right:parent.right
    }

    onStateChanged: console.log("Header State::"+state)
    height: manager.isProfile ? scaleY(8) : scaleY(12)

    function setNavigaton(qmlpage){
        hdrContent.source=qmlpage
    }
    Rectangle{
        anchors.fill: parent
        color:appstyle.headerBgColor
        opacity: appstyle.bgContentOpacity
    }
    Rectangle{
        anchors.fill: parent
        gradient: appstyle.bgHeaderGradient
        opacity: appstyle.bgHeavyOpacity
    }


    Row{
        id:hdrLayout
        anchors{
            left:parent.left
            right:optionContainer.left
            top:parent.top
            bottom:parent.bottom
        }

        Loader{
            id:hdrContent
            anchors{
                left:parent.left
                top:parent.top
                right:parent.right
                bottom:parent.bottom
            }
            source: "Clock.qml"

        }
    }

    Item{
        id:optionContainer
        width: parent.height

        anchors{
            right: parent.right
            top:parent.top
            bottom:parent.bottom
        }
        Column{
            id:optionCol
            spacing:2
            anchors.fill: parent
            anchors.verticalCenter: parent.verticalCenter

            Rectangle{
                color: appstyle.darkhighlight
                height: hdr.height /5
                width: height
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Rectangle{
                color: appstyle.darkhighlight
                height: hdr.height /5
                anchors.horizontalCenter: parent.horizontalCenter
                width: height
            }
            Rectangle{
                color: appstyle.darkhighlight
                height: hdr.height /5
                width: height
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        MouseArea{
            anchors.fill: parent
            onReleased: showOptions =! showOptions
        }
    }

    states: [
        State {
            name: "open"
            extend: "default"
            when:manager.currentScreen==="Screen_1.qml" && uiOn
            PropertyChanges {
                target: hdr
                height: manager.isProfile ? scaleY(8) : scaleY(12)
            }
            PropertyChanges{
                target:hdrContent
                source:"Clock.qml"
            }
        },
        State {
            name: "closed"
            extend:"default"
            PropertyChanges {
                target: hdr
                height:0
            }

        },
        State {
            name: "screen_saver"

            when:!uiOn
            AnchorChanges{
                target:hdr
                anchors{
                    top:undefined
                    bottom:qmlroot.top
                }
            }
        },
        State {
            name: "grid"
            extend:"default"
            PropertyChanges{
                target:hdrContent
                source:"MediaGridHeader.qml"
            }
        },
        State {
            name: "floorplan"
            extend:"default"
            when:hdrContent.source==="FloorplanNav.qml"
        },
        State {
            name: "tv"
            extend:"default"
            PropertyChanges{
                target:hdrContent
                source:"MediaGridHeader.qml"
            }

        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration:appstyle.animation_quick
                easing:appstyle.animation_easing
            }
        }
    ]

}
