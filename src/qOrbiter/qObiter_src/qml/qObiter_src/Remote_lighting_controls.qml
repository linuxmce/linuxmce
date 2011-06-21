import QtQuick 1.0

Rectangle {
    id: rectangle1

    width: 75
    height: 200
    color: style.advanced_bg

    Arrow{id: down; x: 13; y: 146; arrow_label: "Down Arrow"}

    Arrow{id: up; x: 13; y: 40; arrow_label: "Up Arrow" }

    Roundbuttonbase{id: floorplan; x: 25; y: 88; rnd_but_label: "f" }


}
