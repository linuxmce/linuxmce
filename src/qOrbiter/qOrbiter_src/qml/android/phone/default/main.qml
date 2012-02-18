  import QtQuick 1.1

  import "components"
  import "js/ComponentLoader.js" as MyJs
import "js/ScreenChange.js" as ScreenLogic

  Item {
      id: item
    width:appW
      height:appH

      signal close()
      signal changeScreen(string s)
      signal setupStart(int x, string y)


      property string locationinfo: "standby"
      property string screenfile
  property string dynamic_height 
  property string dynamic_width 

  function checkLayout()
  {
  console.log("c++ slot orientation changed")
  item.height=appH
  item.width=appW

  }
  
  Connections{
	  target: manager
      onOrientationChanged: checkLayout()

      }

      Image {
	  id: bg
	  source: "img/bkg.png"
	  anchors.fill:parent
      }

      function scaleX(x){
	  return x/100*appW
      }
      function scaleY(y){
	  return y/100*appH
      }


      function screenchange(screen) {
          var component;

          console.log("QML Command to change to:" +screen);
          component = Qt.createComponent(screen);
          if (component.status == Component.Ready)
          {
              console.log("loaded!");
              finishCreation(component);
          }
          else
          {
              console.log("delayed load, waiting..")

              component.statusChanged.connect(finishCreation(component));              
              component.progressChanged.connect(checkStatus(component))
          }
      }

      function finishCreation(component) {
        var sprite;
          if (component.status == Component.Ready) {
              console.log("Finishing creation")
              sprite = component.createObject(this, {"x": 0, "y": 0});
              pageLoader.sourceComponent = component
              if (sprite == null) {
                  // Error Handling
                  console.log("Not finished!");
              }
          } else if (component.status == Component.Error) {
              // Error Handling
              console.log("Error loading component:", component.errorString());
          }
      }

      function checkStatus(component)
      {
         console.log(component.progress)
      }


      Loader {
	  id:pageLoader
	  objectName: "loadbot"

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
