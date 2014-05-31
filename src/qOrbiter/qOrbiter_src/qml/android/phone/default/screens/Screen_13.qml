import QtQuick 1.0
import "../components"
StyledScreen{
    id:securitypanel
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

    //delegate
    Component {
        id: securityDelegate
	Item{
            clip: true
	    width: modeView.cellWidth
	    height: modeView.cellHeight

            ButtonSq {
                id: secDelegateBt
		width : parent.width
		height: parent.height
                buttontext: name
                onActivated: manager.setHouseMode(text_input1.text, mode)
            }
        }
    }

    // The CMD_Set_Text is sent to the orbiter when an invalid pin is entered, we can catch it here and do something useful with it
    Connections {
	target: manager
	onTextChanged: {
	    if (iPK_Text == 617) {
		isInvalidPin = true
		text_input1.text = sText;
		text_input1.echoMode = TextInput.Normal;
		rectangle2.color = "red";
	    }
	}
    }

    GridView {
	id: modeView
	anchors {
	    left: parent.left
	    right: parent.right
	    top: keypadSection.bottom
	    bottom: parent.bottom
	}
        clip: true
        model: securityModes
	cellHeight: scaleX(18)
	cellWidth: scaleX(50)
        delegate: securityDelegate
    }

    function resetInvalidPin() {
	if (isInvalidPin) {
	    text_input1.echoMode = TextInput.Password;
	    text_input1.text = "";
	    isInvalidPin;
	    rectangle2.color = "white";
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

    Rectangle{
	id: keypadSection
	anchors{
            top:parent.top
            left:parent.left
            right:parent.right
	}
	height: appH / 1.76
	color: style.button_system_color
        opacity:.65

        Text {
            id: securitypanellabel
	    anchors.top: parent.top
	    anchors.topMargin: 5
	    anchors.horizontalCenter: parent.horizontalCenter
            text: "Security Panel"
            font.family: "Droid Sans"
            font.bold: true
            font.pointSize: 15
	    color: "white"
        }

        Rectangle {
            id: rectangle1
	    anchors.top: securitypanellabel.bottom
	    anchors.horizontalCenter: parent.horizontalCenter    
	    anchors.topMargin: 5
	    anchors.bottom: parent.bottom
	    anchors.bottomMargin: 5
            color: "#789199"
            radius: 22
	    width: scaleX(70)
            Rectangle {
                id: rectangle2
		anchors.top: rectangle1.top
		anchors.topMargin: 5
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin : 5
		anchors.rightMargin : 5
                height: 30
                color: "#ffffff"
                radius: 19

                TextInput {
                    id: text_input1
		    anchors.fill: parent
                    text: ""
                    echoMode: TextInput.Password
                    font.family: "Droid Sans Mono"
                    cursorVisible: true
                    readOnly: false
                    anchors.margins: 0
                    horizontalAlignment: TextInput.AlignHCenter
                    font.pixelSize: 23
                }
            }

            Rectangle {
                id: rectangle3
		anchors.top: rectangle2.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 5
                color: "#ffffff"
                radius: 28

                Flow {
                    id: flow1
                    anchors.top: parent.top
		    anchors.horizontalCenter: parent.horizontalCenter
		    anchors.margins: 6
		    width: (scaleX(16) + 5) * 3
                    spacing: 5
                    Repeater{
                        model: 9;

			ButtonSq {
			    width : scaleX(16)
			    height: scaleX(16)
			    buttontext: (index+1)
			    onActivated: typeText(index+1)
			}
                    }
		    ButtonSq {
			width : scaleX(16)
			height: scaleX(16)
			buttontext: "<"
			onActivated: clearText();
		    }
		    ButtonSq {
			width : scaleX(16)
			height: scaleX(16)
			buttontext: "0"
			onActivated: typeText("0")
		    }
		    ButtonSq {
			width : scaleX(16)
			height: scaleX(16)
			buttontext: "Map"
			onActivated: {
			    manager.showfloorplan(1)
			    manager.setFloorplanType(1)
			}
		    }
                }
            }
        }
    }
    
}


