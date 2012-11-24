import QtQuick 1.0

MouseArea{
    anchors.fill: parent
    onClicked:{
       currentFilterModel.setSelectionStatus(name)
    }
}
