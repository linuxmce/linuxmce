import QtQuick 2.2
import "../."
Panel {
    id:numberPad

    anchors.verticalCenter: parent.verticalCenter
    anchors.verticalCenterOffset: Style.scaleY(8)
    anchors.horizontalCenter: parent.horizontalCenter
    height: parent.height*.65
    width: parent.width *.30
    headerTitle: qsTr("Channel:", "Tv Channel Entry") + targetChannel

    function submit(){
        manager.gridChangeChannel(targetChannel, targetChannel)
        //        var pad = 3 - targetChannel.length
        //        switch (pad){
        //        case 1:
        //            targetChannel= "0"+targetChannel;
        //            break;
        //        case 2:
        //            targetChannel = "00"+targetChannel;
        //            break;
        //        default:
        //            break;
        //        }

        //        var chan ="i"+String(playlistPanel.bcastSource)+targetChannel
        //        manager.gridChangeChannel(chan, chan)
        targetChannel=""

    }

    function addNumber(newNum){
        targetChannel= targetChannel+String(newNum)
    }

    property string targetChannel:""

    Flow {
        id: flow1
        anchors.top:headerRect.bottom
        anchors.horizontalCenter: headerRect.horizontalCenter
        width:headerRect.width
        height:  Style.appButtonHeight *3

        Repeater{
            model: 9
            StyledButton {
                height: Style.appButtonHeight
                width: headerRect.width/3
                buttonText: (index+1)
                onActivated: if(targetChannel.length!==3) {targetChannel = targetChannel+String(index+1); }else {submit()}
            }
        }
    }
    Row{
        id:bottomRow
        height: childrenRect.height
        width:flow1.width
        anchors.top: flow1.bottom
        anchors.right: flow1.right

        StyledButton {
            height: Style.appButtonHeight
            width: headerRect.width/3
            buttonText: "<"

        }
        StyledButton {
            height: Style.appButtonHeight
            width: headerRect.width/3
            buttonText: "0"
            onActivated:addNumber(0)

        }
        StyledButton {
            height: Style.appButtonHeight
            width: headerRect.width/3
            buttonText: qsTr("Enter", "Confirm Entry")
            onActivated: {
                submit()
            }
        }
    }
}

