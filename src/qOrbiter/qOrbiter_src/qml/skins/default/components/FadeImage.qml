import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import "."

/*! QML based screensaver optimized for qt5 */
Image{
    id:img1
    anchors.fill: parent
    asynchronous: true
    property bool closing:false
    onOpacityChanged: if(opacity==0){source=""; closing=false}
    onStatusChanged:{
        if(img1.status==Image.Ready){
            opacity=1
            closing=true
        }

    }
    
    
    Behavior on opacity {
        PropertyAnimation{
            duration: 500
        }
    }
}
