import QtQuick 1.0
import "../components"
import "../js" as MyJs

 Item {
     id: item
     signal close()
     signal changeScreen(string s)
     property string locationinfo: "standby"

    // Style {id:style}
    width: style.orbiterW;
    height: style.orbiterH;

     function screenchange(screenname )
     {
         if(pageLoader.source = "../screens/"+screenname)
         {
             console.log("Command to change to:" + screenname+ " was successfull")
         }
         else
         {
             console.log("Command to change to:" + screename + " failed!")
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
