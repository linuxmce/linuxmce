import QtQuick 2.2
import org.linuxmce.enums 1.0
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0
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
                Component.onCompleted: {
                    state="filter"
                }
                property int authUser:-1
                Connections{
                    target:userList
                    onPrivateUserChanged:{
                        if(userList.currentPrivateUser==authUser){
                            pop.close()
                        } else {
                            hdr.color="red"
                            keyPass.text=""
                        }
                    }
                }

                property bool selectingUser:selectionView.model==userList
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
                    ExclusiveGroup{
                        id:mdloptions
                    }

                    anchors{
                        right:parent.right
                        top:parent.top
                        bottom:parent.bottom
                        left:selectionDisplay.right
                    }
                    delegate:  StyledButton {
                        height: Style.appButtonHeight /2
                        width: parent.width
                        buttonRadius: status ? 5 : 0
                        buttonText: filterdlg.selectingUser? username: name
                       txtObj.font.bold: status
                       property bool isSelected:status
                       onIsSelectedChanged: {
                        console.log(name + "qml status "+ status)
                       }
                        onActivated: {
                            if(filterdlg.selectingUser){
                                filterdlg.authUser=pkUser
                                filterdlg.state="auth";
                                return;
                            }

                            selectionView.model.setSelectionStatus(name);
                            if(!selectionView.model.allowMulti)
                                pop.close()

                        }
                        // arrow:filterdlg.selectingUser ? "false" : selectionView.model.getSelectionStatus(name)
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
                        onActivated: selectionView.model=fileformatmodel

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
                        onActivated:{/*filterdlg.selectingUser=true;*/ selectionView.model=userList}
                    }
                }

                Item{
                    id:userEntryKeys
                    height: parent.height *.75
                    width: parent.width*.75
                    anchors.centerIn: parent
                    Rectangle{
                        id:hdr
                        height: Style.appNavigation_panelHeight
                        color: Style.appcolor_background_light
                        width: parent.width
                        anchors{
                            top:parent.top
                            left:parent.left
                        }
                    }

                    StyledText{
                        id:entryLabel
                        text:qsTr("Enter Authorization %1").arg(filterdlg.authUser)
                        anchors.left: hdr.left
                        anchors.verticalCenter: hdr.verticalCenter
                    }

                    TextInput{
                        id:keyPass
                        anchors{
                            left:entryLabel.right
                            top:entryLabel.top
                            bottom:entryLabel.bottom
                            right:hdr.right
                        }
                    }

                    BackButton {
                        id: back
                        onActivated: filterdlg.state="filter"
                        height: hdr.height/2
                        width: hdr.height
                        anchors{
                            right:hdr.right
                            verticalCenter: hdr.verticalCenter
                        }
                    }

                    Flow{
                        id:numberFlow
                        height: Style.appButtonHeight*3
                        width: Style.appButtonWidth*3
                        anchors{
                            top:hdr.bottom
                            horizontalCenter: hdr.horizontalCenter
                        }

                        Repeater{
                            model:9
                            delegate: StyledButton{
                                buttonText: index+1
                                onActivated: keyPass.text=keyPass.text+String(buttonText)
                            }
                        }
                    }
                    Row{
                        id:extraBtns
                        anchors{
                            top:numberFlow.bottom
                            left:numberFlow.left
                        }

                        StyledButton{
                            id:del
                            buttonText:"X"
                            onActivated: keyPass.text=""
                        }
                        StyledButton{
                            id:zeroBtn
                            buttonText: "0"
                            onActivated: keyPass.text=keyPass.text+String(buttonText)
                        }
                        StyledButton{
                            id:goBtn
                            buttonText: qsTr("Enter")
                            onActivated: {
                                manager.requestUserMediaAuth(
                                            manager.q_mediaType,
                                            keyPass.text,
                                            filterdlg.authUser
                                            )
                            }
                        }
                    }
                }

                states: [
                    State {
                        name: "filter"
                        PropertyChanges {
                            target: selectionDisplay
                            visible:true
                        }
                        PropertyChanges {
                            target: selectionView
                            visible:true
                        }
                        PropertyChanges {
                            target: userEntryKeys
                            visible:false
                        }
                    },
                    State {
                        name: "auth"
                        PropertyChanges {
                            target: selectionDisplay
                            visible:false
                        }
                        PropertyChanges {
                            target: selectionView
                            visible:false
                        }
                        PropertyChanges {
                            target: userEntryKeys
                            visible:true
                        }
                    }

                ]
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

    ScrollRow{
        id:row_scroll
        anchors{
            top:parent.top
            left: row_label.right
            bottom:parent.bottom
            right:back.left
            margins: 5
        }
        FocusRow {
            id: optionContainer

            onEscapeRow: mediaFilesView.forceActiveFocus()
            width: childrenRect.width
            height: row_scroll.height

            spacing: 10

            LargeStyledButton{
                buttonText: qsTr("Filters", "Media Filters")
                fontSize: Style.appFontSize_list
                arrow:false
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
                onActivated: manager.playMedia("!G", true)
            }
            LargeStyledButton{
                buttonText: qsTr("A - Z", "Aplhabet")
                fontSize: Style.appFontSize_description
                width: Style.appButtonWidth
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                }

                onActivated: if(!a_z.sourceComponent){
                                 a_z.sourceComponent=alpha
                             } else {
                                 a_z.sourceComponent=undefined
                             }

            }
        }

    }

    BackButton{
        id:back
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }
        height:parent.height
        width:height
        rotation: 180
        onActivated: manager.goBackGrid()
    }
}
