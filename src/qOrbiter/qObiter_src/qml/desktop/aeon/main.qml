import QtQuick 1.0
import "components"
//import "../js/ComponentLoader.js" as MyJs
import "js/dateFormat.js" as DateFunctions

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
    width: style.orbiterW;
    height: style.orbiterH;

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


    Loader {
        id:pageLoader
        objectName: "loadbot"
        source: "Splash.qml"
        onSourceChanged:  loadin
        focus: true
        onLoaded: {

            console.log("Screen Changed:" + pageLoader.source)

        }
    }

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
    }
}
