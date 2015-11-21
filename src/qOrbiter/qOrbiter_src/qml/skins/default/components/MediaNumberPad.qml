import QtQuick 2.2
import "../."

Panel {
    id:numberPad
    height: Style.appButtonHeight*5
    width: manager.isProfile ? parent.width *.65 : parent.width *.30
    headerTitle: qsTr("Channel:", "Tv Channel Entry") + targetChannel
    centered: false
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
        height: ( Style.appButtonHeight -1)*3

        Repeater{
            model: 9
            StyledButton {
                height: Style.appButtonHeight -1
                width: headerRect.width/3
                buttonText: (index+1)
                onActivated: if(targetChannel.length!==3) {targetChannel = targetChannel+String(index+1); }else {submit()}
            }
        }
    }
    Row{
        id:bottomRow

        width:flow1.width
        anchors.top: flow1.bottom
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter

        StyledButton {
            height: Style.appButtonHeight-1
            width: headerRect.width/3
            buttonText: "<"

        }
        StyledButton {
            height: Style.appButtonHeight-1
            width: headerRect.width/3
            buttonText: "0"
            onActivated:addNumber(0)

        }
        StyledButton {
            height: Style.appButtonHeight-1
            width: headerRect.width/3
            buttonText: qsTr("Enter", "Confirm Entry")
            onActivated: {
                submit()
            }
        }
    }
}

