import QtQuick 1.1

Item{
    id:pinButton
    height: width
    width: parent.width /3

    Rectangle{
        radius: 5
        anchors.fill: parent
        color:ms.pressed ? "grey" : "green"
    }

    StyledText{
        text:modelData
        color:"white"
        font.pointSize: scaleY(4)
        anchors.centerIn: parent
    }

    MouseArea{
        id:ms
        anchors.fill: parent
        onReleased: {

            if(typeof(modelData)==="number"){
                userPass.text = userPass.text+modelData
                return
            }else if(modelData==="X"){
                if(userPass.text !=="")
                userPass.text = userPass.text.substring(0, userPass.text.length-1)
                return
            }else if(modelData==="GO"){
                manager.authUserMedia(manager.q_mediaType, userPass.text, requestedUser)
            }
        }
    }

}
