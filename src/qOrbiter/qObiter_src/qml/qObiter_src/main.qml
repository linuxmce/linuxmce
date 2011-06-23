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

     }

     Loader {
         id:pageLoader
         objectName: "loadbot"
         source: "Splash.qml"

         onLoaded: console.log("Screen Changed:" + pageLoader.source)

     }


 }
