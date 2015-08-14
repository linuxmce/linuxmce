import QtQuick 2.0
import "../components"
import "../."
StyledScreen {
Component.onCompleted: console.log("SCREEN 205 OPEN!")
screen:qsTr("Need to Regenerate Orbiter")
    Panel{
       headerTitle: screen


       Panel{
           useHeader: false
           anchors.centerIn: parent
           fillColor: Style.appcolor_background_light
           width: parent.width/2
           height: parent.width/2
           Row{
               anchors.fill: parent
               StyledButton{
                   buttonText: qsTr("Regenerate Orbiter Now?")
               }
               StyledButton{
                   buttonText: qsTr("Regenerate Later (ignore)")
               }
           }

       }

    }
}

