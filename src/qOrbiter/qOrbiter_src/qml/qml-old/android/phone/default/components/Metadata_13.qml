import QtQuick 1.1

Column{
    id:textcol
    spacing: scaleY(1.5)
    width: scaleX(100)
    height: childrenRect.height+(scaleY(2))

    StyledText{
        text:"Device:\n"+dcenowplaying.qs_mainTitle
        color:"White"
        fontSize: 50
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

}
