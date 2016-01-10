import QtQuick 2.0
import "../components"

/*
  Screen Params for this screen
 Param:  10  Value:
  Param:  159  Value:  187
  Param:  184  Value:  device from
 Param:  186  Value:   pk_device_mediasource 23
  Param:  187  Value:  stream id 1009
  Param:  188  Value:  Position CHAPTER:0 POS:110284 TITLE:0 SUBTITLE:-1 AUDIO:-1 TOTAL:191526 QUEUE_POS:8  CHAPTER:0 POS:110284 TITLE:0 SUBTITLE:-1 AUDIO:-1 TOTAL:191526
  Param:  189  Value:  Users
  Param:  190  Value:  Pk_MediaType
  Param:  226  Value:  screen to go to after decision
  Param:  251  Value:  0
  Param:  252  Value:  0
 Param:  253  Value:  0
  */

   Rectangle {
    id:asktoresume
    width: style.orbiterW
    height: style.orbiterH
    color: style.highlight2

            Rectangle {
            id:containerrect
            height:scaleY(50)
            width: scaleX(60)
            color: style.bgcolor
            clip: true
            border.color: style.highlight1
            border.width: 2
            anchors.centerIn: parent
            radius: 10

            StyledText {
                id: text1
                width: parent.width -10
                height: scaleY(5)
                text: "Would you Like to Auto resume your media?"
                z: 2
                font.family: "Droid Sans"
                font.italic: false
                horizontalAlignment: StyledText.AlignHCenter
                font.bold: false
                color: "aliceblue"
                wrapMode: StyledText.WrapAnywhere
                font.pointSize: 18
                anchors.centerIn: containerrect
            }

            Row
            {
                height: childrenRect.height + scaleY(1)
                width: childrenRect.width
                spacing: scaleX(1)
                anchors.top: text1.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                ButtonSq{
                    buttontext: "Yes Resume"
                    buttonsqradius: 10
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {manager.setPosition(screenparams.getParam(188)) ;
                            setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
                        }
                    }
                }
                ButtonSq{
                    buttontext: "No thank you"
                    buttonsqradius: 10
                    MouseArea{
                        anchors.fill: parent
                        onClicked: setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
                    }
                }

                ButtonSq{
                    buttontext: "Yes Always Resume"
                    buttonsqradius: 10
                    MouseArea{
                        anchors.fill: parent
                       // onClicked: setCurrentScreen()
                    }
                }

                ButtonSq{
                    buttontext: "No thank you, Never resume"
                    buttonsqradius: 10
                    MouseArea{
                        anchors.fill: parent
                        //onClicked: setCurrentScreen()
                    }
                }

            }
            }

            HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
        }

