import QtQuick 2.2

Item{
    width: parent.width
    height: scaleY(15)
    property string title
    property string value
    property alias editedValue:input.text
    signal set()
    Rectangle{
        anchors.fill: parent
        color:"green"
        radius:5
    }
    Row{
        width: parent.width - 15
        height: childrenRect.height
        anchors.centerIn: parent
        Text{
            text:title
            font.pointSize: scaleY(6)
            width:parent.width /2
            color:"white"
            font.weight: Font.Light
        }
        TextInput{
            id:input
            //displayText: value
            text:value
            width:parent.width*.45
            color:"white"
            echoMode: TextInput.Normal /*< important because it links the value to the assigned property value. */
            font.underline: true
            font.pointSize: scaleY(6)
            Keys.onEnterPressed: set()
            font.weight: Font.DemiBold
            horizontalAlignment: TextInput.AlignRight
        }
        
    }
}
