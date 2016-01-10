import QtQuick 1.1
import "../components"
/*!
 *\brief Resume media screen.
 *
 *\ingroup qml_desktop_default
 *
 *When a user starts a file that is marked with resume, this shows on relevant orbiters. The parameters for the options
 are passed as screen parameters. see properties for the screen param values
 */


Rectangle {
    id:asktoresume
    height: manager.appHeight
    width: manager.appWidth
    color: skinStyle.highlight2
    property int device_from:screenparams.getParam(184) /*!< \brief Device from */
    property int device_mediasource:screenparams.getParam(186)/*!< \brief Device's mediasource id */
    property int stream_id:screenparams.getParam(187)/*!< \brief The stream id of the media being resumed. */
    property string position:screenparams.getParam(188) /*!< \brief Position to resume to.  To be passed on to the resume function with dei */
    property int users:screenparams.getParam(189)/*!< \brief Users for the media */
    property int mediatype:screenparams.getParam(190) /*!< \brief media type of the media in question */
    property int pending_screen:screenparams.getParam(226) /*!< \brief The next screen to go to after the decision.  */

    Rectangle {
        id:containerrect
        height:scaleY(50)
        width: scaleX(60)
        color: skinStyle.bgcolor
        clip: true
        border.color: skinStyle.highlight1
        border.width: 2
        anchors.centerIn: parent
        radius: 10

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
                    onClicked: {manager.setSeekPosition(screenparams.getParam(188)) ;
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

