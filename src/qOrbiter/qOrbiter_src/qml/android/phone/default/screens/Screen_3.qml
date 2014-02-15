import QtQuick 1.0
import "../components"

PhoneFloorplanLayout{
    id:media


    ButtonSq {
        id: btOff
	anchors.bottom: media.bottom.top - 30

        buttontext: "Off"
	// TODO: call stopMedia with selected EA
        onActivated: manager.stopMedia()
    }

}
