import QtQuick 1.1
Item{
    id:videoDelegate  
    clip:true
    height: currentCellHeight
    width:currentCellWidth
    Image{
        source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
        anchors.left: parent.left
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        smooth: true
        asynchronous: true
    }
    Rectangle{
        anchors.fill: parent
        opacity: .65
        color:trap.pressed ? "darkgreen" : "black"

    }

    StyledText{
        text: name
        anchors.centerIn: parent
        fontSize: 36
        color: "White"
    }

    MouseArea{
        id:trap
        anchors.fill: parent
        onReleased: manager.setStringParam(4, id);
    }
}
