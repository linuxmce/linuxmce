import QtQuick 2.0

Item {
    anchors.fill: parent
    id:avcodes_root

    Component.onCompleted: {
        state="loading"
        manager.showAvControl()
    }

    Item{
        id:avcodes_hdr
        Rectangle{
            id:codefiller
            anchors.fill: parent
            color:"transparent"
        }
        StyledText{
            id:hdrText
            text:"Please choose device to control"
            anchors.centerIn: parent
            visible:avcodes_root.state==="ready"
        }
    }



    states: [
        State {
            name: "ready"
            PropertyChanges {
                target: avcodes_hdr
                height:scaleY(8)

            }
            PropertyChanges {
                target: colorfiller
                color:"green"
            }

            AnchorChanges{
                target: avcodes_hdr
                anchors{
                    top:avcodes_root.top
                    left:avcodes_root.left
                    right:avcodes_root.right
                }
            }
        },
        State{
            name:"loading"
            PropertyChanges {
                target: avcodes_hdr
                height:scaleY(2)

            }
            PropertyChanges {
                target: colorfiller
                color:"yellow"
            }
            AnchorChanges{
                target: avcodes_hdr
                anchors{
                    top:avcodes_root.top
                    left:avcodes_root.left
                    right:avcodes_root.right
                }
            }
        }

    ]
}
