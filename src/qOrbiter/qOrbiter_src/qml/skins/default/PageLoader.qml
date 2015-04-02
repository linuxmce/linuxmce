import QtQuick 2.3
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0
import QtMultimedia 5.0
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Loader {
    id:pageLoader
    objectName: "loadbot"
    focus: true
   // source:"screens/Screen_1.qml"
    // visible:qml_root.uiOn
    property string nextScreen:manager.currentScreen
    property string currentScreen:""

    Connections{
        target:manager
        onScreenChange:{
            console.log( manager.currentScreen)
        }
    }
    
    onNextScreenChanged: {
        console.log("Loading next screen")
        if(!nextScreen.match(".qml")){
            return
        } else {
            
        }
        console.log("current screen "+currentScreen)
        console.log("next screen "+ nextScreen)

        if(currentScreen=="screens/"+nextScreen){
                return;
        }
        
        if(!pageLoader.item){
            console.log("Last screen likely had errors, loading next screen==>"+nextScreen)
            loadNext()
            return;
        }
        
        if( pageLoader.item.noForce===true){
            console.log("pageloader::"+source+" declares noforce::"+pageLoader.item.noForce+", ignoring "+nextScreen)
            return;
        }else {
            console.log("pageloader::"+source+" noforce::"+pageLoader.item.noForce+" , loading next screen.")
            console.log("next screen==>"+nextScreen)
            startChange()
        }
        
    }
    
    function startChange(){
        
        
        if(!pageLoader.item || pageLoader.item.screen){
            console.log("pageloader::closing page "+ currentScreen)
            pageLoader.item.state="closing"
        } else{
            console.log("pageloader::no page jumping to next ==>"+nextScreen)
            loadNext()
        }
        
    }
    
    function loadNext(){
        
        
        if(nextScreen===""){
            nextScreen="Screen_1.qml"
            return
        }
        
        console.log("pageloader--default::loading next screen::"+"screens/"+nextScreen)
        var path = manager.selectPath(Qt.resolvedUrl("screens/"+nextScreen))
        console.log("pageloader--default::path" +Qt.resolvedUrl(path))
        pageLoader.source=path
    }
    
    //opacity: uiOn ? 1 : 0
    Behavior on opacity {
        PropertyAnimation{
            duration: 750
        }
    }
    
    Keys.onBackPressed: console.log("back")
    
    onLoaded: {
        console.log("Screen Changed:" + pageLoader.source)
    }
    onStatusChanged:  if (pageLoader.status == Component.Ready)
                      {
                          
                          manager.setDceResponse("Command to change to:" + source+ " was successfull")                         
                          currentScreen=manager.currentScreen
                          
                          // contentItem=item.screen_root
                      }
                      else if (pageLoader.status == Component.Loading)
                      {
                          console.log("loading page from network")
                          
                      }
                      else if(pageLoader.status == Component.Error)
                      {
                          // pageLoader.e
                         // source="screens/Screen_X.qml"
                          console.log("Command to change to:" + source + " failed!")
                         manager.currentScreen="Screen_X.qml"

                      }
}
