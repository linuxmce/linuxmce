
//==========proper network loading functions. use these!!!

var component            //var to hold the component
var screen               //var to hold the screen you want the component parented to
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
