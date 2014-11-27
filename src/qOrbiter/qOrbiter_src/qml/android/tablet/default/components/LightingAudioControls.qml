import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0

Item{
    id:lightingAudioControls
    height:scaleY(8*3.2)
    width:scaleX(15)

    
    Rectangle{
        anchors.fill: parent
        gradient: style.buttonGradient
    }
    
    Column{
        anchors{
            top:parent.top
            left:parent.left
            right:parent.verticalCenter
            bottom: parent.bottom
            margins: 5
        }
        spacing: 10
        
        StyledButton{
            buttonText: ""
            height: volUp.height
            width: volUp.width
            onActivated: manager.adjustVolume(1)
            Image {
                id: volUp
                source: "../images/volup_up.png"
                height: scaleY(8)
                fillMode: Image.PreserveAspectFit
                width: height
                smooth:true
                
            }
        }
        
        
        Image {
            id: volmute
            source: "../images/mute_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.mute()
            }
        }
        Image {
            id: voldn
            source: "../images/voldown_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.adjustVolume(-5)
            }
        }
    }
    
    Column{
        
        anchors{
            top:parent.top
            left:parent.verticalCenter
            right:parent.right
            bottom: parent.bottom
            margins: 10
        }
        spacing:5
        
        Image {
            id: lightUp
            source: "../images/lightsup_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.adjustLights("+10")
            }
        }
        StyledButton{
            height: lightUp.height
            width: height
            buttonText: " F "
            onActivated: manager.setCurrentScreen("Screen_2.qml")
        }
        
        Image {
            id: lightdn
            source: "../images/lightsdown_up.png"
            height: scaleY(8)
            fillMode: Image.PreserveAspectFit
            width: height
            smooth:true
            MouseArea{
                anchors.fill: parent
                onClicked: manager.adjustLights("-10")
            }
        }
        
    }
}
