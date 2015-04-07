import QtQuick 1.0
import "../components"
StyledScreen{
    id:securitypanel


    Item{
        id:content
        anchors.fill: parent

        property bool isInvalidPin : false
        ListModel{
            id:securityModes

            ListElement{
                name: "Home"
                image: "security/Home.png"
                mode: 1
            }

            ListElement{
                name: "Away"
                image: "security/Away.png"
                mode: 2
            }

            ListElement{
                name: "At Home Secure"
                image: "security/HomeSecure.png"
                mode: 3
            }

            ListElement{
                name: "Sleeping"
                image: "security/Sleeping.png"
                mode: 4
            }

            ListElement{
                name: "Entertaining"
                image: "security/Entertaining.png"
                mode: 5
            }

            ListElement{
                name: "Extended Away"
                image: "security/Extended.png"
                mode: 6
            }

        }


        // The CMD_Set_Text is sent to the orbiter when an invalid pin is entered, we can catch it here and do something useful with it
        Connections {
            target: manager
            onTextChanged: {
                if (iPK_Text == 617) {
                    content.isInvalidPin = true
                    text_input1.text = sText;
                    text_input1.echoMode = TextInput.Normal;
                    text_input1.fillColor = "red";
                    text_input1.forceActiveFocus()
                }
            }
        }


        function resetInvalidPin() {
            if (isInvalidPin) {
                text_input1.echoMode = TextInput.Password;
                text_input1.text = "";
                text_input1.fillColor = "green";
                isInvalidPin = false;
            }
        }
        function typeText(text) {
            resetInvalidPin();
            text_input1.text = text_input1.text+text;
        }
        function clearText() {
            resetInvalidPin();
            if (text_input1.text.length > 0)
                text_input1.text = text_input1.text.substring(0,text_input1.text.length-1);
        }

        Panel{
            id: keypadSection
            anchors{
                top:parent.top
                left:parent.left
                right:parent.right
                bottom:parent.bottom
            }

            headerTitle: "Security Panel"
            property int flowButtonHeight: flow1.height/3
            property int flowButtonWidth:flow1.width/3

            TextInput {
                id: text_input1
                width: parent.width
                height: scaleY(8)
                anchors.top: keypadSection.top
                text: ""
                echoMode: TextInput.Password
                color: "white"
                fillColor: "green"
                cursorVisible: true
                readOnly: false
                horizontalAlignment: TextInput.AlignHCenter
                font.pixelSize: 44
            }
            Flow {
                id: flow1
                anchors.top: text_input1.bottom
                width: parent.width /2
                height:scaleY(35)

                Repeater{
                    model: 9

                    StyledButton {
                        height: keypadSection.flowButtonHeight
                        width: keypadSection.flowButtonWidth
                        buttonText: (index+1)
                        onActivated: content.typeText(index+1)
                    }
                }
            }
            Row{
                id:bottomRow
                height: keypadSection.flowButtonHeight
                width:parent.width
                anchors.top: flow1.bottom

                StyledButton {
                    height: keypadSection.flowButtonHeight
                    width: keypadSection.flowButtonWidth
                    buttonText: "<"
                    onActivated: content.clearText();
                }
                StyledButton {
                    height: keypadSection.flowButtonHeight
                    width: keypadSection.flowButtonWidth
                    buttonText: "0"
                    onActivated: content.typeText("0")
                }
                StyledButton {
                    height: keypadSection.flowButtonHeight
                    width: keypadSection.flowButtonWidth
                    buttonText: "Map"
                    onActivated: {
                        manager.showfloorplan(1)
                        manager.setFloorplanType(1)
                    }
                }
            }
            GridView {
                id: modeView
                anchors {
                    top:flow1.top
                    left:flow1.right
                    right:parent.right
                    bottom:parent.bottom
                }

                clip: true
                model: securityModes
                cellHeight: keypadSection.flowButtonHeight
                cellWidth: width
                delegate:    StyledButton {
                    id: secDelegateBt
                    height: keypadSection.flowButtonHeight
                    width: parent.width
                    buttonText: name
                    onActivated: manager.setHouseMode(text_input1.text, mode)
                }
            }

        }
    }
}

