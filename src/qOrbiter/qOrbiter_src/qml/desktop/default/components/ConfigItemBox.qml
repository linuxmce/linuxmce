// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:configBoxContainer
    width: scaleX(15)
    height: scaleY(25)
    color:"transparent"



    property string headerText: "enter header text"
    property string configProperty:""

    property bool toggleValue:false
    property bool configSwitch:false
    property bool useText:false
    property bool useToggle:false

    Component.onCompleted: configSwitch = toggleValue

    Rectangle{
        id:configHeader
        height: configBoxContainer.height*.20
        width: configBoxContainer.width
        anchors.top: parent.top
        radius: 5
        color: "#E89F6B"
        StyledText {
            id: hdrLabel
            textLabel: headerText
            color:"black"
            font.pixelSize: 12
        }
    }

    Rectangle{
        id:configContent
        height: configBoxContainer.height *.80
        width: configBoxContainer.width
        anchors.top: configHeader.bottom
        radius:5


        TextEdit{
            id:propertyEntry
            width: parent.width * .50
            height: parent.height*.30
            font.pixelSize: 14
            color: "black"
            anchors.centerIn: parent
            visible:toggleValue ? true:false
            text:configProperty

            Rectangle{
                id:confirm
                width: propertyEntry.width
                height: parent.height /2
                radius: 5
                color: "green"
                StyledText {
                    id: lbk
                    textLabel: qsTr("Confirm")
                    anchors.centerIn: parent
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: configProperty = propertyEntry.text
                }

            }
        }



        Rectangle{
            id:basicToggle
            width: parent.width * .65
            height: parent.height*.30
            color: "lightgrey"
            visible: useToggle ? true : false
            anchors.centerIn: parent
            Rectangle{
                id:selectDongle
                height: basicToggle.height
                width: basicToggle.width / 2
                color: toggleValue ? "green" : "red"
                state: toggleValue ? "yes" : "no"

                anchors.verticalCenter: parent.verticalCenter
                StyledText {
                    id: keyText
                    textLabel: toggleValue
                    anchors.centerIn: parent
                }
                MouseArea{
                    anchors.fill: selectDongle
                    onClicked: {
                        configSwitch = !configSwitch
                        configSwitch ? basicToggle.state="yes" : basicToggle.state = "no"
                    }
                }
            }

            states: [
                State {
                    name: "yes"

                    AnchorChanges{
                        target: selectDongle
                        anchors.left: basicToggle.left
                        anchors.right: undefined

                    }
                },
                State {
                    name: "no"
                    AnchorChanges{
                        target: selectDongle
                        anchors.left: undefined
                        anchors.right: basicToggle.right
                    }
                }
            ]
        }

    }

}
