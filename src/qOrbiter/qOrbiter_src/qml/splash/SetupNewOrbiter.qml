// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2
 import "."
Item {
    width: appW
    height: appH

    ListModel{
        id:langMdl
        ListElement{
            name:"English"
            code:"en"
        }
        ListElement{
            name:qsTr("German")
            code:"de"
        }
    }

    property int userSelection:-1
    property int roomSelection:-1
    property int langSelection:-1

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
    Component.onCompleted: window.showSetup()

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
                text: qsTr("Setup A New Orbiter.")
                color:"white"
                font.pointSize: 18
                font.bold: true
                font.family: myFont.name
            }
        }

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
                onClicked: window.setupNewOrbiter(selectedUser, selectedRoom, 1, 1, appH, appW)
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

        }
    }
}
