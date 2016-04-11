import QtQuick 1.0
import "../components"
StyledScreen{
    id:no_screen


    StyledText {
        id: screen_not_found
        anchors.centerIn: parent
        text: "Error Loading Screen  \n" + lastLoaderError
        wrapMode: "WrapAtWordBoundaryOrAnywhere"

    }

}
