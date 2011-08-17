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

    function getDate(){
        var d = new Date();
        return d.format("dddd, mmmm d, yyyy | h:MM TT");
    }
    function mainItemSelected(index){
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
    // Load some custom fonts
    FontLoader {
        id: scout
        source: "../fonts/scout.ttf"
    }
    FontLoader {
        id: scoutLight
        source: "../fonts/scoutlight.ttf"
    }
    FontLoader {
        id: scoutCond
        source: "../fonts/scoutcond.ttf"
    }
    FontLoader {
        id: aeonRss
        source: "../fonts/aeon_rss.ttf"
    }

    Timer { // Simulate a simple PhotoScreensaver
        interval: 15000; running: true; repeat: true
        onTriggered: changeBGimage()
    }
    Timer { // Update the clock element periodically
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
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
                    PropertyAnimation{duration:3000}
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
                    PropertyAnimation{duration:3000}
                }
            }

        }
        // Clock
        Image {
            id: imgClockBg
            anchors.right: parent.right
            source:  "../img/common/timepanel_mid.png"
            y: parent.height*.405
            scale: .67
            transformOrigin: Item.Right
            smooth:  true
        }
        Text{
            id: txtDate
            text: getDate()
            anchors.rightMargin: 10
            font.family: "scout";
            font.pixelSize: imgClockBg.height*.6;
            color: "white";
            smooth: true
            anchors.right: parent.right
            anchors.verticalCenter: imgClockBg.verticalCenter

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
                anchors.verticalCenter: mainmenu.verticalCenter
                opacity:  (mainmenu.activeFocus && PathView.isCurrentItem) ? 1 : .5
                Rectangle{
                    anchors.fill: parent;
                    color: "transparent"
                    Text{
                        text:name;
                        anchors.centerIn: parent;
                        font.family: "scoutCond";
                        font.pixelSize: parent.height;
                        color: "white";
                        //font.letterSpacing: -3;
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
                        mainItemSelected(index)
                    }
                }
            }
            path: Path {
                startX: 0; startY: mainmenu.height/2
                PathLine { x: mainmenu.width; y: mainmenu.height/2 }
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
            //focus: true
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
                //anchors.verticalCenter: submenu.verticalCenter
                opacity:  (submenu.activeFocus && ListView.isCurrentItem) ? 1 : .5
                Rectangle{
                    anchors.fill: parent;
                    color: "transparent"
                    Text{
                        text:label;
                        anchors.centerIn: parent;
                        font.family: "aeonRss";
                        font.pixelSize: parent.height/2;
                        color: "white";
                        smooth: true
                        //font.capitalization: Font.AllUppercase
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
                        execGrp(params)
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        mainmenu.forceActiveFocus()
        changeBGimage()
    }
}

