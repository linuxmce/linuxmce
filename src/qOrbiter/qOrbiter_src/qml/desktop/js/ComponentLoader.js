

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

function createFileDetails() {

    detailcomponent = Qt.createComponent("../components/FileDetails.qml");
       detailsprite = detailcomponent.createObject(fileviewscreen , {"x": 0, "y": 0});

       if (detailsprite == null) {
           // Error Handling
           console.log("Error File Detail object")
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
