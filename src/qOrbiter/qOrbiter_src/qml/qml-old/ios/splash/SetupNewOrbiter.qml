// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2

Rectangle {
    width: manager.appWidth
    height: manager.appHeight
    color: "transparent"
    //palette?
    property string orangeRed: "#993300"
    property string deYork: "#99CC99"
    property string midnightBlue: "#003366"
    //---------
    property int startButtonHeight:manager.isProfile ? manager.appHeight*.15 : manager.appHeight *.20
    property int startButtonWidth:manager.isProfile ? manager.appWidth *.20 : manager.appWidth *.15
    property int itemFontSize: manager.isProfile ? manager.appHeight *.04 : manager.appHeight *.05
    property int selectedUser:-1
    property int selectedRoom:-1
    Component.onCompleted: orbiterWindow.showSetup()


    Timer{
        id:effectTimer
        interval: 250
        running:true
        onTriggered: {
            newOrbiterSetupContainer.opacity=1;
            newOrbiterSetupContainer.scale =1
        }
    }

    Rectangle{
        id:newOrbiterSetupContainer
        height: parent.height*.90
        width: parent.width *.90
        color:"transparent"

        radius: 10
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
                duration:500
            }
        }
        Row{
            height: childrenRect.height
            width: parent.width
            Text {
                id: welcome
                text: qsTr("Setup A New Orbiter - Choose user and room, then select go.")
                color:"white"
                font.pointSize: 18
                font.bold: true
                font.family: myFont.name
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }

        Column{
            id:contentColumn
            height: parent.height *.55
            width: parent.width *.75
            anchors.centerIn: newOrbiterSetupContainer
            spacing: 75

            ListView{
                id:usersView
                height: 125
                clip:true
                width: parent.width
                orientation: ListView.Horizontal
                spacing: 20
                model:users
                Component.onCompleted: currentIndex = -1
                delegate:Rectangle{
                    height:startButtonHeight
                    width:startButtonWidth
                    radius:10
                    border.color:usersView.currentIndex === index ? midnightBlue : orangeRed
                    color: usersView.currentIndex === index ? deYork : "white"
                    Text {
                        text: dataTitle
                        font.pointSize: itemFontSize
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        anchors.centerIn: parent
                        font.family: myFont.name
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            usersView.currentIndex = index
                            selectedUser=index
                        }
                        hoverEnabled: true
                    }

                }
            }

            ListView{
                id:roomsView
                height:manager.isProfile ? manager.appHeight*.33 :manager.appHeight *.25
                width: parent.width
                orientation: ListView.Horizontal
                model:rooms
                contentHeight: startButtonHeight
                contentWidth: startButtonWidth
                spacing:20
                Component.onCompleted: currentIndex = -1
                delegate:
                    Rectangle{
                    height:startButtonHeight
                    width: startButtonWidth
                    radius:10
                    border.color:roomsView.currentIndex === index ? midnightBlue : orangeRed
                    color: roomsView.currentIndex === index ? deYork : "white"
                    Text {
                        text: dataTitle
                        font.pointSize: infoTextSize
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width *.75
                        anchors.centerIn: parent
                        font.family: myFont.name
                        font.pixelSize: itemFontSize
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            roomsView.currentIndex = index
                            selectedRoom=index;
                        }
                        hoverEnabled: true
                    }

                }
            }

            ListView{
                id:langView
                height: 75
                width: parent.width
                orientation: ListView.Horizontal
                model:languages
                spacing:20
                Component.onCompleted: currentIndex = -1
                delegate:Rectangle{
                    height:newOrbiterSetupContainer.height *.15
                    width: newOrbiterSetupContainer.width *.09
                    radius:10
                    border.color:langView.currentIndex === index ? midnightBlue : orangeRed
                    color: langView.currentIndex === index ? deYork : "white"

                    Text {
                        text: lang
                        font.pointSize: infoTextSize
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width *.75
                        anchors.centerIn: parent
                        font.family: myFont.name
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

        //        Text {
        //            id: logo
        //            text: qsTr("LinuxMCE")
        //            anchors.bottom: parent.bottom
        //            anchors.right: parent.right
        //            anchors.rightMargin: 10
        //            font.bold: true
        //            color:"white"
        //            font.pointSize: 12
        //            font.family: myFont.name
        //        }

        Rectangle{
            id:setupButton
            height: scaleY(8)
            width: scaleX(14)
            color: "white"
            radius: 8
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            Text{
                text:"Go!"
                anchors.centerIn: parent
                color:"green"
                font.family: myFont.name
            }
            MouseArea{
                anchors.fill: parent
                onClicked: orbiterWindow.setupNewOrbiter(selectedUser, selectedRoom, 1, 1, appH, appW)
            }

        }
        Rectangle{
            id:exitButton
            height: scaleY(8)
            width: scaleX(14)
            color: "white"
            radius: 8
            anchors.bottom: parent.bottom
            anchors.left: setupButton.right
            anchors.leftMargin: scaleX(2)

            Text{
                text:"Exit!"
                anchors.centerIn: parent
                color:"red"
                font.family: myFont.name
                font.pixelSize: infoTextSize
            }
            MouseArea{
                anchors.fill: parent
                onClicked: pageLoader.source ="SplashView.qml"       //screenChange("SplashView.qml")
            }

        }

        //        Column{
        //            id:confirmCol
        //            anchors.bottom: parent.bottom
        //            anchors.left: parent.left
        //            height: parent.height *.15
        //            width: parent.width / 3
        //            spacing:5

        //                Text {
        //                    id: selectedUser
        //                    text: qsTr("Please Select a Default User")
        //                    font.bold: true
        //                    color: deYork
        //                    font.pixelSize: 14
        //                    font.family: myFont.name
        //                    x:0
        //                    onTextChanged: {
        //                        x = -50
        //                        userSelected.restart()
        //                    }
        //                }
        //                ParallelAnimation{
        //                    id:userSelected

        //                    PropertyAnimation{
        //                        target: selectedUser
        //                        property:"x"
        //                        to:confirmCol.width
        //                        duration: 2500
        //                    }

        //                    PropertyAnimation{
        //                        target:selectedUser
        //                        property: "opacity"
        //                        from:0
        //                        to:1
        //                        duration:1000
        //                    }
        //                }


        //                Text {
        //                    id: selectedRoom
        //                    text: qsTr("Please Select A Default Room")
        //                    font.family: myFont.name
        //                }



        //                Text{
        //                    id:selectedLang
        //                    text:qsTr("Please Select a Default Language")
        //                    font.family: myFont.name
        //                }


        //                Text {
        //                    id: selectedResolution
        //                    text:"Size "+newOrbiterSetupContainer.height
        //                    font.family: myFont.name
        //                }
        //        }

    }
}
