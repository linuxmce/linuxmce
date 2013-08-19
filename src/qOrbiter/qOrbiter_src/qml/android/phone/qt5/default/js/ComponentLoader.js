

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
    setRequestMore(false)

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

var qml_component;
var sprite;

function screenchange(screenname) {
    console.log("Command to change to:" + screenname);
    qml_component = Qt.createComponent("screens/"+screenname);
    if (qml_component.status == Component.Ready)
    {
        console.log("loaded!");
        finishCreation();
    }
    else if (qml_component.status === Component.Loading)
    {
        console.log("Network Loading Enabled, processing...")
        qml_component.statusChanged.connect(finishCreation());
    }
}

function finishCreation() {

    if (qml_component.status === Component.Ready) {
        sprite = qml_component.createObject(item, {"x": 0, "y": 0});

        if (sprite === null) {
            // Error Handling
            console.log("Not finished!");
        }
    } else if (qml_component.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", qml_component.errorString());
    }
}


var listcomponent
var listscreen
var screen

function createListComponent(incavcomponent, screen) {

    listcomponent = Qt.createComponent("../components/"+incavcomponent);

    if(listcomponent.status === Component.Ready)
    {
        finishListComponent()
    }
    else if (listcomponent.status === Component.Error)
    {
        console.log("Error in List component object")
    }
    else if (listcomponent.status === Component.Loading)
    {
        listcomponent.statusChanged.connect(finishListComponent())
    }

}

function finishListComponent()
{

    listscreen = listcomponent.createObject(selector, {"x":0, "y": 0});

}



var stagecomponent
var stagescreen
var stgscreen

function createStageComponent(incavcomponent, screen) {

    stagecomponent = Qt.createComponent("../components/"+incavcomponent);

    if(stagecomponent.status === Component.Ready)
    {
        stagescreen = stagecomponent.createObject(stage, {"x":0, "y": 0});

    }
    else if (stagecomponent.status === Component.Error)
    {
        console.log("Error in stage component object")
    }
    else if (stagecomponent.status === Component.Loading)
    {
        stagecomponent.statusChanged.connect(finishStageCreation())
    }

}

function finishStageCreation()
{
    if (stagescreen.status === Component.Ready)
    {
        stagescreen = stagecomponent.createObject(stage, {"x":0, "y": 0});
    }
    else
    {
        finishStageCreation();
    }


}
