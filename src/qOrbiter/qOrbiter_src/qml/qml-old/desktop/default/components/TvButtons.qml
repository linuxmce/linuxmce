import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"
import "../../../skins-common/lib/handlers"


Row{
    spacing:scaleY(1)
    anchors.horizontalCenter: parent.horizontalCenter
    AvOptionButton{
        id:zoom
        buttontext: qsTr("Zoom & Aspect")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/ZoomAspect.qml",satcableboxremote )
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("PVR Menu")
        MouseArea{
            anchors.fill: parent
            onClicked: manager.extraButtonPressed("pvrmenu")
        }
    }
    AvOptionButton{
        buttontext: qsTr("Resend AV Codes")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/Avcodes.qml",satcableboxremote )
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Thumbnail")
        MouseArea{
            anchors.fill: parent
            onClicked: manager.grabFileImage()
        }

    }
    Flickable{
        height: zoom.height
        width: scaleX(35)
        flickableDirection: "HorizontalFlick"
        clip:true
        boundsBehavior: "DragAndOvershootBounds"
        
        contentWidth: scaleX(100)
        Row{
            width:scaleX(100)
            height: parent.height
            
            AvOptionButton{
                buttontext: qsTr("Live")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("livetv")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Schedule")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("schedule")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Recordings")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("recordings")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Music")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("music")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Guide")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("guide")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Menu")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("menu")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Info")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("info")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Exit")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("exit")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Favorites")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("favorites")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Record")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("record")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Help")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("help")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Bookmark Channel")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("channelbookmark")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Change Inputs")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("changeinputs")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Page Up")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("pageup")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Page Down")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("pagedown")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Next Day")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("nextday")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Previous day")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtonPressed("previousday")
                }
            }
        }
        
    }
    AvOptionButton{
        buttontext: qsTr("Power")
        MouseArea{
            anchors.fill: parent
            onClicked: manager.stopMedia()
        }
    }
    HomeButton{}
}
