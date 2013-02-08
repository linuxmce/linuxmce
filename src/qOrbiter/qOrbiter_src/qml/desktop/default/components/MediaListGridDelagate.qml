import QtQuick 1.0
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import "../effects"
import "../components"
import "../animation"
import "../../lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Component
{
    id: contactDelegate
    
    Rectangle
    {
        id:mainItem
        width: scaleX(21);
        height: scaleY(21)
        color: "transparent"
        opacity: 0
        scale:0
        rotation: 360
        
        MouseArea{
            anchors.fill: mainItem
            hoverEnabled: true
            onEntered: {
                mainItem.color = style.darkhighlight
                mainItem.scale = 1.25
                mainItem.z = 10
                
            }
            onExited: {
                mainItem.color = "transparent"
                mainItem.scale = 1
                mainItem.z = 1
            }
        }
        ParallelAnimation {
            id:fade_and_scale
            running: false
            PropertyAnimation { target: mainItem; property: "opacity"; to: 1; duration: 1000}
            PropertyAnimation { target: mainItem; property: "scale"; to: 1; duration: 500}
            PropertyAnimation { target: mainItem; property: "rotation"; to: 0; duration: 500}
            
        }
        
        Component.onCompleted: {fade_and_scale.running = true}
        property bool pop:ping
        onPopChanged:{
            console.log(pop)
            if(pop===true)

           gridView.positionViewAtIndex(index,ListView.Beginning)
        }
        Rectangle
        {
            id:frame
            
            width: scaleX(21);
            height: scaleY(21)
            anchors.centerIn: mainItem
            clip:true
            color: "transparent"
            
            MediaListClickHandler {
            }
            
            BorderImage {
                id: borderimg
                horizontalTileMode: BorderImage.Repeat
                source: "../img/icons/drpshadow.png"
                anchors.fill: imagerect
                anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                border { left: 10; top: 10; right: 10; bottom: 10 }
                smooth: true
            }
            
            Image
            {
                id: imagerect;
                source:path !=="" ? "http://"+srouterip+"/lmce-admin/MediaImage.php?img="+path : ""
                height: scaleY(16);
                width: scaleX(16);
                anchors.centerIn: parent;
                fillMode: Image.PreserveAspectCrop
                smooth: true
                asynchronous: true

            }
            Rectangle{
                id:textmask
                color: "grey"
                anchors.fill:celllabel
                opacity: .5
            }

            Text
            {
                id:celllabel
                text: name;
                font.pointSize: 12;
                color: "white" ;
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: imagerect.width
                font.bold: true
                anchors.top: imagerect.bottom
                anchors.horizontalCenter: imagerect.horizontalCenter
            }

            

        }
    }
}
