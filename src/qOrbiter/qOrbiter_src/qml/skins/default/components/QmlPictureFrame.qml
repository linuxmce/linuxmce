import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import "."
import "../."

/*! QML based screensaver optimized for qt5 */
Item{
    id:qmlPictureFrame
    anchors.fill: parent
    property variant pictureList:[]
    property string pictureUrl:"http://"+manager.currentRouter+"/lmce-admin/imdbImage.php?type=screensaver&val="
    property bool useList:true
    property bool random:true
    property int currentPic:0


    property int switchTimer:60000
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

        if (!settings.isScreenSaverEnabled()) {
            console.log("Orbiter screensaver not enabled")
            return;
        }

        img1.source=getImage();
        screenSaverTimer.start()
        console.log("Orbiter Consume Screensaver images")
        console.log("Orbiter counts " + pictureList.length)

    }

    function loadNextImage(){
        if(manager.currentScreen !=="Screen_1.qml")
            return;

        if (!settings.isScreenSaverEnabled()) {
            console.log("Orbiter screensaver not enabled")
            return;
        }
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

    Item{
        id:clock
        clip:false
        property bool longDate:true
        property bool verticalMirror: false
        property color clockColor:"white"
        property bool boldClock:false

        width:parent.width/1.5
        height: 50
        opacity: qmlRoot.screensaverActive ? 1 : 0
        function getDate(){
            if(!longDate)
                return  new Date().toTimeString()
            else
                return Qt.formatDateTime(new Date(), "dddd ,MMMM d yyyy \n hh:mm:ss ");
        }
        anchors{
            bottom:parent.bottom
            bottomMargin: Style.scaleX(10)
            left:parent.left
        }

        Behavior on opacity{
            PropertyAnimation{
                duration: Style.transitionFadeTime
            }
        }

        Timer { // Update the clock element periodically
            interval: 1000; running: clock.opacity==1; repeat: true
            onTriggered: txtDate.text = clock.getDate()
        }


        StyledText{
            id: txtDate
            text: clock.getDate()
            color: clock.clockColor
            font.letterSpacing: 2
            smooth: true
            width: parent.width
            anchors.centerIn: parent
            font.pointSize: 32
            horizontalAlignment: Text.AlignHCenter
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
