import QtQuick 1.1
Item{
    /* Item sized using anchors */


    property int flowButtonHeight: buttonFlow.height/3
    property int flowButtonWidth:buttonFlow.width/3
    property string numberString:""

    Item {
        id:padHeader
        width: parent.width
        height: scaleX(10)

        Rectangle{
            anchors.fill: parent
            gradient:appStyle.bgHeaderGradient
        }

        StyledText{
            anchors{
                left:parent.left
                verticalCenter: parent.verticalCenter
            }
            text:"Channel "+numberString
        }

    }

    Flow{
        id:buttonFlow
        width: 3*scaleX(18)
        height:3*scaleY(12)

        anchors{
            top:padHeader.bottom
            horizontalCenter: padHeader.horizontalCenter
        }

        Repeater{
            id:buttonRepeater
            model: 9

            delegate: StyledButton{
                height: flowButtonHeight
                width: flowButtonWidth
                state:"fixed"
                buttonText: index+1
                onActivated: {
                    numberString = numberString+(index+1)
                }
            }
        }
    }




    Row{
        id:actionbuttons
        anchors{
            top:buttonFlow.bottom
            left:buttonFlow.left
            right:buttonFlow.right
        }
        height:childrenRect.height
        spacing: 0
        StyledButton{
            id:backbutton
            state:"fixed"

            buttonText: qsTr("BACK")

            textSize: 10
            onActivated: {
                numberString = ""
            }
        }

        StyledButton{
            state:"fixed"
            buttonText: "0"
            onActivated: {
                numberString = numberString+"0"
            }
        }

        StyledButton{
            state:"fixed"
            buttonText: qsTr("GO")
            onActivated: {
                manager.changeChannels(numberString)
                numberString = ""
            }
        }
    }

}
