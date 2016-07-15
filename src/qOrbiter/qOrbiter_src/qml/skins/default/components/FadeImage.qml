import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import "."

/*! QML based screensaver optimized for qt5 */
Image{
    id:fade_img
    anchors.fill: parent
    asynchronous: true
    property bool closing:false
    signal readyToShow()
    signal badImageError()
    fillMode: Image.PreserveAspectCrop

    smooth: true

    onOpacityChanged:{
        if(opacity==0){                 //opacity 0 means this is now the 'off' image, we dump the source so it can be used later
            source="";                  //source cleared
            closing=false               //flag set for what phase of transition its in.
        }

        if(opacity==1){
            closing=true                //flag set for transition phase
        }
    }

    onStatusChanged:{
        if(fade_img.status==Image.Ready){
                readyToShow()           

        } else if (fade_img.status==Image.Error){
            badImageError()
        }

    }
    
    
    Behavior on opacity {
        PropertyAnimation{
            duration: 2000
        }
    }
}
