import QtQuick 2.3
import "."

Item{
    id:om_root
    anchors.fill: parent
    property string title:qsTr("Options")
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
        width:Style.appMenuPanel_widthSmall
        anchors.centerIn: parent



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

        Rectangle{
            anchors.fill:customList
            color: Style.appcolor_background_list
        }

        Column{
            id:customList
            anchors{
                left:parent.left
                right:parent.right
                bottom:parent.bottom
                top:titleText.bottom
                topMargin: scaleY(2.5)
            }
            Item{
                width: parent.width
                height: Style.listViewItemHeight
                Text {
                    id: skinsLabel
                    text: qsTr("Use Network Skins?")
                    color:Style.apptext_color_active

                }
            }
            Item{
                width: parent.width
                height: Style.listViewItemHeight
                Text {
                    id: langlabel
                    text: qsTr("Language")
                    color:Style.apptext_color_active
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.setLanguage("de_DE")
                }
            }
        }

    }

}
