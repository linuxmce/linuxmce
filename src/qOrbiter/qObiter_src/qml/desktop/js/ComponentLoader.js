

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
