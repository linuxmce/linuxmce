import QtQuick 1.1
import "components"
//import "../js/ComponentLoader.js" as MyJs
//import "../js/dateFormat.js" as DateFunctions

Item {
    id: item
    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    property string locationinfo: "standby"
    property string screenfile
    property string screenerror
    property int mainMenuIndex

    // Style {id:style}
    width: style.orbiterW; //RayBe
    height: style.orbiterH; //RayBe

    function scaleX(x){
        return x/100*style.orbiterW
    }
    function scaleY(y){
        return y/100*style.orbiterH
    }


    function screenchange(screenname )
    {
        pageLoader.source = "screens/"+screenname
        if (pageLoader.status == 1)
        {
            console.log("Command to change to:" + screenname+ " was successfull")
        }
        else
        {
            console.log("Command to change to:" + screenname + " failed!")
            screenfile = screenname
            //screenerror = pageLoader.item.e
            pageLoader.source = "screens/Screen_x.qml"
        }
    }

//***---screen_1---***

    function mainItemSelected(index){
        //mainmenu.currentIndex = index
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
        //signal swapStyle() //RayBe denk iets van screensaver
        height: style.orbiterH
        width: style.orbiterW
        color: "lightgrey" //RayBe: was transparent


        //----------

            Column{
                id:mainNavigationColumn
                height:childrenRect.height //+5 //RayBe
                width: childrenRect.width
                //anchors.centerIn: panelbg //RayBe
                y: scaleY(10)
                x: scaleX(1)
                spacing:0
                clip: false

                ButtonMain{
                    buttontext: "Advance"
                    //buttontextfontsize: 14//RayBe
                    height: scaleY(8)
                    width:  scaleX(13)
/*
                    Image {
                        id: buttonImageAdvance
                        //source: "img/ui3/homebig.png"
                        anchors.centerIn: parent
                        height: scaleY(13)
                        width:  scaleX(8)
                    }
*/
                    MouseArea{
                        anchors.fill: parent
                        //onClicked:setCurrentScreen("Screen_1.qml")
                        onClicked: {
                            console.log("RayBe you have Clicked on: Advance")
                            setCurrentScreen("Screen_44.qml")
                        }
                    }
                }

                ButtonMain{
                    buttontext: "Lights"
                    //buttontextfontsize: 14
                    height: scaleY(8)
                    width:  scaleX(13)

                    //Trying to make the selected button have a highlight for as long as its active
                    Rectangle{
                        id: lightButtonSelect
                        color: "yellow"
                        anchors.fill: parent
                        opacity: mainMenuIndex.valueOf(0)  ? 0 : .1
                    }

                    Image {
                        id: buttonImageLights
                        //source: "img/ui3/homebig.png"
                        anchors.centerIn: parent
                        height: scaleY(13)
                        width:  scaleX(8)
                    }
                    MouseArea{
                        anchors.fill: parent
                        //onClicked:setCurrentScreen("Screen_1.qml")
                        onClicked: {
                            console.log("RayBe you have Clicked on: Lights")
                            mainItemSelected(0)
                        }
                    }
                }

                ButtonMain{
                    buttontext: "Media"
                    //buttontextfontsize: 14
                    height: scaleY(8)
                    width:  scaleX(13)

                    Rectangle{
                        id: mediaButtonSelect
                        color: "purple"
                        anchors.fill: parent
                        opacity: mainMenuIndex.valueOf(1)  ? 0 : .1
                    }

                    Image {
                        id: buttonImageMedia
                        //source: "img/ui3/homebig.png"
                        anchors.centerIn: parent
                        height: scaleY(13)
                        width:  scaleX(8)
                    }
                    MouseArea{
                        anchors.fill: parent
                        //onClicked:setCurrentScreen("Screen_1.qml")
                        onClicked: {
                            console.log("RayBe you have Clicked on: Media")
                            mainItemSelected(1)
                        }
                    }
                }

                ButtonMain{
                    buttontext: "Climate"
                    //buttontextfontsize: 14
                    height: scaleY(8)
                    width:  scaleX(13)

                    Rectangle{
                        id: climateButtonSelect
                        color: "green"
                        anchors.fill: parent
                        opacity: mainMenuIndex.valueOf(2)  ? 0 : .1
                    }

                    Image {
                        id: buttonImageClimate
                        //source: "img/ui3/homebig.png"
                        anchors.centerIn: parent
                        height: scaleY(13)
                        width:  scaleX(8)
                    }
                    MouseArea{
                        anchors.fill: parent
                        //onClicked:setCurrentScreen("Screen_1.qml")
                        onClicked: {
                            console.log("RayBe you have Clicked on: Climate")
                            mainItemSelected(2)
                        }
                    }
                }

                ButtonMain{
                    buttontext: "Telecom"
                    //buttontextfontsize: 14
                    height: scaleY(8)
                    width:  scaleX(13)
                    Image {
                        id: buttonImageTelecom
                        //source: "img/ui3/homebig.png"
                        anchors.centerIn: parent
                        height: scaleY(13)
                        width:  scaleX(8)
                    }
                    MouseArea{
                        anchors.fill: parent
                        //onClicked:setCurrentScreen("Screen_1.qml")
                        onClicked: {
                            console.log("RayBe you have Clicked on: Lights")
                            mainItemSelected(3)
                        }
                    }
                }

                ButtonMain{
                    buttontext: "Security"
                    //buttontextfontsize: 14
                    height: scaleY(8)
                    width:  scaleX(13)
                    Image {
                        id: buttonImageSecurity
                        //source: "img/ui3/homebig.png"
                        anchors.centerIn: parent
                        height: scaleY(13)
                        width:  scaleX(8)
                    }
                    MouseArea{
                        anchors.fill: parent
                        //onClicked:setCurrentScreen("Screen_1.qml")
                        onClicked: {
                            console.log("RayBe you have Clicked on: Lights")
                            mainItemSelected(4)
                        }
                    }
                }

            }

        //----------




        Clock{
            anchors.right: parent.right
            anchors.top: parent.top
            //y: scaleY(43.89) // 316
        }
/*
        // Main menu strip
        Image {
            id: imgMenu
            //anchors.centerIn:  parent
            //anchors.fill: parent;
            //y: scaleY(41.94) // 302
            y: parent.Top // 302 main menu strip naar boven geplaatst
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
*/
        // Scenarios menu strip
        Image {
            id: imgSubMenu
            //x: scaleX(6.17) // 79
            //y: scaleY(56.39) // 406
            anchors.horizontalCenter: stage.horizontalCenter
            anchors.bottom: stage.bottom
            anchors.bottomMargin : 10
            anchors.leftMargin : 10
            //width: scaleX(87.73) // 1123
            width: scaleX(100) // 1123
            //height: scaleY(4.58) // 33
            height: scaleY(8) // 33
            //source: "../img/home/submenubar_music.png"
            smooth:  true
        }
        ListView{
            id: submenu
            height: imgSubMenu.height
            width: imgSubMenu.width//*.922
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: imgSubMenu.verticalCenter
            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5
            clip:  true
            interactive: true
            orientation: ListView.Horizontal

            delegate: Item{
                height: submenu.height
                id: test
                width: submenu.width/8
                //width: submenu.width/12
                //opacity:  (submenu.activeFocus && ListView.isCurrentItem) ? 1 : .9 //RayBe

//--------onderstaand heb ik ingevoegd om te proberen
                ButtonMain{
                    buttontext: title //title is de variabele waarin de tekst van de scenario-buttons zit
                    anchors.fill: parent //RayBe
                    height: scaleY(8) //RayBe
                    width:  scaleX(13) //RayBe
                    }

//---------

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        console.log("Clicked on:",index)
                        submenu.currentIndex = index
                        //execGrp(params) //RayBe
                        setCurrentScreen("Screen_1.qml") //RayBe to reset the screen in main erea
                        manager.executeCommandGroup(params)
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
            anchors.left: parent.left
            anchors.top: parent.top
            source:  "../img/common/timepanel_mid.png"
            width: scaleX(35.23) // 451
            height:  scaleY(3.75) // 27
            smooth:  true

/* RayBe advance button from Aeon skin
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
                    onClicked: setCurrentScreen("Screen_44.qml")
                }
            }
*/

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
        //mainmenu.forceActiveFocus() //RayBe
        //mainItemSelected(mainMenuIndex) //RayBe
    }

//***---screen_1---***

/* background commented out by RayBe for testing purposses
    property int currentBGimg: 0
    function getBGimage(){
        var files = ["../aeon/img/backdrops/autumn/autumn.png",
                     "../aeon/img/backdrops/winter/winter.png",
                     "../aeon/img/backdrops/spring/spring.png",
                     "../aeon/img/backdrops/summer/summer.png"];
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

*/
    // Load some custom fonts
    FontLoader {
        id: scout
        source: "../aeon/fonts/scout.ttf"
    }



//--- RayBe's
    FontLoader {
        id: didactGothic
        source: "../crystalshades/fonts/DidactGothic.ttf"
    }
//---

    FontLoader {
        id: scoutLight
        source: "../aeon/fonts/scoutlight.ttf"
    }
    FontLoader {
        id: scoutCond
        source: "../aeon/fonts/scoutcond.ttf"
    }
    FontLoader {
        id: aeonRss
        source: "../aeon/fonts/aeon_rss.ttf"
    }
    FontLoader {
        id: aeonNowPlaying
        source: "../aeon/fonts/aeon_player.ttf"
    }
    FontLoader {
        id: aeonNowPlayingCE
        source: "../aeon/fonts/aeon_player_ce.ttf"
    }
    FontLoader {
        id: aeonEdit
        source: "../aeon/fonts/aeon_edit.ttf"
    }
    FontLoader {
        id: aeonEditCE
        source: "../aeon/fonts/aeon_edit_ce.ttf"
    }
    FontLoader {
        id: aeonSettings
        source: "../aeon/fonts/aeon_settings.ttf"
    }

/* RayBe
    Timer { // Simulate a simple PhotoScreensaver
        interval: 15000; running: true; repeat: true
        onTriggered: changeBGimage()
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
    }

*/

    Rectangle {
        id: mainComponentScreenLoader
        height: style.orbiterH *.7
        width: style.orbiterW *.7
        anchors.horizontalCenter: item.horizontalCenter //RayBe
        anchors.verticalCenter: item.verticalCenter //RayBe
        color: "blue"

        Loader {
            id:pageLoader
            objectName: "loadbot"
            anchors.fill: parent
            anchors.margins: 10 //RayBe
            sourceComponent: rect
            //anchors.horizontalCenter: item.horizontalCenter //RayBe
            //anchors.verticalCenter: item.verticalCenter //RayBe
            //source: "../crystalshades/screens/Splash.qml" //RayBe
            //onSourceChanged:  loadin //RayBe
            focus: true
            onLoaded: {

                console.log("Screen Changed:" + pageLoader.source)

            }
        }
    }


/* RayBe
    SequentialAnimation{
        id:loadin

        PropertyAnimation{
            id:fadeout
            target:pageLoader
            properties: "opacity"; to: "0"; duration: 5000

        }
        PropertyAnimation{
            id: fadein
            target:pageLoader
            properties: "opacity"; to: "1"; duration: 5000
        }

    }

    Component.onCompleted: {
        changeBGimage()
    } */
}
