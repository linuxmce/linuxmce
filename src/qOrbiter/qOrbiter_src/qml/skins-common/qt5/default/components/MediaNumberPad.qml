import QtQuick 2.2

Panel {

    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
    anchors.verticalCenterOffset: scaleY(8)
    height: childrenRect.height
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
        width: parent.width
        height:3*scaleY(15)

        Repeater{
            model: 9
            StyledButton {
                state:"numberpad-small"
                buttonText: (index+1)
                onActivated: if(targetChannel.length!==3) {targetChannel = targetChannel+String(index+1); }else {submit()}
            }
        }
    }
    Row{
        id:bottomRow
        height: childrenRect.height
        width:parent.width
        anchors.top: flow1.bottom

        StyledButton {
            state:"numberpad-small"
            buttonText: "<"

        }
        StyledButton {
            state:"numberpad-small"
            buttonText: "0"
            onActivated:addNumber(0)

        }
        StyledButton {
            state:"numberpad-small"
            buttonText: qsTr("Enter", "Confirm Entry")
            onActivated: {
                submit()
            }
        }
    }
}

