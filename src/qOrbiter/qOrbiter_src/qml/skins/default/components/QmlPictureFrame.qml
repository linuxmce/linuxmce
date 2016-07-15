import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import "."
import "../."

/*! QML based screensaver optimized for qt5 */
Item{
    id:qmlPictureFrame
    anchors.fill: parent
    ListModel {
        id: pictureList;
    }
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

    Component.onCompleted: {
        loadImageList()
    }

    Connections{
        target:manager
        onScreenSaverImagesReady:{
            loadImageList()
        }
    }

    function handleBadImage(){
        console.log("Removing bad image "+pictureList.get(currentPic).img)
        pictureList.remove(currentPic)
        console.log(pictureList.count + " screen saver images in list.")
        loadNextImage()
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
                currentPic=Math.floor((Math.random()* pictureList.count))
            }else {
                if(currentPic==pictureList.count){
                    currentPic=0
                }
                currentPic++
            }
            return pictureUrl+pictureList.get(currentPic).img
        } else {
            return pictureUrl
        }

    }

    function loadImageList(){
        var li = manager.getScreensaverImages()

        if(li.length==0)
            return

        for(var img in li){
            var str = li[img]
         //   console.log(str)
            pictureList.append({"img":str})
        }

        if (!settings.isScreenSaverEnabled()) {
            console.log("Orbiter screensaver not enabled")
            return;
        }

        img1.source=getImage();
        screenSaverTimer.start()
        console.log("Orbiter Consume Screensaver images")
        console.log("Orbiter counts " + pictureList.count)

    }

    function loadNextImage(){

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
        qmlPictureFrame.visible=true
        console.log("Screensaver Started")
    }
    function stopScreenSaver(){
        screenSaverTimer.stop()
        qmlPictureFrame.visible=false
        img1.source=""
        img2.source=""
        console.log("Screen Saver Stopped")
    }

    FadeImage {
        id: img1
        opacity: 0
        onBadImageError: handleBadImage()
        onReadyToShow: transitionPlanes()

    }
    FadeImage{
        id:img2
        onBadImageError: handleBadImage()
        onReadyToShow: transitionPlanes()
    }

    Item{
        id:clock
        clip:false
        property bool longDate:true
        property bool verticalMirror: false
        property color clockColor:"white"
        property bool boldClock:false

        width:parent.width
        height: 50
        opacity: qmlRoot.screensaverActive ? .75 : 0
        function getDate(){
            if(!longDate)
                return  new Date().toTimeString()
            else
                return Qt.formatDateTime(new Date(), "dddd ,MMMM d yyyy \t hh:mm:ss ");
        }


        function getTime(){
            return Qt.formatDateTime(new Date(), "hh:mm:ss");
        }

        function getShortDate(){
            return Qt.formatDateTime(new Date(), "dddd ,MMMM d yyyy");
        }

        anchors{
            bottom:parent.bottom
            bottomMargin: Style.scaleX(5)
            left:parent.left
        }

        Behavior on opacity{
            PropertyAnimation{
                duration: Style.transitionFadeTime
            }
        }

        Timer { // Update the clock element periodically
            interval: 1000; running: clock.opacity!==0; repeat: true
            onTriggered:{ txtDate.text = clock.getShortDate(); txtTime.text = clock.getTime(); }
        }

        Column{
            id:temporalid
            height:childrenRect.height
            width:parent.width/2

            anchors{
                right:parent.right
                bottom:parent.bottom
            }

                StyledText{
                    id: txtDate
                    color: clock.clockColor
                    font.letterSpacing: 2
                    smooth: true
                    width: parent.width
                    font.pixelSize: 42
                    horizontalAlignment: Text.AlignHCenter
                    opacity: .55
                    font.weight: Font.Bold
                }

                StyledText{
                    id: txtTime
                    color: clock.clockColor
                    font.letterSpacing: 2
                    smooth: true
                    width: parent.width
                    font.pixelSize:  42
                    font.weight: Font.Light
                    horizontalAlignment: Text.AlignHCenter
                    opacity: .55
                }

            }
        }


//        StyledText{
//            id: txtDate
//            color: clock.clockColor
//            font.letterSpacing: 2
//            smooth: true
//            width: parent.width
//            anchors.centerIn: parent
//            font.pointSize: Style.appFontSize_header
//            horizontalAlignment: Text.AlignHCenter
//            opacity: .55
//        }

//        InnerShadow{
//           color:"black"
//           source: txtDate
//           anchors.fill: txtDate
//           radius: 6
//           samples: 16
//           spread: 0.0
//        }


    states: [
        State {
            name: "active"
        },
        State {
            name: "inactive"

        }
    ]

}
