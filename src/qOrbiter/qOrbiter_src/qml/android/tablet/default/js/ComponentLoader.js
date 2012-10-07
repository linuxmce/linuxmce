
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
    if (component.status === Component.Ready)
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
    component = Qt.createComponent("../components/"+toBeCreated);

    if(component.status === Component.Ready)
    {
        component_sprite = component.createObject(fileviewscreen, {"x":0, "y": 0});
    }
    else if (component.status === Component.Error)
    {
        console.log("Error in stage component object")
    }
    else if (component.status === Component.Loading)
    {
        component.statusChanged.connect(finishFilterCreation())
    }
}

function finishFilterCreation(){

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
var mediatype

function createFileDetails(mediatype) {


    detailcomponent = Qt.createComponent("../components/FileDetails"+mediatype+".qml");
    detailsprite = detailcomponent.createObject(fileviewscreen , {"x": 0, "y": 0, "z": 10});

    if (detailsprite == null) {
        // Error Handling
        console.log("Error File Detail object")
        detailcomponent = Qt.createComponent("../components/FileDetails.qml");
        detailsprite = detailcomponent.createObject(fileviewscreen , {"x": 0, "y": 0, "z": 10});
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

var powercomponent
var powersprite

function createPowerButton() {

    powercomponent = Qt.createComponent("../components/Power.qml");
    powersprite = powercomponent.createObject(stage, {"x":200, "y": 500});

    if (powersprite == null) {
        // Error Handling
        console.log("Error File Detail object")
    }
}

var avcomponent
var avsprite
var screen

function createAvComponent(incavcomponent, screen) {

    avcomponent = Qt.createComponent(incavcomponent);
    avsprite = avcomponent.createObject(screen, {"x":200, "y": 500});

    if (avsprite == null) {
        // Error Handling
        console.log("Error in av component object")
    }
}


var thumbcomponent
var thumbsprite
var thumbscreen

function createThumbComponent(incavcomponent, thumbscreen) {

    thumbcomponent = Qt.createComponent(incavcomponent);
    thumbsprite = thumbcomponent.createObject(thumbscreen, {"x":200, "y": 50});

    if (thumbsprite == null) {
        // Error Handling
        console.log("Error in av component object")
    }
}
