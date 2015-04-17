import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"
import "../."

Item{
    id:option_row
    height: Style.appNavigation_panelHeight
    focus:true
    onActiveFocusChanged: if(activeFocus) optionContainer.forceActiveFocus()

    Component{
        id:mediaFilters
        GenericPopup{
            id:pop
            title: qsTr("Media Filters")
            content: Item{
                id:filterdlg
                anchors.fill: parent
                MouseArea{
                    anchors.fill: parent
                    onClicked: pop.close()
                }
                Rectangle{
                    id:bg
                    color:"black"
                    anchors.fill: parent
                    opacity: .85
                }

                GenericListModel{
                    id:selectionView
                    label: qsTr("Selections")
                    anchors{
                        right:parent.right
                        top:parent.top
                        bottom:parent.bottom
                        left:selectionDisplay.right
                    }
                    delegate:  LargeStyledButton {
                        height: Style.appButtonHeight /2
                        width: parent.width
                        buttonRadius: 0
                        buttonText: name
                        onActivated: {selectionView.model.setSelectionStatus(name); arrow=!arrow}
                        arrow:selectionView.model.getSelectionStatus(name)
                    }
                }

                Column{
                    id:selectionDisplay
                    width: Style.scaleX(35)
                    anchors{
                        left:parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    spacing: 5
                    height: childrenRect.height
                    StyledButton{
                        width: parent.width
                        buttonText: qsTr("Attribute")
                        height: Style.appButtonHeight / 2
                        onActivated: selectionView.model=attribfilter
                    }
                    StyledButton{
                        width: parent.width
                        height: Style.appButtonHeight / 2
                        buttonText: qsTr("Genre")
                        onActivated: selectionView.model=genrefilter
                    }
                    StyledButton{
                        width: parent.width
                        buttonText: qsTr("MediaType")
                        height: Style.appButtonHeight / 2
                        onActivated: selectionView.model=mediatypefilter
                    }
                    StyledButton{
                        width: parent.width
                        buttonText: qsTr("Resolution")
                        height: Style.appButtonHeight / 2

                    }
                    StyledButton{
                        width: parent.width
                        buttonText: qsTr("Sources")
                        height: Style.appButtonHeight / 2

                    }
                    StyledButton{
                        width: parent.width
                        buttonText: qsTr("Users")
                        height: Style.appButtonHeight / 2
                    }
                }

            }
        }
    }

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
        fontSize: Style.appFontSize_list
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
            fontSize: Style.appFontSize_list
            anchors{
                top:parent.top
                bottom:parent.bottom
            }
            onActivated: qmlRoot.createPopup(mediaFilters)
        }
        LargeStyledButton{
            buttonText: qsTr("Play All", "Play All Media on this Screen")
            fontSize: Style.appFontSize_description
            width: Style.appButtonWidth
            anchors{
                top:parent.top
                bottom:parent.bottom
            }

        }
    }
}
