import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"
import "../."

Item{
    id:option_row
    height: Style.appNavigation_panelHeight
    focus:true
    onActiveFocusChanged: if(activeFocus) optionContainer.forceActiveFocus()
    property bool active:optionContainer.activeFocus
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }
    Rectangle{
        anchors.fill: parent
        color:  Style.appcolor_background_medium
        opacity:option_row.active ? Style.appPanel_opacity : Style.appList_opacity
    }
    StyledText{
        id:row_label
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        text:qsTr("Options", "Media Grid Options")
    }

    FocusRow {
        id: optionContainer

        onEscapeRow: mediaFilesView.forceActiveFocus()

        anchors{
            top:parent.top
            left: row_label.right
            bottom:parent.bottom
            right:parent.right
            margins: 5
        }
        spacing: 10

        LargeStyledButton{
            buttonText: qsTr("Filters", "Media Filters")
            anchors{
                top:parent.top
                bottom:parent.bottom
            }
        }
        LargeStyledButton{
            buttonText: qsTr("Play All", "Play All Media on this Screen")
            anchors{
                top:parent.top
                bottom:parent.bottom
            }

        }
    }
}
