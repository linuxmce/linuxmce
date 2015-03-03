import QtQuick 2.3
import "."

Item{
    id:existing_orbiters
    height: scaleY(65)
    width: Style.listViewWidth_large
    anchors.left: parent.right
    anchors.verticalCenter: parent.verticalCenter
    visible: true
    opacity: 0

    Rectangle{
        id:hdr
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        color:Style.appcolor_background
        height:scaleY(10)
    }

    Text{
        id:lbl
        text:qsTr("Existing Orbiters", "list of existing qorbiters")
        font.pointSize: Style.appFontSize_list
        color:Style.apptext_color_active
        anchors.centerIn: hdr
    }

    Rectangle{
        id:fill
        anchors{
            left:parent.left
            top:hdr.bottom
            right: parent.right
            bottom:parent.bottom
        }
        color: Style.appcolor_background_list
    }

    ListView{
        id:existing_orbiters_view
        clip: true
        anchors.fill: fill
        model:orbiterList

        Behavior on opacity{
            PropertyAnimation{
                duration: 1500
            }
        }

        delegate: Item{
            id:existing_orbiter_delegate
            height: labelColumn.height
            width: existing_orbiters.width

            Column{
                id:labelColumn
                height: childrenRect.height
                width: parent.width *.95
                anchors.centerIn: parent

                Text {
                    id: orbiter_label
                    text: qsTr("Orbiter:")+ label
                    font.pointSize: Style.appFontSize_list
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    width: parent.width
                    color: "white"
                }
                Text {
                    id: dev_num
                    text:qsTr("Device:")+ device
                    font.pointSize: Style.appFontSize_list
                    font.weight: Font.Light
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color:"white"
                    width: parent.width
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: window.qmlSetupLmce(device, window.router)
            }
        }

    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: existing_orbiters
                opacity:0
            }
        },
        State {
            name: "showing"
            PropertyChanges {
                target: existing_orbiters
                opacity:1
                anchors.rightMargin: 10
            }
            AnchorChanges{
                target: existing_orbiters
                anchors.left: undefined
                anchors.right: parent.right
            }
        }
    ]

}
