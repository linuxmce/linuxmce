import QtQuick 2.2
import Qt.labs.shaders 1.0
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
    height: scaleX(15)
    width: scaleX(15)
    property bool activated:false /*! bool value to determine when spinner should activate */
    z:activated ? 50 : -20
    Connections{
        target:manager
        onExecuteCMD:activated = true
        onCommandCompleted:activated=false
    }

    PropertyAnimation{
        target:waitSpinner
        property: "rotation"
        from:0
        to:360
        duration: 1000
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
