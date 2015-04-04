import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import "."

/*! QML based screensaver optimized for qt5 */
Item{
    id:qmlPictureFrame
    anchors.fill: parent
    property variant pictureList:[]
    property int currentPic:0
    property bool switched:false
    //  onPictureListChanged: console.log("Picture Count "+pictureList)
    property int switchTimer:10000
    readonly property int transitionTime:switchTimer-4500

    Component.onCompleted: loadImageList()

    Connections{
        target:manager
        onScreenSaverImagesReady:{
            loadImageList()


        }
    }
    function getNextImage(){
        console.log("get next image of " +pictureList.length+" images")
        // var picIndex=Math.floor((Math.random()* pictureList.length)+1)
        if(currentPic==pictureList.length){
            currentPic=0
            getNextImage()
        }

        currentPic++


        console.log("Getting next image")
        screenSaverTimer.stop()
        var nextImg = new XMLHttpRequest();
        nextImg.onreadystatechange= function(){
            if(nextImg.readyState == XMLHttpRequest.HEADERS_RECIEVED){
                console.log("recieving image headers");
            } else if (nextImg.readyState == XMLHttpRequest.DONE){
                var docData = nextImg.responseText

                if(docData){
                    console.log("Image data valid, setting to image element")
                    var link = "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=screensaver&val="+pictureList[currentPic]
                    if(img1.closing) { img2.source= link; img1.opacity=0; }
                    else {img1.source=link; img2.opacity=0;}

                } else {
                    console.log("No Image Data. striking image " +currentPic +" from the list ")
                    pictureList.unshift()
                    getNextImage()
                }

                screenSaverTimer.restart()
            }
        }

        nextImg.open("GET", "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=screensaver&val="+pictureList[currentPic])
        nextImg.send()
    }


    Timer{
        id:screenSaverTimer
        interval: transitionTime
        onTriggered: getNextImage();
        running:false
        triggeredOnStart: false
        repeat: true
    }

    function getImage(){

        img1.source="http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=screensaver&val="+pictureList[Math.floor((Math.random()* pictureList.length)+1)]

    }

    function loadImageList(){
        pictureList = manager.getScreensaverImages()
        if(pictureList.length==0)
            return

        screenSaverTimer.start()
        getNextImage()
        console.log("Orbiter Consume Screensaver images")
        console.log("Orbiter counts " + pictureList.length)

    }

    function loadNextImage(){

        getImage()
    }

    function startScreenSaver(){

    }
    function stopScreenSaver(){

    }

    FadeImage {
        id: img1
        StyledButton{
            anchors.centerIn: parent
            buttonText: "Plane 1"
        }
    }
    FadeImage{
        id:img2
        StyledButton{
            anchors.centerIn: parent
            buttonText: "Plane 2"
        }
    }



    states: [
        State {
            name: "active"
        },
        State {
            name: "inactive"
        }
    ]

}
