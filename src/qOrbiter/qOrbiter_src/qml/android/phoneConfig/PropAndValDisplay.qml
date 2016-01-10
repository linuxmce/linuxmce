import QtQuick 1.1

Item{
    id:displayRoot
    width: parent.width
    height: manager.appHeight*.15
    property string title
    property string value
    property alias editedValue:input.text
    property bool useList:false
    property bool useText:true
    signal setVal()


    Rectangle{
        anchors.fill: parent
        color:"green"
        radius:5
        border.color: "white"
        border.width: 1
    }
    Row{
        width: parent.width - 15
        height: manager.appHeight*.06
        anchors.centerIn: parent
        Text{
            text:title
            font.pixelSize: manager.appHeight*.04
            width:parent.width /2-parent.width*.15
            color:"white"
            font.weight: Font.Light
        }
        TextInput{
            id:input
            //displayText: value
            text:value
            visible:useText
            width:parent.width*.45
            color:"white"
            echoMode: TextInput.Normal /*< important because it links the value to the assigned property value. */
            font.underline: true
            font.pointSize: 16
            Keys.onEnterPressed: setVal()
            font.weight: Font.DemiBold
            horizontalAlignment: TextInput.AlignRight
             onActiveFocusChanged: {
                 if(!activeFocus)
                     closeSoftwareInputPanel()
             }

            Keys.onReleased: {
                switch(event.key){
                case Qt.Key_Return:
                    setVal();
                    event.accepted = true
                    break;
                case Qt.Key_Down:
                    setVal();
                    event.accepted= true
                    break;
                default:
                    console.log("I dont know what key this is==>"+event.key )
                    break;
                }
            }
        }
        Item{
            height: parent.height-5
            width: parent.width*.15
            anchors.verticalCenter: parent.verticalCenter

            Rectangle{
                id:clear
                radius: 5
                height: parent.height-5
                width: height
                anchors.verticalCenter: parent.verticalCenter
                color:"darkgreen"
                border.color: "white"
                border.width: 2
                Text {
                   anchors.centerIn: parent
                    text: qsTr("X")
                }
                MouseArea{
                    anchors.fill: parent
                    id:ms
                    onReleased: {
                        value=""
                    }
                }

            }
        }


    }
}
