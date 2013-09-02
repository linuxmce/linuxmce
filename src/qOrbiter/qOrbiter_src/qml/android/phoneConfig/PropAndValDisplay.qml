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
    signal set()


    Rectangle{
        anchors.fill: parent
        color:"green"
        radius:5
        border.color: "white"
        border.width: 1
    }
    Row{
        width: parent.width - 15
        height: childrenRect.height
        anchors.centerIn: parent
        Text{
            text:title
            font.pixelSize: manager.appHeight*.04
            width:parent.width /2
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
            Keys.onEnterPressed: set()
            font.weight: Font.DemiBold
            horizontalAlignment: TextInput.AlignRight
            Keys.onReleased: {
                switch(event.key){
                case Qt.Key_Return:
                    set();
                    event.accepted = true
                    break;
                case Qt.Key_Down:
                    set();
                    event.accepted= true
                    break;
                default:
                    console.log("I dont know what key this is==>"+event.key )
                    break;
                }
            }
        }
    }
}
