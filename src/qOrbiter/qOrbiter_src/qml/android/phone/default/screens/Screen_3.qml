import QtQuick 1.0
import "../components"

PhoneFloorplanLayout{
    id:media

    property variant mediaStreamModel: manager.getDataGridModel("mediaStreams", 10)

    Row {
        id: rowButtons
	anchors.top: parent.verticalCenter
	anchors.left: parent.left
	anchors.right: parent.right
//	leftMargin: 10
//	rightMargin: 10
        ButtonSq {
            id: btOff
	    height: style.stdbuttonh * 0.5
            buttontext: "Stop"
	    // call stopMedia with selected EA
            onActivated: {
	        console.log("manager.stopPlayback with EA = " + media.getSelections().get(0).device)
	     	 manager.stopMediaInEa(media.getSelections().get(0).device)
		  activeStreams.model.refreshData()
            }
        }
        ButtonSq {
            id: btRemote
	    height: style.stdbuttonh * 0.5
            buttontext: "Remote"
	    // call stopMedia with selected EA
            onActivated: {
            }
        }
    }

    ListView {
        id:activeStreams
        anchors{
            top: rowButtons.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
	    topMargin: 10
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
	       var list = media.getSelections()
	       if (list.count > 0) {
	       	  var eas = ''
		  for (var i = 0; i < list.count; i++)
		  {
		    if (i > 0)
		       eas += ','
		    eas += list.get(0).device
		  }
		  manager.doMoveMedia(eas,streamID)
		  activeStreams.model.refreshData()
	       }
            }
        }
    }


}
