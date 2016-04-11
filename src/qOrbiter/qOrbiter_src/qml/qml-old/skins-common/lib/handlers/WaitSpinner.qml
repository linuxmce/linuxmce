import QtQuick 1.1

/*!
 *\class WaitSpinner
 *\brief This is a spinner to display the app is waiting for a response
 *
 *\ingroup lib_components
 *
 * The purpose of this generalize component is to provide visual feedback that something was clicked and the application is waiting for
 * a response from the DCERouter
 *
 *
 *
 */
Rectangle{
    id:waitSpinner
    height: 25
    width: 25
    color:"transparent"
    property bool activated:false/*! bool value to determine when spinner should activate */
    z:activated ? 50 : -20
    Connections{
        target:manager
        onExecuteCMD:activated = true
        onCommandCompleted:activated=false
    }

    Image{
        id:waitImage
        source: manager.imagePath+"ui3/spinner_icon.png"
        anchors.fill: parent
    }

    PropertyAnimation{
        target:waitImage
        property: "rotation"
        from:0
        to:360
        duration: 2000
        loops: Animation.Infinite
        running: activated
    }

    SequentialAnimation{
        running: activated
        loops: Animation.Infinite
       ColorAnimation { from: "white"; to: "black"; duration: 500 }
       ColorAnimation { from: "black"; to: "white"; duration: 500 }
    }

}
