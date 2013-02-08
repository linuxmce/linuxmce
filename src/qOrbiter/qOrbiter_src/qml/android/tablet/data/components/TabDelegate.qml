import QtQuick 1.1

Item {
    id: container
     property bool showSeperator: true
    width: parent.itemWidth
    height: 84
    state: (itemIndex == parent.selectedIndex ? "SELECTED" : "DESELECTED")
    states: [
        State {
	    name: "DESELECTED"
	    PropertyChanges { target: tabIndicator; height: 3 }      
	},
	State {	
	    name: "SELECTED"
	    PropertyChanges { target: tabIndicator; height: 8 }
	}
    ]

    Rectangle {
        id: pressbkg
	anchors.fill: parent
	color: "#33B5E5"
	state: "UNCLICKED"
	states: [
	    State {
	    	  name: "CLICKED"
		  PropertyChanges { target: pressbkg; opacity: 1.0 }
	    },
	    State {
	          name: "UNCLICKED"
		  PropertyChanges { target: pressbkg; opacity: 0.0 } 
	    }
	]
	transitions: [
	    Transition {
	        from: "UNCLICKED"
		to: "CLICKED"
		PropertyAnimation { properties: "opacity"; target: pressbkg; duration: 100 }
	    },
	    Transition {
	        from: "CLICKED"
		to: "UNCLICKED"
	        PropertyAnimation { properties: "opacity"; target: pressbkg; duration: 100 }
	    }
	]
    }

    Text {
       text: itemText
       font.family: "Roboto"
       font.pointSize: 13
       font.bold: false
       color: "#FFFFFF"
       anchors.fill: parent
       horizontalAlignment: Text.AlignHCenter
       verticalAlignment: Text.AlignVCenter	 
    }
 
    Rectangle {
        id: tabIndicator
        color: "#33B5E5"
        width: parent.width - 1
	anchors.bottom: parent.bottom
    }

    Rectangle {
        id: seperator
	color: "#494949"
	visible: ( ( itemIndex != parent.parent.actions.count - 1 ) ? true : false ) 
        width: 1
	height: 48
	anchors.left: parent.right
	anchors.verticalCenter: parent.verticalCenter 
    }

    MouseArea {
        id: hotspot
	anchors.fill: parent
	onPressed: {
	    pressbkg.state = "CLICKED";
	    console.log("itemIndex: "+itemIndex);
	    console.log("parent.count" + parent.parent.actions.count);
	}
	onReleased: {
            pressbkg.state = "UNCLICKED";
	}
	onClicked: {  
	    parent.parent.selectedIndex = itemIndex;
	}
    }

}