import QtQuick 2.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
Rectangle {

    // property alias synStyledText:
    id: dvdmenu
    height: manager.appHeight
    width: manager.appWidth
    radius: 0
    opacity: 1
    color: "transparent"

    Image {
        id: bgimage
        source: "../img/icons/orbiterbg.png"
    }

    Timer{
        id:streamtimer
        repeat: true
        interval: 1000
        triggeredOnStart: true
        running: true
        onTriggered: manager.grabStreamImage()
    }

    Connections{
        target:dcenowplaying        
        onStreamImageChanged: { nowplayingimage.source = "image://listprovider/stream/"+securityvideo.timestamp;

        }
    }

    Component.onCompleted:setNowPlayingData()


    //main 'now playing rect containing all the other items
    Image {
        id: panelimg
        source: "../img/icons/displaypanel.png"
        height: containerrect.height
        width:containerrect.width
        anchors.centerIn: containerrect
    }

    Rectangle{
        id:containerrect
        height:childrenRect.height + scaleY(5)
        width: childrenRect.width + scaleX(4)
        clip:true
        radius: 10
        border.color: style.highlight1
        border.width: 3
        color: "transparent"
        anchors.centerIn: parent

        Row{
            id:mainrow
            height: childrenRect.height
            width: childrenRect.width
            spacing: scaleX(1)
            anchors.top:parent.top
            anchors.topMargin: scaleY(1.5)
            anchors.horizontalCenter: parent.horizontalCenter

            Row {
                id:metarow
                width: childrenRect.width
                height: childrenRect.height
                spacing: scaleX(1)


                Image {
                    id: nowplayingimage
                    width: scaleX(65)
                    height:scaleY(65)
                    fillMode: Image.PreserveAspectFit
                    source: ""
                }

                Remote_lighting_controls{ id: remote_lighting_controls1; }
                Remote_Audio_controls{ id: remote1; }
            }
        }

        Row{
            id:controlrow
            anchors.top: mainrow.bottom
            anchors.topMargin: scaleY(2)
            height: childrenRect.height
            width: childrenRect.width
            anchors.horizontalCenter: parent.horizontalCenter
            Column{
                height: childrenRect.height
                width: childrenRect.width
                spacing: scaleY(1.5)

                NavigationArrows {
                    id: navarrows
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Row{
                    height: childrenRect.height
                    width: childrenRect.width
                    spacing: scaleX(1)
                    AvOptionButton{
                        buttontext: qsTr("Zoom & Aspect")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createAvComponent("../components/ZoomAspect.qml", dvdmenu)
                            }
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Bookmarks")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Resend AV Codes")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createAvComponent("../components/Avcodes.qml", dvdmenu)
                            }
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Menu")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: showMenu()
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Dvd Options")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Copy Disc")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Bookmarks")
                    }
                    AvOptionButton{
                        buttontext: qsTr("View Attributes")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Thumbnail")
                    }

                    AvOptionButton{
                        buttontext: qsTr("Power")
                    }
                    HomeButton{}
                }
            }
        }
    }
}

