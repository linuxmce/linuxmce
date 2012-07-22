import QtQuick 1.1


import "components"
import "js/ComponentLoader.js" as MyJs

/*
  Main.qml
  This file's purpose is to serve as the logic backend for the various screens and such that are loading as part of using linuxMCE.
  LinuxMCE is unique in that while javascript will accomplish some of what we need, it was faster and easier to use this method for the
  template. Its a straight forward method of loading data:

  Main.qml serves as the 'background'. In addition to functions, it has 1 object. A QML 'Loader' element. This elements purpose is to load
  other qml components. This is useful to use, because our linuxmce 'screens' are essentially components, comprised of other components. By
  loading the main component, that being the 'screen' we also load everything else associated with it, thusly loading the 'screen' and its
  functionality.

  So, even if you decide not to use this mechanism, you should be aware that the c++ side of this application will always be looking for a
  function called screenchange() in the root object of the qml tree. This is where it will send the name of the 'screen' to be navigated to.
  If you dont know what all that means, its ok. Just use this template because as you will see, the way its designed, its a blank slate.

  ComponentLoader.Js is a special file with a default javascript function to allow you to load components on the fly. What kind of components
  you say? Well there are many contextual popups, and this allows you to show them without requiring an entire screen change. The function is
  network transparent, so even if the skin is local or remote it will load.

  Use:
  MyJs.createStageComponent("nameofcomponent.qml", id_of_parent_)
  So the first parameter is the name of the component, which is in your 'components directory'. It should be the file name enclosed in quotes.
  The second parameter is the id of the parent object. You may notice the first QML element on this page has the id of 'item'. i would simply
  pass the id of the where i wanted my component to appear at 0,0 at and base its x,y on. This allows you to be very general or very specific as
  you create popups. its up to you!


  */

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
    }

    Connections{
        target: manager
        onOrientationChanged: checkLayout()

    }

//setting an image in the main.qml allows you to have a skin wide background image. Or some other fancy item.
    Image {
        id: bg
        source: ""
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
        pageLoader.source = "screens/"+screenname
        if (pageLoader.status == Component.Ready)
        {
            manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
        }
        else if (pageLoader.status == Component.Loading)
        {
            console.log("loading page from network")
            finishLoading(screenname)
        }
        else
        {
            console.log("Command to change to:" + screenname + " failed!")
            screenfile = screenname
            pageLoader.source = "screens/Screen_x.qml"
        }
    }

    function finishLoading (screenname)
    {
        if(pageLoader.status != Component.Ready)
        {
            console.log("finishing load")
            pageLoader.source = "screens/"+screenname
            console.log("screen" + screenname + " loaded.")
        }
        else
        {
            finishLoading(screenname)
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

}
