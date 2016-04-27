import QtQuick 1.0
import "../components"
import org.linuxmce.grids 1.0

StyledScreen{
  id:styledScreen

  PhoneFloorplanLayout{
    id:media
    anchors.fill: parent

    Row {
        id: rowButtons
	anchors.top: parent.verticalCenter
	anchors.left: parent.left
	anchors.right: parent.right
	spacing: 10
        StyledButton {
            id: btOff
	    height: 50
	    width: 200
            buttonText: "Stop"
	    // call stopMedia with selected EA
            onActivated: {
	        console.log("manager.stopPlayback with EA = " + media.getSelections().get(0).device)
	     	 manager.stopMediaInEa(media.getSelections().get(0).device)
		  activeStreams.model.refreshData()
            }
        }
        StyledButton {
            id: btRemote
	    height: 50
	    width: 200
            buttonText: "Remote"
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
        model: manager.getDataGridModel("mediaStreams", DataGrids.Floorplan_Media_Streams)
        delegate:
            StyledButton{
            id:gridBtn
            buttonText: description
            textSize: 16
	    height: scaleY(10)
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
}
