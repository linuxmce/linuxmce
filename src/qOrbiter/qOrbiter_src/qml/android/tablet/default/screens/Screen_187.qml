import QtQuick 1.1
import "../components"
Item{
    id:screen_187
    height: pageLoader.height
    width: pageLoader.width

    property int device_from:screenparams.getParam(184) /*!< \brief Device from */
    property int device_mediasource:screenparams.getParam(186)/*!< \brief Device's mediasource id */
    property int stream_id:screenparams.getParam(187)/*!< \brief The stream id of the media being resumed. */
    property string position:screenparams.getParam(188) /*!< \brief Position to resume to.  To be passed on to the resume function with dei */
    property int users:screenparams.getParam(189)/*!< \brief Users for the media */
    property int mediatype:screenparams.getParam(190) /*!< \brief media type of the media in question */
    property int pending_screen:screenparams.getParam(226) /*!< \brief The next screen to go to after the decision.  */

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity:.65
    }

    StyledText{
        text: qsTr("Would you like to resume your media?")
        fontSize: scaleY(8)
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row{
        height: childrenRect.height
        width: parent.width
        spacing: scaleX(2)
        StyledButton{
            buttonText: qsTr("Yes Resume")
            hitArea.onReleased: {manager.setSeekPosition(screenparams.getParam(188)) ;
                setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
            }
        }

        StyledButton{
            buttonText: qsTr("No Thank You")
            hitArea.onReleased: setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
        }

        StyledButton{
            buttonText: qsTr("Yes Always Resume")
            hitArea.onReleased: setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
        }

        StyledButton{
            buttonText: qsTr("No thank you, Never resume")
            hitArea.onReleased: setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
        }
    }

}
