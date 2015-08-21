// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    width: appW
    height: appH
property int sUser:-1
    property int selectedRoom:-1
    Rectangle{
        anchors.fill: parent
        gradient: Gradient{
            GradientStop{ position:0.0;color:"transparent"}
            GradientStop{ position:0.65;color:"black"}
        }
    }


    //palette?
    property string orangeRed: "#993300"
    property string deYork: "#99CC99"
    property string midnightBlue: "#003366"
    //---------
    function scaleX(x){
        return x/100*appH
    }
    function scaleY(y){
        return y/100*appW
    }
    Component.onCompleted: orbiterWindow.showSetup()


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
        opacity:0
        scale:0

        Behavior on opacity {
            PropertyAnimation{
                duration: 500
            }
        }
        Behavior on scale{
            PropertyAnimation{
                duration:1200
                easing.type: Easing.InQuad
            }
        }
        Row{
            Text {
                id: welcome
                text: qsTr("Setup A New Orbiter")
                color:"white"
                font.pointSize: 18
                font.bold: true
                font.family: myFont.name
            }
        }

        Row{
            id:contentColumn
            height: parent.height *.55
            width: parent.width *.75
            anchors.centerIn: newOrbiterSetupContainer
            spacing: 15

            ListView{
                id:usersView
                height: parent.height
                width: parent.width/4
                orientation: ListView.Vertical
                spacing: 20
                model:users
                Component.onCompleted: currentIndex = -1
                delegate:Item{
                    height:newOrbiterSetupContainer.height *.15
                    width: parent.width
                    Rectangle{
                        anchors.fill: parent
                        radius:7.5
                        border.color:usersView.currentIndex === index ? "green" : "white"
                        color: usersView.currentIndex === index ? "green" : "black"
                        opacity: .65
                    }

                    Text {
                        text: dataTitle
                        font.pointSize: 12
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width
                        anchors.centerIn: parent
                        font.family: myFont.name
                        font.bold: true
                        color:"white"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            usersView.currentIndex = index
                            selectedUser.text = "You Selected: "+ dataTitle+ ", "+data_id
                            sUser =data_id
                        }
                        hoverEnabled: true
                    }

                }
            }

            ListView{
                id:roomsView
                height: parent.height
                width: parent.width /4
                orientation: ListView.Vertical
                model:rooms
                spacing:10
                Component.onCompleted: currentIndex = -1
                delegate:
                    Item{
                    height:newOrbiterSetupContainer.height *.15
                    width: parent.width
                    Rectangle{
                        anchors.fill: parent
                        radius:7.5
                        border.color:roomsView.currentIndex === index ? "green" : "white"
                        color: roomsView.currentIndex === index ? "green" : "black"
                        opacity: .65
                    }


                    Text {
                        text: dataTitle
                        font.pointSize: 12
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width
                        anchors.centerIn: parent
                        font.family: myFont.name
                        font.bold: true
                        color:"white"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            roomsView.currentIndex = index
                            selectedRoomText.text = "You Selected: "+dataTitle+" , "+data_id
                            selectedRoom = data_id
                        }
                        hoverEnabled: true
                    }

                }
            }

            ListView{
                id:langView
                height: parent.height
                width: parent.width/4
                orientation: ListView.Vertical
                model:languages
                spacing:10
                Component.onCompleted: currentIndex = -1
                delegate:Item{
                    height:newOrbiterSetupContainer.height *.15
                    width: parent.width
                    Rectangle{
                        anchors.fill: parent
                        radius:7.5
                        border.color:langView.currentIndex === index ? "green" : "white"
                        color: langView.currentIndex === index ? "green" : "black"
                        opacity: .65
                    }
                    Text {
                        text: dataTitle
                        font.pointSize: 12
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width
                        anchors.centerIn: parent
                        font.family: myFont.name
                        font.bold: true
                        color:"white"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            langView.currentIndex = index
                            selectedLang.text = lang
                        }
                        hoverEnabled: true
                    }
                }
            }
        }

        Text {
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
            color: "blue"
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
                onClicked: orbiterWindow.setupNewOrbiter(String(sUser), String(selectedRoom), 1, 1, appH, appW)
            }

        }
        Rectangle{
            id:exitButton
            height: scaleY(8)
            width: scaleX(14)
            color: "blue"
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

        }

        Column{
            id:confirmCol
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            height: parent.height *.15
            width: parent.width / 3
            spacing:5

            Text {
                id: selectedUser
                text: qsTr("Please Select a Default User")
                font.bold: true
                color: "white"
                font.pixelSize: 14
                font.family: myFont.name
                x:0
                onTextChanged: {
                    x = -50
                    userSelected.restart()
                }
            }
            Text {
                id: selectedRoomText
                text: qsTr("Please Select a Default Room")
                font.bold: true
                color: "white"
                font.pixelSize: 14
                font.family: myFont.name
                anchors.left: selectedUser.left
                onTextChanged: roomSelected.restart()
            }
            ParallelAnimation{
                id:userSelected

                PropertyAnimation{
                    target: selectedUser
                    property:"x"
                    to:confirmCol.width
                    duration: 2500
                }

                PropertyAnimation{
                    target:selectedUser
                    property: "opacity"
                    from:0
                    to:1
                    duration:1000
                }
            }
            ParallelAnimation{
                id:roomSelected

                PropertyAnimation{
                    target:selectedRoomText
                    property: "opacity"
                    from:0
                    to:1
                    duration:1000
                }
            }

            Text{
                id:selectedLang
                text:qsTr("Please Select a Default Language")
                font.family: myFont.name
            }


            Text {
                id: selectedResolution
                text:"Size "+newOrbiterSetupContainer.height
                font.family: myFont.name
            }
        }

    }
}
