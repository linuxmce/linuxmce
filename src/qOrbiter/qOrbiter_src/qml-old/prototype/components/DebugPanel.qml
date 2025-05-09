// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:debugpanel
    width: scaleX(15)
    height: scaleY(20)
    property string debugMessage:""
    property int messageTimeOut:6
    property bool active:false
    visible: manager.debugMode ? true : false
    onDebugMessageChanged: if(debugMessage.toString() !== ""){
                               active = true
                               x = 0+scaleX(1)                           }

    x:0-debugpanel.width


    onActiveChanged: active ? hideTimer.start() : hideTimer.stop()

    Timer{
        id:hideTimer
        interval: messageTimeOut *1000
        triggeredOnStart: false
        onTriggered:{ active = false; x = 0 - debugpanel.width}
    }

    Behavior on x{
        PropertyAnimation {duration: 750;}
    }

    gradient: Gradient{
        GradientStop{ position: 0.0; color:"red"}
        GradientStop{ position: 0.33; color:"darkred"}
    }

    Text {
        id: debuggingOutput
        text: debugMessage
        height: parent.height
        width: parent.width
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.pixelSize: scaleY(3)
    }


}
