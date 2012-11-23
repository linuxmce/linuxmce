import QtQuick 1.0
MouseArea
{ //only for use as a click handler with the media list. It takes the id of the clicked item and decides if its a file or folder
    anchors.fill: frame
    onClicked: {setStringParam(4, id);}
}
