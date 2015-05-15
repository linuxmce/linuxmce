import QtQuick 2.2
import "."

Item{
    id:om_root
    anchors.fill: parent
    property string title:qsTr("Options")
    Component.onCompleted: state="switch"
    property int innerBoxWidth:theme.appMenuPanel_widthLarge
    property int innerBoxHeight:scaleY(53)
    ListModel{
        id:languages
        ListElement{
            name:"English"
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
        height: innerBoxHeight
        width:innerBoxWidth
        anchors.centerIn: parent
        clip:true
        Rectangle{
            anchors.fill: parent
            color: theme.appcolor_background
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
            font.pixelSize: theme.appFontSize_title
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
            color: theme.appcolor_background_list
            opacity:.85

        }

        Column{
            id:customList
            width: om_content.width
            spacing:1
            anchors{
                right:om_content.right
                bottom:om_content.bottom
                top:titleText.bottom
                topMargin: scaleY(1.8)
            }
            Switch {
                switchLabel: qsTr("Use Network Skins?")
                enabled: manager.useNetworkSkins
                onTriggered: manager.setUseNetworkSkins(!manager.useNetworkSkins)
            }
            Item{
                width: parent.width
                height: theme.listViewItemHeight
                Rectangle{
                    anchors.fill: parent
                    color:"black"
                    opacity: .55
                }
                Text {
                    id: langlabel
                    text: qsTr("Language Selection")
                    font.pixelSize: theme.appFontSize_list
                    color:theme.apptext_color_active
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left:parent.left
                        leftMargin: theme.listViewMargin
                    }
                }

                Image {
                    id: lng_pointer
                    source: "images/vertical_arrow.png"
                    height: parent.height /2
                    fillMode: Image.PreserveAspectFit
                    width: height
                    rotation: -90
                    anchors{
                        right:parent.right
                        verticalCenter: parent.verticalCenter
                    }
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: om_root.state="lang"
                }
            }

            Item {
                width: parent.width
                height: theme.listViewItemHeight
                Rectangle{
                    anchors.fill: parent
                    color:"black"
                    opacity: .55
                }
                Text {
                    id: settingsLabel
                    text: qsTr("Clear Settings")
                    font.pixelSize: theme.appFontSize_list
                    color:theme.apptext_color_active
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left:parent.left
                        leftMargin: theme.listViewMargin
                    }
                }


                MouseArea{
                    anchors.fill: parent
                    onClicked: settings.destroySettingsData()
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
                duration: theme.transition_animationTime
                easing.type: Easing.InCubic
            }
        }
    ]

}
