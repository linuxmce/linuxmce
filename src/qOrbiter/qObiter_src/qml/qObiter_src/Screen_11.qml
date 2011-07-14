import Qt 4.7
import "components"
Item{
    id:calldialnumber

    Rectangle{
        height: 430
        width: 800
        color: style.bgcolor
        Text {
            id: dialcall
            x: 74
            y: 101
            text: "Make Call Dial Number"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
