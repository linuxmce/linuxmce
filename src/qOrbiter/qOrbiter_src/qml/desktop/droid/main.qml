import QtQuick 1.0
//import "../components"
//import "../js/ComponentLoader.js" as MyJs


 Item {
     id: item
     signal close()
     signal changeScreen(string s)
     signal setupStart(int x, string y)

     property string locationinfo: "standby"
     property string screenfile

    // Style {id:style}
    width: style.orbiterW;
    height: style.orbiterH;

     function screenchange(screenname )
     {
        pageLoader.source = "/qml/"+screenname
        if (pageLoader.status == 1)
         {
             console.log("Command to change to:" + screenname+ " was successfull")
         }
         else
         {
            console.log("Command to change to:" + screenname + " failed!")
            screenfile = screenname
            //pageLoader.source = "../screens/Screen_x.qml"
         }
     }



     Loader {
         id:pageLoader
         objectName: "loadbot"
         source: "Splash.qml"
         onSourceChanged:  loadin
         onLoaded: {

             console.log("Screen Changed:" + pageLoader.source)

            }
         }

     SequentialAnimation{
         id:loadin

         PropertyAnimation{
             id:fadeout
             target:pageLoader
             properties: "opacity"; to: "0"; duration: 5000

         }
         PropertyAnimation{
             id: fadein
             target:pageLoader
             properties: "opacity"; to: "1"; duration: 5000
         }

     }


 }
