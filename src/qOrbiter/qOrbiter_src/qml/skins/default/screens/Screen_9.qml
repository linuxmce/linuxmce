import QtQuick 2.2
import "../components"
import "../."
import org.linuxmce.grids 1.0

StyledScreen{
    id:makecallphonebook

    Panel{
        id: phonebookcall
        property string numberToCall: ''
        property int level: 0

        headerTitle: "Make Call from phonebook."
        anchors.centerIn: parent

        content: Rectangle {
            id: phoneList
            border.color : "white"
            border.width : 1
            color: "black"
            opacity: .85
            radius: 2
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: 4
            anchors.bottomMargin: appStyle.scaleY(10)

            ListView {
                id: phoneBookList
                model: manager.getDataGridModel("phoneBook", DataGrids.Phone_Book_Auto_Compl, '')
                Connections {
                    target: phoneBookList.model
                    onScrollToItem: {

                        phoneBookList.positionViewAtIndex(item, ListView.Beginning);
                    }
                }
                anchors.fill : parent
                anchors.margins: 2
                delegate: StyledButton{
                    anchors.left: parent.left
                    anchors.right: parent.right
                    buttonText: phonebookcall.level == 1 ? description + ': ' + value :  description
                    textSize: appStyle.appFontSize_list
                    height: appStyle.appButtonHeight / 2
                    onActivated:   {
                        if (phonebookcall.level == 0) {
                            // Switch to phone number list
                            phonebookcall.level = 1
                            manager.clearDataGrid("phoneNumber")
                            phoneBookList.model = manager.getDataGridModel("phoneNumber", DataGrids.Phone_Book_List_of_Nos, value)
                        } else if (phonebookcall.level == 1) {
                            // switch to call from device list
                            phonebookcall.numberToCall = value
                            phonebookcall.level = 2
                            telecomDevices.visible = true
                            searchArea.visible = false
                        }
                    }
                }
            }
        }

        TelecomDevices {
            id: telecomDevices
            visible: false
            anchors.fill: parent
            anchors.bottomMargin: 20
            z: 100
            showCallToDevices: false
            onSelected: {
                visible = false
                manager.makeCall(currentuser, phonebookcall.numberToCall, selectedDevice, 0)
                phonebookcall.level = 1
            }
            onClose: {
                phonebookcall.level = 1
            }
        }

        Rectangle {
            id: searchArea
            border.color : "white"
            border.width : 1
            color: "black"
            opacity: .85
            radius: 2
            anchors.bottom: parent.bottom
            anchors.left : parent.left
            anchors.right: parent.right
            anchors.margins: 5
            height: appStyle.appButtonHeight
            MouseArea{
                anchors.fill: parent
            }

            StyledText {
                id : searchLabel
                text: "Search..."
                color: "white"
                anchors.fill: parent.fill
                fontSize: appStyle.appFontSize_list
            }
            TextInput {
                id: text_input1
                anchors.fill: parent
                anchors.margins: 0
                anchors.topMargin: appStyle.scaleY(5)
                color: "white"
                text: ""
                cursorVisible: true
                readOnly: false
                horizontalAlignment: TextInput.AlignHCenter
                font.pixelSize: appStyle.appFontSize_list
                onTextChanged: {
                    manager.refreshDataGrid("phoneBook", DataGrids.Phone_Book_Auto_Compl, "1|%"+text)
                }
            }
        }
    }

}
