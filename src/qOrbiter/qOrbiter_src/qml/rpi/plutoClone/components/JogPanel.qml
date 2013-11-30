import QtQuick 2.0
//import Qt.labs.shaders 1.0
import "../effects"

Item{
    id:jogpanel
 Component.onCompleted: hidetimer.running = true
 anchors.bottom: parent.bottom
 anchors.bottomMargin: scaleY(15)
 anchors.horizontalCenter: parent.horizontalCenter
 height: buttonContainer.height
 width: buttonContainer.width

SequentialAnimation{
    id:opacityAnimation
    running:false
    PropertyAnimation{
        target: buttonContainer
        property:"opacity"
        to:0
        duration:2500
    }
    ScriptAction{script:jogpanel.destroy()}

}


 Timer{
        id:hidetimer
        repeat: false
        interval: 5000
        triggeredOnStart: false
         onTriggered: opacityAnimation.start()
    }



    Rectangle {
        id:buttonContainer
        width: buttonrow.width + scaleX(1)
        height: buttonrow.height + scaleY(1)
        color:"transparent"
        opacity: 0
        Component.onCompleted:  PropertyAnimation {

            target:buttonContainer
            property:"opacity"
            to:1
            duration: 2500
        }

        Row{
            id:buttonrow
            height: childrenRect.height
            width: childrenRect.width
            spacing: scaleX(1)
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: hidetimer.restart()
            }

            AvOptionButton{
                buttontext: "-8"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { manager.JogStream("-480") ; hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "-4"
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {manager.JogStream("-240"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "-2"
                MouseArea{
                    anchors.fill: parent
                    onClicked: {manager.JogStream("-120"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "-30"
                MouseArea{
                    anchors.fill: parent
                    onClicked: {manager.JogStream("-30"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "-15"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { manager.JogStream("-15"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+15"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { manager.JogStream("+15"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+3-"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { manager.JogStream("+30"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+2"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { manager.JogStream("+120"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+4"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { manager.JogStream("+240"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+8"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { manager.JogStream("+480"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

        }
    }
}
