import QtQuick 2.2
import "."

Item{
    id:languageList

    Rectangle{
        anchors.fill: languageList
        color: theme.appcolor_background_medium
    }
    ListView{
        anchors.fill: languageList
        model:languages
        delegate: Item{
            height: theme.listViewItemHeight
            width: parent.width
            Rectangle{
                anchors.fill: parent
                color:"black"
                opacity: .55
            }

            Text{
                anchors.centerIn: parent
                font.pointSize: theme.appFontSize_list
                text:name
                color:"white"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: manager.setLanguage(val)
            }
        }
    }
}
