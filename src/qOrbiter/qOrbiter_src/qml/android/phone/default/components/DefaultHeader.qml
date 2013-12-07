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

    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .75
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
        width: manager.b_orientation ? scaleX(6) : scaleX(8)

        anchors{
            right: parent.right
            top:parent.top
            bottom:parent.bottom
        }
        Column{
            id:optionCol
            spacing:2
            anchors.fill: parent
            Rectangle{
                color: "green"
                height: hdr.height /4
                width: height
            }
            Rectangle{
                color: "green"
                height: hdr.height /4
                width: height
            }
            Rectangle{
                color: "green"
                height: hdr.height /4
                width: height
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
        }
    ]

}
