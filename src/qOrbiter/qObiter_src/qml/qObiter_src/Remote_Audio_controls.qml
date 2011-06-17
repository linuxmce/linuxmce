import QtQuick 1.0

Rectangle {
    width: 100
    height: 200
    color: "darkseagreen"

    Column {
        id: column1
        anchors.fill: parent
        spacing: 5

        Arrow{id: down; arrow_label: "Vol Up ";anchors.centerIn: parent.top}

        Roundbuttonbase{id: floorplan; rnd_but_label: "Mute" }

        Arrow{id: up; arrow_label: "Vol Dwn" }
    }

}
