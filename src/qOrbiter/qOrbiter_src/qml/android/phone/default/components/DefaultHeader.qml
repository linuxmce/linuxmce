import QtQuick 1.1
//phone - default
Item{
    id:hdr
    anchors{
        left:parent.left
        top:parent.top
        right:parent.right
    }
    height: manager.b_orientation ? scaleY(8) : scaleY(12)

    function setNavigaton(qmlpage){
        hdrContent.source=qmlpage
    }
    Rectangle{
        anchors.fill: parent
        color:skinStyle.headerBgColor
        opacity: skinStyle.bgContentOpacity
    }
    Rectangle{
        anchors.fill: parent
        gradient: skinStyle.bgHeaderGradient
        opacity: skinStyle.bgHeavyOpacity
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
                color: skinStyle.darkhighlight
                height: hdr.height /5
                width: height
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Rectangle{
                color: skinStyle.darkhighlight
                height: hdr.height /5
                anchors.horizontalCenter: parent.horizontalCenter
                width: height
            }
            Rectangle{
                color: skinStyle.darkhighlight
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
            when:manager.currentScreen==="Screen_1.qml"
            PropertyChanges {
                target: hdr
                height: manager.b_orientation ? scaleY(8) : scaleY(12)
            }
            PropertyChanges{
                target:hdrContent
                source:"Clock.qml"
            }
        },
        State {
            name: "closed"
            PropertyChanges {
                target: hdr
                height:0
            }

        },
        State {
            name: "grid"
            PropertyChanges{
                target:hdrContent
                source:"MediaGridHeader.qml"
            }
        },
        State {
            name: "floorplan"
            when:hdrContent.source==="FloorplanNav.qml"
        },
        State {
            name: "tv"
            PropertyChanges{
                target:hdrContent
                source:"MediaGridHeader.qml"
            }

        }
    ]

}
