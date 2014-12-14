import QtQuick 2.2
Item{
    id:controlPanel
    anchors{
        top:headerRect.bottom
        left:parent.left
        right:parent.right
        bottom: parent.bottom
    }

    clip:true
    visible:state!=="unloaded"  
    state:"unloaded"

    Rectangle{
        id:fil
        anchors.fill: parent
        color: style.contentBgColor
        opacity:style.bgContentOpacity
    }

    Loader{
        id:slideOut
        anchors{
            left:optionContainer.right
            right:optionContainer.right
            top:controlPanel.top
            bottom:controlPanel.bottom
        }
        source: ""

        onStatusChanged:{
            if(slideOut.source ===""){
                slideOut.anchors.right = undefined
                slideOut.anchors.right = optionContainer.right
            } else if (slideOut.source !=="" && slideOut.status===Component.Ready){
                slideOut.anchors.right =undefined
                slideOut.anchors.right = parent.right
            }
        }

//        Behavior on {
//            AnchorAnimation{
//                duration: 350
//                easing.type: Easing.InCubic
//            }
//        }
    }

    Rectangle{
        id:optionContainer
        gradient:style.bgContentGradient
        width: scaleX(15)

        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
            leftMargin: 10
        }

        ListView{
            id:optionView
            anchors.fill: parent
            model:availbleOptions
            delegate: StyledButton{
                label: name
                anchors{
                    left:parent.left
                    right:parent.right
                }
                height:scaleY(20)
                onActivated: slideOut.source = control
            }
        }
    }


    ListModel{
        id:availbleOptions

        ListElement{
            name:"Zoom & Aspect"
            control:"Zoom.qml"
        }
        ListElement{
            name:"Bookmarks"
            control:"Bookmarks.qml"
        }
        ListElement{
            name:"Resend \n Command"
            control:"Resend.qml"
        }

        ListElement{
            name:"Manage Playlist"
            control:"PlaylistEditor.qml"
        }

        ListElement{
            name:"Thumbnail"
            control:"Thumbnail.qml"
        }
        ListElement{
            name:"Jog"
            control:"Jog.qml"
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
                opacity:.65
            }
        }
    ]
}
