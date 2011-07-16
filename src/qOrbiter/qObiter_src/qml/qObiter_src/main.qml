import QtQuick 1.0


 Item {
     id: item
     signal close()
     signal changeScreen(string s)
     property string locationinfo: "standby"




     //property string currentRoomLights: lightingModel
    //  property string currentRoomMedia: mediaModel
    //  property string currentRoomClimate: climateModel
    //  property string currentRoomTelecom: telecomModel
    //  property string currentRoomSecurity: lightingModel

     function screenchange(screenname)
     {
         pageLoader.source = screenname
         console.log("Command to change to:" + screenname)
         return "success!"
     }
     Style {id:style}
     width: style.orbiterW; height: style.orbiterH;




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
