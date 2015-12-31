// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: appW
    height: appH
    color: "slategrey"
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

    Rectangle{
        id:newOrbiterSetupContainer
        height: parent.height*.90
        width: parent.width *.90
        color:"transparent"

        gradient: Gradient{
            GradientStop{
                position: 0.0
                color:midnightBlue
            }
            GradientStop{
                position:.15
                color: midnightBlue
            }
            GradientStop{
                position:.16
                color:"white"
            }
            GradientStop{
                position:.85
                color:"white"
            }
            GradientStop{
                position:.86
                color:midnightBlue
            }
        }

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
            Text {
                id: welcome
                text: qsTr("Setup A New Orbiter")
                color:"white"
                font.pointSize: 18
                font.bold: true
            }
            Rectangle{
                height: scaleX(14)
                width: scaleX(14)
                color: "blue"
                Text{
                    text:"Go!"
                    anchors.centerIn: parent
                    color:"white"

                }

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
                height: 75
                width: parent.width
                orientation: ListView.Horizontal
                spacing: 20
                model:users
                Component.onCompleted: currentIndex = -1
                delegate:Rectangle{
                    height:newOrbiterSetupContainer.height *.15
                    width: newOrbiterSetupContainer.width *.09
                    radius:10
                    border.color:usersView.currentIndex === index ? midnightBlue : orangeRed
                    color: usersView.currentIndex === index ? deYork : "white"
                    Text {
                        text: dataTitle
                        font.pointSize: 12
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width *.75
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            usersView.currentIndex = index
                            selectedUser.text = "You Selected: "+ dataTitle
                        }
                        hoverEnabled: true
                    }

                }
            }

            ListView{
                id:roomsView
                height: 75
                width: parent.width
                orientation: ListView.Horizontal
                model:rooms
                contentHeight: newOrbiterSetupContainer.height *.15
                contentWidth: newOrbiterSetupContainer.width *.09
                spacing:20
                  Component.onCompleted: currentIndex = -1
                delegate:
                    Rectangle{
                    height:newOrbiterSetupContainer.height *.15
                    width: newOrbiterSetupContainer.width *.09
                    radius:10
                    border.color:roomsView.currentIndex === index ? midnightBlue : orangeRed
                    color: roomsView.currentIndex === index ? deYork : "white"
                    Text {
                        text: dataTitle
                        font.pointSize: 12
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width *.75
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            roomsView.currentIndex = index
                            selectedRoom.text = dataTitle
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
                        font.pointSize: 12
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        width: parent.width *.75
                        anchors.centerIn: parent
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
                    color: deYork
                    font.pixelSize: 14
                    x:0
                    onTextChanged: {
                        x = -50
                        userSelected.restart()
                    }
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


                Text {
                    id: selectedRoom
                    text: qsTr("Please Select A Default Room")
                }



                Text{
                    id:selectedLang
                    text:qsTr("Please Select a Default Language")
                }


                Text {
                    id: selectedResolution
                    text:"Size "+newOrbiterSetupContainer.height
                }
        }

    }
}
