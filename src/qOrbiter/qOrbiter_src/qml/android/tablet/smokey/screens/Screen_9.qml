import QtQuick 1.0
import "../components"
StyledScreen{
    id:makecallphonebook

    Panel{
        id: phonebookcall
        headerTitle: "Make Call from phonebook."
        anchors.centerIn: parent

        Rectangle {
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
            anchors.topMargin: scaleY(10)
            anchors.bottomMargin: scaleY(10)
            ListView {
                id: phoneBookList
                property int level: 0
                model: manager.getDataGridModel("phoneBook", 21)
                anchors.fill : parent
                anchors.margins: 2
                delegate: StyledButton{
                    height: scaleY(10)
                    anchors.left: parent.left
                    anchors.right: parent.right
                    buttonText: description
                    textSize: 42
                    onActivated:   {
                        if (phoneBookList.level == 0) {
                            phoneBookList.model = manager.getDataGridModel("phoneNumber", 22)
                        }
                    }
                }
            }
        }
        Connections {
            target: phoneBookList.model
            onScrollToItem: { phoneBookList.positionViewAtIndex(item, ListView.Beginning); }
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
            height: scaleY(10)

            StyledText {
                id : searchLabel
                text: "Search..."
                color: "white"
                anchors.fill: parent.fill
                anchors.horizontalCenter: parent.horizontalCenter
                height: scaleY(5)
                width: parent.width
            }
            TextInput {
                id: text_input1
                anchors.fill: parent
                anchors.margins: 0
                anchors.topMargin: scaleY(5)
                color: "white"
                text: ""
                cursorVisible: true
                readOnly: false
                horizontalAlignment: TextInput.AlignHCenter
                font.pixelSize: 23
                onTextChanged: manager.seekGrid("phoneBook", text)
            }
        }
    }

}
