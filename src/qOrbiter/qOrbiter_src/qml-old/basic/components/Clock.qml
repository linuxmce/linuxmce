import QtQuick 1.1
Image {
	property bool verticalMirror: false
	function getDate(){
        	var d = new Date();
		
	        return d; // .format("dddd, mmmm d, yyyy | h:MM TT");
	}
	Timer { // Update the clock element periodically
	        interval: 5; running: true; repeat: true
	        onTriggered: txtDate.text = getDate()
	}

	id: imgClockBg
	//anchors.right: parent.right
	source: verticalMirror?"../img/common/timepanel_mid-v.png":"../img/common/timepanel_mid.png"
	//y: scaleY(43.89) // 316
	width: scaleX(35.23) // 451
	height:  scaleY(3.75) // 27
	smooth:  true
	Text{
        	id: txtDate
	        text: getDate()
	        anchors.rightMargin: scaleX(.78)
	        font.family: aeonRss.name;
	        font.pixelSize: parent.height*.7;
	        color: "#e5e5e5";
	        smooth: true
	        anchors.right: parent.right
	        anchors.verticalCenter: parent.verticalCenter
	
	}
}
