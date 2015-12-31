import QtQuick 1.0
import "../components"

StyledScreen {

    Component.onCompleted: {
        updateQorbiter()
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: .85
    }

 property string requestUrl: ("http://"+manager.m_ipAddress+"/lmce-admin/qOrbiterGenerator.php?d="+manager.iPK_Device)
    StyledText {
        id: label
        text: qsTr("Regenerating your Configuration data...")
        anchors.centerIn: parent
        font.bold: false
        color:"white"
        font.weight: Font.Light
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        width: parent.width
        font.pixelSize: scaleY(8)
    }

    function updateQorbiter(){

        var paramsRequest = new XMLHttpRequest();
        paramsRequest.onreadystatechange = function(){
            if(paramsRequest.readyState===XMLHttpRequest.DONE){
                label.text = "Finished"
                manager.gotoQScreen("Screen_1.qml")
            }
        }

        paramsRequest.open("POST",requestUrl)
        paramsRequest.send();
    }

}
