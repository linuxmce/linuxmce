
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


//======================================================

var component;
var sprite;

function createComponentObjects(toBeCreated) {
    component = Qt.createComponent("../components/SkinSelector.qml");
       sprite = component.createObject(advancedscreen , {"x": 0, "y": 0});
    console.log("showing!")
       if (sprite == null) {
           // Error Handling
           console.log("Error creating object")
}
}

var toBeCreated
var component

function createFilterObjects(toBeCreated) {
    console.log(toBeCreated)
    component = Qt.createComponent(toBeCreated);
       sprite = component.createObject(fileviewscreen , {"x": 0, "y": 0});
    console.log("showing!")
       if (sprite == null) {
           // Error Handling
           console.log("Error creating object")
}
}

var toBeDeleted

function deleteFilterObjects(toBeDeleted) {
    console.log(toBeCreated)
    component = Qt.createComponent(toBeCreated);
       sprite = component.createObject(fileviewscreen , {"x": 0, "y": 0});
    console.log("showing!")
       if (sprite == null) {
           // Error Handling
           console.log("Error creating object")
}
}


var detailcomponent
var detailsprite

function loadComponent(parent,file) {

    detailcomponent = Qt.createComponent("../components/"+file);
    if (detailcomponent.status == Component.Ready) {

        detailsprite = detailcomponent.createObject(parent , {"x": 0, "y": 0});

        if (detailsprite == null) {
            // Error Handling
            console.log("Error File Detail object")
        }
    } else if (detailcomponent.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", detailcomponent.errorString());
    }

}

var roomcomponent
var roomsprite

function createRoomSelector() {

    roomcomponent = Qt.createComponent("../components/RoomSelector.qml");
       roomsprite = roomcomponent.createObject(stage, {"x":200, "y": 200});

       if (roomsprite == null) {
           // Error Handling
           console.log("Error File Detail object")
}
}
