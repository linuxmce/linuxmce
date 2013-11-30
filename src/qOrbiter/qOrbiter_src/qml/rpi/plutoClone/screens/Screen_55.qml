import QtQuick 2.0
import "../components"
Item{
    id:linphones

    Rectangle{
        height: manager.appHeight
        width: manager.appWidth
        color: "transparent"
        StyledText {
            id: linphoneslabel
            x: 74
            y: 101
            text: "LinPhones(?)"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
