import QtQuick 1.0
Item{
    id:jogpanel
 Component.onCompleted: hidetimer.running = true

 Timer{
        id:hidetimer
        repeat: false
        interval: 5000
        triggeredOnStart: false
         onTriggered: jogpanel.destroy()

    }



    Rectangle {
        width: buttonrow.width + scaleX(1)
        height: buttonrow.height + scaleY(1)
        color:style.darkhighlight
        radius: 2.5


        Row{
            id:buttonrow
            height: childrenRect.height
            width: childrenRect.width
            spacing: scaleX(1)

            AvOptionButton{
                buttontext: "-8"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { jogPosition("-480") ; hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "-4"
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {jogPosition("-240"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "-2"
                MouseArea{
                    anchors.fill: parent
                    onClicked: {jogPosition("-120"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "-30"
                MouseArea{
                    anchors.fill: parent
                    onClicked: {jogPosition("-30"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "-15"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { jogPosition("-15"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+15"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { jogPosition("+15"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+3-"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { jogPosition("+30"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+2"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { jogPosition("+120"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+4"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { jogPosition("+240"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

            AvOptionButton{
                buttontext: "+8"
                MouseArea{
                    anchors.fill: parent
                    onClicked: { jogPosition("+480"); hidetimer.running = false; hidetimer.running = true  }
                }
            }

        }
    }
}
