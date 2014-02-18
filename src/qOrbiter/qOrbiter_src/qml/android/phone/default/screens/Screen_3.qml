import QtQuick 1.0
import "../components"

PhoneFloorplanLayout{
    id:media


    ButtonSq {
        id: btOff
	anchors.bottom: media.bottom.top - 30

        buttontext: "Stop"
	// call stopMedia with selected EA
        onActivated: {
	    console.log("manager.stopPlayback with EA = " + media.getSelections().get(0).device)
	    manager.stopMediaInEa(media.getSelections().get(0).device)
        }
    }

}
