
/*
  ComponentLoader.js is a special file that utilizes the javascript integration into qml to load dynamic components.
  In our default case here, its used to create components from the components directory in our structure. Since components make
  up a screen, but are never a screen by themselves, we load them seperately from the screens themselves.
  This function is network transparent, meaning if you were to load something from the local storage, or from the network, it would
  adapt and function the same.

  In the future, it will also be possible to attatch a custom loading animation via a special loader.
  */
//==========proper network loading functions. use these!!!

var component            //var to hold the component
var screen               //var to hold the screen/object/component you want the component parented to
var component_sprite     //var representing the actual instance of the object

function createStageComponent(incavcomponent, screen) {

    component = Qt.createComponent("../components/"+incavcomponent);

    if(component.status === Component.Ready)
    {
        component_sprite = component.createObject(screen, {"x":0, "y": 0});
    }
    else if (component.status === Component.Error)
    {
        console.log("Error in stage component object")
    }
    else if (component.status === Component.Loading)
    {
        component.statusChanged.connect(finishStageCreation())
    }

}

function finishStageCreation()
{
    if (stagescreen.status === Component.Ready)
    {
        component_sprite = component.createObject(screen, {"x":0, "y": 0});
    }
    else
    {
        console.log(component.Status)
    }


}

//
