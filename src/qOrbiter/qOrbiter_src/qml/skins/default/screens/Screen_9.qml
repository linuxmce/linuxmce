import QtQuick 2.2
import "../components"
import "../."
StyledScreen{
    id:makecallphonebook

    Panel{
        id: phonebookcall
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
            anchors.topMargin: Style.scaleY(10)
            anchors.bottomMargin: Style.scaleY(10)

            ListView {
                id: phoneBookList
                property int level: 0
                model: manager.getDataGridModel("phoneBook", 21)
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
                    buttonText: description
                    textSize: Style.appFontSize_list
                    onActivated:   {
                        if (phoneBookList.level == 0) {
                            phoneBookList.model = manager.getDataGridModel("phoneNumber", 22)
                        }
                    }
                }
            }
        }


        Rectangle {
            border.color : "white"
            border.width : 1
            color: "black"
            opacity: .85
            radius: 2
            anchors.bottom: parent.bottom
            anchors.left : parent.left
            anchors.right: parent.right
            anchors.margins: 5
            height: Style.appButtonHeight
            MouseArea{
                anchors.fill: parent
            }

            StyledText {
                id : searchLabel
                text: "Search..."
                color: "white"
                anchors.fill: parent.fill
                fontSize: Style.appFontSize_list
            }
            TextInput {
                id: text_input1
                anchors.fill: parent
                anchors.margins: 0
                anchors.topMargin: Style.scaleY(5)
                color: "white"
                text: ""
                cursorVisible: true
                readOnly: false
                horizontalAlignment: TextInput.AlignHCenter
                font.pixelSize: Style.appFontSize_list
                onTextChanged:manager.getDataGridModel("phoneBook", 21).setOption("1|%"+text) //manager.seekGrid("phoneBook", text)
            }
        }
    }

}
