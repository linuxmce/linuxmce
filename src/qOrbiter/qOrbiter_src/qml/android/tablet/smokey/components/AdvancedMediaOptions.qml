import QtQuick 1.0
Item{
    id:controlPanel
    height: parent.height
    width: parent.width
    clip:true
    anchors.right: metaDataPanel.left
    state:"unloaded"
    MouseArea{
        anchors.fill: parent
        onReleased: media_playback_base.state="metadata"
    }

    Rectangle{
        id:fil
        anchors.fill: parent
        color: "darkgreen"
    }

    Rectangle{
        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
            leftMargin: 10
        }
        width: scaleX(15)

        ListView{
            id:optionView
            anchors.fill: parent
            model:availbleOptions
            delegate: StyledButton{
                buttonText: name
                anchors{
                    left:parent.left
                    right:parent.right
                }
                height:scaleY(20)
            }
        }
    }


    ListModel{
        id:availbleOptions

        ListElement{
            name:"Zoom & Aspect"
            control:"zm"
        }
        ListElement{
            name:"Bookmarks"
            control:"bm"
        }

        ListElement{
            name:"Manage Playlist"
            control:"pl"
        }

        ListElement{
            name:"Thumbnail"
            control:"tm"
        }
        ListElement{
            name:"Jog"
            control:"jg"
        }
    }

    states: [
        State {
            name: "unloaded"
            PropertyChanges {
                target: fil
                opacity:0

            }
        },
        State {
            name: "loaded"
            PropertyChanges {
                target: fil
                opacity:1
            }
        }
    ]
}
