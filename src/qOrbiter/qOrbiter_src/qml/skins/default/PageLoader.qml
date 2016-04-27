import QtQuick 2.2
import QtGraphicalEffects 1.0


import QtMultimedia 5.0
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Loader {
    id:pageLoader
    objectName: "loadbot"
    focus: true
    // source:"screens/Screen_1.qml"
    // visible:qml_root.uiOn
    property alias contentItem:pageLoader.item
    property string nextScreen:manager.currentScreen
    property string currentScreen:""
    //  Component.onCompleted: manager.currentScreen="Screen_1.qml"
    Connections{
        target:manager
        onScreenChange:{
            qmlRoot.resetTimeout();
            console.log( "Manager screen::"+ manager.currentScreen)
            console.log("Current PageLoader Screen::"+currentScreen)
            nextScreen=manager.currentScreen
            qmlRoot.clearDialogs();
        }
    }

    Timer{
        id:recovery
        interval: 1000
        running:false
        onTriggered:loadNext()
    }

    function toggleContent(toggle){
        if(toggle)
            pageLoader.opacity=1
        else
            pageLoader.opacity=0
    }
    
    onNextScreenChanged: {
        console.log("Loading next screen " +nextScreen)
        if(!nextScreen.match(".qml")){
            return
        } else {
            
        }
        console.log("current screen "+currentScreen)
        console.log("next screen "+ nextScreen)

        if(currentScreen==nextScreen){
            console.log("duplicate screen error!")
            nextScreen=manager.currentScreen
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
        

        if(pageLoader.item.styledScreen){
            console.log("pageloader::closing page "+ currentScreen)
            pageLoader.item.state="closing"
            recovery.start()
        } else{
            console.log("pageloader::no page jumping to next ==>"+nextScreen)
            loadNext()
        }
        
    }
    
    function loadNext(){
        recovery.stop()

        if(nextScreen===""){
            console.log("Next Screen error!")
            nextScreen="Screen_1.qml"
            return
        }
        
        console.log("pageloader--default::loading next screen::"+"screens/"+nextScreen)
        var path = manager.selectPath(Qt.resolvedUrl("screens/"+nextScreen))
        console.log("pageloader--default::path " +Qt.resolvedUrl(path))
        pageLoader.source=path
    }
    
    //opacity: uiOn ? 1 : 0
    Behavior on opacity {
        PropertyAnimation{
            duration: 750
        }
    }
    

    
    onLoaded: {
        console.log("Screen Changed:" + pageLoader.source)
    }
    onStatusChanged:  if (pageLoader.status == Component.Ready)
                      {
                          manager.setDceResponse("Command to change to:" + source+ " was successfull")
                          currentScreen=manager.currentScreen
                          manager.addScreenToHistory(currentScreen)
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
