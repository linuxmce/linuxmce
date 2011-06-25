import QtQuick 1.0

 Item {
     id: item
     signal close()
     signal changeScreen(string s)

     function screenchange(screenname)
     {
         pageLoader.source = screenname
         console.log("Command to change to:" + screenname)
         return "success!"
     }
     width: 640; height: 480

     Loader{
         id:styleLoader
         objectName: "style_loader"
         sourceComponent: Style1 {id: style}
     }

     Loader {
         id:pageLoader
         objectName: "loadbot"
         source: "Splash.qml"
         onSourceChanged:  loadin
         onLoaded: console.log("Screen Changed:" + pageLoader.source)

     }

     SequentialAnimation{
         id:loadin
         PropertyAnimation{
             id:fadeout
             target:pageLoader
             properties: "opacity"; to: "0"; duration: 100
         }
         PropertyAnimation{
             id: fadein
             target:pageLoader
             properties: "opacity"; to: "1"; duration: 100
         }

     }


 }
