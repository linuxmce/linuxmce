import QtQuick 1.0

Rectangle {
    width: 100
    height: 200
    color: style.advanced_bg

    Arrow{id: down; x: 26; y: 142; arrow_label: "Vol Up "}

    Roundbuttonbase{id: floorplan; x: 38; y: 88; rnd_but_label: "Mute" }

    Arrow{id: up; x: 26; y: 36; arrow_label: "Vol Dwn" }

}
