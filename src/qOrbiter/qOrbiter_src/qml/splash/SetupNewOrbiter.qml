// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2
import org.linuxmce.settings 1.0

import "."
Item {
    id:setup
    anchors.fill: parent
    Component.onCompleted:{
        orbiterWindow.showSetup()
        forceActiveFocus()
    }
    Keys.onPressed: {
        switch(event.key){
        case Qt.BackButton:
        case Qt.Key_Backspace:
        case Qt.Key_Back:
            splashLogic.state="no-connection";
            break;
        }
    }
    Connections{
        target:orbiterWindow
        onCreationComplete:{
            splashLogic.state="no-connection"
        }
    }

    VisualItemModel{
        id:setup_screens
        SetupScreen{
            width:style.scaleX(90)
            height: style.scaleY(75)
            title:qsTr("Select Room")
            listmodel: rooms
            trackedProperty: roomSelection
            custom:false
            onSelectionMade:{
                roomSelection=id
                roomName=choice
            }
        }
        SetupScreen{
            width:style.scaleX(90)
            height: style.scaleY(85)
            title:qsTr("Select User")
            listmodel: users
            trackedProperty: userSelection
            custom:false
            onSelectionMade:{
                userSelection=id
                userName=choice
            }
        }
        SetupScreen{
            width:style.scaleX(90)
            height: style.scaleY(85)
            title:qsTr("Select Language")
            listmodel: langMdl
            trackedProperty: langSelection
            custom:false
            onSelectionMade:{
                langSelection=id
                langName=choice
            }

        }
        SetupScreen{
            width:style.scaleX(90)
            height: style.scaleY(85)
            title:qsTr("Select Language")
            listmodel: langMdl
            trackedProperty: langSelection
            custom:true
            contentItem: Item{
                anchors.fill: parent

                Text{
                    id:error
                    visible:langSelection==-1 || roomSelection==-1 || userSelection==-1
                    text: qsTr("Error!\n You must make selections for all steps before you attempt to create a QOrbiter. ")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    width: parent.width *.75
                    height: parent.height*.75
                    anchors.centerIn: parent
                    font.pointSize: 22
                }

                Text {
                    id: end_text
                    visible: !error.visible
                    anchors.centerIn: parent
                    font.pointSize: 18
                    color:"white"
                    text: qsTr("This is the last part of the setup.\n We will create a new QOrbiter device in: %1\n For User: %2\n Using Language: %3 \n It Will be placed in an EA named: %4").arg(roomName).arg(userName).arg(langName).arg(nameInput.text)
                }

                Item{
                    visible:!error.visible
                    id:create_button
                    width: parent.width*.65
                    height: parent.height*.12
                    anchors{
                        bottom: parent.bottom
                        bottomMargin: 10
                        horizontalCenter: parent.horizontalCenter
                    }

                    Rectangle{
                        anchors.fill: parent
                        color:mickey.pressed ? "darkgrey" : "grey"
                    }
                    Text {
                        id: create_label
                        text: qsTr("Create New QOrbiter")
                        anchors.centerIn: parent
                        color:"white"
                    }

                    MouseArea{
                        id:mickey
                        anchors.fill: parent
                        onClicked: {
                            settings.setOption(SettingsType.Settings_Network, SettingsKey.Setting_Network_DeviceName, nameInput.text)
                            onClicked: orbiterWindow.setupNewOrbiter(userSelection, roomSelection+1, 1, 1, appH, appW, nameInput.text)
                        }
                    }
                }
            }
        }

    }

    ListModel{
        id:langMdl
        ListElement{
            dataTitle:"English"
            code:"en"
        }
        ListElement{
            dataTitle:"German"
        }
    }

    property int userSelection:-1
    property int roomSelection:-1
    property int langSelection:-1
    property string userName;
    property string roomName;
    property string langName;

    //palette?
    property string orangeRed: "#993300"
    property string deYork: "#99CC99"
    property string midnightBlue: "#003366"
    //---------
    function scaleX(x){
        return x/100*manager.appHeight
    }
    function scaleY(y){
        return y/100*manager.appWidth
    }


    Timer{
        id:effectTimer
        interval: 250
        running:true
        onTriggered: {newOrbiterSetupContainer.opacity=1;
            newOrbiterSetupContainer.scale =1}
    }

    Item{
        id:newOrbiterSetupContainer
        height: parent.height*.90
        width: parent.width *.90
        anchors.centerIn: parent

        Behavior on opacity { PropertyAnimation{ duration: style.transition_animationTime } }
        Behavior on scale{ PropertyAnimation{ duration:style.transition_animationTime } }
        opacity:0
        scale:0

        Rectangle{
            anchors.fill: parent
            color:"darkgrey"
            radius: 10
        }

        Row{
            id:hdr
            width: parent.width
            height: parent.height*.12
            spacing: style.scaleX(1)
            Text {
                id: welcome
                text: qsTr("Setup as a  New QOrbiter.")
                color:"white"
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignLeft
                font.pointSize: 18
                font.bold: true
                font.family: myFont.name
                width: parent.width*.45
            }
            Text {
                id: changeName
                text: qsTr("Click to change Name")
                color:"white"
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 18
                font.bold: true
                font.family: myFont.name
            }

            Rectangle{
                height: parent.height /2
                width: parent.width *.15
                anchors.verticalCenter: parent.verticalCenter
                color:"white"
                radius:5
                TextInput{
                    id:nameInput
                    anchors.fill: parent
                    font.pointSize: 18
                    font.bold: true
                    font.family: myFont.name
                    text:settings.getOption(SettingsType.Settings_Network, SettingsKey.Setting_Network_DeviceName)
                }
            }
        }

        ListView{
            id:selectionSteps
            model:setup_screens
            orientation: ListView.Horizontal
            snapMode: ListView.SnapOneItem
            spacing: style.scaleX(2)
            clip:true

            anchors{
                top:hdr.bottom
                bottom:parent.bottom
                left:parent.left
                right:parent.right
            }
        }

        /*
        Row{
            id:contentColumn
            height: parent.height *.75
            width:parent.width
            anchors.centerIn: newOrbiterSetupContainer
            spacing:5

            GenericSplashList{
                label:qsTr("User Selection")
                model:users
                delegate: Item{
                    width: parent.width
                    height: contentColumn.height*.12
                    Rectangle{
                        anchors.fill: parent
                       color:userSelection==index ? "darkgrey" : "lightgrey"
                    }
                    Text {
                        id: userText
                        text: dataTitle
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                          userSelection=index
                        }
                    }
                }
            }

            GenericSplashList{
                id:rm
                label:qsTr("Room Selection")
                model:rooms
                delegate: Item{
                    width: parent.width
                    height: contentColumn.height*.12
                    Rectangle{
                        anchors.fill: parent
                        color:roomSelection===index ? "green" : "lightgrey"
                    }
                    Text {
                        id: roomText
                        text: dataTitle
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            roomSelection=index
                        }
                    }
                }
            }

            GenericSplashList{
                label:qsTr("Language Selection")
                model:langMdl
                delegate: Item{
                    width: parent.width
                    height: contentColumn.height*.12
                    Rectangle{
                        anchors.fill: parent
                        color:langSelection==index ? "darkgrey" : "lightgrey"
                    }
                    Text {
                        id: lang
                        text: name
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            langSelection=index
                        }
                    }
                }
            }
        }
*/
        /* Text {
            id: logo
            text: qsTr("LinuxMCE")
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 10
            font.bold: true
            color:"white"
            font.pointSize: 12
            font.family: myFont.name
        }

        Rectangle{
            id:setupButton
            height: scaleY(8)
            width: scaleX(14)
            color: "lightgreen"
            radius: 8
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            Text{
                text:"Go!"
                anchors.centerIn: parent
                color:"white"
                font.family: myFont.name
            }
            MouseArea{
                anchors.fill: parent
                onClicked: orbiterWindow.setupNewOrbiter(userSelection, roomSelection, 1, 1, appH, appW)
            }

        }
        Rectangle{
            id:exitButton
            height: scaleY(8)
            width: scaleX(14)
            color: "red"
            radius: 8
            anchors.bottom: parent.bottom
            anchors.left: setupButton.right
            anchors.leftMargin: scaleX(2)

            Text{
                text:"Exit!"
                anchors.centerIn: parent
                color:"white"
                font.family: myFont.name
            }
            MouseArea{
                anchors.fill: parent
                onClicked: pageLoader.source ="SplashView.qml"       //screenChange("SplashView.qml")
            }
        }*/
    }
}
