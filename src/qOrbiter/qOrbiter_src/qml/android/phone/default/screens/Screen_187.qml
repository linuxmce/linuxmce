import QtQuick 1.0
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

StyledScreen {
    id:asktoresume


    Rectangle {
        id:containerrect
        anchors.fill: parent
        color: skinStyle.bgcolor
        clip: true
        border.color: skinStyle.highlight1
        border.width: 2
        anchors.centerIn: parent
        radius: 10
        opacity:.85
    }

    Flow{
        height: childrenRect.height + scaleY(1)
        width: childrenRect.width
        spacing: scaleX(1)

        anchors.centerIn: parent

        StyledButton{
            buttonText: "Yes Resume"
            onActivated:  {manager.setPosition(screenparams.getParam(188)) ;gotoQScreen("Screen_"+screenparams.getParam(226)+".qml") }

        }

        StyledButton{
            buttonText: "No thank you"
            onActivated: gotoQScreen("Screen_"+screenparams.getParam(226)+".qml")

        }

        StyledButton{
            buttonText: "Yes Always Resume"
        }

        StyledButton{
            buttonText: "No thank you, Never resume"
        }

    }

    Text {
        id: text1
        width: parent.width -10
        height: scaleY(5)
        text: "Would you Like to Auto resume your media?"
        z: 2
        font.family: "Droid Sans"
        font.italic: false
        horizontalAlignment: Text.AlignHCenter
        font.bold: false
        color: "aliceblue"
        wrapMode: Text.WrapAnywhere
        font.pixelSize: 18
        anchors.centerIn: containerrect
        anchors.verticalCenterOffset: 50
    }

    HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
}

