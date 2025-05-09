import QtQuick 1.1

import "../components"
import "../js/ComponentLoader.js" as JsLib
import "../../../skins-common/lib/handlers"

Item
{
    function mainItemSelected(index){
        mainmenu.currentIndex = index
        mainMenuIndex = index
        switch (index) {
        case 0:
            submenu.model = currentRoomLights
            break
        case 1:
            submenu.model = currentRoomMedia
            break
        case 2:
            submenu.model = currentRoomClimate
            break
        case 3:
            submenu.model = currentRoomTelecom
            break
        case 4:
            submenu.model = currentRoomSecurity
            break
        }
    }
    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true
        onTriggered: nowPlayingArt.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Connections{
        target:dcenowplaying
        onPlayListPositionChanged: nowPlayingArt.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Rectangle {
        id:stage


        signal swapStyle()
       anchors.fill: parent
        color: "transparent"

        Clock{
            anchors.right: parent.right
            y: scaleY(43.89) // 316
        }

        // Main menu strip
        Image {
            id: imgMenu
            //anchors.centerIn:  parent
            //anchors.fill: parent;
            y: scaleY(41.94) // 302
            width: parent.width
            height: scaleY(20.83) // 150
            source: "../img/home/homemenu_back.png"
            //fillMode:  Image.PreserveAspectFit
            smooth:  true
        }
        PathView{
            id: mainmenu
            width:  parent.width*1.2
            anchors.horizontalCenter: parent.horizontalCenter
            //height: imgMenu.height/9
            //anchors.centerIn: parent
            y: scaleY(46) // 343 47.64
            height: scaleY(12.5) // 61 8.47
            //anchors.verticalCenter: imgMenu.verticalCenter
            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5
            focus: true
            opacity: mainmenu.activeFocus?1:.6
            interactive: true
            Keys.enabled: true
            Keys.onLeftPressed: {
                decrementCurrentIndex()
                mainItemSelected(currentIndex)
            }
            Keys.onRightPressed: {
                incrementCurrentIndex()
                mainItemSelected(currentIndex)
            }
            Keys.onDownPressed: {
                submenu.forceActiveFocus()
            }

            onMovementEnded: mainItemSelected(currentIndex)
            model: ListModel {
                ListElement { name: "LIGHTING" }
                ListElement { name: "MEDIA" }
                ListElement { name: "CLIMATE" }
                ListElement { name: "TELECOM" }
                ListElement { name: "SECURITY" }
            }
            delegate: Item{
                height: mainmenu.height
                width: mainmenu.width/5
                //anchors.verticalCenter: mainmenu.verticalCenter
                opacity:  PathView.isCurrentItem ? 1 : .6
                Rectangle{
                    anchors.fill: parent;
                    color: "transparent"
                    Text{
                        text:name;
                        anchors.centerIn: parent;
                        font.family: scoutCond.name;
                        font.pixelSize: parent.height;
                        color: "white";
                        smooth: true
                        font.bold: true

                    }
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        console.log("Clicked on:",mainmenu.model.get(index).name)
                        mainItemSelected(index)
                    }
                }
            }
            path: Path {
                startX: 0; startY: mainmenu.height/2
                PathLine { x: mainmenu.width; y: mainmenu.height/2 }
            }

        }
        Image {
            id: imgMenuGradient
            anchors.fill: imgMenu;
            source: "../img/home/homemenu_gradient.png"
            //fillMode:  Image.PreserveAspectFit
            smooth:  true
        }

        // Scenarios menu strip
        Image {
            id: imgSubMenu
            x: scaleX(6.17) // 79
            y: scaleY(56.39) // 406
            width: scaleX(87.73) // 1123
            height: scaleY(4.58) // 33
            source: "../img/home/submenubar_music.png"
            smooth:  true
        }
        ListView{
            id: submenu
            height: imgSubMenu.height
            width: imgSubMenu.width*.922
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: imgSubMenu.verticalCenter
            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5
            clip:  true
            interactive: true
            orientation: ListView.Horizontal
            Keys.enabled: true
            Keys.onLeftPressed: decrementCurrentIndex()
            Keys.onRightPressed: incrementCurrentIndex()
            Keys.onUpPressed: {
                mainmenu.forceActiveFocus()
            }
            Keys.onEnterPressed: console.log("Clicked on:",currentIndex)
            model: ListModel{
                ListElement{label:"FLOORPLAN"}
                ListElement{label:"ON"}
                ListElement{label:"OFF"}
            }

            delegate: Item{
                height: submenu.height
                id: test
                width: submenu.width/8
                opacity:  (submenu.activeFocus && ListView.isCurrentItem) ? 1 : .5
                Rectangle{
                    anchors.fill: parent;
                    color: "transparent"
                    Text{
                        text:title
                        anchors.centerIn: parent;
                        font.family: aeonRss.name;
                        font.pixelSize: parent.height/2;
                        color: "white";
                        smooth: true
                    }
                }
                ScenarioButtonHandler{
                    onReleased: {
                        console.log("Clicked on:",index)
                        submenu.currentIndex = index
                    }
                }
            }
        }

        // Now Playing
        Image{
            id: nowPlaying
            x: 0
            y: scaleY(33.33) // 204
            width:  parent.width // 1280
            height: scaleY(87.5) // 630
            //anchors.bottom: parent.bottom
            source:  "../img/home/nowplaying_shadow.png"
            //fillMode:  Image.PreserveAspectFit
            smooth:  true
            visible: dcenowplaying.b_mediaPlaying
            Image{
                x: scaleX(1.4) // 18
                y: scaleY(28.3) // 446 242
                height:  scaleY(36.39) // 262 36.39
                width:  scaleY(36.39) // 262 20.5
                source: "../img/home/nowplaying_back.png"
                //fillMode:  Image.PreserveAspectFit
                smooth:  true
                Image{
                    x: scaleY(33.75) // 261 243
                    y: scaleY(8.333) // 506 60
                    height:  scaleY(24.4) // 176
                    width:  scaleY(35.97) // 259
                    source: "../img/home/nowplaying_info.png"
                    //fillMode:  Image.PreserveAspectFit
                    smooth:  true
                }

                Rectangle{
                    anchors.centerIn: parent
                    width:  parent.width*.90
                    height: parent.height*.91
                    radius: 6
                    //color: "white"
                    color: "transparent"
                    Image{ // Album art here
                        id: nowPlayingArt
                        anchors.fill: parent
                        source: "image://updateobject/"+securityvideo.timestamp
                        fillMode:  Image.PreserveAspectFit
                        smooth:  true
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked:setCurrentScreen(dcenowplaying.qs_screen)
                    }
                }
            }

            Image{
                x: scaleX(2.2)
                y: scaleY(29.7)
                height:  scaleY(33.61)
                width:  scaleY(33.61)
                source: "../img/home/nowplaying_overlay.png"
                //fillMode:  Image.PreserveAspectFit
                smooth:  true
            }
            Text{
                id: nowPlayingArtist
                text: dcenowplaying.qs_mainTitle
                x: scaleY(41.11)
                y: scaleY(32.78) // 478 236
                font.family: aeonNowPlaying.name
                //font.capitalization: Font.AllUppercase
                smooth: true
                font.pixelSize: scaleY(2.92) // 21
                color: "white"
            }
            Text{
                id: nowPlayingAlbum
                text: dcenowplaying.qs_mainTitle2
                x: scaleY(41.11)
                y: scaleY(37.22) // 510 268
                font.family: aeonNowPlaying.name
                //font.capitalization: Font.AllUppercase
                smooth: true
                font.pixelSize: scaleY(2.22) // 16
                opacity:  .7
                color: "white"
            }
            Text{
                id: nowPlayingStatus
                text: "NOW PLAYING"
                x: scaleY(41.11)
                y: scaleY(54.44) // 634 392
                font.family: aeonNowPlaying.name
                smooth: true
                font.pixelSize: scaleY(2.22) // 16
                opacity:  .7
                color:  "#749a9a"
            }
            Text{
                id: nowPlayingTrack
                text: dcenowplaying.qs_subTitle
                x: scaleY(41.11)
                y: scaleY(56.67) // 650 408
                font.family: aeonNowPlaying.name
                //font.capitalization: Font.AllUppercase
                smooth: true
                font.pixelSize: scaleY(3.06) // 22
                opacity:  .7
                color: "white"
            }
        }
        // Bottom options menu
        Image {
            id: mnuBottomRight
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            source:  "../img/common/timepanel_mid.png"
            width: scaleX(35.23) // 451
            height:  scaleY(3.75) // 27
            smooth:  true

            Rectangle {
                id: recAdvanced
                height:  parent.height*.7
                width:  txtAdvanced.width
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: lblRoom.left
                anchors.rightMargin: scaleX(.78)
                color: "transparent"
                border.width: 0
                Text {
                    id: txtAdvanced
                    anchors.verticalCenter: parent.verticalCenter
                    text:  "Advanced"
                    font.family: scout.name;
                    font.pixelSize: parent.height;
                    color: "white";
                    smooth: true
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: manager.setCurrentScreen("Screen_44.qml")
                }
            }

            Text{
                id:lblRoom
                text:"Room:"
                font.family: scout.name;
                font.pixelSize: parent.height*.7;
                color: "white";
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: txtRoom.left
                anchors.rightMargin: scaleX(.78)
            }
            Text{
                id:txtRoom
                text:currentroom.length ? currentroom : "--"
                font.family: scout.name;
                font.pixelSize: parent.height*.7;
                color: "#e5e5e5";
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: lblUser.left
                anchors.rightMargin: scaleX(.78)
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        JsLib.loadComponent(stage,"RoomSelector.qml")
                    }
                }
            }
            Text{
                id:lblUser
                text:"User: "
                font.family: scout.name;
                font.pixelSize: parent.height*.7;
                color: "white";
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: txtUser.left
                anchors.rightMargin: scaleX(.78)
            }
            Text {
                id:txtUser
                text:currentuser.length ? currentuser : "--"
                font.family: scout.name;
                font.pixelSize: parent.height*.7;
                color: "#e5e5e5";
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: scaleX(.78)
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        JsLib.loadComponent(stage,"UserSelector.qml")
                    }
                }
            }

        }

    }
    Component.onCompleted: {
        setNowPlayingData()
        mainmenu.forceActiveFocus()
        mainItemSelected(mainMenuIndex)
    }
}

