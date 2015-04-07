import QtQuick 1.1

Rectangle {

	  property alias buttonText: bText.text
	  width: parent.width / buttonRow.children.length
	  height: 74
	  color: "#282828"

	  Text {
	       id: bText
	       color: "white"
	       font.pixelSize: 22
	       font.family: "Roboto"
	       font.bold: false
	       anchors.centerIn: parent
	  }

	  MouseArea {
	  	    anchors.fill: parent;
		    onPressed: { parent.color = "#59adc9"; }
		    onReleased: { parent.color = "#282828"; }
	  }

}
