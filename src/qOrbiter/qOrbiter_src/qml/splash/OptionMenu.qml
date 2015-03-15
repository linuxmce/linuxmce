import QtQuick 2.3
import "."

Item{
    id:om_root
    anchors.fill: parent
    property string title:qsTr("Options")
   Component.onCompleted: state="switch"
    ListModel{
        id:languages
        ListElement{
            name:qsTr("English")
            val:"en_US"
        }
        ListElement{
            name:"German"
            val:"de_DE"
        }
    }

    Rectangle{
        id:mask
        anchors.fill: parent
        color: "black"
        opacity: .55
        MouseArea{
            anchors.fill: parent
            onClicked: om_root.destroy()
        }
    }

    Item {
        id:om_content
        height: scaleY(53)
        width:Style.appMenuPanel_widthMedium
        anchors.centerIn: parent
        clip:true
        Rectangle{
            anchors.fill: parent
            color: Style.appcolor_background
        }

        Text{
            id:titleText
            anchors{
                left:parent.left
                leftMargin: scaleX(2)
                top:parent.top
                topMargin: scaleY(2.5)
            }
            text:title
            font.weight: Font.Light
            font.pointSize: Style.appFontSize_title
        }

        LanguageList {
            id: languageList
            anchors{
                left:customList.right
                bottom:parent.bottom
                top:titleText.bottom
                topMargin: scaleY(1.8)
            }
            width: om_content.width
        }

        Rectangle{
            anchors.fill:customList
            color: Style.appcolor_background_list
            opacity:.85
        }

        Column{
            id:customList
            width: om_content.width
            spacing:scaleY(1)
            anchors{
                right:om_content.right
                bottom:om_content.bottom
                top:titleText.bottom
                topMargin: scaleY(1.8)
            }
            Switch {
                switchLabel: qsTr("Use Network Skins?")
                enabled: true
                onTriggered: enabled=!enabled
            }
            Item{
                width: parent.width
                height: Style.listViewItemHeight
                Text {
                    id: langlabel
                    text: qsTr("Language Selection")
                    font.pointSize: Style.appFontSize_list
                    color:Style.apptext_color_active
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left:parent.left
                        leftMargin: Style.listViewMargin
                    }
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: om_root.state="lang"
                }
            }
        }

    }

    states: [
        State {
            name: "switch"
            AnchorChanges{
                target: customList
                anchors.right: om_content.right
            }

            PropertyChanges{
                target: titleText
                text:qsTr("Options")
            }
        },
        State {
            name: "lang"
            AnchorChanges{
                target: customList
                anchors.right: om_content.left
            }
            PropertyChanges{
                target: titleText
                text:qsTr("Language Selection")
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: Style.transition_animationTime
                easing.type: Easing.InCubic
            }
        }
    ]

}
