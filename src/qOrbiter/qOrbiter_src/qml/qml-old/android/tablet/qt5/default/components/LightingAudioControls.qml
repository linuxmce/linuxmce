import QtQuick 2.2
import "../../../../skins-common/lib/handlers"
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0

Item{
    id:lightingAudioControls
    height:scaleY(8*3.5)
    width:(volUp.width+lightUp.width)+scaleX(4)

    
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
            height: volUp.height+2
            width: height
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
        StyledButton{
            buttonText: ""
            height: volmute.height+2
            width: height
            Image {
                id: volmute
                source: "../images/mute_up.png"
                height: scaleY(8)
                fillMode: Image.PreserveAspectFit
                width: height
                smooth:true               
            }            
            onActivated: manager.mute()
        }
        
StyledButton{
    buttonText: ""
    height: voldn.height+2
    width: height
    Image {
        id: voldn
        source: "../images/voldown_up.png"
        height: scaleY(8)
        fillMode: Image.PreserveAspectFit
        width: height
        smooth:true       
    }
    onActivated: manager.adjustVolume(-5)
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

        StyledButton{
            buttonText: ""
            height: lightUp.height+5
            width: height
            Image {
                id: lightUp
                source: "../images/lightsup_up.png"
                height: scaleY(8)
                fillMode: Image.PreserveAspectFit
                width: height
                smooth:true             
            }
            onActivated: {
                 onClicked: manager.adjustLights("+10")
            }
        }


        StyledButton{
            height: lightUp.height
            width: height
            buttonText: " F "
            onActivated: manager.setCurrentScreen("Screen_2.qml")
        }

        StyledButton{
            buttonText: ""
            height: lightdn.height+2
            width: height
            Image {
                id: lightdn
                source: "../images/lightsdown_up.png"
                height: scaleY(8)
                fillMode: Image.PreserveAspectFit
                width: height
                smooth:true               
            }
            onActivated: manager.adjustLights("-10")
        }
        

        
    }
}
