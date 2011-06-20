import QtQuick 1.0

 Item {
     id: item

     function screenchange(screenname)
     {
         pageLoader.source = screenname
         console.log("Command to change to:" + screenname)
         return "success!"
     }
     width: 640; height: 480
     Loader {
         id:pageLoader
         objectName: "loadbot"
         source: "Splash.qml"
         onLoaded: console.log("Screen Changed:" + pageLoader.source)

     }


 }
