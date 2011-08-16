import QtQuick 1.0
import ScreenSaverModule 1.0
import "../components"

Item
{
    property int currentBGimg: 0
    function getBGimage(){
        var files = ["../img/backdrops/autumn/autumn.png",
                     "../img/backdrops/winter/winter.png",
                     "../img/backdrops/spring/spring.png",
                     "../img/backdrops/summer/summer.png"];
        var bgfile = files[currentBGimg];
        if (currentBGimg >= 3) {
            currentBGimg = 0;
        } else {
            currentBGimg++;
        }
        return bgfile;
    }
    function changeBGimage(){
        var newSource = getBGimage()
        var currentImg=imgBg1, newImg=imgBg2;
        if (imgBg2.opacity == 1) {
            currentImg = imgBg2;
            newImg = imgBg1;
        }
        newImg.source = newSource;
        newImg.opacity = 1;
        currentImg.opacity = 0;
    }

    Item {
        Timer {
            interval: 30000; running: true; repeat: true
            onTriggered: changeBGimage()
        }
    }
    Rectangle {
        id:stage


        signal swapStyle()
        height: style.orbiterH
        width: style.orbiterW
        opacity: 1

        Rectangle{
            id:bgrect
            anchors.fill: parent
            z:0
            color: "black"
            Image {
                id: imgBg1
                anchors.centerIn: parent
                fillMode: Image.Stretch
                //source: getBGimage()
                anchors.fill: parent
                opacity: 1
                Behavior on opacity {
                    PropertyAnimation{duration:5000}
                }
            }
            Image {
                id: imgBg2
                anchors.centerIn: parent
                fillMode: Image.Stretch
                //source: getBGimage()
                anchors.fill: parent
                opacity: 0
                Behavior on opacity {
                    PropertyAnimation{duration:5000}
                }
            }

        }
        //        Rectangle{
        //            width:  parent.width
        //            height: parent.height/9
        //            anchors.centerIn: parent
        //            color: "black"
        //        }

        // Main menu strip
        Image {
            id: imgMenu
            anchors.centerIn:  parent
            anchors.fill: parent;
            source: "../img/home/homemenu_back.png"
            fillMode:  Image.PreserveAspectFit
            smooth:  true
        }
        PathView{
            id: mainmenu
            width:  parent.width
            height: imgMenu.height/9
            anchors.centerIn: parent
            //anchors.verticalCenter: imgMenu.verticalCenter
            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5
            focus: true
            interactive: true
            Keys.enabled: true
            Keys.onLeftPressed: decrementCurrentIndex()
            Keys.onRightPressed: incrementCurrentIndex()

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
                anchors.verticalCenter: mainmenu.verticalCenter
                opacity:  PathView.isCurrentItem?1:.5
                Rectangle{
                    anchors.fill: parent;
                    color: "transparent"
                    Text{
                        text:name;
                        anchors.centerIn: parent;
                        font.family: "Helvetica";
                        font.pointSize: 36;
                        color: "white";
                        font.letterSpacing: -3;
                        smooth: true
                        //transform: Scale { // causes an offset relative to the parent
                        //    xScale: .75
                        //    yScale: 1.5
                        //}
                    }
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        console.log("Clicked on:",mainmenu.model.get(index).name)
                        mainmenu.currentIndex = index
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
                }
            }
            path: Path {
                startX: 0; startY: parent.height/2
                PathLine { x: mainmenu.width; y: parent.height/2 }
            }

        }

        // Scenarios menu strip
        Image {
            id: imgSubMenu
            //anchors.centerIn:  parent
            width: parent.width/9*8;
            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height/9*5-10
            source: "../img/home/submenubar_music.png"
            fillMode:  Image.PreserveAspectFit
            smooth:  true
        }
        ListView{
            id: submenu
            width: parent.width/9*7.6;
            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height/9*5-10
            height: imgSubMenu.height
            //anchors.verticalCenter: imgMenu.verticalCenter
            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5
            focus: true
            clip:  true
            interactive: true
            orientation: ListView.Horizontal
            Keys.enabled: true
            Keys.onLeftPressed: decrementCurrentIndex()
            Keys.onRightPressed: incrementCurrentIndex()
            //model: currentRoomLights
            model: ListModel{
                ListElement{label:"FLOORPLAN"}
                ListElement{label:"ON"}
                ListElement{label:"OFF"}
            }

            delegate: Item{
                height: submenu.height
                id: test
                width: submenu.width/8
                //anchors.verticalCenter: submenu.verticalCenter
                opacity:  ListView.isCurrentItem?1:.5
                Rectangle{
                    anchors.fill: parent;
                    color: "transparent"
                    Text{
                        text:label;
                        anchors.centerIn: parent;
                        font.family: "Helvetica";
                        font.pointSize: 18;
                        color: "white";
                        font.letterSpacing: -1;
                        smooth: true
                        font.capitalization: Font.AllUppercase
                        elide: Text.ElideRight
                        //transform: Scale { // causes an offset relative to the parent
                        //    xScale: .75
                        //    yScale: 1.5
                        //}
                    }
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        console.log("Clicked on:",index)
                        submenu.currentIndex = index
                        console.debug(parent.id)
                    }
                }
            }
        }
    }
    Component.onCompleted: changeBGimage()
}

