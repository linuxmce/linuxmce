/*
  Blatant copy of example doc @ http://doc.qt.nokia.com/4.8-snapshot/qdeclarativedynamicobjects.html
  */

var component;
 var sprite;


 function initiateScreenChange(screenname) {
     component = Qt.createComponent("screens/"+screenname);
     if (component.status == Component.Ready)
         finishCreation();

     else
         component.statusChanged.connect(finishScreenChange);
    // component.progressChanged.connect(screenStatus())
 }

 function finishScreenChange() {
     if (component.status == Component.Ready)
     {
         sprite = component.createObject(item, {"x": 0, "y": 0});
         console.log("created!")
         if (sprite == null)
         {
             // Error Handling
             console.log("Error creating object");

         }

     else if (component.status == Component.Error) {
         // Error Handling
         console.log("Error loading component:", component.errorString());
     }

 }
}

function screenStatus(){
    console.log(component.progress)
}
