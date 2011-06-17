import QtQuick 1.0

Grid {
    columns: 1
    rows: 5
    spacing: 0
    x:60
    //anchors.left: parent.left

    Repeater{
        model: ["Lights", "Media", "Climate", "Telecom", "Security"]
        ControlRow {id: index; rowlabel: modelData}
        }

    }




