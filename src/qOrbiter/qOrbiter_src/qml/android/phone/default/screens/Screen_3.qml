import QtQuick 1.0
import "../components"

PhoneFloorplanLayout{
    id:media

    property variant mediaStreamModel: manager.getDataGridModel("mediaStreams", 10)

    ButtonSq {
        id: btOff
	anchors.top: parent.verticalCenter
	anchors.left: parent.left + 10
	height: style.stdbuttonh/2
        buttontext: "Stop"
	// call stopMedia with selected EA
        onActivated: {
	    console.log("manager.stopPlayback with EA = " + media.getSelections().get(0).device)
	    manager.stopMediaInEa(media.getSelections().get(0).device)
        }
    }

    ListView {
        id:activeStreams
        anchors{
            top: btOff.bottom+10
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            leftMargin: 10
	    rightMargin: 10
        }
        model: mediaStreamModel
        delegate:
            StyledButton{
            id:gridBtn
            buttonText: description
            textSize: 16
	    height: scaleX(10)
	    width: parent.width
	    
            onActivated: {
//                manager.execGrp(params)
//                showSecondary = false

            }
        }
    }


}
