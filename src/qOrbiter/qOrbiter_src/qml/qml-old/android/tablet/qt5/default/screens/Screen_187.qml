import QtQuick 2.2
import "../components"
import org.linuxmce.screens 1.0
StyledScreen{
    id:screen_187
    property int device_from:screenparams.getParam(184) /*!< \brief Device from */
    property int device_mediasource:screenparams.getParam(186)/*!< \brief Device's mediasource id */
    property int stream_id:screenparams.getParam(187)/*!< \brief The stream id of the media being resumed. */
    property string position:screenparams.getParam(188) /*!< \brief Position to resume to.  To be passed on to the resume function with dei */
    property int users:screenparams.getParam(189)/*!< \brief Users for the media */
    property int mediatype:screenparams.getParam(190) /*!< \brief media type of the media in question */
    property int pending_screen:screenparams.getParam(226) /*!< \brief The next screen to go to after the decision.  */
    noForce: true
    keepHeader: false
    Component.onCompleted: {
        console.log("resume screen!")
    }

    StyledButton{
        id:home
        buttonText: "Home"
        onActivated: {
            noForce=false;
            manager.setCurrentScreen(Screens.Main)
        }
    }

    Panel{
        id:resume_content
        anchors.fill: parent
        headerTitle: "Resume Options"

        StyledText{
            text: qsTr("Would you like to resume your media?")
            fontSize: scaleY(8)
            anchors.top:resume_content.headerRect.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 10
        }

        Row{
            height: parent.height/2
            width: parent.width
            spacing: scaleX(2)
            anchors.bottom: parent.bottom
            anchors.bottomMargin: scaleY(15)
            StyledButton{
                buttonText: qsTr("Yes Resume")
                hitArea.onReleased: {
                    noForce=false;
                    manager.setSeekPosition(screenparams.getParam(188)) ;
                    setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
                }
            }

            StyledButton{
                buttonText: qsTr("No Thank You")
                hitArea.onReleased: {noForce=false;setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")}
            }

            StyledButton{
                buttonText: qsTr("Yes Always Resume")
                hitArea.onReleased:{ noForce=false;setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")}
            }

            StyledButton{
                buttonText: qsTr("No thank you, Never resume")
                hitArea.onReleased: {noForce=false;setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")}
            }
        }
    }




}
