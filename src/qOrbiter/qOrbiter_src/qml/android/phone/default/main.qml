import QtQuick 1.0

  import "components"
  import "js/ComponentLoader.js" as MyJs

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

      function screenchange(screenname )
      {
          MyJs.screenchange(screenname)
          /*
          pageLoader.source = "screens/"+screenname
          if (pageLoader.status == 1)
          {
              console.log("Command to change to:" + screenname+ " was successfull")
          }
          else
          {
              console.log("Command to change to:" + screenname + " failed!")
              screenfile = screenname
              pageLoader.source = "../screens/Screen_x.qml"
          }
          */
          pageLoader.source = "screens/"+screenname
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
