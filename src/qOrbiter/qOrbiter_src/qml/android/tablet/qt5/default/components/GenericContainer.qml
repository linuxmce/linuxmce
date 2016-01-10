import QtQuick 2.2
import "../../../../../skins-common/lib/handlers"

Item{
    id:genericContainer
    property bool fullscreen:false
    property string headerLabel:"lorem foo"
opacity: 0
    anchors.right: componentLoader.right

    Component.onCompleted: {
        genericContainer.state="opening"
        if(fullscreen){
            toggleBars(true)
        }
        genericContainer.state="ready"
    }

    StyledHeader {
        id: styledHdr
        labelText: headerLabel
        onCloseSignal: genericContainer.state="closing"
    }

    Item{
        id:content
        anchors{
            top:styledHdr.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        Rectangle{
            anchors.fill: parent
            color:style.contentBgColor
            opacity:style.bgContentOpacity
        }
    }

    states: [
        State {
            name: "opening"

            AnchorChanges{
                target:genericContainer
                anchors{
                    top:componentLoader.top
                    bottom:componentLoader.bottom
                    left:componentLoader.left
                    right:componentLoader.left
                }
            }
        },
        State {
            name: "ready"

            AnchorChanges{
                target:genericContainer
                anchors{
                    top:componentLoader.top
                    bottom:componentLoader.bottom
                    left:componentLoader.left
                    right:componentLoader.right
                }
            }
        },
        State {
            name: "closing"

            AnchorChanges{
                target:genericContainer
                anchors{
                    top:componentLoader.top
                    bottom:componentLoader.bottom
                    left:componentLoader.right
                    right:componentLoader.right
                }
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "closing"
            AnchorAnimation{
                duration:style.mediumAnimation
            }
            SequentialAnimation{
                PropertyAnimation{target:genericContainer; property:"opacity"; to:0; duration:style.quickAnimation }
                ScriptAction{
                    script:{toggleBars(false);componentLoader.close()}
                }
            }
        },
        Transition {
            from: "*"
            to: "ready"
             PropertyAnimation{target:genericContainer; property:"opacity"; to:1; duration:style.quickAnimation }
            AnchorAnimation{
                duration:style.mediumAnimation
            }
        }
    ]
}
