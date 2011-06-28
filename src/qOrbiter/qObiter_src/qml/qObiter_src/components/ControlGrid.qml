import QtQuick 1.0

Item {
    id: column_layout
    width: parent.width
    height: parent-5
Column {

    spacing: 10

    HomeLightingRow{id:lighting}


       HomeMediaRow{id:media}

Rectangle{
    height: 100
    color: "black"; anchors.centerIn: parent}
}

}



