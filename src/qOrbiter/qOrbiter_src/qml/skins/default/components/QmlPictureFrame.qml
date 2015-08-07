import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import "."

/*! QML based screensaver optimized for qt5 */
Item{
    id:qmlPictureFrame
    anchors.fill: parent
    property variant pictureList:[]
    property string pictureUrl:"http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=screensaver&val="
    property bool useList:true
    property bool random:true
    property int currentPic:0

    property int switchTimer:20000
    readonly property int transitionTime:switchTimer-4500

    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    Component.onCompleted: loadImageList()

    Connections{
        target:manager
        onScreenSaverImagesReady:{
            loadImageList()

        }
    }

    function transitionPlanes(){

        if(img1.opacity==0){
            img1.opacity=1
            img2.opacity=0
        } else {
            img2.opacity=1
            img1.opacity=0
        }
        screenSaverTimer.restart()

    }

    function getNextImage(){ //Only used for debug. must uncomment return for it to function
        return;

    }


    Timer{
        id:screenSaverTimer
        interval: transitionTime
        onTriggered: loadNextImage()
        running:false
        triggeredOnStart: false
        repeat: true
    }

    function getImage(){

        if(useList){

            if(random){
              currentPic=Math.floor((Math.random()* pictureList.length)+1)

            }else {
                if(currentPic==pictureList.length){
                    currentPic=0
                }
                currentPic++
            }

           return pictureUrl+pictureList[currentPic]
        } else {
            return pictureUrl
        }

    }

    function loadImageList(){
        pictureList = manager.getScreensaverImages()
        if(pictureList.length==0)
            return

      img1.source=getImage();
        screenSaverTimer.start()
        console.log("Orbiter Consume Screensaver images")
        console.log("Orbiter counts " + pictureList.length)

    }

    function loadNextImage(){
        if(manager.currentScreen !=="Screen_1.qml")
            return;

        var link = getImage()
       // console.log("Getting "+link)
        if(img1.closing) {
            img2.source= link;
        }
        else{
            img1.source=link;
        }
    }

    function startScreenSaver(){
        screenSaverTimer.start()
        visible=true
    }
    function stopScreenSaver(){
        screenSaverTimer.stop()
        visible=false
    }

    FadeImage {
        id: img1
        opacity: 0
        onBadImageError: loadNextImage()
        onReadyToShow: transitionPlanes()

    }
    FadeImage{
        id:img2
        onBadImageError: loadNextImage()
        onReadyToShow: transitionPlanes()       
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
