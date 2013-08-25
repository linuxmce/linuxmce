import QtQuick 1.1

Item{
    width: parent.width
    height: scaleY(5)
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
        height: parent.height
        anchors.centerIn: parent
        Text{
            text:title
            font.pointSize: scaleY(2)
        }
        TextInput{
            id:input
            //displayText: value
            text:value
            echoMode: TextInput.Normal
            font.pointSize: scaleY(2)
            Keys.onEnterPressed: set()
        }
        
    }
}
