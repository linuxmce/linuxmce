import QtQuick 1.0
import org.linuxmce.enums 1.0
Item{
    id:imgContainer
    height:0
    width: 0
    property string s_aspect: dcenowplaying.aspect
    property double dvdPosterRatio:955/1080
    property double hdPosterRatio:755/1080
    property double cdCoverRatioFront:1080/1080
    property double cdCoverRatioBack:1080/1264
    property double vcdRatio:1080/1080
    property double vhsRatio:1280/620
    onS_aspectChanged: {
        nowplayingimage.source="image://listprovider/updateobject/"+securityvideo.timestamp
        console.log("checking mediatype "+manager.i_current_mediaType)
        switch(manager.i_current_mediaType){
        case MediaTypes.LMCE_StoredAudio:
            console.log("LMCE_StoredAudio::ASPECT RATIO==>"+dcenowplaying.aspect)
            state="wideImage"
            break;

        case MediaTypes.LMCE_StoredVideo:

            if(dcenowplaying.aspect=="poster"){
                console.log("LMCE_StoredVideo::ASPECT RATIO==>"+dcenowplaying.aspect)
                imgContainer.state="posterImage"
            } else if (dcenowplaying.aspect=="wide"){
                console.log("LMCE_StoredVideo::ASPECT RATIO==>"+dcenowplaying.aspect)
                imgContainer.state="wideImage"
            } else if(dcenowplaying.aspect=="square"){
                console.log("LMCE_StoredVideo::ASPECT RATIO==>"+dcenowplaying.aspect)
                imgContainer.state="squareImage"
            }
            break;
        }
    }


    Connections{
        target:dcenowplaying
        onImageChanged:refreshtimer.start()
    }

    Image {
        id: nowplayingimage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
       // source: "image://listprovider/updateobject/"+securityvideo.timestamp
        //  anchors.horizontalCenter: parent.horizontalCenter
        sourceSize.height: parent.height
        sourceSize.width: parent.width
        smooth: true
        visible: source == undefined ? false : true
        onSourceChanged: {
            console.log("nowplayingimage::ASPECT==>"+dcenowplaying.aspect)
            console.log("Source Size ==>"+sourceSize)
        }
    }

    Timer{
        id:refreshtimer
        interval: 500
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: false
    }

    states: [
        State {
            name: "wideImage"
            PropertyChanges {
                target: imgContainer
                height: manager.isProfile ? hdPosterRatio*width : hdPosterRatio*width
                width: manager.isProfile ? scaleX(33) : scaleX(30)
            }
        },
        State {
            name: "squareImage"
            when:manager.i_current_mediaType == MediaTypes.LMCE_StoredAudio
            PropertyChanges {
                target: imgContainer
                height: scaleX(25)
                width: height

            }
        },
        State{
            name:"posterImage"
            PropertyChanges{
                target: imgContainer
                height: manager.isProfile ? scaleY(14) : scaleY(38)
                width: manager.isProfile ? scaleX(35) : scaleX(14)
            }
        }

    ]
}
