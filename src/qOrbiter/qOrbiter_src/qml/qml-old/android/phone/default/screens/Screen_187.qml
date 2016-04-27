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
    id:ask_to_resume

    Panel{
        headerTitle: "Would you like to resume your media?"


        Flow{
            height: childrenRect.height + scaleY(1)
            width: childrenRect.width
            spacing: scaleX(1)
            anchors.centerIn: parent

            StyledButton{
                buttonText: "Yes Resume"
                onActivated:  {manager.setPosition(screenparams.getParam(188)) ;setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml") }

            }

            StyledButton{
                buttonText: "No thank you"
                onActivated: setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")

            }

            StyledButton{
                buttonText: "Yes Always Resume"
            }

            StyledButton{
                buttonText: "No thank you, Never resume"
            }

        }

    }


  }

