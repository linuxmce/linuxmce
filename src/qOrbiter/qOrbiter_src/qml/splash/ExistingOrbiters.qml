import QtQuick 2.2
import "."

Item{
    id:existing_orbiters

    width: theme.listViewWidth_large
    anchors.left: parent.right

    visible: true
    opacity: 0

    Rectangle{
        id:hdr
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        color:theme.appcolor_background
        height:scaleY(10)
        opacity:.65
    }

    Text{
        id:lbl
        text:qsTr("Existing Orbiters", "list of existing qorbiters")
        font.pointSize: theme.appFontSize_list
        color:theme.apptext_color_active
        anchors.centerIn: hdr
    }
    Image{
        id:icn
        source:"images/vertical_arrow.png"
        height: parent.height*.65
        fillMode: Image.PreserveAspectFit
        anchors{
            right: parent.right
            rightMargin:scaleX(10)
            verticalCenter: parent.verticalCenter
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
        color: theme.appcolor_background_list
    }

    ListView{
        id:existing_orbiters_view
        clip: true
        anchors.fill: fill
        model:orbiterList
        spacing:5
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
                font.pointSize: theme.appFontSize_list
                font.weight: Font.Light
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color:"white"
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
            }

            Column{
                id:labelColumn
                height: parent.height
                width: parent.width *.95
                anchors.centerIn: parent

                Text {
                    id: orbiter_label
                    text: qsTr("Orbiter: ")+ label
                    font.pointSize: theme.appFontSize_list
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "white"
                }
                Text {
                    id: loc
                    text:qsTr("Location:")+ location
                    font.pointSize: theme.appFontSize_list
                    font.weight: Font.Light
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color:"white"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: orbiterWindow.qmlSetupLmce(device, orbiterWindow.router)
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
             when:orbiterList.count!==0
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
