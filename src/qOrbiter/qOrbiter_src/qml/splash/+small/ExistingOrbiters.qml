import QtQuick 2.3
import "."

Item{
    id:existing_orbiters
    width: parent.width
    anchors{
        top:topContainer.bottom
        bottom:newOrbiterButton.top
    }

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
        height:Style.appPanelHeight
        opacity:.65
    }

    Text{
        id:lbl
        text:qsTr("Existing Orbiters", "list of existing qorbiters")
        font.pointSize: Style.appFontSize_list
        color:Style.apptext_color_active
        anchors.centerIn: hdr
    }
    Image{
        id:icn
        source:"images/vertical_arrow.png"
        height: parent.height*.65
        fillMode: Image.PreserveAspectFit
        anchors{
            right: hdr.right
            rightMargin:Style.scaleX(10)
            verticalCenter: hdr.verticalCenter
        }
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
        spacing:2
        Behavior on opacity{
            PropertyAnimation{
                duration: 1500
            }
        }
        delegate: Item{
            id:existing_orbiter_delegate
            height: scaleY(8)
            width: existing_orbiters.width
            Text {
                id: dev_num
                text: device
                font.pointSize: scaleY(Style.appFontSize_list)
                font.weight: Font.Light
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color:"white"
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
            }
            Image{
                source: "images/arrow.png"
                width: parent.width*.10
                fillMode: Image.PreserveAspectFit
                anchors{
                    left:parent.left
                    verticalCenter: parent.verticalCenter
                }

            }

            Column{
                id:labelColumn
                height: parent.height
                width: parent.width *.95
                anchors.centerIn: parent

                Text {
                    id: orbiter_label
                    text: qsTr("Orbiter: ")+ label
                    font.pointSize:scaleY(Style.appFontSize_list)
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "white"
                }
                Text {
                    id: loc
                    text:qsTr("Location:")+ location
                    font.pointSize:scaleY(Style.appFontSize_list)
                    font.weight: Font.Light
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color:"white"
                    anchors.horizontalCenter: parent.horizontalCenter
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
            }

        }
    ]

}
