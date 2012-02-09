

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

var component;
var sprite;

function screenchange(screenname) {
    console.log("Command to change to:" + screenname);
    component = Qt.createComponent("screens/"+screenname);
    if (component.status == Component.Ready)
    {
        console.log("loaded!");
        finishCreation();
    }
    else
    {
        console.log("delayed load, waiting..")
        component.statusChanged.connect(finishCreation);
    }
}

function finishCreation() {

    if (component.status == Component.Ready) {
        //sprite = component.createObject(item, {"x": 100, "y": 0});
        //item.pageLoader.source = sprite
        if (sprite == null) {
            // Error Handling
            console.log("Not finished!");
        }
    } else if (component.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString());
    }
}


var listcomponent
var listscreen
var screen

function createListComponent(incavcomponent, screen) {

    listcomponent = Qt.createComponent(incavcomponent);
       listscreen = listcomponent.createObject(screen, {"x":75, "y": 10});

       if (listscreen == null) {
           // Error Handling
           console.log("Error in av component object")
}
}

