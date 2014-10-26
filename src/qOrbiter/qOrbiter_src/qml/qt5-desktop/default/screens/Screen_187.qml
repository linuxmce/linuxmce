import QtQuick 2.1
import "../components"

Item {
    id:asktoresume
    height: manager.appHeight
    width: manager.appWidth
    onActiveFocusChanged: {
        if(activeFocus){
            resume_options.forceActiveFocus()
            console.log("Passed focus to button row.")
        }
    }

    Component.onCompleted: {
        resume_options.forceActiveFocus()
        console.log("Passed focus to button row.")
    }

    property int device_from:screenparams.getParam(184) /*!< \brief Device from */
    property int device_mediasource:screenparams.getParam(186)/*!< \brief Device's mediasource id */
    property int stream_id:screenparams.getParam(187)/*!< \brief The stream id of the media being resumed. */
    property string position:screenparams.getParam(188) /*!< \brief Position to resume to.  To be passed on to the resume function with dei */
    property int users:screenparams.getParam(189)/*!< \brief Users for the media */
    property int mediatype:screenparams.getParam(190) /*!< \brief media type of the media in question */
    property int pending_screen:screenparams.getParam(226) /*!< \brief The next screen to go to after the decision.  */


    Rectangle {
        id: phil
        anchors.fill: parent
        color:"black"
        opacity:.75
    }

    StyledText {
        id: text1
        width: parent.width -10
        height: scaleY(5)
        text: "Would you Like to Auto resume your media?"
        z: 2
        horizontalAlignment: Text.AlignHCenter
        font.bold: false
        color: "aliceblue"
        wrapMode: Text.WrapAnywhere
        font.pixelSize: 42
        anchors.bottom: resume_options.top
    }

    FocusRow{
        id:resume_options
        anchors.centerIn: parent
        width: parent.width

        RowButton{
            text:"Yes Resume."
            onSelect: {manager.setSeekPosition(screenparams.getParam(188)) ;
                setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
            }
        }
        RowButton{
            text:"No Thank You."
            onSelect:  setCurrentScreen("Screen_"+screenparams.getParam(226)+".qml")
        }
    }
}

