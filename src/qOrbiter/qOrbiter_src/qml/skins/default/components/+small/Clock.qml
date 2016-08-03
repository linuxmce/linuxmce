import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import "../."
import "../../."

/*! QML based screensaver optimized for qt5 */
Item{
    id:clock
    clip:false
    property bool longDate:true
    property bool verticalMirror: false
    property color clockColor:"white"
    property bool boldClock:false

    width:parent.width
    height: 50
    opacity: qmlRoot.screensaverActive ? .75 : 0
    function getDate(){
        if(!longDate)
            return  new Date().toTimeString()
        else
            return Qt.formatDateTime(new Date(), "dddd ,MMMM d yyyy \t hh:mm:ss ");
    }


    function getTime(){
        return Qt.formatDateTime(new Date(), "hh:mm:ss");
    }

    function getShortDate(){
        return Qt.formatDateTime(new Date(), "dddd ,MMMM d yyyy");
    }

    anchors{
        bottom:parent.bottom
        bottomMargin: Style.scaleX(5)
        left:parent.left
    }

    Behavior on opacity{
        PropertyAnimation{
            duration: Style.transitionFadeTime
        }
    }

    Timer { // Update the clock element periodically
        id:clockTimer
        interval: 1000; running: clock.opacity!==0; repeat: true
        onTriggered:{ txtDate.text = clock.getShortDate(); txtTime.text = clock.getTime(); }
    }
    Rectangle{
        opacity: clockTimer.running ? .55 : 0
        color:"black"

        height: temporalid.height
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }

        PropertyAnimation on opacity {
            duration: Style.transition_animationTime
        }
    }
    Column{
        id:temporalid
        height:childrenRect.height
        width:parent.width*.65

        anchors{
            right:parent.right
            bottom:parent.bottom
        }

        StyledText{
            id: txtDate
            color: clock.clockColor
            font.letterSpacing: 2
            smooth: true
            width: parent.width
            fontSize: txtTime.fontSize /2
            horizontalAlignment: Text.AlignHCenter
            opacity: .65
            font.weight: Font.Bold
        }

        StyledText{
            id: txtTime
            color: clock.clockColor
            font.letterSpacing: 2
            smooth: true
            width: parent.width
            fontSize: Style.appFontSize_clock * 3
            font.weight: Font.Light
            horizontalAlignment: Text.AlignHCenter
            opacity: .65
        }

    }
}
